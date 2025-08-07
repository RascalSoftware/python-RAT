/*<%
import glob
cfg['sources'] = [*glob.glob('RAT/*.cpp'), *glob.glob('RAT/*.c')]
cfg['parallel'] = True
setup_pybind11(cfg)
%>*/

#include <cstdlib>
#include <iostream>
#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include "RAT/RATMain.h"
#include "RAT/RATMain_initialize.h"
#include "RAT/RATMain_terminate.h"
#include "RAT/RATMain_types.h"
#include "RAT/makeSLDProfile.h"
#include "RAT/dylib.hpp"
#include "RAT/events/eventManager.h"
#include "includes/defines.h"
#include "includes/functions.h"

namespace py = pybind11;

const int DEFAULT_DOMAIN = -1;
const int DEFAULT_NREPEATS = 1;

class DylibEngine
{
    public:
    std::unique_ptr<dylib> library;
    std::string functionName;
    
    DylibEngine(std::string libName, std::string functionName)
    {
        this->functionName = functionName;             
        this->library = std::unique_ptr<dylib>(new dylib(libName));
        if (!library)
        {
            std::cerr << "dynamic library failed to load" << std::endl;
            return;
        }  
    };

    ~DylibEngine(){};

    py::list invoke(std::vector<double>& xdata, std::vector<double>& params)
    {   
        try{
            std::vector<double> output;

            auto func = library->get_function<void(std::vector<double>&, std::vector<double>&, std::vector<double>&)>(functionName);
            func(xdata, params, output);
            
            return py::cast(output);

        }catch (const dylib::symbol_error &) {
            throw std::runtime_error("failed to get dynamic library symbol for " + functionName);
        }        
    };

    py::tuple invoke(std::vector<double>& params, std::vector<double>& bulkIn, std::vector<double>& bulkOut, int contrast, int domain=DEFAULT_DOMAIN)
    {   
        try{
            std::vector<double> tempOutput;
            double *outputSize = new double[2]; 
            double roughness = 0.0;

            if (domain != -1) {
                auto func = library->get_function<void(std::vector<double>&, std::vector<double>&, std::vector<double>&, 
                                                       int, int, std::vector<double>&, double*, double*)>(functionName);
                func(params, bulkIn, bulkOut, contrast, domain, tempOutput, outputSize, &roughness); 
            }
            else {
                auto func = library->get_function<void(std::vector<double>&, std::vector<double>&, std::vector<double>&, 
                                                       int, std::vector<double>&, double*, double*)>(functionName);
                func(params, bulkIn, bulkOut, contrast, tempOutput, outputSize, &roughness);
            } 
            
            py::list output;
            for (int32_T idx1{0}; idx1 < outputSize[0]; idx1++)
            {
                py::list rows;  
                for (int32_T idx2{0}; idx2 < outputSize[1]; idx2++)
                {
                    rows.append(tempOutput[(int32_T)outputSize[1] * idx1 + idx2]);
                }
                output.append(rows);
            }
            return py::make_tuple(output, roughness);    

        }catch (const dylib::symbol_error &) {
            throw std::runtime_error("failed to get dynamic library symbol for " + functionName);
        }        
    };
};

class EventBridge
{
    public:
    std::unique_ptr<dylib> library;
    py::function callback;
    
    EventBridge(py::function callback)
    {   
        std::string filename = "eventManager" + std::string(dylib::extension);
        this->library = std::unique_ptr<dylib>(new dylib(std::getenv("RAT_PATH"), filename.c_str()));
        if (!library)
        {
            std::cerr << "event manager dynamic library failed to load" << std::endl;
            return;
        }
        this->callback = callback;
    };
    
    py::list unpackDataToCell(int rows, int cols, double* data, double* nData, 
                              double* data2, double* nData2, bool isOutput2D=false)   
    {
        py::list allResults;
        int dims[2] = {0, 0};
        int offset = 0;
        for (int i = 0; i < rows; i++){
            dims[0] = (int)nData[2*i];
            dims[1] = (int)nData[2*i+1];
            auto result = py::array_t<double,  py::array::f_style>({dims[0], dims[1]});
            std::memcpy(result.request().ptr, data + offset, result.nbytes());
            offset += result.size();
            if (isOutput2D){
                py::list rowList;
                rowList.append(result);
                allResults.append(rowList);
            }
            else{
                allResults.append(result);
            }
        }

        if (data2 != NULL && nData2 != NULL)
        {
            // This is used to unpack the domains data into the second column 
            offset = 0;
            for ( int i = 0; i < rows; i++){
                dims[0] = (int)nData2[2*i];
                dims[1] = (int)nData2[2*i+1];
                auto result = py::array_t<double,  py::array::f_style>({dims[0], dims[1]});
                std::memcpy(result.request().ptr, data2 + offset, result.nbytes());
                offset +=  result.size();
                auto rowList = allResults[i].cast<py::list>();
                rowList.append(result);
            }
        }

        return allResults;
    };

    void eventCallback(const baseEvent& event)
    {
        if (event.type == EventTypes::Message) {
            messageEvent* mEvent = (messageEvent*)&event; 
            this->callback(event.type, mEvent->msg);
        } else if (event.type == EventTypes::Progress){
            progressEvent* pEvent = (progressEvent*)&event;
            ProgressEventData eventData;
            
            eventData.message = pEvent->msg;
            eventData.percent = pEvent->percent;
            this->callback(event.type, eventData);
        } else if (event.type == EventTypes::Plot){
            plotEvent* pEvent = (plotEvent*)&event;
            PlotEventData eventData;
            
            eventData.modelType = std::string(pEvent->data->modelType);

            eventData.subRoughs = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.subRoughs.request().ptr, pEvent->data->subRoughs, eventData.subRoughs.nbytes());

            eventData.resample = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.resample.request().ptr, pEvent->data->resample, eventData.resample.nbytes());

            eventData.dataPresent = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.dataPresent.request().ptr, pEvent->data->dataPresent, eventData.dataPresent.nbytes());

            eventData.reflectivity = unpackDataToCell(pEvent->data->nContrast, 1, 
                                                      pEvent->data->reflect, pEvent->data->nReflect, NULL, NULL);

            eventData.shiftedData = unpackDataToCell(pEvent->data->nContrast, 1, 
                                                     pEvent->data->shiftedData, pEvent->data->nShiftedData, NULL, NULL);
            
            eventData.sldProfiles = unpackDataToCell(pEvent->data->nContrast, (pEvent->data->nSldProfiles2 == NULL) ? 1 : 2,
                                                     pEvent->data->sldProfiles, pEvent->data->nSldProfiles, 
                                                     pEvent->data->sldProfiles2, pEvent->data->nSldProfiles2, true);
            
            eventData.resampledLayers = unpackDataToCell(pEvent->data->nContrast, (pEvent->data->nLayers2 == NULL) ? 1 : 2, 
                                                         pEvent->data->layers, pEvent->data->nLayers, 
                                                         pEvent->data->layers2, pEvent->data->nLayers2, true);
            
            int offset = 0;
            for (int i = 0; i < pEvent->data->nContrast; i++){
               eventData.contrastNames.append(std::string(pEvent->data->contrastNames + offset, 
                                                          pEvent->data->nContrastNames[i]));
               offset += pEvent->data->nContrastNames[i];
            }
            this->callback(event.type, eventData);
        }
    };
    

    void registerEvent(EventTypes eventType)
    {
        std::function<void(const baseEvent& event)> caller = std::bind(&EventBridge::eventCallback, this, std::placeholders::_1);
        auto addListener = library->get_function<void(EventTypes, std::function<void(const baseEvent&)>)>("addListener");
        addListener(eventType, caller);    
    };

    void clear()
    {
        auto clearListeners = library->get_function<void(void)>("clearListeners");
        clearListeners();
    };
};

RAT::b_ParamNames createParamNamesStruct(const NameStore& names)
{
    RAT::b_ParamNames  names_struct;
    names_struct.params = customCaller("NameStore.params", pyListToRatCellWrap02d, names.params);
    names_struct.backgroundParams = customCaller("NameStore.backgroundParams", pyListToRatCellWrap02d, names.backgroundParams);
    names_struct.scalefactors = customCaller("NameStore.scalefactors", pyListToRatCellWrap02d, names.scalefactors);
    names_struct.bulkIns = customCaller("NameStore.bulkIns", pyListToRatCellWrap02d, names.bulkIns);
    names_struct.bulkOuts = customCaller("NameStore.bulkOuts", pyListToRatCellWrap02d, names.bulkOuts);
    names_struct.resolutionParams = customCaller("NameStore.resolutionParams", pyListToRatCellWrap02d, names.resolutionParams);
    names_struct.domainRatios = customCaller("NameStore.domainRatios", pyListToRatCellWrap02d, names.domainRatios);
    names_struct.contrasts = customCaller("NameStore.contrasts", pyListToRatCellWrap02d, names.contrasts);
    
    return names_struct;
}

RAT::CheckFlags createCheckFlagsStruct(const Checks& checks)
{
    RAT::CheckFlags checks_struct;
    checks_struct.params = customCaller("Checks.params", pyArrayToRatRowArray1d, checks.params);
    checks_struct.backgroundParams = customCaller("Checks.backgroundParams", pyArrayToRatRowArray1d, checks.backgroundParams);
    checks_struct.scalefactors = customCaller("Checks.scalefactors", pyArrayToRatRowArray1d, checks.scalefactors);
    checks_struct.bulkIns = customCaller("Checks.bulkIns", pyArrayToRatRowArray1d, checks.bulkIns);
    checks_struct.bulkOuts = customCaller("Checks.bulkOuts", pyArrayToRatRowArray1d, checks.bulkOuts);
    checks_struct.resolutionParams = customCaller("Checks.resolutionParams", pyArrayToRatRowArray1d, checks.resolutionParams);
    checks_struct.domainRatios = customCaller("Checks.domainRatios", pyArrayToRatRowArray1d, checks.domainRatios);
    
    return checks_struct;
}

RAT::b_ProblemDefinition createProblemDefinitionStruct(const ProblemDefinition& problem)
{
    RAT::b_ProblemDefinition problem_struct;
    
    stringToRatBoundedArray(problem.TF, problem_struct.TF.data, problem_struct.TF.size);
    problem_struct.resample = customCaller("Problem.resample", pyArrayToRatRowArray1d, problem.resample);
    problem_struct.data = customCaller("Problem.data", pyListToRatCellWrap1, problem.data);
    problem_struct.dataPresent = customCaller("Problem.dataPresent", pyArrayToRatRowArray1d, problem.dataPresent);
    problem_struct.dataLimits = customCaller("Problem.dataLimits", pyListToRatCellWrap2, problem.dataLimits);
    problem_struct.simulationLimits = customCaller("Problem.simulationLimits", pyListToRatCellWrap2, problem.simulationLimits);   
    problem_struct.numberOfContrasts = problem.numberOfContrasts;
    stringToRatBoundedArray(problem.geometry, problem_struct.geometry.data, problem_struct.geometry.size);
    problem_struct.useImaginary = problem.useImaginary;
    problem_struct.repeatLayers = customCaller("Problem.repeatLayers", pyArrayToRatRowArray1d, problem.repeatLayers);
    problem_struct.contrastBackgroundParams = customCaller("Problem.contrastBackgroundParams", pyListToRatCellWrap3, problem.contrastBackgroundParams); 
    problem_struct.contrastBackgroundTypes = customCaller("Problem.contrastBackgroundTypes", pyListToRatCellWrap02d, problem.contrastBackgroundTypes);
    problem_struct.contrastBackgroundActions = customCaller("Problem.contrastBackgroundActions", pyListToRatCellWrap02d, problem.contrastBackgroundActions);
    problem_struct.contrastScalefactors = customCaller("Problem.contrastScalefactors", pyArrayToRatRowArray1d, problem.contrastScalefactors);
    problem_struct.contrastBulkIns = customCaller("Problem.contrastBulkIns", pyArrayToRatRowArray1d, problem.contrastBulkIns);
    problem_struct.contrastBulkOuts = customCaller("Problem.contrastBulkOuts", pyArrayToRatRowArray1d, problem.contrastBulkOuts);
    problem_struct.contrastResolutionParams = customCaller("Problem.contrastResolutionParams", pyListToRatCellWrap4, problem.contrastResolutionParams);
    problem_struct.contrastResolutionTypes = customCaller("Problem.contrastResolutionTypes", pyListToRatCellWrap02d, problem.contrastResolutionTypes);
    problem_struct.backgroundParams = customCaller("Problem.backgroundParams", pyArrayToRatRowArray1d, problem.backgroundParams);
    problem_struct.scalefactors = customCaller("Problem.scalefactors", pyArrayToRatRowArray1d, problem.scalefactors);
    problem_struct.bulkIns = customCaller("Problem.bulkIns", pyArrayToRatRowArray1d, problem.bulkIns);
    problem_struct.bulkOuts = customCaller("Problem.bulkOuts", pyArrayToRatRowArray1d, problem.bulkOuts);
    problem_struct.resolutionParams = customCaller("Problem.resolutionParams", pyArrayToRatRowArray1d, problem.resolutionParams);
    problem_struct.params = customCaller("Problem.params", pyArrayToRatRowArray1d, problem.params);
    problem_struct.numberOfLayers = problem.numberOfLayers;
    problem_struct.contrastLayers = customCaller("Problem.contrastLayers", pyListToRatCellWrap5, problem.contrastLayers);
    problem_struct.layersDetails = customCaller("Problem.layersDetails", pyListToRatCellWrap6, problem.layersDetails);
    problem_struct.customFiles = customCaller("Problem.customFiles", py_function_array_to_rat_cell_wrap_0, problem.customFiles);   
    stringToRatBoundedArray(problem.modelType, problem_struct.modelType.data, problem_struct.modelType.size);
    problem_struct.contrastCustomFiles = customCaller("Problem.contrastCustomFiles", pyArrayToRatRowArray1d, problem.contrastCustomFiles);
    problem_struct.contrastDomainRatios = customCaller("Problem.contrastDomainRatios", pyArrayToRatRowArray1d, problem.contrastDomainRatios);
    problem_struct.domainRatios = customCaller("Problem.domainRatios", pyArrayToRatRowArray1d, problem.domainRatios);
    problem_struct.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_struct.domainContrastLayers = customCaller("Problem.domainContrastLayers", pyListToRatCellWrap5, problem.domainContrastLayers);
    problem_struct.fitParams =  customCaller("Problem.fitParams", pyArrayToRatRowArray1d, problem.fitParams);
    problem_struct.fitLimits =  customCaller("Problem.fitLimits", pyArrayToRatArray2d, problem.fitLimits);
    problem_struct.priorNames = customCaller("Problem.priorNames", pyListToRatCellWrap01d, problem.priorNames);
    problem_struct.priorValues =  customCaller("Problem.priorValues", pyArrayToRatArray2d, problem.priorValues);

    problem_struct.names = createParamNamesStruct(problem.names);
    problem_struct.checks = createCheckFlagsStruct(problem.checks);


    return problem_struct;
}


RAT::Controls createControlsStruct(const Control& control)
{
    RAT::Controls control_struct;
    control_struct.funcTolerance = control.funcTolerance;
    control_struct.maxFuncEvals = control.maxFuncEvals;
    control_struct.maxIterations = control.maxIterations;
    control_struct.populationSize = control.populationSize;
    control_struct.fWeight = control.fWeight;
    control_struct.crossoverProbability = control.crossoverProbability;
    control_struct.targetValue = control.targetValue;
    control_struct.numGenerations = control.numGenerations;
    control_struct.strategy = control.strategy;
    control_struct.nLive = control.nLive;
    control_struct.nMCMC = control.nMCMC;
    control_struct.propScale = control.propScale;
    control_struct.nsTolerance = control.nsTolerance;
    control_struct.numSimulationPoints = control.numSimulationPoints;
    control_struct.updateFreq = control.updateFreq;
    control_struct.updatePlotFreq = control.updatePlotFreq;
    control_struct.nSamples = control.nSamples;
    control_struct.nChains = control.nChains;
    control_struct.jumpProbability = control.jumpProbability;
    control_struct.pUnitGamma = control.pUnitGamma;
    stringToRatBoundedArray(control.parallel, control_struct.parallel.data, control_struct.parallel.size);
    stringToRatBoundedArray(control.procedure, control_struct.procedure.data, control_struct.procedure.size);
    stringToRatBoundedArray(control.display, control_struct.display.data, control_struct.display.size);
    control_struct.xTolerance = control.xTolerance;
    control_struct.resampleMinAngle = control.resampleMinAngle;
    control_struct.resampleNPoints = control.resampleNPoints;
    stringToRatBoundedArray(control.boundHandling, control_struct.boundHandling.data, control_struct.boundHandling.size);
    control_struct.adaptPCR = control.adaptPCR;
    control_struct.calcSLD = false;
    stringToRatBoundedArray(control.IPCFilePath, control_struct.IPCFilePath.data, control_struct.IPCFilePath.size);

    return control_struct;
}

OutputResult OutputResultFromStruct(const RAT::Results result)
{
    // Copy problem to output
    OutputResult output_result;
    for (int32_T idx0{0}; idx0 < result.reflectivity.size(0); idx0++) {
        auto tmp = result.reflectivity[idx0];
        auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
        std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
        output_result.reflectivity.append(array);
    }

    for (int32_T idx0{0}; idx0 < result.simulation.size(0); idx0++) {
        auto tmp = result.simulation[idx0];
        auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
        std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
        output_result.simulation.append(array);
    }

    for (int32_T idx0{0}; idx0 < result.shiftedData.size(0); idx0++) {
        auto tmp = result.shiftedData[idx0];
        auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
        std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
        output_result.shiftedData.append(array);
    }

    for (int32_T idx0{0}; idx0 < result.backgrounds.size(0); idx0++) {
        auto tmp = result.backgrounds[idx0];
        auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
        std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
        output_result.backgrounds.append(array);
    }

    for (int32_T idx0{0}; idx0 < result.resolutions.size(0); idx0++) {
        auto tmp = result.resolutions[idx0];
        auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
        std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
        output_result.resolutions.append(array);
    }

    for (int32_T idx0{0}; idx0 < result.sldProfiles.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < result.sldProfiles.size(1); idx1++) {
            auto tmp = result.sldProfiles[idx0 +  result.sldProfiles.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        output_result.sldProfiles.append(inner_list);
    }

    for (int32_T idx0{0}; idx0 < result.layers.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < result.layers.size(1); idx1++) {
            auto tmp = result.layers[idx0 +  result.layers.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        output_result.layers.append(inner_list);
    }

    for (int32_T idx0{0}; idx0 < result.resampledLayers.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < result.resampledLayers.size(1); idx1++) {
            auto tmp = result.resampledLayers[idx0 +  result.resampledLayers.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        output_result.resampledLayers.append(inner_list);
    }

    for (int32_T idx0{0}; idx0 < result.fitNames.size(0); idx0++) {
        auto tmp = result.fitNames[idx0];
        std::string str(tmp.f1.data(), tmp.f1.size(1));        
        output_result.fitNames.append(str);
    }    

    output_result.fitParams = py::array_t<real_T>(result.fitParams.size(1));
    auto buffer = output_result.fitParams.request();
    std::memcpy(buffer.ptr, result.fitParams.data(), output_result.fitParams.size()*sizeof(real_T));

    output_result.contrastParams.scalefactors = py::array_t<real_T>(result.contrastParams.scalefactors.size(0));
    buffer = output_result.contrastParams.scalefactors.request();
    std::memcpy(buffer.ptr, result.contrastParams.scalefactors.data(), output_result.contrastParams.scalefactors.size()*sizeof(real_T));

    output_result.contrastParams.bulkIn = py::array_t<real_T>(result.contrastParams.bulkIn.size(0));
    buffer = output_result.contrastParams.bulkIn.request();
    std::memcpy(buffer.ptr, result.contrastParams.bulkIn.data(), output_result.contrastParams.bulkIn.size()*sizeof(real_T));

    output_result.contrastParams.bulkOut = py::array_t<real_T>(result.contrastParams.bulkOut.size(0));
    buffer = output_result.contrastParams.bulkOut.request();
    std::memcpy(buffer.ptr, result.contrastParams.bulkOut.data(), output_result.contrastParams.bulkOut.size()*sizeof(real_T));

    output_result.calculationResults.sumChi = result.calculationResults.sumChi;
    output_result.calculationResults.chiValues = py::array_t<real_T>(result.calculationResults.chiValues.size(0));
    buffer = output_result.calculationResults.chiValues.request();
    std::memcpy(buffer.ptr, result.calculationResults.chiValues.data(), output_result.calculationResults.chiValues.size()*sizeof(real_T));

    output_result.contrastParams.subRoughs = py::array_t<real_T>(result.contrastParams.subRoughs.size(0));
    buffer = output_result.contrastParams.subRoughs.request();
    std::memcpy(buffer.ptr, result.contrastParams.subRoughs.data(), output_result.contrastParams.subRoughs.size()*sizeof(real_T));

    output_result.contrastParams.resample = py::array_t<real_T>(result.contrastParams.resample.size(1));
    buffer = output_result.contrastParams.resample.request();
    std::memcpy(buffer.ptr, result.contrastParams.resample.data(), output_result.contrastParams.resample.size()*sizeof(real_T));

    return output_result;
}

ProblemDefinition problemDefinitionFromStruct(const RAT::b_ProblemDefinition problem)
{
    ProblemDefinition problem_def;
    
    stringFromRatBoundedArray(problem.TF.data, problem.TF.size, problem_def.TF);    
    problem_def.resample = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resample);
    problem_def.data = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_1, 2U>>(problem.data);
    problem_def.dataPresent = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.dataPresent);
    problem_def.dataLimits =  pyListFromRatCellWrap2(problem.dataLimits);
    problem_def.simulationLimits =  pyListFromRatCellWrap2(problem.simulationLimits);
    problem_def.numberOfContrasts = problem.numberOfContrasts;
    stringFromRatBoundedArray(problem.geometry.data, problem.geometry.size, problem_def.geometry);
    problem_def.useImaginary = problem.useImaginary;       
    problem_def.repeatLayers = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.repeatLayers);
    problem_def.contrastBackgroundParams = pyListFromBoundedCellWrap<coder::array<RAT::cell_wrap_3, 2U>>(problem.contrastBackgroundParams);
    problem_def.contrastBackgroundTypes = pyListFromRatCellWrap02d(problem.contrastBackgroundTypes);
    problem_def.contrastBackgroundActions = pyListFromRatCellWrap02d(problem.contrastBackgroundActions);    
    problem_def.contrastScalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastScalefactors);
    problem_def.contrastBulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkIns);
    problem_def.contrastBulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkOuts);
    problem_def.contrastResolutionParams = pyListFromBoundedCellWrap<coder::array<RAT::cell_wrap_4, 2U>>(problem.contrastResolutionParams);
    problem_def.contrastResolutionTypes = pyListFromRatCellWrap02d(problem.contrastResolutionTypes);
    problem_def.backgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.backgroundParams);
    problem_def.scalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.scalefactors);
    problem_def.bulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkIns);
    problem_def.bulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkOuts);
    problem_def.resolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resolutionParams);
    problem_def.params = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.params);
    problem_def.numberOfLayers = problem.numberOfLayers;
    problem_def.contrastLayers = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_5, 2U>>(problem.contrastLayers);
    problem_def.layersDetails = pyListFromBoundedCellWrap<coder::array<RAT::cell_wrap_6, 2U>>(problem.layersDetails);
    // problem_def.customFiles is not set here since the object has been converted to function handles 
    stringFromRatBoundedArray(problem.modelType.data, problem.modelType.size, problem_def.modelType);
    problem_def.contrastCustomFiles = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastCustomFiles);
    problem_def.contrastDomainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastDomainRatios);
    problem_def.domainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.domainRatios);
    problem_def.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_def.domainContrastLayers = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_5, 2U>>(problem.domainContrastLayers);
    problem_def.fitParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.fitParams);
    problem_def.fitLimits = pyArrayFromRatArray2d(problem.fitLimits);
    problem_def.priorNames = pyListFromRatCellWrap01d(problem.priorNames);
    problem_def.priorValues = pyArrayFromRatArray2d(problem.priorValues);
    
    problem_def.names.params = pyListFromRatCellWrap02d(problem.names.params);
    problem_def.names.backgroundParams = pyListFromRatCellWrap02d(problem.names.backgroundParams);
    problem_def.names.scalefactors = pyListFromRatCellWrap02d(problem.names.scalefactors);
    problem_def.names.bulkIns = pyListFromRatCellWrap02d(problem.names.bulkIns);
    problem_def.names.bulkOuts = pyListFromRatCellWrap02d(problem.names.bulkOuts);
    problem_def.names.resolutionParams = pyListFromRatCellWrap02d(problem.names.resolutionParams);
    problem_def.names.domainRatios = pyListFromRatCellWrap02d(problem.names.domainRatios);
    problem_def.names.contrasts = pyListFromRatCellWrap02d(problem.names.contrasts);
    
    problem_def.checks.params = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.params);
    problem_def.checks.backgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.backgroundParams);
    problem_def.checks.scalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.scalefactors);
    problem_def.checks.bulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.bulkIns);
    problem_def.checks.bulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.bulkOuts);
    problem_def.checks.resolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.resolutionParams);
    problem_def.checks.domainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.domainRatios);

    return problem_def;
}

OutputBayesResult OutputBayesResultsFromStruct(const RAT::BayesResults results)
{
    OutputBayesResult bayesResults;

    bayesResults.chain = pyArrayFromRatArray2d(results.chain);

    bayesResults.predictionIntervals.reflectivity = pyList1DFromRatCellWrap1D<coder::array<RAT::cell_wrap_11, 1U>>(results.predictionIntervals.reflectivity);
    bayesResults.predictionIntervals.sld = pyList2dFromRatCellWrap<coder::array<RAT::cell_wrap_11, 2U>>(results.predictionIntervals.sld);
    bayesResults.predictionIntervals.sampleChi = pyArray1dFromBoundedArray<coder::bounded_array<real_T, 1000U, 1U>>(results.predictionIntervals.sampleChi);

    bayesResults.confidenceIntervals.percentile95 = pyArrayFromRatArray2d(results.confidenceIntervals.percentile95);
    bayesResults.confidenceIntervals.percentile65 = pyArrayFromRatArray2d(results.confidenceIntervals.percentile65);
    bayesResults.confidenceIntervals.mean = pyArrayFromRatArray2d(results.confidenceIntervals.mean);

    bayesResults.nestedSamplerOutput.logZ = results.nestedSamplerOutput.LogZ;
    bayesResults.nestedSamplerOutput.logZErr = results.nestedSamplerOutput.LogZErr;
    bayesResults.nestedSamplerOutput.nestSamples = pyArrayFromRatArray2d(results.nestedSamplerOutput.nestSamples);
    bayesResults.nestedSamplerOutput.postSamples = pyArrayFromRatArray2d(results.nestedSamplerOutput.postSamples);

    bayesResults.dreamOutput.allChains = pyArrayFromRatArray3d(results.dreamOutput.allChains);
    bayesResults.dreamOutput.outlierChains = pyArray2dFromBoundedArray<coder::bounded_array<real_T, 2000U, 2U>>(results.dreamOutput.outlierChains);
    bayesResults.dreamOutput.runtime = results.dreamOutput.runtime;
    bayesResults.dreamOutput.iteration = results.dreamOutput.iteration;
    bayesResults.dreamOutput.R_stat = pyArrayFromRatArray2d(results.dreamOutput.R_stat);
    bayesResults.dreamOutput.CR = pyArrayFromRatArray2d(results.dreamOutput.CR);
    bayesResults.dreamOutput.AR = pyArray2dFromBoundedArray<coder::bounded_array<real_T, 2000U, 2U>>(results.dreamOutput.AR);

    bayesResults.dreamParams.nParams = results.dreamParams.nParams;
    bayesResults.dreamParams.nChains = results.dreamParams.nChains;
    bayesResults.dreamParams.nGenerations = results.dreamParams.nGenerations;
    bayesResults.dreamParams.parallel = results.dreamParams.parallel;
    bayesResults.dreamParams.CPU = results.dreamParams.CPU;
    bayesResults.dreamParams.jumpProbability = results.dreamParams.jumpProbability;
    bayesResults.dreamParams.pUnitGamma = results.dreamParams.pUnitGamma;
    bayesResults.dreamParams.nCR = results.dreamParams.nCR;
    bayesResults.dreamParams.delta = results.dreamParams.delta;
    bayesResults.dreamParams.steps = results.dreamParams.steps;
    bayesResults.dreamParams.zeta = results.dreamParams.zeta;
    bayesResults.dreamParams.outlier = std::string(results.dreamParams.outlier, 3);
    bayesResults.dreamParams.adaptPCR = results.dreamParams.adaptPCR;
    bayesResults.dreamParams.thinning = results.dreamParams.thinning;
    bayesResults.dreamParams.epsilon = results.dreamParams.epsilon;
    bayesResults.dreamParams.ABC = results.dreamParams.ABC;
    bayesResults.dreamParams.IO = results.dreamParams.IO;
    bayesResults.dreamParams.storeOutput = results.dreamParams.storeOutput;
    bayesResults.dreamParams.R = pyArrayFromRatArray2d(results.dreamParams.R);

    return bayesResults;
}

const std::string docsRATMain = R"(Entry point for the main reflectivity computation.

Parameters
----------
problem_def : Rat.rat_core.ProblemDefinition
    The project input for the RAT calculation.
control : RATapi.rat_core.Control
    The controls object for the RAT calculation.

Returns
-------
out_problem_def : Rat.rat_core.ProblemDefinition
    The project input with the updated fit values.
results : Rat.rat_core.OutputResult
    The results from a RAT calculation.
bayes_result : Rat.rat_core.OutputBayesResult
    The extra results if RAT calculation is Bayesian.
)";

py::tuple RATMain(const ProblemDefinition& problem_def, const Control& control)
{
    RAT::b_ProblemDefinition problem_def_struct = createProblemDefinitionStruct(problem_def);
    RAT::Controls control_struct = createControlsStruct(control);
    // Output
    RAT::Results results;
    RAT::BayesResults bayesResults;
    // Call the entry-point
    RAT::RATMain(&problem_def_struct, &control_struct, &results, &bayesResults);
    // Copy result to output
    auto out_problem_def = problemDefinitionFromStruct(problem_def_struct);
    out_problem_def.customFiles = problem_def.customFiles.attr("copy")(); 
    return py::make_tuple(out_problem_def, 
                          OutputResultFromStruct(results), 
                          OutputBayesResultsFromStruct(bayesResults));    
}

const std::string docsMakeSLDProfile = R"(Creates the profiles for the SLD plots

Parameters
----------
bulk_in : float
    Bulk in value for contrast.
bulk_out : float
    Bulk out value for contrast.
layers : np.ndarray[np.float]
    Array of parameters for each layer in the contrast.
ssub : float
    Substrate roughness.
number_of_repeats : int, default: 1
    Number of times the layers are repeated.

Returns
-------
sld_profile : np.ndarray[np.float]
    Computed SLD profile
)";

py::array_t<real_T> makeSLDProfile(real_T bulk_in,
                                   real_T bulk_out,
                                   const py::array_t<real_T> &layers,
                                   real_T ssub,
                                   int number_of_repeats=DEFAULT_NREPEATS)
{
    coder::array<real_T, 2U> out;
    coder::array<real_T, 2U> layers_array = pyArrayToRatArray2d(layers);
    RAT::makeSLDProfile(bulk_in,
                        bulk_out,
                        layers_array,
                        ssub,
                        number_of_repeats,
                        out);

    return pyArrayFromRatArray2d(out);
    
}

class Module
{
public:
    Module() 
    {
        RAT::RATMain_initialize();
    }
    ~Module()
    {
        RAT::RATMain_terminate();
    }
};

template <typename... Args>
using overload_cast_ = pybind11::detail::overload_cast_impl<Args...>;

PYBIND11_MODULE(rat_core, m) {
    static Module module;
   
    py::class_<EventBridge>(m, "EventBridge")
        .def(py::init<py::function>())
        .def("register", &EventBridge::registerEvent)
        .def("clear", &EventBridge::clear);

    py::enum_<EventTypes>(m, "EventTypes")
        .value("Message", EventTypes::Message)
        .value("Plot", EventTypes::Plot)
        .value("Progress", EventTypes::Progress);

    py::class_<DylibEngine>(m, "DylibEngine")
        .def(py::init<std::string, std::string>())
        .def("invoke", overload_cast_<std::vector<double>&, std::vector<double>&, 
                                      std::vector<double>&, int, int>()(&DylibEngine::invoke), 
                                      py::arg("params"), py::arg("bulkIn"), 
                                      py::arg("bulkOut"), py::arg("contrast"), 
                                      py::arg("domain") = DEFAULT_DOMAIN)
        .def("invoke", overload_cast_<std::vector<double>&, 
                                      std::vector<double>&>()(&DylibEngine::invoke), py::arg("xdata"), py::arg("param"));

    py::class_<PredictionIntervals>(m, "PredictionIntervals", docsPredictionIntervals.c_str())
        .def(py::init<>())
        .def_readwrite("reflectivity", &PredictionIntervals::reflectivity)
        .def_readwrite("sld", &PredictionIntervals::sld)
        .def_readwrite("sampleChi", &PredictionIntervals::sampleChi);
    
    py::class_<PlotEventData>(m, "PlotEventData", docsPlotEventData.c_str())
        .def(py::init<>())
        .def_readwrite("reflectivity", &PlotEventData::reflectivity)
        .def_readwrite("shiftedData", &PlotEventData::shiftedData)
        .def_readwrite("sldProfiles", &PlotEventData::sldProfiles)
        .def_readwrite("resampledLayers", &PlotEventData::resampledLayers)
        .def_readwrite("subRoughs", &PlotEventData::subRoughs)
        .def_readwrite("resample", &PlotEventData::resample)
        .def_readwrite("dataPresent", &PlotEventData::dataPresent)
        .def_readwrite("modelType", &PlotEventData::modelType)
        .def_readwrite("contrastNames", &PlotEventData::contrastNames)
        .def(py::pickle(
            [](const PlotEventData &evt) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(evt.reflectivity, evt.shiftedData, evt.sldProfiles, evt.resampledLayers, evt.subRoughs, evt.resample, 
                                      evt.dataPresent, evt.modelType, evt.contrastNames);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 9)
                    throw std::runtime_error("Encountered invalid state unpickling PlotEventData object!");

                /* Create a new C++ instance */
                PlotEventData evt;

                evt.reflectivity = t[0].cast<py::list>();
                evt.shiftedData = t[1].cast<py::list>();
                evt.sldProfiles = t[2].cast<py::list>();
                evt.resampledLayers = t[3].cast<py::list>();
                evt.subRoughs = t[4].cast<py::array_t<double>>();
                evt.resample = t[5].cast<py::array_t<double>>();
                evt.dataPresent = t[6].cast<py::array_t<double>>();
                evt.modelType = t[7].cast<std::string>();
                evt.contrastNames = t[8].cast<py::list>();

                return evt;
            }));

    py::class_<ProgressEventData>(m, "ProgressEventData", docsProgressEventData.c_str())
        .def(py::init<>())
        .def_readwrite("message", &ProgressEventData::message)
        .def_readwrite("percent", &ProgressEventData::percent)
        .def(py::pickle(
            [](const ProgressEventData &evt) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(evt.message, evt.percent);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 2)
                    throw std::runtime_error("Encountered invalid state unpickling ProgressEventData object!");

                /* Create a new C++ instance */
                ProgressEventData evt;

                evt.message = t[0].cast<std::string>(); 
                evt.percent = t[1].cast<double>();

                return evt;
            }));

    py::class_<ConfidenceIntervals>(m, "ConfidenceIntervals", docsConfidenceIntervals.c_str())
        .def(py::init<>())
        .def_readwrite("percentile95", &ConfidenceIntervals::percentile95)
        .def_readwrite("percentile65", &ConfidenceIntervals::percentile65)
        .def_readwrite("mean", &ConfidenceIntervals::mean);

    py::class_<DreamParams>(m, "DreamParams")
        .def(py::init<>())
        .def_readwrite("nParams", &DreamParams::nParams)
        .def_readwrite("nChains", &DreamParams::nChains)
        .def_readwrite("nGenerations", &DreamParams::nGenerations)
        .def_readwrite("parallel", &DreamParams::parallel)
        .def_readwrite("CPU", &DreamParams::CPU)
        .def_readwrite("jumpProbability", &DreamParams::jumpProbability)
        .def_readwrite("pUnitGamma", &DreamParams::pUnitGamma)
        .def_readwrite("nCR", &DreamParams::nCR)
        .def_readwrite("delta", &DreamParams::delta)
        .def_readwrite("steps", &DreamParams::steps)
        .def_readwrite("zeta", &DreamParams::zeta)
        .def_readwrite("outlier", &DreamParams::outlier)
        .def_readwrite("adaptPCR", &DreamParams::adaptPCR)
        .def_readwrite("thinning", &DreamParams::thinning)
        .def_readwrite("epsilon", &DreamParams::epsilon)
        .def_readwrite("ABC", &DreamParams::ABC)
        .def_readwrite("IO", &DreamParams::IO)
        .def_readwrite("storeOutput", &DreamParams::storeOutput)
        .def_readwrite("R", &DreamParams::R);

    py::class_<NestedSamplerOutput>(m, "NestedSamplerOutput", docsNestedSamplerOutput.c_str())
        .def(py::init<>())
        .def_readwrite("logZ", &NestedSamplerOutput::logZ)
        .def_readwrite("logZErr", &NestedSamplerOutput::logZErr)
        .def_readwrite("nestSamples", &NestedSamplerOutput::nestSamples)
        .def_readwrite("postSamples", &NestedSamplerOutput::postSamples);

    py::class_<DreamOutput>(m, "DreamOutput", docsDreamOutput.c_str())
        .def(py::init<>())
        .def_readwrite("allChains", &DreamOutput::allChains)
        .def_readwrite("outlierChains", &DreamOutput::outlierChains)
        .def_readwrite("runtime", &DreamOutput::runtime)
        .def_readwrite("iteration", &DreamOutput::iteration)
        .def_readwrite("AR", &DreamOutput::AR)
        .def_readwrite("R_stat", &DreamOutput::R_stat)
        .def_readwrite("CR", &DreamOutput::CR);

    py::class_<OutputBayesResult>(m, "OutputBayesResult", docsOutputBayesResult.c_str())
        .def(py::init<>())
        .def_readwrite("predictionIntervals", &OutputBayesResult::predictionIntervals)
        .def_readwrite("confidenceIntervals", &OutputBayesResult::confidenceIntervals)
        .def_readwrite("dreamParams", &OutputBayesResult::dreamParams)
        .def_readwrite("dreamOutput", &OutputBayesResult::dreamOutput)
        .def_readwrite("nestedSamplerOutput", &OutputBayesResult::nestedSamplerOutput)
        .def_readwrite("chain", &OutputBayesResult::chain);

    py::class_<Calculation>(m, "Calculation", docsCalculation.c_str())
        .def(py::init<>())
        .def_readwrite("chiValues", &Calculation::chiValues)
        .def_readwrite("sumChi", &Calculation::sumChi);

    py::class_<ContrastParams>(m, "ContrastParams", docsContrastParams.c_str())
        .def(py::init<>())
        .def_readwrite("scalefactors", &ContrastParams::scalefactors)
        .def_readwrite("bulkIn", &ContrastParams::bulkIn)
        .def_readwrite("bulkOut", &ContrastParams::bulkOut)
        .def_readwrite("subRoughs", &ContrastParams::subRoughs)
        .def_readwrite("resample", &ContrastParams::resample);
    
    py::class_<OutputResult>(m, "OutputResult", docsOutputResult.c_str())
        .def(py::init<>())
        .def_readwrite("reflectivity", &OutputResult::reflectivity)
        .def_readwrite("simulation", &OutputResult::simulation)
        .def_readwrite("shiftedData", &OutputResult::shiftedData)
        .def_readwrite("backgrounds", &OutputResult::backgrounds)
        .def_readwrite("resolutions", &OutputResult::resolutions)
        .def_readwrite("sldProfiles", &OutputResult::sldProfiles)
        .def_readwrite("layers", &OutputResult::layers)
        .def_readwrite("resampledLayers", &OutputResult::resampledLayers)
        .def_readwrite("calculationResults", &OutputResult::calculationResults)
        .def_readwrite("contrastParams", &OutputResult::contrastParams)        
        .def_readwrite("fitParams", &OutputResult::fitParams)
        .def_readwrite("fitNames", &OutputResult::fitNames);

    py::class_<NameStore>(m, "NameStore", docsNameStore.c_str())
        .def(py::init<>())
        .def_readwrite("params", &NameStore::params)
        .def_readwrite("backgroundParams", &NameStore::backgroundParams)
        .def_readwrite("scalefactors", &NameStore::scalefactors)
        .def_readwrite("bulkIns", &NameStore::bulkIns)
        .def_readwrite("bulkOuts", &NameStore::bulkOuts)
        .def_readwrite("resolutionParams", &NameStore::resolutionParams)
        .def_readwrite("domainRatios", &NameStore::domainRatios)
        .def_readwrite("contrasts", &NameStore::contrasts)
        .def(py::pickle(
            [](const NameStore &names) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(names.params, names.backgroundParams, names.scalefactors, names.bulkIns, names.bulkOuts, names.resolutionParams, 
                                      names.domainRatios, names.contrasts);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 8)
                    throw std::runtime_error("Encountered invalid state unpickling NameStore object!");

                /* Create a new C++ instance */
                NameStore names;

                names.params = t[0].cast<py::list>(); 
                names.backgroundParams = t[1].cast<py::list>(); 
                names.scalefactors = t[2].cast<py::list>();
                names.bulkIns = t[3].cast<py::list>();
                names.bulkOuts = t[4].cast<py::list>();
                names.resolutionParams = t[5].cast<py::list>();
                names.domainRatios = t[6].cast<py::list>();
                names.contrasts = t[7].cast<py::list>();

                return names;
            }));

    py::class_<Checks>(m, "Checks", docsChecks.c_str())
        .def(py::init<>())
        .def_readwrite("params", &Checks::params)
        .def_readwrite("backgroundParams", &Checks::backgroundParams)
        .def_readwrite("scalefactors", &Checks::scalefactors)
        .def_readwrite("bulkIns", &Checks::bulkIns)
        .def_readwrite("bulkOuts", &Checks::bulkOuts)
        .def_readwrite("resolutionParams", &Checks::resolutionParams)
        .def_readwrite("domainRatios", &Checks::domainRatios)
        .def(py::pickle(
            [](const Checks &chk) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(chk.params, chk.backgroundParams, chk.scalefactors, chk.bulkIns, chk.bulkOuts, 
                                      chk.resolutionParams, chk.domainRatios);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 7)
                    throw std::runtime_error("Encountered invalid state unpickling Checks object!");

                /* Create a new C++ instance */
                Checks chk;

                chk.params = t[0].cast<py::array_t<real_T>>(); 
                chk.backgroundParams = t[1].cast<py::array_t<real_T>>(); 
                chk.scalefactors = t[2].cast<py::array_t<real_T>>(); 
                chk.bulkIns = t[3].cast<py::array_t<real_T>>(); 
                chk.bulkOuts = t[4].cast<py::array_t<real_T>>(); 
                chk.resolutionParams = t[5].cast<py::array_t<real_T>>(); 
                chk.domainRatios = t[6].cast<py::array_t<real_T>>();

                return chk;
            }));

    py::class_<Control>(m, "Control", docsControl.c_str())
        .def(py::init<>())
        .def_readwrite("parallel", &Control::parallel)
        .def_readwrite("procedure", &Control::procedure)
        .def_readwrite("display", &Control::display)
        .def_readwrite("xTolerance", &Control::xTolerance)
        .def_readwrite("funcTolerance", &Control::funcTolerance)
        .def_readwrite("maxFuncEvals", &Control::maxFuncEvals)
        .def_readwrite("maxIterations", &Control::maxIterations)
        .def_readwrite("populationSize", &Control::populationSize)
        .def_readwrite("fWeight", &Control::fWeight)  
        .def_readwrite("crossoverProbability", &Control::crossoverProbability)  
        .def_readwrite("targetValue", &Control::targetValue)
        .def_readwrite("numGenerations", &Control::numGenerations)
        .def_readwrite("strategy", &Control::strategy)
        .def_readwrite("nLive", &Control::nLive)
        .def_readwrite("nMCMC", &Control::nMCMC)
        .def_readwrite("propScale", &Control::propScale)
        .def_readwrite("nsTolerance", &Control::nsTolerance)
        .def_readwrite("numSimulationPoints", &Control::numSimulationPoints)
        .def_readwrite("resampleMinAngle", &Control::resampleMinAngle)
        .def_readwrite("resampleNPoints", &Control::resampleNPoints)
        .def_readwrite("updateFreq", &Control::updateFreq)
        .def_readwrite("updatePlotFreq", &Control::updatePlotFreq)
        .def_readwrite("nSamples", &Control::nSamples)
        .def_readwrite("nChains", &Control::nChains)
        .def_readwrite("jumpProbability", &Control::jumpProbability)
        .def_readwrite("pUnitGamma", &Control::pUnitGamma)
        .def_readwrite("boundHandling", &Control::boundHandling)
        .def_readwrite("adaptPCR", &Control::adaptPCR)
        .def_readwrite("IPCFilePath", &Control::IPCFilePath)
        .def(py::pickle(
            [](const Control &ctrl) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(ctrl.parallel, ctrl.procedure, ctrl.display, ctrl.xTolerance, ctrl.funcTolerance, 
                                      ctrl.maxFuncEvals, ctrl.maxIterations, ctrl.populationSize, ctrl.fWeight, ctrl.crossoverProbability, 
                                      ctrl.targetValue, ctrl.numGenerations, ctrl.strategy, ctrl.nLive, ctrl.nMCMC, ctrl.propScale, 
                                      ctrl.nsTolerance, ctrl.numSimulationPoints, ctrl.resampleMinAngle, ctrl.resampleNPoints,
                                      ctrl.updateFreq, ctrl.updatePlotFreq, ctrl.nSamples, ctrl.nChains, ctrl.jumpProbability,
                                      ctrl.pUnitGamma, ctrl.boundHandling, ctrl.adaptPCR, ctrl.IPCFilePath);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 29)
                    throw std::runtime_error("Encountered invalid state unpickling ProblemDefinition object!");

                /* Create a new C++ instance */
                Control ctrl;
                
                ctrl.parallel = t[0].cast<std::string>();
                ctrl.procedure  = t[1].cast<std::string>();
                ctrl.display = t[2].cast<std::string>();
                ctrl.xTolerance = t[3].cast<real_T>();
                ctrl.funcTolerance = t[4].cast<real_T>();
                ctrl.maxFuncEvals = t[5].cast<real_T>();
                ctrl.maxIterations = t[6].cast<real_T>();
                ctrl.populationSize = t[7].cast<real_T>();
                ctrl.fWeight = t[8].cast<real_T>();
                ctrl.crossoverProbability = t[9].cast<real_T>();
                ctrl.targetValue = t[10].cast<real_T>();
                ctrl.numGenerations = t[11].cast<real_T>();
                ctrl.strategy = t[12].cast<real_T>();
                ctrl.nLive = t[13].cast<real_T>();
                ctrl.nMCMC = t[14].cast<real_T>();
                ctrl.propScale = t[15].cast<real_T>();
                ctrl.nsTolerance = t[16].cast<real_T>();
                ctrl.numSimulationPoints = t[17].cast<real_T>();
                ctrl.resampleMinAngle = t[18].cast<real_T>();
                ctrl.resampleNPoints = t[19].cast<real_T>();
                ctrl.updateFreq = t[20].cast<real_T>();
                ctrl.updatePlotFreq = t[21].cast<real_T>();
                ctrl.nSamples = t[22].cast<real_T>();
                ctrl.nChains = t[23].cast<real_T>();
                ctrl.jumpProbability = t[24].cast<real_T>();
                ctrl.pUnitGamma = t[25].cast<real_T>();
                ctrl.boundHandling = t[26].cast<std::string>();
                ctrl.adaptPCR = t[27].cast<boolean_T>();
                ctrl.IPCFilePath = t[28].cast<std::string>();

                return ctrl;
            }));

    py::class_<ProblemDefinition>(m, "ProblemDefinition", docsProblemDefinition.c_str())
        .def(py::init<>())
        .def_readwrite("TF", &ProblemDefinition::TF)
        .def_readwrite("resample", &ProblemDefinition::resample)
        .def_readwrite("data", &ProblemDefinition::data)
        .def_readwrite("dataPresent", &ProblemDefinition::dataPresent)
        .def_readwrite("dataLimits", &ProblemDefinition::dataLimits)
        .def_readwrite("simulationLimits", &ProblemDefinition::simulationLimits)
        .def_readwrite("numberOfContrasts", &ProblemDefinition::numberOfContrasts)
        .def_readwrite("geometry", &ProblemDefinition::geometry)
        .def_readwrite("useImaginary", &ProblemDefinition::useImaginary)
        .def_readwrite("repeatLayers", &ProblemDefinition::repeatLayers)
        .def_readwrite("contrastBackgroundParams", &ProblemDefinition::contrastBackgroundParams)
        .def_readwrite("contrastBackgroundTypes", &ProblemDefinition::contrastBackgroundTypes)
        .def_readwrite("contrastBackgroundActions", &ProblemDefinition::contrastBackgroundActions)
        .def_readwrite("contrastScalefactors", &ProblemDefinition::contrastScalefactors)
        .def_readwrite("contrastBulkIns", &ProblemDefinition::contrastBulkIns)
        .def_readwrite("contrastBulkOuts", &ProblemDefinition::contrastBulkOuts)
        .def_readwrite("contrastResolutionParams", &ProblemDefinition::contrastResolutionParams)
        .def_readwrite("contrastResolutionTypes", &ProblemDefinition::contrastResolutionTypes)
        .def_readwrite("backgroundParams", &ProblemDefinition::backgroundParams)
        .def_readwrite("scalefactors", &ProblemDefinition::scalefactors)
        .def_readwrite("bulkIns", &ProblemDefinition::bulkIns)
        .def_readwrite("bulkOuts", &ProblemDefinition::bulkOuts)
        .def_readwrite("resolutionParams", &ProblemDefinition::resolutionParams)
        .def_readwrite("params", &ProblemDefinition::params)
        .def_readwrite("numberOfLayers", &ProblemDefinition::numberOfLayers)
        .def_readwrite("contrastLayers", &ProblemDefinition::contrastLayers)
        .def_readwrite("layersDetails", &ProblemDefinition::layersDetails)
        .def_readwrite("customFiles", &ProblemDefinition::customFiles)
        .def_readwrite("modelType", &ProblemDefinition::modelType)
        .def_readwrite("contrastCustomFiles", &ProblemDefinition::contrastCustomFiles)
        .def_readwrite("contrastDomainRatios", &ProblemDefinition::contrastDomainRatios)
        .def_readwrite("domainRatios", &ProblemDefinition::domainRatios)
        .def_readwrite("numberOfDomainContrasts", &ProblemDefinition::numberOfDomainContrasts)
        .def_readwrite("domainContrastLayers", &ProblemDefinition::domainContrastLayers)
        .def_readwrite("fitParams", &ProblemDefinition::fitParams)
        .def_readwrite("fitLimits", &ProblemDefinition::fitLimits)
        .def_readwrite("priorNames", &ProblemDefinition::priorNames)
        .def_readwrite("priorValues", &ProblemDefinition::priorValues)
        .def_readwrite("names", &ProblemDefinition::names)
        .def_readwrite("checks", &ProblemDefinition::checks)
        .def(py::pickle(
            [](const ProblemDefinition &p) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(p.TF, p.resample, p.data, p.dataPresent, p.dataLimits, p.simulationLimits,
                                      p.numberOfContrasts, p.geometry, p.useImaginary, p.repeatLayers, 
                                      p.contrastBackgroundParams, p.contrastBackgroundTypes, p.contrastBackgroundActions,  
                                      p.contrastScalefactors, p.contrastBulkIns, p.contrastBulkOuts, p.contrastResolutionParams,
                                      p.contrastResolutionTypes, p.backgroundParams, p.scalefactors, p.bulkIns, p.bulkOuts,
                                      p.resolutionParams, p.params, p.numberOfLayers, p.contrastLayers, p.layersDetails, 
                                      p.customFiles, p.modelType, p.contrastCustomFiles, p.contrastDomainRatios, p.domainRatios, 
                                      p.numberOfDomainContrasts, p.domainContrastLayers, p.fitParams, p.fitLimits, p.priorNames, p.priorValues,
                                      p.names.params, p.names.backgroundParams, p.names.scalefactors, p.names.bulkIns,
                                      p.names.bulkOuts, p.names.resolutionParams, p.names.domainRatios, p.names.contrasts,
                                      p.checks.params, p.checks.backgroundParams, p.checks.scalefactors,
                                      p.checks.bulkIns, p.checks.bulkOuts, p.checks.resolutionParams, p.checks.domainRatios);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 53)
                    throw std::runtime_error("Encountered invalid state unpickling ProblemDefinition object!");

                /* Create a new C++ instance */
                ProblemDefinition p;

                p.TF = t[0].cast<std::string>(); 
                p.resample = t[1].cast<py::array_t<real_T>>(); 
                p.data = t[2].cast<py::list>(); 
                p.dataPresent = t[3].cast<py::array_t<real_T>>(); 
                p.dataLimits = t[4].cast<py::list>(); 
                p.simulationLimits = t[5].cast<py::list>(); 
                p.numberOfContrasts = t[6].cast<real_T>();
                p.geometry = t[7].cast<std::string>(); 
                p.useImaginary = t[8].cast<bool>();
                p.repeatLayers = t[9].cast<py::array_t<real_T>>();
                p.contrastBackgroundParams = t[10].cast<py::list>(); 
                p.contrastBackgroundTypes = t[11].cast<py::list>(); 
                p.contrastBackgroundActions = t[12].cast<py::list>(); 
                p.contrastScalefactors = t[13].cast<py::array_t<real_T>>();
                p.contrastBulkIns = t[14].cast<py::array_t<real_T>>();
                p.contrastBulkOuts = t[15].cast<py::array_t<real_T>>();
                p.contrastResolutionParams = t[16].cast<py::list>(); 
                p.contrastResolutionTypes = t[17].cast<py::list>(); 
                p.backgroundParams = t[18].cast<py::array_t<real_T>>();
                p.scalefactors = t[19].cast<py::array_t<real_T>>();
                p.bulkIns = t[20].cast<py::array_t<real_T>>();
                p.bulkOuts = t[21].cast<py::array_t<real_T>>();
                p.resolutionParams = t[22].cast<py::array_t<real_T>>();
                p.params = t[23].cast<py::array_t<real_T>>();
                p.numberOfLayers = t[24].cast<real_T>();
                p.contrastLayers = t[25].cast<py::list>(); 
                p.layersDetails = t[26].cast<py::list>(); 
                p.customFiles = t[27].cast<py::object>(); 
                p.modelType = t[28].cast<std::string>(); 
                p.contrastCustomFiles = t[29].cast<py::array_t<real_T>>(); 
                p.contrastDomainRatios = t[30].cast<py::array_t<real_T>>();
                p.domainRatios = t[31].cast<py::array_t<real_T>>();
                p.numberOfDomainContrasts = t[32].cast<real_T>();
                p.domainContrastLayers = t[33].cast<py::list>();
                p.fitParams = t[34].cast<py::array_t<real_T>>();
                p.fitLimits = t[35].cast<py::array_t<real_T>>();
                p.priorNames = t[36].cast<py::list>();
                p.priorValues = t[37].cast<py::array_t<real_T>>();
                
                p.names.params = t[38].cast<py::list>(); 
                p.names.backgroundParams = t[39].cast<py::list>(); 
                p.names.scalefactors = t[40].cast<py::list>();
                p.names.bulkIns = t[41].cast<py::list>();
                p.names.bulkOuts = t[42].cast<py::list>();
                p.names.resolutionParams = t[43].cast<py::list>();
                p.names.domainRatios = t[44].cast<py::list>();
                p.names.contrasts = t[45].cast<py::list>(); 

                p.checks.params = t[46].cast<py::array_t<real_T>>(); 
                p.checks.backgroundParams = t[47].cast<py::array_t<real_T>>(); 
                p.checks.scalefactors = t[48].cast<py::array_t<real_T>>();
                p.checks.bulkIns = t[49].cast<py::array_t<real_T>>(); 
                p.checks.bulkOuts = t[50].cast<py::array_t<real_T>>(); 
                p.checks.resolutionParams = t[51].cast<py::array_t<real_T>>(); 
                p.checks.domainRatios = t[52].cast<py::array_t<real_T>>();

                return p;
            }));

    m.def("RATMain", &RATMain, docsRATMain.c_str(), py::arg("problem_def"), py::arg("control"));

    m.def("makeSLDProfile", &makeSLDProfile, docsMakeSLDProfile.c_str(), 
          py::arg("bulk_in"), py::arg("bulk_out"), py::arg("layers"), py::arg("ssub"), py::arg("number_of_repeats") = DEFAULT_NREPEATS);
}
