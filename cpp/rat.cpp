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
#include "RAT/makeSLDProfileXY.h"
#include "RAT/classHandle.hpp"
#include "RAT/dylib.hpp"
#include "RAT/events/eventManager.h"

namespace py = pybind11;

const int DEFAULT_DOMAIN = -1;

template <typename Function, typename ... Args> 
auto customCaller(std::string identifier, Function f, Args&& ... args) -> decltype((*f)(std::forward<Args>(args)...))
{
    try 
    {
        return (*f)(std::forward<Args>(args)...);
    }
    catch(const std::runtime_error& re) 
    {
        std::string errorMsg;
        size_t start_pos = std::string(re.what()).find("$id");
        if(start_pos == std::string::npos)
        {
            errorMsg = std::string("Error occurred when setting ") + identifier + ". " + re.what();
        }            
        else
        {
            errorMsg = re.what();
            errorMsg.replace(start_pos, 3, identifier);
        }    
            
        throw std::runtime_error(errorMsg);
    }
}

class Library: public CallbackInterface
{
    public:

    py::function function;

    Library(const py::function function){
        this->function = function;
    };


    void setOutput(py::tuple& result, std::vector<double>& output, double *outputSize)
    {
        int nRows = 0, idx = 0;
        for (py::handle rowHandle : result[0])
        {
            py::list rows = py::cast<py::list>(rowHandle); 
            for (py::handle value : rows)
            {
                output.push_back(py::cast<double>(value));
                idx++;
            }
            nRows++;
        }

        outputSize[0] = nRows;
        outputSize[1] = (nRows == 0) ? 0 : idx / nRows;
    }

    // Domain overload
    void invoke(std::vector<double>& params, std::vector<double>& bulkIn, std::vector<double>& bulkOut, 
                        int contrast, int domainNumber, std::vector<double>& output, double *outputSize, double *roughness)
    {
        auto f = py::cast<std::function<py::tuple(py::list, py::list, py::list, int, int)>>(this->function);
        auto result = f(py::cast(params), py::cast(bulkIn), py::cast(bulkOut), contrast, domainNumber);
        *roughness = py::cast<double>(result[1]);
        setOutput(result, output, outputSize);
    };
    
    // Non-Domain overload
    void invoke(std::vector<double>& params, std::vector<double>& bulkIn, std::vector<double>& bulkOut, 
                        int contrast, std::vector<double>& output, double *outputSize, double *roughness)
    {
        auto f = py::cast<std::function<py::tuple(py::list, py::list, py::list, int)>>(this->function);
        auto result = f(py::cast(params), py::cast(bulkIn), py::cast(bulkOut), contrast);
        *roughness = py::cast<double>(result[1]);
        setOutput(result, output, outputSize);
    };
};

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

struct ProgressEventData
{
    std::string message;
    double percent;
};

struct PlotEventData
{
    py::list reflectivity;
    py::list shiftedData;
    py::list sldProfiles;
    py::list resampledLayers;
    py::array_t<double> subRoughs;
    py::array_t<double> resample;
    py::array_t<double> dataPresent;
    std::string modelType;
    py::list contrastNames;
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

struct PredictionIntervals
{
    py::list reflectivity;
    py::list sld;
    py::list reflectivityXData;
    py::list sldXData;
    py::array_t<real_T> sampleChi; 
};

struct ConfidenceIntervals
{
    py::array_t<real_T> percentile95;
    py::array_t<real_T> percentile65;
    py::array_t<real_T> mean;
};

struct NestedSamplerOutput
{
    real_T logZ;
    py::array_t<real_T> nestSamples;
    py::array_t<real_T> postSamples;
};

struct DreamParams
{
    real_T nParams;
    real_T nChains;
    real_T nGenerations;
    boolean_T parallel;
    real_T CPU;
    real_T jumpProbability;
    real_T pUnitGamma;
    real_T nCR;
    real_T delta;
    real_T steps;
    real_T zeta;
    std::string outlier;
    boolean_T adaptPCR;
    real_T thinning;
    real_T epsilon;
    boolean_T ABC;
    boolean_T IO;
    boolean_T storeOutput;
    py::array_t<real_T> R;
};

struct DreamOutput
{
    py::array_t<real_T> allChains;
    py::array_t<real_T> outlierChains;
    real_T runtime;
    real_T iteration;
    real_T modelOutput;
    py::array_t<real_T> AR;
    py::array_t<real_T> R_stat;
    py::array_t<real_T> CR;
};

struct BayesResults
{
    PredictionIntervals predictionIntervals;
    ConfidenceIntervals confidenceIntervals;
    DreamParams dreamParams;
    DreamOutput dreamOutput;
    NestedSamplerOutput nestedSamplerOutput;
    py::array_t<real_T> chain;
};

struct Priors
{
    py::list param;
    py::list backgroundParam;
    py::list resolutionParam;
    py::list bulkIn;
    py::list bulkOut;
    py::list qzshift;
    py::list scalefactor;
    py::list domainRatio;
    py::list priorNames;
    py::array_t<real_T> priorValues;
};

struct Checks {
    py::array_t<real_T> fitParam;
    py::array_t<real_T> fitBackgroundParam;
    py::array_t<real_T> fitQzshift;
    py::array_t<real_T> fitScalefactor;
    py::array_t<real_T> fitBulkIn;
    py::array_t<real_T> fitBulkOut;
    py::array_t<real_T> fitResolutionParam;
    py::array_t<real_T> fitDomainRatio;
};

struct Calculation
{
    py::array_t<real_T> chiValues;
    real_T sumChi;
};

struct ContrastParams
{
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIn;
    py::array_t<real_T> bulkOut;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> subRoughs;
    py::array_t<real_T> resample;
};

struct OutputResult {
    py::list reflectivity;
    py::list simulation;
    py::list shiftedData;
    py::list layerSlds;
    py::list sldProfiles;
    py::list resampledLayers;
    Calculation calculationResults {};
    ContrastParams contrastParams;
    py::array_t<real_T> fitParams;
    py::list fitNames;
};

struct Limits {
    py::array_t<real_T> param;
    py::array_t<real_T> backgroundParam;
    py::array_t<real_T> scalefactor;
    py::array_t<real_T> qzshift;
    py::array_t<real_T> bulkIn;
    py::array_t<real_T> bulkOut;
    py::array_t<real_T> resolutionParam;
    py::array_t<real_T> domainRatio;
};

struct Cells {
    py::list f1;
    py::list f2;
    py::list f3;
    py::list f4;
    py::list f5;
    py::list f6;
    py::list f7;
    py::list f8;
    py::list f9;
    py::list f10;
    py::list f11;
    py::list f12;
    py::list f13;
    py::object f14;
    py::list f15;
    py::list f16;
    py::list f17;
    py::list f18;
    py::list f19;
    py::list f20;
    py::list f21;
};

struct ProblemDefinition {
    py::array_t<real_T> contrastBackgroundParams;
    py::array_t<real_T> contrastBackgroundActions;
    std::string TF {};
    py::array_t<real_T> resample;
    py::array_t<real_T> dataPresent;
    py::array_t<real_T> oilChiDataPresent;
    real_T numberOfContrasts;
    std::string geometry {};
    bool useImaginary {};
    py::array_t<real_T> contrastQzshifts;
    py::array_t<real_T> contrastScalefactors;
    py::array_t<real_T> contrastBulkIns;
    py::array_t<real_T> contrastBulkOuts;
    py::array_t<real_T> contrastResolutionParams;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> qzshifts;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIn;
    py::array_t<real_T> bulkOut;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> params;
    real_T numberOfLayers {};
    std::string modelType {};
    py::array_t<real_T> contrastCustomFiles;
    py::array_t<real_T> contrastDomainRatios;
    py::array_t<real_T> domainRatio;
    real_T numberOfDomainContrasts {};
    py::array_t<real_T> fitParams;
    py::array_t<real_T> otherParams;
    py::array_t<real_T> fitLimits;
    py::array_t<real_T> otherLimits;
};

struct Control {
    std::string parallel {};
    std::string procedure {};
    std::string display {};
    real_T xTolerance {};
    real_T funcTolerance {};
    real_T maxFuncEvals {};
    real_T maxIterations {};
    real_T populationSize {};
    real_T fWeight {};
    real_T crossoverProbability {};
    real_T targetValue {};
    real_T numGenerations {};
    real_T strategy {};
    real_T nLive {};
    real_T nMCMC {};
    real_T propScale {};
    real_T nsTolerance {};
    boolean_T calcSldDuringFit {};
    real_T resampleMinAngle {};
    real_T resampleNPoints {};
    real_T updateFreq {};
    real_T updatePlotFreq {};
    real_T nSamples {};
    real_T nChains {};
    real_T jumpProbability {};
    real_T pUnitGamma {};
    std::string boundHandling {};
    boolean_T adaptPCR;
    Checks checks {};
    std::string IPCFilePath {};
};


void stringToRatArray(std::string value, char_T result_data[], int32_T result_size[2])
{
    result_size[0] = 1;
    result_size[1] = value.length();

    for (int32_T idx1{0}; idx1 < value.length(); idx1++) {
        result_data[idx1] = value[idx1];
    }
}

void stringToRatCharArray(std::string value, coder::array<char_T, 2U>& result)
{
    result.set_size(1, value.length());

    for (int32_T idx{0}; idx < value.length(); idx++) {
        result[idx] = value[idx];
    }
}

coder::array<real_T, 1U> pyArrayToRatRowArray1d(py::array_t<real_T> value)
{
    coder::array<real_T, 1U> result;

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Expects a 1D numeric array");

    result.set_size(buffer_info.shape[0]);
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result[idx0] = value.at(idx0);
    }

    return result;
}

coder::array<real_T, 2U> pyArrayToRatColArray1d(py::array_t<real_T> value)
{
    coder::array<real_T, 2U> result;

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Expects a 1D numeric array");

    result.set_size(1, buffer_info.shape[0]);
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result[idx0] = value.at(idx0);
    }

    return result;
}

coder::bounded_array<real_T, 10U, 2U> pyArrayToRatBoundedArray(py::array_t<real_T> value)
{
    coder::bounded_array<real_T, 10U, 2U> result {};

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Expects a 1D numeric array");
    
    result.size[0] = 1;
    result.size[1] = buffer_info.shape[0];
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result.data[idx0] = value.at(idx0);
    }

    return result;
}

coder::array<real_T, 2U> pyArrayToRatArray2d(py::array_t<real_T> value)
{
    coder::array<real_T, 2U> result;

    py::buffer_info buffer_info = value.request();

    if (buffer_info.size == 0)
        return result;

    if (buffer_info.ndim != 2)
        throw std::runtime_error("Expects a 2D numeric array");
    
    result.set_size(buffer_info.shape[0], buffer_info.shape[1]);
    
    int32_T idx {0};
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        for (int32_T idx1{0}; idx1 < buffer_info.shape[1]; idx1++) {
            idx  = idx0 + result.size(0) * idx1;  
            result[idx] = value.at(idx0, idx1);
        }
    }

    return result;
}

coder::array<RAT::cell_0, 1U> pyListToUnboundedCell0(py::list values)
{
    coder::array<RAT::cell_0, 1U> result;
    result.set_size(values.size());
    int32_T idx {0};
    for (py::handle list: values)
    { 
        py::list value = py::cast<py::list>(list);
        if (py::len(list) != 4 || !py::isinstance<py::str>(value[0]) || !py::isinstance<py::str>(value[1]) ||
                !py::isinstance<py::float_>(value[2]) || !py::isinstance<py::float_>(value[3]))
            throw std::runtime_error("Expects a 2D list where each row must contain 4 elements. " 
                                     "Columns 1 and 2 must be strings and Columns 3 and 4 must be numeric arrays");

        stringToRatArray(value[0].cast<std::string>(), result[idx].f1.data, result[idx].f1.size);
        stringToRatArray(value[1].cast<std::string>(), result[idx].f2.data, result[idx].f2.size);
        result[idx].f3 = value[2].cast<real_T>();
        result[idx].f4 = value[3].cast<real_T>();
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_1, 1U> pyListToUnboundedCell1(py::list values)
{
    coder::array<RAT::cell_wrap_1, 1U> result;
    result.set_size(values.size());
    int32_T idx {0};
    for (py::handle list: values)
    { 
        if (py::isinstance<py::str>(list)) {
            std::string value = py::cast<std::string>(list);
            stringToRatCharArray(value, result[idx].f1); 
            idx++;
        }
        else
            throw std::runtime_error("Expects a 1D list of strings");
    }

    return result;
}

RAT::struct0_T createStruct0(const ProblemDefinition& problem)
{
    RAT::struct0_T problem_struct;
    
    problem_struct.useImaginary = problem.useImaginary;
    
    problem_struct.numberOfLayers = problem.numberOfLayers;
    problem_struct.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_struct.numberOfContrasts = problem.numberOfContrasts;

    stringToRatArray(problem.modelType, problem_struct.modelType.data, problem_struct.modelType.size);
    stringToRatArray(problem.geometry, problem_struct.geometry.data, problem_struct.geometry.size);
    stringToRatArray(problem.TF, problem_struct.TF.data, problem_struct.TF.size);
    
    problem_struct.contrastBackgroundParams = customCaller("Problem.contrastBackgroundParams", pyArrayToRatColArray1d, problem.contrastBackgroundParams);
    problem_struct.contrastBackgroundActions = customCaller("Problem.contrastBackgroundActions", pyArrayToRatColArray1d, problem.contrastBackgroundActions);
    problem_struct.resample = customCaller("Problem.resample", pyArrayToRatColArray1d, problem.resample);
    problem_struct.dataPresent = customCaller("Problem.dataPresent", pyArrayToRatColArray1d, problem.dataPresent);
    problem_struct.oilChiDataPresent = customCaller("Problem.oilChiDataPresent", pyArrayToRatColArray1d, problem.oilChiDataPresent);
    problem_struct.contrastQzshifts = customCaller("Problem.contrastQzshifts", pyArrayToRatColArray1d, problem.contrastQzshifts);
    problem_struct.contrastScalefactors = customCaller("Problem.contrastScalefactors", pyArrayToRatColArray1d, problem.contrastScalefactors);
    problem_struct.contrastBulkIns = customCaller("Problem.contrastBulkIns", pyArrayToRatColArray1d, problem.contrastBulkIns);
    problem_struct.contrastBulkOuts = customCaller("Problem.contrastBulkOuts", pyArrayToRatColArray1d, problem.contrastBulkOuts);
    problem_struct.contrastResolutionParams = customCaller("Problem.contrastResolutionParams", pyArrayToRatColArray1d, problem.contrastResolutionParams);
    problem_struct.backgroundParams = customCaller("Problem.backgroundParams", pyArrayToRatColArray1d, problem.backgroundParams);
    problem_struct.qzshifts = customCaller("Problem.qzshifts", pyArrayToRatColArray1d, problem.qzshifts);
    problem_struct.scalefactors = customCaller("Problem.scalefactors", pyArrayToRatColArray1d, problem.scalefactors);
    problem_struct.bulkIn = customCaller("Problem.bulkIn", pyArrayToRatColArray1d, problem.bulkIn);
    problem_struct.bulkOut = customCaller("Problem.bulkOut", pyArrayToRatColArray1d, problem.bulkOut);
    problem_struct.resolutionParams = customCaller("Problem.resolutionParams", pyArrayToRatColArray1d, problem.resolutionParams);
    problem_struct.params = customCaller("Problem.params", pyArrayToRatColArray1d, problem.params);

    problem_struct.contrastCustomFiles = customCaller("Problem.contrastCustomFiles", pyArrayToRatColArray1d, problem.contrastCustomFiles);
    problem_struct.contrastDomainRatios = customCaller("Problem.contrastDomainRatios", pyArrayToRatColArray1d, problem.contrastDomainRatios);
    problem_struct.domainRatio = customCaller("Problem.domainRatio", pyArrayToRatColArray1d, problem.domainRatio);

    problem_struct.fitParams =  customCaller("Problem.fitParams", pyArrayToRatRowArray1d, problem.fitParams);
    problem_struct.otherParams =  customCaller("Problem.otherParams", pyArrayToRatRowArray1d, problem.otherParams);
    problem_struct.fitLimits =  customCaller("Problem.fitLimits", pyArrayToRatArray2d, problem.fitLimits);
    problem_struct.otherLimits =  customCaller("Problem.otherLimits", pyArrayToRatArray2d, problem.otherLimits);

    return problem_struct;
}

RAT::struct1_T createStruct1(const Limits& limits)
{
    RAT::struct1_T limits_struct;
    limits_struct.param = customCaller("Limits.param", pyArrayToRatArray2d, limits.param);
    limits_struct.backgroundParam = customCaller("Limits.backgroundParam", pyArrayToRatArray2d, limits.backgroundParam);
    limits_struct.qzshift = customCaller("Limits.qzshift", pyArrayToRatArray2d, limits.qzshift);
    limits_struct.scalefactor = customCaller("Limits.scalefactor", pyArrayToRatArray2d, limits.scalefactor);
    limits_struct.bulkIn = customCaller("Limits.bulkIn", pyArrayToRatArray2d, limits.bulkIn);
    limits_struct.bulkOut = customCaller("Limits.bulkOut", pyArrayToRatArray2d, limits.bulkOut);
    limits_struct.resolutionParam = customCaller("Limits.resolutionParam", pyArrayToRatArray2d, limits.resolutionParam);
    limits_struct.domainRatio = customCaller("Limits.domainRatio", pyArrayToRatArray2d, limits.domainRatio);
    
    return limits_struct;
}

RAT::struct3_T createStruct3(const Checks& checks)
{
    RAT::struct3_T checks_struct;
    checks_struct.fitParam = customCaller("Checks.fitParam", pyArrayToRatColArray1d, checks.fitParam);
    checks_struct.fitBackgroundParam = customCaller("Checks.fitBackgroundParam", pyArrayToRatColArray1d, checks.fitBackgroundParam);
    checks_struct.fitQzshift = customCaller("Checks.fitQzshift", pyArrayToRatColArray1d, checks.fitQzshift);
    checks_struct.fitScalefactor = customCaller("Checks.fitScalefactor", pyArrayToRatColArray1d, checks.fitScalefactor);
    checks_struct.fitBulkIn = customCaller("Checks.fitBulkIn", pyArrayToRatColArray1d, checks.fitBulkIn);
    checks_struct.fitBulkOut = customCaller("Checks.fitBulkOut", pyArrayToRatColArray1d, checks.fitBulkOut);
    checks_struct.fitResolutionParam = customCaller("Checks.fitResolutionParam", pyArrayToRatColArray1d, checks.fitResolutionParam);
    checks_struct.fitDomainRatio = customCaller("Checks.fitDomainRatio", pyArrayToRatColArray1d, checks.fitDomainRatio);
    
    return checks_struct;
}

RAT::struct4_T createStruct4(const Priors& priors)
{
    RAT::struct4_T priors_struct;
    priors_struct.param = customCaller("Priors.param", pyListToUnboundedCell0, priors.param);
    priors_struct.backgroundParam = customCaller("Priors.backgroundParam", pyListToUnboundedCell0, priors.backgroundParam);
    priors_struct.resolutionParam = customCaller("Priors.resolutionParam", pyListToUnboundedCell0, priors.resolutionParam);
    priors_struct.qzshift = customCaller("Priors.qzshift", pyListToUnboundedCell0, priors.qzshift);
    priors_struct.scalefactor = customCaller("Priors.scalefactor", pyListToUnboundedCell0, priors.scalefactor);
    priors_struct.bulkIn = customCaller("Priors.bulkIn", pyListToUnboundedCell0, priors.bulkIn);
    priors_struct.bulkOut = customCaller("Priors.bulkOut", pyListToUnboundedCell0, priors.bulkOut);
    priors_struct.domainRatio = customCaller("Priors.domainRatio", pyListToUnboundedCell0, priors.domainRatio);
    priors_struct.priorNames = customCaller("Priors.priorNames", pyListToUnboundedCell1, priors.priorNames);
    priors_struct.priorValues =  customCaller("Priors.priorValues", pyArrayToRatArray2d, priors.priorValues);
    
    return priors_struct;
}

coder::array<RAT::cell_wrap_2, 2U> pyListToRatCellWrap2(py::list values)
{
    coder::array<RAT::cell_wrap_2, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);    
        if (casted_array.size() != 2)
            throw std::runtime_error("Expects a 2D list where each row contains exactly 2 numbers");
        result[idx].f1[0] = casted_array.at(0);
        result[idx].f1[1] = casted_array.at(1);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_3, 2U> pyListToRatCellWrap3(py::list values)
{
    coder::array<RAT::cell_wrap_3, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatArray2d, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_4, 2U> pyListToRatCellWrap4(py::list values)
{
    coder::array<RAT::cell_wrap_4, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatColArray1d, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_5, 1U> pyListToRatCellWrap5(py::list values)
{
    coder::array<RAT::cell_wrap_5, 1U> result;
    result.set_size(values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatBoundedArray, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_6, 2U> pyListToRatCellWrap6(py::list values)
{
    coder::array<RAT::cell_wrap_6, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        if (py::isinstance<py::str>(array)) {
            std::string name = py::cast<std::string>(array);
            stringToRatArray(name, result[idx].f1.data, result[idx].f1.size);
            idx++;
        }
        else
            throw std::runtime_error("Expects a 1D list of strings");
    }

    return result;
}

coder::array<RAT::cell_wrap_6, 2U> py_function_array_to_rat_cell_wrap_6(py::object values)
{
    auto handles = py::cast<py::list>(values);
    coder::array<RAT::cell_wrap_6, 2U> result;
    result.set_size(1, handles.size());
    int32_T idx {0};
    for (py::handle array: handles)
    { 
        auto func = py::cast<py::function>(array);
        std::string func_ptr = convertPtr2String<CallbackInterface>(new Library(func));
        stringToRatArray(func_ptr, result[idx].f1.data, result[idx].f1.size);
        idx++;
    }

    return result;
}

RAT::cell_7 createCell7(const Cells& cells)
{
    RAT::cell_7 cells_struct;
    cells_struct.f1 = customCaller("Cells.f1", pyListToRatCellWrap2, cells.f1);
    cells_struct.f2 = customCaller("Cells.f2", pyListToRatCellWrap3, cells.f2);
    cells_struct.f3 = customCaller("Cells.f3", pyListToRatCellWrap2, cells.f3);
    cells_struct.f4 = customCaller("Cells.f4", pyListToRatCellWrap2, cells.f4);
    cells_struct.f5 = customCaller("Cells.f5", pyListToRatCellWrap4, cells.f5);
    cells_struct.f6 = customCaller("Cells.f6", pyListToRatCellWrap5, cells.f6);
    cells_struct.f7 = customCaller("Cells.f7", pyListToRatCellWrap6, cells.f7);
    cells_struct.f8 = customCaller("Cells.f8", pyListToRatCellWrap6, cells.f8);
    cells_struct.f9 = customCaller("Cells.f9", pyListToRatCellWrap6, cells.f9);
    cells_struct.f10 = customCaller("Cells.f10", pyListToRatCellWrap6, cells.f10);
    cells_struct.f11 = customCaller("Cells.f11", pyListToRatCellWrap6, cells.f11);
    cells_struct.f12 = customCaller("Cells.f12", pyListToRatCellWrap6, cells.f12);
    cells_struct.f13 = customCaller("Cells.f13", pyListToRatCellWrap6, cells.f13);
    cells_struct.f14 = customCaller("Cells.f14", py_function_array_to_rat_cell_wrap_6, cells.f14);
    cells_struct.f15 = customCaller("Cells.f15", pyListToRatCellWrap6, cells.f15);
    cells_struct.f16 = customCaller("Cells.f16", pyListToRatCellWrap6, cells.f16);
    cells_struct.f17 = customCaller("Cells.f17", pyListToRatCellWrap3, cells.f17);
    cells_struct.f18 = customCaller("Cells.f18", pyListToRatCellWrap2, cells.f18);
    cells_struct.f19 = customCaller("Cells.f19", pyListToRatCellWrap4, cells.f19);
    cells_struct.f20 = customCaller("Cells.f20", pyListToRatCellWrap6, cells.f20);
    cells_struct.f21 = customCaller("Cells.f21", pyListToRatCellWrap6, cells.f21);

    return cells_struct;
}

RAT::struct2_T createStruct2T(const Control& control)
{
    RAT::struct2_T control_struct;
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
    control_struct.calcSldDuringFit = control.calcSldDuringFit;
    control_struct.updateFreq = control.updateFreq;
    control_struct.updatePlotFreq = control.updatePlotFreq;
    control_struct.nSamples = control.nSamples;
    control_struct.nChains = control.nChains;
    control_struct.jumpProbability = control.jumpProbability;
    control_struct.pUnitGamma = control.pUnitGamma;
    stringToRatArray(control.parallel, control_struct.parallel.data, control_struct.parallel.size);
    stringToRatArray(control.procedure, control_struct.procedure.data, control_struct.procedure.size);
    stringToRatArray(control.display, control_struct.display.data, control_struct.display.size);
    control_struct.xTolerance = control.xTolerance;
    control_struct.resampleMinAngle = control.resampleMinAngle;
    control_struct.resampleNPoints = control.resampleNPoints;
    stringToRatArray(control.boundHandling, control_struct.boundHandling.data, control_struct.boundHandling.size);
    control_struct.adaptPCR = control.adaptPCR;
    control_struct.checks = createStruct3(control.checks);
    stringToRatArray(control.IPCFilePath, control_struct.IPCFilePath.data, control_struct.IPCFilePath.size);

    return control_struct;
}


template <typename T> 
py::array_t<real_T> pyArrayFromRatArray1d(T array, bool isCol=true)
{
    auto size = isCol ?  array.size(1) : array.size(0);
    auto result_array = py::array_t<real_T>(size);
    std::memcpy(result_array.request().ptr, array.data(), result_array.nbytes());

    return result_array;
}

py::array_t<real_T> pyArrayFromRatArray2d(coder::array<real_T, 2U> array)
{
    auto result_array = py::array_t<real_T, py::array::f_style>({array.size(0), array.size(1)});
    std::memcpy(result_array.request().ptr, array.data(), result_array.nbytes());

    return result_array;
}


OutputResult OutputResultFromStruct5T(const RAT::struct5_T result)
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

    for (int32_T idx0{0}; idx0 < result.layerSlds.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < result.layerSlds.size(1); idx1++) {
            auto tmp = result.layerSlds[idx0 +  result.layerSlds.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        output_result.layerSlds.append(inner_list);
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

    output_result.contrastParams.backgroundParams = py::array_t<real_T>(result.contrastParams.backgroundParams.size(0));
    buffer = output_result.contrastParams.backgroundParams.request();
    std::memcpy(buffer.ptr, result.contrastParams.backgroundParams.data(), output_result.contrastParams.backgroundParams.size()*sizeof(real_T));

    output_result.contrastParams.scalefactors = py::array_t<real_T>(result.contrastParams.scalefactors.size(0));
    buffer = output_result.contrastParams.scalefactors.request();
    std::memcpy(buffer.ptr, result.contrastParams.scalefactors.data(), output_result.contrastParams.scalefactors.size()*sizeof(real_T));

    output_result.contrastParams.bulkIn = py::array_t<real_T>(result.contrastParams.bulkIn.size(0));
    buffer = output_result.contrastParams.bulkIn.request();
    std::memcpy(buffer.ptr, result.contrastParams.bulkIn.data(), output_result.contrastParams.bulkIn.size()*sizeof(real_T));

    output_result.contrastParams.bulkOut = py::array_t<real_T>(result.contrastParams.bulkOut.size(0));
    buffer = output_result.contrastParams.bulkOut.request();
    std::memcpy(buffer.ptr, result.contrastParams.bulkOut.data(), output_result.contrastParams.bulkOut.size()*sizeof(real_T));

    output_result.contrastParams.resolutionParams = py::array_t<real_T>(result.contrastParams.resolutionParams.size(0));
    buffer = output_result.contrastParams.resolutionParams.request();
    std::memcpy(buffer.ptr, result.contrastParams.resolutionParams.data(), output_result.contrastParams.resolutionParams.size()*sizeof(real_T));

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

ProblemDefinition problemDefinitionFromStruct0T(const RAT::struct0_T problem)
{
    ProblemDefinition problem_def;
    
    problem_def.useImaginary = problem.useImaginary;
    
    problem_def.numberOfLayers = problem.numberOfLayers;
    problem_def.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_def.numberOfContrasts = problem.numberOfContrasts;

    problem_def.modelType.resize(problem.modelType.size[1]);
    memcpy(&problem_def.modelType[0], problem.modelType.data, problem.modelType.size[1]);
    problem_def.geometry.resize(problem.geometry.size[1]);
    memcpy(&problem_def.geometry[0], problem.geometry.data, problem.geometry.size[1]);
    problem_def.TF.resize(problem.TF.size[1]);
    memcpy(&problem_def.TF[0], problem.TF.data, problem.TF.size[1]);
    
    problem_def.contrastBackgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBackgroundParams);
    problem_def.contrastBackgroundActions = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBackgroundActions);
    problem_def.resample = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resample);
    problem_def.dataPresent = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.dataPresent);
    problem_def.oilChiDataPresent = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.oilChiDataPresent);
    problem_def.contrastQzshifts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastQzshifts);
    problem_def.contrastScalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastScalefactors);
    problem_def.contrastBulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkIns);
    problem_def.contrastBulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkOuts);
    problem_def.contrastResolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastResolutionParams);
    problem_def.backgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.backgroundParams);
    problem_def.qzshifts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.qzshifts);
    problem_def.scalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.scalefactors);
    problem_def.bulkIn = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkIn);
    problem_def.bulkOut = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkOut);
    problem_def.resolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resolutionParams);
    problem_def.params = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.params);

    problem_def.contrastCustomFiles = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastCustomFiles);
    problem_def.contrastDomainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastDomainRatios);
    problem_def.domainRatio = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.domainRatio);

    problem_def.fitParams = pyArrayFromRatArray1d<coder::array<real_T, 1U>>(problem.fitParams, false);
    problem_def.otherParams = pyArrayFromRatArray1d<coder::array<real_T, 1U>>(problem.otherParams, false);
    problem_def.fitLimits = pyArrayFromRatArray2d(problem.fitLimits);
    problem_def.otherLimits = pyArrayFromRatArray2d(problem.otherLimits);

    return problem_def;
}

template <typename T> 
py::list pyList1DFromRatCellWrap(const T& values)
{
    py::list result;
    
    for (int32_T idx0{0}; idx0 < values.size(0); idx0++) {
        result.append(pyArrayFromRatArray2d(values[idx0].f1));
    }

    return result;
}

template <typename T> 
py::list pyList2dFromRatCellWrap(const T& values)
{
    py::list result;
    int32_T idx {0};
    for (int32_T idx0{0}; idx0 < values.size(0); idx0++) {
        py::list inner;
        for (int32_T idx1{0}; idx1 < values.size(1); idx1++) {
            idx  = idx0 + values.size(0) * idx1;  
            inner.append(pyArrayFromRatArray2d(values[idx].f1));
        }
        result.append(inner);
    }

    return result;
}

template <class T>
py::array_t<real_T> pyArray1dFromBoundedArray(const T& array)
{
    auto result_array = py::array_t<real_T, py::array::f_style>({array.size[0]});
    std::memcpy(result_array.request().ptr, array.data, result_array.nbytes());
    
    return result_array;
}

template <class T>
py::array_t<real_T> pyArray2dFromBoundedArray(const T& array)
{
    auto result_array = py::array_t<real_T, py::array::f_style>({array.size[0], array.size[1]});
    std::memcpy(result_array.request().ptr, array.data, result_array.nbytes());
    
    return result_array;
}

py::array_t<real_T> pyArrayFromRatArray3d(coder::array<real_T, 3U> array)
{
    auto result_array = py::array_t<real_T, py::array::f_style>({array.size(0), array.size(1), array.size(2)});
    std::memcpy(result_array.request().ptr, array.data(), result_array.nbytes());

    return result_array;
}

BayesResults bayesResultsFromStruct8T(const RAT::struct8_T results)
{
    BayesResults bayesResults;

    bayesResults.chain = pyArrayFromRatArray2d(results.chain);

    bayesResults.predictionIntervals.reflectivity = pyList1DFromRatCellWrap<coder::array<RAT::cell_wrap_11, 1U>>(results.predictionIntervals.reflectivity);
    bayesResults.predictionIntervals.sld = pyList2dFromRatCellWrap<coder::array<RAT::cell_wrap_11, 2U>>(results.predictionIntervals.sld);
    bayesResults.predictionIntervals.reflectivityXData = pyList1DFromRatCellWrap<coder::array<RAT::cell_wrap_12, 1U>>(results.predictionIntervals.reflectivityXData);
    bayesResults.predictionIntervals.sldXData = pyList2dFromRatCellWrap<coder::array<RAT::cell_wrap_12, 2U>>(results.predictionIntervals.sldXData);
    bayesResults.predictionIntervals.sampleChi = pyArray1dFromBoundedArray<coder::bounded_array<real_T, 1000U, 1U>>(results.predictionIntervals.sampleChi);

    bayesResults.confidenceIntervals.percentile95 = pyArrayFromRatArray2d(results.confidenceIntervals.percentile95);
    bayesResults.confidenceIntervals.percentile65 = pyArrayFromRatArray2d(results.confidenceIntervals.percentile65);
    bayesResults.confidenceIntervals.mean = pyArrayFromRatArray2d(results.confidenceIntervals.mean);

    bayesResults.nestedSamplerOutput.logZ = results.nestedSamplerOutput.LogZ;
    bayesResults.nestedSamplerOutput.nestSamples = pyArrayFromRatArray2d(results.nestedSamplerOutput.nestSamples);
    bayesResults.nestedSamplerOutput.postSamples = pyArrayFromRatArray2d(results.nestedSamplerOutput.postSamples);

    bayesResults.dreamOutput.allChains = pyArrayFromRatArray3d(results.dreamOutput.allChains);
    bayesResults.dreamOutput.outlierChains = pyArray2dFromBoundedArray<coder::bounded_array<real_T, 2000U, 2U>>(results.dreamOutput.outlierChains);
    bayesResults.dreamOutput.runtime = results.dreamOutput.runtime;
    bayesResults.dreamOutput.iteration = results.dreamOutput.iteration;
    bayesResults.dreamOutput.modelOutput = results.dreamOutput.modelOutput;
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

py::tuple RATMain(const ProblemDefinition& problem_def, const Cells& cells, const Limits& limits, const Control& control, 
                                  const Priors& priors)
{
    RAT::struct0_T problem_def_struct = createStruct0(problem_def);
    RAT::cell_7 cells_struct = createCell7(cells);
    RAT::struct1_T limits_struct = createStruct1(limits);
    RAT::struct2_T control_struct = createStruct2T(control);
    RAT::struct4_T priors_struct = createStruct4(priors);

    // Output
    RAT::struct5_T results;
    RAT::struct8_T bayesResults;

    // Call the entry-point
    RAT::RATMain(&problem_def_struct, &cells_struct, &limits_struct, &control_struct,
                 &priors_struct, &results, &bayesResults);
    
    // Copy result to output
    return py::make_tuple(problemDefinitionFromStruct0T(problem_def_struct), 
                          OutputResultFromStruct5T(results), 
                          bayesResultsFromStruct8T(bayesResults));    
}

py::array_t<real_T> makeSLDProfileXY(real_T bulk_in,
                                     real_T bulk_out,
                                     real_T ssub,
                                     const py::array_t<real_T> &layers,
                                     real_T number_of_layers,
                                     real_T repeats)
{
    coder::array<real_T, 2U> out;
    coder::array<real_T, 2U> layers_array = pyArrayToRatArray2d(layers);
    RAT::makeSLDProfileXY(bulk_in,
                          bulk_out,
                          ssub,
                          layers_array,
                          number_of_layers,
                          repeats,
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
        .def("invoke", &DylibEngine::invoke, py::arg("params"), py::arg("bulkIn"), 
                                           py::arg("bulkOut"), py::arg("contrast"), 
                                           py::arg("domain") = DEFAULT_DOMAIN);

    py::class_<PredictionIntervals>(m, "PredictionIntervals")
        .def(py::init<>())
        .def_readwrite("reflectivity", &PredictionIntervals::reflectivity)
        .def_readwrite("sld", &PredictionIntervals::sld)
        .def_readwrite("reflectivityXData", &PredictionIntervals::reflectivityXData)
        .def_readwrite("sldXData", &PredictionIntervals::sldXData)
        .def_readwrite("sampleChi", &PredictionIntervals::sampleChi);
    
    py::class_<PlotEventData>(m, "PlotEventData")
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

    py::class_<ProgressEventData>(m, "ProgressEventData")
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

    py::class_<ConfidenceIntervals>(m, "ConfidenceIntervals")
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

    py::class_<NestedSamplerOutput>(m, "NestedSamplerOutput")
        .def(py::init<>())
        .def_readwrite("logZ", &NestedSamplerOutput::logZ)
        .def_readwrite("nestSamples", &NestedSamplerOutput::nestSamples)
        .def_readwrite("postSamples", &NestedSamplerOutput::postSamples);

    py::class_<DreamOutput>(m, "DreamOutput")
        .def(py::init<>())
        .def_readwrite("allChains", &DreamOutput::allChains)
        .def_readwrite("outlierChains", &DreamOutput::outlierChains)
        .def_readwrite("runtime", &DreamOutput::runtime)
        .def_readwrite("iteration", &DreamOutput::iteration)
        .def_readwrite("modelOutput", &DreamOutput::modelOutput)
        .def_readwrite("AR", &DreamOutput::AR)
        .def_readwrite("R_stat", &DreamOutput::R_stat)
        .def_readwrite("CR", &DreamOutput::CR);

    py::class_<BayesResults>(m, "BayesResults")
        .def(py::init<>())
        .def_readwrite("predictionIntervals", &BayesResults::predictionIntervals)
        .def_readwrite("confidenceIntervals", &BayesResults::confidenceIntervals)
        .def_readwrite("dreamParams", &BayesResults::dreamParams)
        .def_readwrite("dreamOutput", &BayesResults::dreamOutput)
        .def_readwrite("nestedSamplerOutput", &BayesResults::nestedSamplerOutput)
        .def_readwrite("chain", &BayesResults::chain);

    py::class_<Calculation>(m, "Calculation")
        .def(py::init<>())
        .def_readwrite("chiValues", &Calculation::chiValues)
        .def_readwrite("sumChi", &Calculation::sumChi);

    py::class_<ContrastParams>(m, "ContrastParams")
        .def(py::init<>())
        .def_readwrite("backgroundParams", &ContrastParams::backgroundParams)
        .def_readwrite("scalefactors", &ContrastParams::scalefactors)
        .def_readwrite("bulkIn", &ContrastParams::bulkIn)
        .def_readwrite("bulkOut", &ContrastParams::bulkOut)
        .def_readwrite("resolutionParams", &ContrastParams::resolutionParams)
        .def_readwrite("subRoughs", &ContrastParams::subRoughs)
        .def_readwrite("resample", &ContrastParams::resample);
    
    py::class_<OutputResult>(m, "OutputResult")
        .def(py::init<>())
        .def_readwrite("reflectivity", &OutputResult::reflectivity)
        .def_readwrite("simulation", &OutputResult::simulation)
        .def_readwrite("shiftedData", &OutputResult::shiftedData)
        .def_readwrite("layerSlds", &OutputResult::layerSlds)
        .def_readwrite("sldProfiles", &OutputResult::sldProfiles)
        .def_readwrite("resampledLayers", &OutputResult::resampledLayers)
        .def_readwrite("calculationResults", &OutputResult::calculationResults)
        .def_readwrite("contrastParams", &OutputResult::contrastParams)        
        .def_readwrite("fitParams", &OutputResult::fitParams)
        .def_readwrite("fitNames", &OutputResult::fitNames);

    py::class_<Checks>(m, "Checks")
        .def(py::init<>())
        .def_readwrite("fitParam", &Checks::fitParam)
        .def_readwrite("fitBackgroundParam", &Checks::fitBackgroundParam)
        .def_readwrite("fitQzshift", &Checks::fitQzshift)
        .def_readwrite("fitScalefactor", &Checks::fitScalefactor)
        .def_readwrite("fitBulkIn", &Checks::fitBulkIn)
        .def_readwrite("fitBulkOut", &Checks::fitBulkOut)
        .def_readwrite("fitResolutionParam", &Checks::fitResolutionParam)
        .def_readwrite("fitDomainRatio", &Checks::fitDomainRatio)
        .def(py::pickle(
            [](const Checks &chk) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(chk.fitParam, chk.fitBackgroundParam, chk.fitQzshift, chk.fitScalefactor, chk.fitBulkIn, chk.fitBulkOut, 
                                      chk.fitResolutionParam, chk.fitDomainRatio);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 8)
                    throw std::runtime_error("Encountered invalid state unpickling Checks object!");

                /* Create a new C++ instance */
                Checks chk;

                chk.fitParam = t[0].cast<py::array_t<real_T>>(); 
                chk.fitBackgroundParam = t[1].cast<py::array_t<real_T>>(); 
                chk.fitQzshift = t[2].cast<py::array_t<real_T>>(); 
                chk.fitScalefactor = t[3].cast<py::array_t<real_T>>(); 
                chk.fitBulkIn = t[4].cast<py::array_t<real_T>>(); 
                chk.fitBulkOut = t[5].cast<py::array_t<real_T>>(); 
                chk.fitResolutionParam = t[6].cast<py::array_t<real_T>>(); 
                chk.fitDomainRatio = t[7].cast<py::array_t<real_T>>();

                return chk;
            }));

    py::class_<Limits>(m, "Limits")
        .def(py::init<>())
        .def_readwrite("param", &Limits::param)
        .def_readwrite("backgroundParam", &Limits::backgroundParam)
        .def_readwrite("qzshift", &Limits::qzshift)
        .def_readwrite("scalefactor", &Limits::scalefactor)
        .def_readwrite("bulkIn", &Limits::bulkIn)
        .def_readwrite("bulkOut", &Limits::bulkOut)
        .def_readwrite("resolutionParam", &Limits::resolutionParam)
        .def_readwrite("domainRatio", &Limits::domainRatio)
        .def(py::pickle(
            [](const Limits &lim) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(lim.param, lim.backgroundParam, lim.qzshift, lim.scalefactor, lim.bulkIn, lim.bulkOut, 
                                      lim.resolutionParam, lim.domainRatio);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 8)
                    throw std::runtime_error("Encountered invalid state unpickling Limits object!");

                /* Create a new C++ instance */
                Limits lim;

                lim.param = t[0].cast<py::array_t<real_T>>(); 
                lim.backgroundParam = t[1].cast<py::array_t<real_T>>(); 
                lim.qzshift = t[2].cast<py::array_t<real_T>>(); 
                lim.scalefactor = t[3].cast<py::array_t<real_T>>(); 
                lim.bulkIn = t[4].cast<py::array_t<real_T>>(); 
                lim.bulkOut = t[5].cast<py::array_t<real_T>>(); 
                lim.resolutionParam = t[6].cast<py::array_t<real_T>>(); 
                lim.domainRatio = t[7].cast<py::array_t<real_T>>();

                return lim;
            }));

    py::class_<Priors>(m, "Priors")
        .def(py::init<>())
        .def_readwrite("param", &Priors::param)
        .def_readwrite("backgroundParam", &Priors::backgroundParam)
        .def_readwrite("qzshift", &Priors::qzshift)
        .def_readwrite("scalefactor", &Priors::scalefactor)
        .def_readwrite("bulkIn", &Priors::bulkIn)
        .def_readwrite("bulkOut", &Priors::bulkOut)
        .def_readwrite("resolutionParam", &Priors::resolutionParam)
        .def_readwrite("domainRatio", &Priors::domainRatio)
        .def_readwrite("priorNames", &Priors::priorNames)
        .def_readwrite("priorValues", &Priors::priorValues)
        .def(py::pickle(
            [](const Priors &prior) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(prior.param, prior.backgroundParam, prior.qzshift, prior.scalefactor, prior.bulkIn, 
                                      prior.bulkOut, prior.resolutionParam, prior.domainRatio, prior.priorNames, prior.priorValues);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 10)
                    throw std::runtime_error("Encountered invalid state unpickling Limits object!");

                /* Create a new C++ instance */
                Priors prior;

                prior.param = t[0].cast<py::list>();
                prior.backgroundParam = t[1].cast<py::list>();
                prior.qzshift = t[2].cast<py::list>();
                prior.scalefactor = t[3].cast<py::list>();
                prior.bulkIn = t[4].cast<py::list>();
                prior.bulkOut = t[5].cast<py::list>();
                prior.resolutionParam = t[6].cast<py::list>();
                prior.domainRatio = t[7].cast<py::list>();
                prior.priorNames = t[8].cast<py::list>();
                prior.priorValues = t[9].cast<py::array_t<real_T>>();

                return prior;
            }));
    
    py::class_<Cells>(m, "Cells")
        .def(py::init<>())
        .def_readwrite("f1", &Cells::f1)
        .def_readwrite("f2", &Cells::f2)
        .def_readwrite("f3", &Cells::f3)
        .def_readwrite("f4", &Cells::f4)
        .def_readwrite("f5", &Cells::f5)
        .def_readwrite("f6", &Cells::f6)
        .def_readwrite("f7", &Cells::f7)
        .def_readwrite("f8", &Cells::f8)
        .def_readwrite("f9", &Cells::f9)
        .def_readwrite("f10", &Cells::f10)
        .def_readwrite("f11", &Cells::f11)
        .def_readwrite("f12", &Cells::f12)
        .def_readwrite("f13", &Cells::f13)
        .def_readwrite("f14", &Cells::f14)
        .def_readwrite("f15", &Cells::f15)
        .def_readwrite("f16", &Cells::f16)
        .def_readwrite("f17", &Cells::f17)
        .def_readwrite("f18", &Cells::f18)
        .def_readwrite("f19", &Cells::f19)
        .def_readwrite("f20", &Cells::f20)
        .def_readwrite("f21", &Cells::f21)
        .def(py::pickle(
            [](const Cells &cell) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(cell.f1, cell.f2, cell.f3, cell.f4, cell.f5, cell.f6, cell.f7, cell.f8, cell.f9, cell.f10, cell.f11, 
                                      cell.f12, cell.f13, cell.f14, cell.f15, cell.f16, cell.f17, cell.f18, cell.f19, cell.f20, cell.f21);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 21)
                    throw std::runtime_error("Encountered invalid state unpickling Cells object!");

                /* Create a new C++ instance */
                Cells cell;

                cell.f1 = t[0].cast<py::list>();
                cell.f2 = t[1].cast<py::list>(); 
                cell.f3 = t[2].cast<py::list>(); 
                cell.f4 = t[3].cast<py::list>(); 
                cell.f5 = t[4].cast<py::list>(); 
                cell.f6 = t[5].cast<py::list>(); 
                cell.f7 = t[6].cast<py::list>(); 
                cell.f8 = t[7].cast<py::list>(); 
                cell.f9 = t[8].cast<py::list>(); 
                cell.f10 = t[9].cast<py::list>(); 
                cell.f11 = t[10].cast<py::list>(); 
                cell.f12 = t[11].cast<py::list>();
                cell.f13 = t[12].cast<py::list>(); 
                cell.f14 = t[13].cast<py::object>(); 
                cell.f15 = t[14].cast<py::list>(); 
                cell.f16 = t[15].cast<py::list>(); 
                cell.f17 = t[16].cast<py::list>(); 
                cell.f18 = t[17].cast<py::list>(); 
                cell.f19 = t[18].cast<py::list>(); 
                cell.f20 = t[19].cast<py::list>(); 
                cell.f21 = t[20].cast<py::list>(); 

                return cell;
            }));

    py::class_<Control>(m, "Control")
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
        .def_readwrite("calcSldDuringFit", &Control::calcSldDuringFit)
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
        .def_readwrite("checks", &Control::checks)
        .def_readwrite("IPCFilePath", &Control::IPCFilePath)
        .def(py::pickle(
            [](const Control &ctrl) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(ctrl.parallel, ctrl.procedure, ctrl.display, ctrl.xTolerance, ctrl.funcTolerance, 
                                      ctrl.maxFuncEvals, ctrl.maxIterations, ctrl.populationSize, ctrl.fWeight, ctrl.crossoverProbability, 
                                      ctrl.targetValue, ctrl.numGenerations, ctrl.strategy, ctrl.nLive, ctrl.nMCMC, ctrl.propScale, 
                                      ctrl.nsTolerance, ctrl.calcSldDuringFit, ctrl.resampleMinAngle, ctrl.resampleNPoints,
                                      ctrl.updateFreq, ctrl.updatePlotFreq, ctrl.nSamples, ctrl.nChains, ctrl.jumpProbability, ctrl.pUnitGamma, 
                                      ctrl.boundHandling, ctrl.adaptPCR, ctrl.IPCFilePath, ctrl.checks.fitParam, ctrl.checks.fitBackgroundParam, 
                                      ctrl.checks.fitQzshift, ctrl.checks.fitScalefactor, ctrl.checks.fitBulkIn, ctrl.checks.fitBulkOut, 
                                      ctrl.checks.fitResolutionParam, ctrl.checks.fitDomainRatio);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 37)
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
                ctrl.calcSldDuringFit = t[17].cast<boolean_T>();
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
                
                ctrl.checks.fitParam = t[29].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitBackgroundParam = t[30].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitQzshift = t[31].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitScalefactor = t[32].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitBulkIn = t[33].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitBulkOut = t[34].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitResolutionParam = t[35].cast<py::array_t<real_T>>(); 
                ctrl.checks.fitDomainRatio = t[36].cast<py::array_t<real_T>>();

                return ctrl;
            }));
    
    py::class_<ProblemDefinition>(m, "ProblemDefinition")
        .def(py::init<>())
        .def_readwrite("contrastBackgroundParams", &ProblemDefinition::contrastBackgroundParams)
        .def_readwrite("contrastBackgroundActions", &ProblemDefinition::contrastBackgroundActions)
        .def_readwrite("TF", &ProblemDefinition::TF)
        .def_readwrite("resample", &ProblemDefinition::resample)
        .def_readwrite("dataPresent", &ProblemDefinition::dataPresent)
        .def_readwrite("oilChiDataPresent", &ProblemDefinition::oilChiDataPresent)
        .def_readwrite("numberOfContrasts", &ProblemDefinition::numberOfContrasts)
        .def_readwrite("geometry", &ProblemDefinition::geometry)
        .def_readwrite("useImaginary", &ProblemDefinition::useImaginary)  
        .def_readwrite("contrastQzshifts", &ProblemDefinition::contrastQzshifts)  
        .def_readwrite("contrastScalefactors", &ProblemDefinition::contrastScalefactors)
        .def_readwrite("contrastBulkIns", &ProblemDefinition::contrastBulkIns)
        .def_readwrite("contrastBulkOuts", &ProblemDefinition::contrastBulkOuts)
        .def_readwrite("contrastResolutionParams", &ProblemDefinition::contrastResolutionParams)
        .def_readwrite("backgroundParams", &ProblemDefinition::backgroundParams)
        .def_readwrite("qzshifts", &ProblemDefinition::qzshifts)
        .def_readwrite("scalefactors", &ProblemDefinition::scalefactors)
        .def_readwrite("bulkIn", &ProblemDefinition::bulkIn)
        .def_readwrite("bulkOut", &ProblemDefinition::bulkOut)
        .def_readwrite("resolutionParams", &ProblemDefinition::resolutionParams)
        .def_readwrite("params", &ProblemDefinition::params)
        .def_readwrite("numberOfLayers", &ProblemDefinition::numberOfLayers)
        .def_readwrite("modelType", &ProblemDefinition::modelType)
        .def_readwrite("contrastCustomFiles", &ProblemDefinition::contrastCustomFiles)
        .def_readwrite("contrastDomainRatios", &ProblemDefinition::contrastDomainRatios)
        .def_readwrite("domainRatio", &ProblemDefinition::domainRatio)
        .def_readwrite("numberOfDomainContrasts", &ProblemDefinition::numberOfDomainContrasts)
        .def_readwrite("fitParams", &ProblemDefinition::fitParams)
        .def_readwrite("otherParams", &ProblemDefinition::otherParams)
        .def_readwrite("fitLimits", &ProblemDefinition::fitLimits)
        .def_readwrite("otherLimits", &ProblemDefinition::otherLimits)
        .def(py::pickle(
            [](const ProblemDefinition &p) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(p.contrastBackgroundParams, p.contrastBackgroundActions, p.TF, p.resample, p.dataPresent, p.oilChiDataPresent, 
                                      p.numberOfContrasts, p.geometry, p.useImaginary, p.contrastQzshifts, p.contrastScalefactors, 
                                      p.contrastBulkIns, p.contrastBulkOuts, p.contrastResolutionParams, p.backgroundParams,
                                      p.qzshifts, p.scalefactors, p.bulkIn, p.bulkOut, p.resolutionParams, p.params,
                                      p.numberOfLayers, p.modelType, p.contrastCustomFiles, p.contrastDomainRatios,
                                      p.domainRatio,  p.numberOfDomainContrasts, p.fitParams, p.otherParams, p.fitLimits, p.otherLimits);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 31)
                    throw std::runtime_error("Encountered invalid state unpickling ProblemDefinition object!");

                /* Create a new C++ instance */
                ProblemDefinition p;

                p.contrastBackgroundParams = t[0].cast<py::array_t<real_T>>(); 
                p.contrastBackgroundActions = t[1].cast<py::array_t<real_T>>(); 
                p.TF = t[2].cast<std::string>(); 
                p.resample = t[3].cast<py::array_t<real_T>>(); 
                p.dataPresent = t[4].cast<py::array_t<real_T>>(); 
                p.oilChiDataPresent = t[5].cast<py::array_t<real_T>>(); 
                p.numberOfContrasts = t[6].cast<real_T>(); 
                p.geometry = t[7].cast<std::string>(); 
                p.useImaginary = t[8].cast<bool>(); 
                p.contrastQzshifts = t[9].cast<py::array_t<real_T>>(); 
                p.contrastScalefactors = t[10].cast<py::array_t<real_T>>(); 
                p.contrastBulkIns = t[11].cast<py::array_t<real_T>>(); 
                p.contrastBulkOuts = t[12].cast<py::array_t<real_T>>(); 
                p.contrastResolutionParams = t[13].cast<py::array_t<real_T>>(); 
                p.backgroundParams = t[14].cast<py::array_t<real_T>>();
                p.qzshifts = t[15].cast<py::array_t<real_T>>(); 
                p.scalefactors = t[16].cast<py::array_t<real_T>>(); 
                p.bulkIn= t[17].cast<py::array_t<real_T>>(); 
                p.bulkOut= t[18].cast<py::array_t<real_T>>(); 
                p.resolutionParams= t[19].cast<py::array_t<real_T>>(); 
                p.params = t[20].cast<py::array_t<real_T>>(),
                p.numberOfLayers = t[21].cast<real_T>(); 
                p.modelType = t[22].cast<std::string>(); 
                p.contrastCustomFiles = t[23].cast<py::array_t<real_T>>(); 
                p.contrastDomainRatios = t[24].cast<py::array_t<real_T>>(),
                p.domainRatio = t[25].cast<py::array_t<real_T>>();  
                p.numberOfDomainContrasts = t[26].cast<real_T>(); 
                p.fitParams = t[27].cast<py::array_t<real_T>>(); 
                p.otherParams = t[28].cast<py::array_t<real_T>>(); 
                p.fitLimits = t[29].cast<py::array_t<real_T>>(); 
                p.otherLimits = t[30].cast<py::array_t<real_T>>();

                return p;
            }));

    m.def("RATMain", &RATMain, "Entry point for the main reflectivity computation.");

    m.def("makeSLDProfileXY", &makeSLDProfileXY, "Creates the profiles for the SLD plots");
}
