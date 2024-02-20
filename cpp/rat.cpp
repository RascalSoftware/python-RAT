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
#include "RAT/classHandle.hpp"
#include "RAT/dylib.hpp"
#include "RAT/events/eventManager.h"

namespace py = pybind11;

const int DEFAULT_DOMAIN = -1;

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
            std::cerr << "dynamic libray failed to load" << std::endl;
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
            throw std::runtime_error("failed to get dynamic libray symbol for ***functionName");
        }        
    };
};


struct PlotEventData
{
    py::list reflectivity;
    py::list shiftedData;
    py::list sldProfiles;
    py::list allLayers;
    py::array_t<double> ssubs;
    py::array_t<double> resample;
    py::array_t<double> dataPresent;
    std::string modelType;
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
            std::cerr << "event manager dynamic libray failed to load" << std::endl;
            return;
        }
        this->callback = callback;
    };
    
    py::list unpackDataToCell(int rows, int cols, double* data, double* nData, 
                              double* data2, double* nData2, int dataCol)   
    {
        py::list allResults;
        int dims[2] = {0, dataCol};
        int offset = 0;
        for (int i = 0; i < rows; i++){
            py::list rowList;
            dims[0] = (int)nData[i] / dataCol;
            auto result = py::array_t<double,  py::array::f_style>({dims[0], dims[1]});
            std::memcpy(result.request().ptr, data + offset, result.nbytes());
            offset += result.size();
            rowList.append(result);
            allResults.append(rowList);
        }

        if (data2 != NULL && nData2 != NULL)
        {
            // This is used to unpack the domains data into the second column 
            offset = 0;
            for ( int i = 0; i < rows; i++){
                dims[0] = (int)nData2[i] / dataCol;
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
        } else if (event.type == EventTypes::Plot){
            plotEvent* pEvent = (plotEvent*)&event;
            PlotEventData eventData;
            
            eventData.modelType = std::string(pEvent->data->modelType);

            eventData.ssubs = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.ssubs.request().ptr, pEvent->data->ssubs, eventData.ssubs.nbytes());

            eventData.resample = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.resample.request().ptr, pEvent->data->resample, eventData.resample.nbytes());

            eventData.dataPresent = py::array_t<double>(pEvent->data->nContrast);
            std::memcpy(eventData.dataPresent.request().ptr, pEvent->data->dataPresent, eventData.dataPresent.nbytes());

            eventData.reflectivity = unpackDataToCell(pEvent->data->nContrast, 1, 
                                                      pEvent->data->reflect, pEvent->data->nReflect, NULL, NULL, 2);

            eventData.shiftedData = unpackDataToCell(pEvent->data->nContrast, 1, 
                                                     pEvent->data->shiftedData, pEvent->data->nShiftedData, NULL, NULL, 3);
            
            eventData.sldProfiles = unpackDataToCell(pEvent->data->nContrast, (pEvent->data->nSldProfiles2 == NULL) ? 1 : 2,
                                                     pEvent->data->sldProfiles, pEvent->data->nSldProfiles, 
                                                     pEvent->data->sldProfiles2, pEvent->data->nSldProfiles2, 2);

            eventData.allLayers = unpackDataToCell(pEvent->data->nContrast, (pEvent->data->nLayers2 == NULL) ? 1 : 2, 
                                                   pEvent->data->layers, pEvent->data->nLayers, 
                                                   pEvent->data->layers2, pEvent->data->nLayers, 2);
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

struct Predlims
{
    py::list refPredInts;
    py::list sldPredInts;
    py::list refXdata;
    py::list sldXdata;
    py::array_t<real_T> sampleChi; 
};

struct BestFitsMean
{
    py::list ref;
    py::list sld;
    real_T chi;
    py::list data;
};

struct ParConfInts
{
    py::array_t<real_T> par95;
    py::array_t<real_T> par65;
    py::array_t<real_T> mean;
};

struct NestOutput
{
    real_T logZ;
    py::array_t<real_T> nestSamples;
    py::array_t<real_T> postSamples;
};

struct DREAMPars
{
    real_T d;
    real_T N;
    real_T T;
    boolean_T parallel;
    real_T CPU;
    real_T lambda;
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
    boolean_T modout;
    boolean_T restart;
    boolean_T save;
    py::array_t<real_T> R;
};

struct MeasInfo
{
    real_T Y;
    real_T N;
};

struct DreamOutput
{
    py::array_t<real_T> outlier;
    real_T runtime;
    DREAMPars DREAMPar;
    MeasInfo Meas_info;
    real_T iteration;
    real_T iloc;
    real_T fx;
    py::array_t<real_T> AR;
    py::array_t<real_T> R_stat;
    py::array_t<real_T> CR;
};

struct BayesOutput
{
    py::array_t<real_T> allChains;
    DreamOutput dreamOutput;
    NestOutput nestOutput;
};

struct BayesResults
{
    BestFitsMean bestFitsMean;
    Predlims predlims;
    ParConfInts parConfInts;
    py::array_t<real_T> bestPars;
    BayesOutput bayesRes;
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
    py::array_t<real_T> allChis;
    real_T sumChi;
};

struct ContrastParams
{
    py::array_t<real_T> ssubs;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> qzshifts;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIn;
    py::array_t<real_T> bulkOut;
    py::array_t<real_T> resolutionParams;
    Calculation calculations {};
    py::array_t<real_T> allSubRough;
    py::array_t<real_T>  resample;
};

struct OutputResult {
    py::list f1;
    py::list f2;
    py::list f3;
    py::list f4;
    py::list f5;
    py::list f6;
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
    py::list f14;
    py::list f15;
    py::list f16;
    py::list f17;
    py::list f18;
    py::list f19;
    py::list f20;
};

struct ProblemDefinition {
    py::array_t<real_T> contrastBackgrounds;
    py::array_t<real_T> contrastBackgroundsType;
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
    py::array_t<real_T> contrastResolutions;
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
    real_T tolX {};
    real_T tolFun {};
    real_T maxFunEvals {};
    real_T maxIter {};
    real_T populationSize {};
    real_T fWeight {};
    real_T crossoverProbability {};
    real_T targetValue {};
    real_T numGenerations {};
    real_T strategy {};
    real_T Nlive {};
    real_T Nmcmc {};
    real_T propScale {};
    real_T nsTolerance {};
    boolean_T calcSldDuringFit {};
    py::array_t<real_T> resamPars;
    real_T updateFreq {};
    real_T updatePlotFreq {};
    real_T nSamples {};
    real_T nChains {};
    real_T jumpProbability {};
    real_T pUnitGamma {};
    std::string boundHandling {};
    boolean_T adaptPCR;
    Checks checks {};
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

coder::array<real_T, 2U> pyArrayToRatArray1d(py::array_t<real_T> value)
{
    coder::array<real_T, 2U> result;

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Number of dimensions must be 1");

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
        throw std::runtime_error("Number of dimensions must be 1");
    
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
        throw std::runtime_error("Number of dimensions must be 2");
    
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
        if (py::len(list) != 4)
            throw std::runtime_error("Number of dimensions for each row must be 4");
        stringToRatCharArray(value[0].cast<std::string>(), result[idx].f1);
        stringToRatCharArray(value[1].cast<std::string>(), result[idx].f2);
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
        std::string value = py::cast<std::string>(list);
        //TODO: validate dimension
        stringToRatCharArray(value, result[idx].f1); 
        idx++;
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
    
    problem_struct.contrastBackgrounds = pyArrayToRatArray1d(problem.contrastBackgrounds);
    problem_struct.contrastBackgroundsType = pyArrayToRatArray1d(problem.contrastBackgroundsType);
    problem_struct.resample = pyArrayToRatArray1d(problem.resample);
    problem_struct.dataPresent = pyArrayToRatArray1d(problem.dataPresent);
    problem_struct.oilChiDataPresent = pyArrayToRatArray1d(problem.oilChiDataPresent);
    problem_struct.contrastQzshifts = pyArrayToRatArray1d(problem.contrastQzshifts);
    problem_struct.contrastScalefactors = pyArrayToRatArray1d(problem.contrastScalefactors);
    problem_struct.contrastBulkIns = pyArrayToRatArray1d(problem.contrastBulkIns);
    problem_struct.contrastBulkOuts = pyArrayToRatArray1d(problem.contrastBulkOuts);
    problem_struct.contrastResolutions = pyArrayToRatArray1d(problem.contrastResolutions);
    problem_struct.backgroundParams = pyArrayToRatArray1d(problem.backgroundParams);
    problem_struct.qzshifts = pyArrayToRatArray1d(problem.qzshifts);
    problem_struct.scalefactors = pyArrayToRatArray1d(problem.scalefactors);
    problem_struct.bulkIn = pyArrayToRatArray1d(problem.bulkIn);
    problem_struct.bulkOut = pyArrayToRatArray1d(problem.bulkOut);
    problem_struct.resolutionParams = pyArrayToRatArray1d(problem.resolutionParams);
    problem_struct.params = pyArrayToRatArray1d(problem.params);

    problem_struct.contrastCustomFiles = pyArrayToRatArray1d(problem.contrastCustomFiles);
    problem_struct.contrastDomainRatios = pyArrayToRatArray1d(problem.contrastDomainRatios);
    problem_struct.domainRatio = pyArrayToRatArray1d(problem.domainRatio);

    problem_struct.fitParams =  pyArrayToRatArray1d(problem.fitParams);
    problem_struct.otherParams =  pyArrayToRatArray1d(problem.otherParams);
    problem_struct.fitLimits =  pyArrayToRatArray2d(problem.fitLimits);
    problem_struct.otherLimits =  pyArrayToRatArray2d(problem.otherLimits);

    return problem_struct;
}

RAT::struct1_T createStruct1(const Limits& limits)
{
    RAT::struct1_T limits_struct;
    limits_struct.param = pyArrayToRatArray2d(limits.param);
    limits_struct.backgroundParam = pyArrayToRatArray2d(limits.backgroundParam);
    limits_struct.qzshift = pyArrayToRatArray2d(limits.qzshift);
    limits_struct.scalefactor = pyArrayToRatArray2d(limits.scalefactor);
    limits_struct.bulkIn = pyArrayToRatArray2d(limits.bulkIn);
    limits_struct.bulkOut = pyArrayToRatArray2d(limits.bulkOut);
    limits_struct.resolutionParam = pyArrayToRatArray2d(limits.resolutionParam);
    limits_struct.domainRatio = pyArrayToRatArray2d(limits.domainRatio);
    
    return limits_struct;
}

RAT::struct3_T createStruct3(const Checks& checks)
{
    RAT::struct3_T checks_struct;
    checks_struct.fitParam = pyArrayToRatArray1d(checks.fitParam);
    checks_struct.fitBackgroundParam = pyArrayToRatArray1d(checks.fitBackgroundParam);
    checks_struct.fitQzshift = pyArrayToRatArray1d(checks.fitQzshift);
    checks_struct.fitScalefactor = pyArrayToRatArray1d(checks.fitScalefactor);
    checks_struct.fitBulkIn = pyArrayToRatArray1d(checks.fitBulkIn);
    checks_struct.fitBulkOut = pyArrayToRatArray1d(checks.fitBulkOut);
    checks_struct.fitResolutionParam = pyArrayToRatArray1d(checks.fitResolutionParam);
    checks_struct.fitDomainRatio = pyArrayToRatArray1d(checks.fitDomainRatio);
    
    return checks_struct;
}

RAT::struct4_T createStruct4(const Priors& priors)
{
    RAT::struct4_T priors_struct;
    priors_struct.param = pyListToUnboundedCell0(priors.param);
    priors_struct.backgroundParam = pyListToUnboundedCell0(priors.backgroundParam);
    priors_struct.resolutionParam = pyListToUnboundedCell0(priors.resolutionParam);
    priors_struct.qzshift = pyListToUnboundedCell0(priors.qzshift);
    priors_struct.scalefactor = pyListToUnboundedCell0(priors.scalefactor);
    priors_struct.bulkIn = pyListToUnboundedCell0(priors.bulkIn);
    priors_struct.bulkOut = pyListToUnboundedCell0(priors.bulkOut);
    priors_struct.domainRatio = pyListToUnboundedCell0(priors.domainRatio);
    priors_struct.priorNames = pyListToUnboundedCell1(priors.priorNames);
    priors_struct.priorValues = pyArrayToRatArray2d(priors.priorValues);
    
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
        result[idx].f1 = pyArrayToRatArray2d(casted_array);
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
        result[idx].f1 = pyArrayToRatArray1d(casted_array);
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
        result[idx].f1 = pyArrayToRatBoundedArray(casted_array);
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
        std::string name = py::cast<std::string>(array);
        stringToRatArray(name, result[idx].f1.data, result[idx].f1.size);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_6, 2U> py_function_array_to_rat_cell_wrap_6(py::list values)
{
    coder::array<RAT::cell_wrap_6, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
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
    cells_struct.f1 = pyListToRatCellWrap2(cells.f1);
    cells_struct.f2 = pyListToRatCellWrap3(cells.f2);
    cells_struct.f3 = pyListToRatCellWrap2(cells.f3);
    cells_struct.f4 = pyListToRatCellWrap2(cells.f4);
    cells_struct.f5 = pyListToRatCellWrap4(cells.f5);
    cells_struct.f6 = pyListToRatCellWrap5(cells.f6);
    cells_struct.f7 = pyListToRatCellWrap6(cells.f7);
    cells_struct.f8 = pyListToRatCellWrap6(cells.f8);
    cells_struct.f9 = pyListToRatCellWrap6(cells.f9);
    cells_struct.f10 = pyListToRatCellWrap6(cells.f10);
    cells_struct.f11 = pyListToRatCellWrap6(cells.f11);
    cells_struct.f12 = pyListToRatCellWrap6(cells.f12);
    cells_struct.f13 = pyListToRatCellWrap6(cells.f13);
    cells_struct.f14 = py_function_array_to_rat_cell_wrap_6(cells.f14);
    cells_struct.f15 = pyListToRatCellWrap6(cells.f15);
    cells_struct.f16 = pyListToRatCellWrap6(cells.f16);
    cells_struct.f17 = pyListToRatCellWrap3(cells.f17);
    cells_struct.f18 = pyListToRatCellWrap2(cells.f18);
    cells_struct.f19 = pyListToRatCellWrap4(cells.f19);
    cells_struct.f20 = pyListToRatCellWrap6(cells.f20);

    return cells_struct;
}

RAT::struct2_T createStruct2T(const Control& control)
{
    RAT::struct2_T control_struct;
    control_struct.tolFun = control.tolFun;
    control_struct.maxFunEvals = control.maxFunEvals;
    control_struct.maxIter = control.maxIter;
    control_struct.populationSize = control.populationSize;
    control_struct.fWeight = control.fWeight;
    control_struct.crossoverProbability = control.crossoverProbability;
    control_struct.targetValue = control.targetValue;
    control_struct.numGenerations = control.numGenerations;
    control_struct.strategy = control.strategy;
    control_struct.Nlive = control.Nlive;
    control_struct.Nmcmc = control.Nmcmc;
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
    control_struct.tolX = control.tolX;
    control_struct.resamPars[0] = control.resamPars.at(0);
    control_struct.resamPars[1] = control.resamPars.at(1);
    stringToRatArray(control.boundHandling, control_struct.boundHandling.data, control_struct.boundHandling.size);
    control_struct.adaptPCR = control.adaptPCR;
    control_struct.checks = createStruct3(control.checks);

    return control_struct;
}

py::array_t<real_T> pyArrayFromRatArray1d(coder::array<real_T, 2U> array)
{
    auto size = (array.size(0) > 1) ?  array.size(0) : array.size(1);
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

py::list resultArrayToList(const RAT::cell_wrap_9 results[])
{
    py::list outer_list_1;
    for (int32_T idx0{0}; idx0 < results[0].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[0].f1.size(1); idx1++) {
            auto tmp = results[0].f1[idx0 +  results[0].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);       
        }
        outer_list_1.append(inner_list);
    }

    py::list outer_list_2;
    for (int32_T idx0{0}; idx0 < results[1].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[1].f1.size(1); idx1++) {
            auto tmp = results[1].f1[idx0 +  results[1].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        outer_list_2.append(inner_list);
    }

    py::list outer_list_3;
    for (int32_T idx0{0}; idx0 < results[2].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[2].f1.size(1); idx1++) {
            auto tmp = results[2].f1[idx0 +  results[2].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        outer_list_3.append(inner_list);
    }

    py::list outer_list_4;
    for (int32_T idx0{0}; idx0 < results[3].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[3].f1.size(1); idx1++) {
            auto tmp = results[3].f1[idx0 +  results[3].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        outer_list_4.append(inner_list);
    }

    py::list outer_list_5;
    for (int32_T idx0{0}; idx0 < results[4].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[4].f1.size(1); idx1++) {
            auto tmp = results[4].f1[idx0 +  results[4].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        outer_list_5.append(inner_list);
    }

    py::list outer_list_6;
    for (int32_T idx0{0}; idx0 < results[5].f1.size(0); idx0++) {
        py::list inner_list;
        for (int32_T idx1{0}; idx1 < results[5].f1.size(1); idx1++) {
            auto tmp = results[5].f1[idx0 +  results[5].f1.size(0) * idx1];
            auto array = py::array_t<real_T, py::array::f_style>({tmp.f1.size(0), tmp.f1.size(1)});
            std::memcpy(array.request().ptr, tmp.f1.data(), array.nbytes());
            inner_list.append(array);
        }
        outer_list_6.append(inner_list);
    }
    py::list output_result;
    output_result.append(outer_list_1);
    output_result.append(outer_list_2);
    output_result.append(outer_list_3);
    output_result.append(outer_list_4);
    output_result.append(outer_list_5);
    output_result.append(outer_list_6);

    return output_result;
}

ContrastParams contrastParamsFromStruct5T(const RAT::struct5_T problem)
{
    // Copy problem to output
    ContrastParams output_problem;
    output_problem.ssubs = py::array_t<real_T>(problem.ssubs.size(0));
    auto buffer = output_problem.ssubs.request();
    std::memcpy(buffer.ptr, problem.ssubs.data(), output_problem.ssubs.size()*sizeof(real_T));

    output_problem.backgroundParams = py::array_t<real_T>(problem.backgroundParams.size(0));
    buffer = output_problem.backgroundParams.request();
    std::memcpy(buffer.ptr, problem.backgroundParams.data(), output_problem.backgroundParams.size()*sizeof(real_T));

    output_problem.qzshifts = py::array_t<real_T>(problem.qzshifts.size(0));
    buffer = output_problem.qzshifts.request();
    std::memcpy(buffer.ptr, problem.qzshifts.data(), output_problem.qzshifts.size()*sizeof(real_T));

    output_problem.scalefactors = py::array_t<real_T>(problem.scalefactors.size(0));
    buffer = output_problem.scalefactors.request();
    std::memcpy(buffer.ptr, problem.scalefactors.data(), output_problem.scalefactors.size()*sizeof(real_T));

    output_problem.bulkIn = py::array_t<real_T>(problem.bulkIn.size(0));
    buffer = output_problem.bulkIn.request();
    std::memcpy(buffer.ptr, problem.bulkIn.data(), output_problem.bulkIn.size()*sizeof(real_T));

    output_problem.bulkOut = py::array_t<real_T>(problem.bulkOut.size(0));
    buffer = output_problem.bulkOut.request();
    std::memcpy(buffer.ptr, problem.bulkOut.data(), output_problem.bulkOut.size()*sizeof(real_T));

    output_problem.resolutionParams = py::array_t<real_T>(problem.resolutionParams.size(0));
    buffer = output_problem.resolutionParams.request();
    std::memcpy(buffer.ptr, problem.resolutionParams.data(), output_problem.resolutionParams.size()*sizeof(real_T));

    output_problem.calculations.sumChi = problem.calculations.sumChi;
    output_problem.calculations.allChis = py::array_t<real_T>(problem.calculations.allChis.size(0));
    buffer = output_problem.calculations.allChis.request();
    std::memcpy(buffer.ptr, problem.calculations.allChis.data(), output_problem.calculations.allChis.size()*sizeof(real_T));

    output_problem.allSubRough = py::array_t<real_T>(problem.allSubRough.size(0));
    buffer = output_problem.allSubRough.request();
    std::memcpy(buffer.ptr, problem.allSubRough.data(), output_problem.allSubRough.size()*sizeof(real_T));

    output_problem.resample = py::array_t<real_T>(problem.resample.size(1));
    buffer = output_problem.resample.request();
    std::memcpy(buffer.ptr, problem.resample.data(), output_problem.resample.size()*sizeof(real_T));

    return output_problem;
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
    
    problem_def.contrastBackgrounds = pyArrayFromRatArray1d(problem.contrastBackgrounds);
    problem_def.contrastBackgroundsType = pyArrayFromRatArray1d(problem.contrastBackgroundsType);
    problem_def.resample = pyArrayFromRatArray1d(problem.resample);
    problem_def.dataPresent = pyArrayFromRatArray1d(problem.dataPresent);
    problem_def.oilChiDataPresent = pyArrayFromRatArray1d(problem.oilChiDataPresent);
    problem_def.contrastQzshifts = pyArrayFromRatArray1d(problem.contrastQzshifts);
    problem_def.contrastScalefactors = pyArrayFromRatArray1d(problem.contrastScalefactors);
    problem_def.contrastBulkIns = pyArrayFromRatArray1d(problem.contrastBulkIns);
    problem_def.contrastBulkOuts = pyArrayFromRatArray1d(problem.contrastBulkOuts);
    problem_def.contrastResolutions = pyArrayFromRatArray1d(problem.contrastResolutions);
    problem_def.backgroundParams = pyArrayFromRatArray1d(problem.backgroundParams);
    problem_def.qzshifts = pyArrayFromRatArray1d(problem.qzshifts);
    problem_def.scalefactors = pyArrayFromRatArray1d(problem.scalefactors);
    problem_def.bulkIn = pyArrayFromRatArray1d(problem.bulkIn);
    problem_def.bulkOut = pyArrayFromRatArray1d(problem.bulkOut);
    problem_def.resolutionParams = pyArrayFromRatArray1d(problem.resolutionParams);
    problem_def.params = pyArrayFromRatArray1d(problem.params);

    problem_def.contrastCustomFiles = pyArrayFromRatArray1d(problem.contrastCustomFiles);
    problem_def.contrastDomainRatios = pyArrayFromRatArray1d(problem.contrastDomainRatios);
    problem_def.domainRatio = pyArrayFromRatArray1d(problem.domainRatio);

    problem_def.fitParams =  pyArrayFromRatArray1d(problem.fitParams);
    problem_def.otherParams =  pyArrayFromRatArray1d(problem.otherParams);
    problem_def.fitLimits =  pyArrayFromRatArray2d(problem.fitLimits);
    problem_def.otherLimits =  pyArrayFromRatArray2d(problem.otherLimits);

    return problem_def;
}

py::list pyList1DFromRatCellWrap8(const coder::array<RAT::cell_wrap_8, 1U>& values)
{
    py::list result;
    
    for (int32_T idx0{0}; idx0 < values.size(0); idx0++) {
        result.append(pyArrayFromRatArray2d(values[idx0].f1));
    }

    return result;
}

py::list pyList2dFromRatCellWrap8(const coder::array<RAT::cell_wrap_8, 2U>& values)
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

BayesResults bayesResultsFromStruct7T(const RAT::struct7_T results)
{
    BayesResults bayesResults;

    bayesResults.bestPars = pyArrayFromRatArray2d(results.bestPars);
    bayesResults.chain = pyArrayFromRatArray2d(results.chain);

    bayesResults.bestFitsMean.ref = pyList1DFromRatCellWrap8(results.bestFitsMean.ref);
    bayesResults.bestFitsMean.sld = pyList2dFromRatCellWrap8(results.bestFitsMean.sld);
    bayesResults.bestFitsMean.chi = results.bestFitsMean.chi;
    bayesResults.bestFitsMean.data = pyList1DFromRatCellWrap8(results.bestFitsMean.data);

    bayesResults.predlims.refPredInts = pyList1DFromRatCellWrap8(results.predlims.refPredInts);
    bayesResults.predlims.sldPredInts = pyList2dFromRatCellWrap8(results.predlims.sldPredInts);
    bayesResults.predlims.refXdata = pyList1DFromRatCellWrap8(results.predlims.refXdata);
    bayesResults.predlims.sldXdata = pyList2dFromRatCellWrap8(results.predlims.sldXdata);
    bayesResults.predlims.sampleChi = pyArray1dFromBoundedArray<coder::bounded_array<real_T, 1000U, 1U>>(results.predlims.sampleChi);

    bayesResults.parConfInts.par95 = pyArrayFromRatArray2d(results.parConfInts.par95);
    bayesResults.parConfInts.par65 = pyArrayFromRatArray2d(results.parConfInts.par65);
    bayesResults.parConfInts.mean = pyArrayFromRatArray2d(results.parConfInts.mean);

    bayesResults.bayesRes.allChains = pyArrayFromRatArray3d(results.bayesRes.allChains);

    bayesResults.bayesRes.nestOutput.logZ = results.bayesRes.nestOutput.LogZ;
    bayesResults.bayesRes.nestOutput.nestSamples = pyArrayFromRatArray2d(results.bayesRes.nestOutput.nestSamples);
    bayesResults.bayesRes.nestOutput.postSamples = pyArrayFromRatArray2d(results.bayesRes.nestOutput.postSamples);

    bayesResults.bayesRes.dreamOutput.runtime = results.bayesRes.dreamOutput.RunTime;
    bayesResults.bayesRes.dreamOutput.iteration = results.bayesRes.dreamOutput.iteration;
    bayesResults.bayesRes.dreamOutput.iloc = results.bayesRes.dreamOutput.iloc;
    bayesResults.bayesRes.dreamOutput.fx = results.bayesRes.dreamOutput.fx;
    bayesResults.bayesRes.dreamOutput.R_stat = pyArrayFromRatArray2d(results.bayesRes.dreamOutput.R_stat);
    bayesResults.bayesRes.dreamOutput.CR = pyArrayFromRatArray2d(results.bayesRes.dreamOutput.CR);
    bayesResults.bayesRes.dreamOutput.AR = pyArray2dFromBoundedArray<coder::bounded_array<real_T, 2000U, 2U>>(results.bayesRes.dreamOutput.AR);
    bayesResults.bayesRes.dreamOutput.outlier = pyArray2dFromBoundedArray<coder::bounded_array<real_T, 2000U, 2U>>(results.bayesRes.dreamOutput.outlier);

    bayesResults.bayesRes.dreamOutput.Meas_info.Y = results.bayesRes.dreamOutput.Meas_info.Y;
    bayesResults.bayesRes.dreamOutput.Meas_info.N = results.bayesRes.dreamOutput.Meas_info.N;

    bayesResults.bayesRes.dreamOutput.DREAMPar.d = results.bayesRes.dreamOutput.DREAMPar.d;
    bayesResults.bayesRes.dreamOutput.DREAMPar.N = results.bayesRes.dreamOutput.DREAMPar.N;
    bayesResults.bayesRes.dreamOutput.DREAMPar.T = results.bayesRes.dreamOutput.DREAMPar.T;
    bayesResults.bayesRes.dreamOutput.DREAMPar.parallel = results.bayesRes.dreamOutput.DREAMPar.parallel;
    bayesResults.bayesRes.dreamOutput.DREAMPar.CPU = results.bayesRes.dreamOutput.DREAMPar.CPU;
    bayesResults.bayesRes.dreamOutput.DREAMPar.lambda = results.bayesRes.dreamOutput.DREAMPar.lambda;
    bayesResults.bayesRes.dreamOutput.DREAMPar.pUnitGamma = results.bayesRes.dreamOutput.DREAMPar.pUnitGamma;
    bayesResults.bayesRes.dreamOutput.DREAMPar.nCR = results.bayesRes.dreamOutput.DREAMPar.nCR;
    bayesResults.bayesRes.dreamOutput.DREAMPar.delta = results.bayesRes.dreamOutput.DREAMPar.delta;
    bayesResults.bayesRes.dreamOutput.DREAMPar.steps = results.bayesRes.dreamOutput.DREAMPar.steps;
    bayesResults.bayesRes.dreamOutput.DREAMPar.zeta = results.bayesRes.dreamOutput.DREAMPar.zeta;
    bayesResults.bayesRes.dreamOutput.DREAMPar.outlier = std::string(results.bayesRes.dreamOutput.DREAMPar.outlier);
    bayesResults.bayesRes.dreamOutput.DREAMPar.adaptPCR = results.bayesRes.dreamOutput.DREAMPar.adaptPCR;
    bayesResults.bayesRes.dreamOutput.DREAMPar.thinning = results.bayesRes.dreamOutput.DREAMPar.thinning;
    bayesResults.bayesRes.dreamOutput.DREAMPar.epsilon = results.bayesRes.dreamOutput.DREAMPar.epsilon;
    bayesResults.bayesRes.dreamOutput.DREAMPar.ABC = results.bayesRes.dreamOutput.DREAMPar.ABC;
    bayesResults.bayesRes.dreamOutput.DREAMPar.IO = results.bayesRes.dreamOutput.DREAMPar.IO;
    bayesResults.bayesRes.dreamOutput.DREAMPar.modout = results.bayesRes.dreamOutput.DREAMPar.modout;
    bayesResults.bayesRes.dreamOutput.DREAMPar.restart = results.bayesRes.dreamOutput.DREAMPar.restart;
    bayesResults.bayesRes.dreamOutput.DREAMPar.save = results.bayesRes.dreamOutput.DREAMPar.save;
    bayesResults.bayesRes.dreamOutput.DREAMPar.R = pyArrayFromRatArray2d(results.bayesRes.dreamOutput.DREAMPar.R);

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

    RAT::cell_wrap_9 results[6];
    RAT::struct5_T problem;
    RAT::struct7_T bayesResults;

    // Call the entry-point
    RAT::RATMain(&problem_def_struct, &cells_struct, &limits_struct, &control_struct,
                 &priors_struct, &problem, results, &bayesResults);
    
    // Copy result to output
    return py::make_tuple(problemDefinitionFromStruct0T(problem_def_struct), 
                          contrastParamsFromStruct5T(problem), 
                          resultArrayToList(results), bayesResultsFromStruct7T(bayesResults));    
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
        .value("Plot", EventTypes::Plot);

    py::class_<DylibEngine>(m, "DylibEngine")
        .def(py::init<std::string, std::string>())
        .def("invoke", &DylibEngine::invoke, py::arg("params"), py::arg("bulkIn"), 
                                           py::arg("bulkOut"), py::arg("contrast"), 
                                           py::arg("domain") = DEFAULT_DOMAIN);

    py::class_<Predlims>(m, "Predlims")
        .def(py::init<>())
        .def_readwrite("refPredInts", &Predlims::refPredInts)
        .def_readwrite("sldPredInts", &Predlims::sldPredInts)
        .def_readwrite("refXdata", &Predlims::refXdata)
        .def_readwrite("sldXdata", &Predlims::sldXdata)
        .def_readwrite("sampleChi", &Predlims::sampleChi);
    
    py::class_<PlotEventData>(m, "PlotEventData")
        .def(py::init<>())
        .def_readonly("reflectivity", &PlotEventData::reflectivity)
        .def_readonly("shiftedData", &PlotEventData::shiftedData)
        .def_readonly("sldProfiles", &PlotEventData::sldProfiles)
        .def_readonly("allLayers", &PlotEventData::allLayers)
        .def_readonly("ssubs", &PlotEventData::ssubs)
        .def_readonly("resample", &PlotEventData::resample)
        .def_readonly("dataPresent", &PlotEventData::dataPresent)
        .def_readonly("modelType", &PlotEventData::modelType);

    py::class_<BestFitsMean>(m, "BestFitsMean")
        .def(py::init<>())
        .def_readwrite("ref", &BestFitsMean::ref)
        .def_readwrite("sld", &BestFitsMean::sld)
        .def_readwrite("chi", &BestFitsMean::chi)
        .def_readwrite("data", &BestFitsMean::data);    

    py::class_<ParConfInts>(m, "ParConfInts")
        .def(py::init<>())
        .def_readwrite("par95", &ParConfInts::par95)
        .def_readwrite("par65", &ParConfInts::par65)
        .def_readwrite("mean", &ParConfInts::mean);

    py::class_<MeasInfo>(m, "MeasInfo")
        .def(py::init<>())
        .def_readwrite("Y", &MeasInfo::Y)
        .def_readwrite("N", &MeasInfo::N);

    py::class_<DREAMPars>(m, "DREAMPars")
        .def(py::init<>())
        .def_readwrite("d", &DREAMPars::d)
        .def_readwrite("N", &DREAMPars::N)
        .def_readwrite("T", &DREAMPars::T)
        .def_readwrite("parallel", &DREAMPars::parallel)
        .def_readwrite("CPU", &DREAMPars::CPU)
        .def_readwrite("lambda_", &DREAMPars::lambda)
        .def_readwrite("pUnitGamma", &DREAMPars::pUnitGamma)
        .def_readwrite("nCR", &DREAMPars::nCR)
        .def_readwrite("delta", &DREAMPars::delta)
        .def_readwrite("steps", &DREAMPars::steps)
        .def_readwrite("zeta", &DREAMPars::zeta)
        .def_readwrite("outlier", &DREAMPars::outlier)
        .def_readwrite("adaptPCR", &DREAMPars::adaptPCR)
        .def_readwrite("thinning", &DREAMPars::thinning)
        .def_readwrite("epsilon", &DREAMPars::epsilon)
        .def_readwrite("ABC", &DREAMPars::ABC)
        .def_readwrite("IO", &DREAMPars::IO)
        .def_readwrite("modout", &DREAMPars::modout)
        .def_readwrite("restart", &DREAMPars::restart)
        .def_readwrite("save", &DREAMPars::save)
        .def_readwrite("R", &DREAMPars::R);

    py::class_<NestOutput>(m, "NestOutput")
        .def(py::init<>())
        .def_readwrite("logZ", &NestOutput::logZ)
        .def_readwrite("nestSamples", &NestOutput::nestSamples)
        .def_readwrite("postSamples", &NestOutput::postSamples);

    py::class_<DreamOutput>(m, "DreamOutput")
        .def(py::init<>())
        .def_readwrite("outlier", &DreamOutput::outlier)
        .def_readwrite("runtime", &DreamOutput::runtime)
        .def_readwrite("DREAMPar", &DreamOutput::DREAMPar)
        .def_readwrite("Meas_info", &DreamOutput::Meas_info)
        .def_readwrite("iteration", &DreamOutput::iteration)
        .def_readwrite("iloc", &DreamOutput::iloc)
        .def_readwrite("fx", &DreamOutput::fx)
        .def_readwrite("AR", &DreamOutput::AR)
        .def_readwrite("R_stat", &DreamOutput::R_stat)
        .def_readwrite("CR", &DreamOutput::CR);

    py::class_<BayesOutput>(m, "BayesOutput")
        .def(py::init<>())
        .def_readwrite("allChains", &BayesOutput::allChains)
        .def_readwrite("dreamOutput", &BayesOutput::dreamOutput)
        .def_readwrite("nestOutput", &BayesOutput::nestOutput);

    py::class_<BayesResults>(m, "BayesResults")
        .def(py::init<>())
        .def_readwrite("bestFitsMean", &BayesResults::bestFitsMean)
        .def_readwrite("predlims", &BayesResults::predlims)
        .def_readwrite("parConfInts", &BayesResults::parConfInts)
        .def_readwrite("bestPars", &BayesResults::bestPars)
        .def_readwrite("bayesRes", &BayesResults::bayesRes)
        .def_readwrite("chain", &BayesResults::chain);

    py::class_<Calculation>(m, "Calculation")
        .def(py::init<>())
        .def_readwrite("allChis", &Calculation::allChis)
        .def_readwrite("sumChi", &Calculation::sumChi);

    py::class_<ContrastParams>(m, "ContrastParams")
        .def(py::init<>())
        .def_readwrite("ssubs", &ContrastParams::ssubs)
        .def_readwrite("backgroundParams", &ContrastParams::backgroundParams)
        .def_readwrite("qzshifts", &ContrastParams::qzshifts)
        .def_readwrite("scalefactors", &ContrastParams::scalefactors)
        .def_readwrite("bulkIn", &ContrastParams::bulkIn)
        .def_readwrite("bulkOut", &ContrastParams::bulkOut)
        .def_readwrite("resolutionParams", &ContrastParams::resolutionParams)
        .def_readwrite("calculations", &ContrastParams::calculations)
        .def_readwrite("allSubRough", &ContrastParams::allSubRough)
        .def_readwrite("resample", &ContrastParams::resample);

    py::class_<OutputResult>(m, "OutputResult")
        .def(py::init<>())
        .def_readwrite("f1", &OutputResult::f1)
        .def_readwrite("f2", &OutputResult::f2)
        .def_readwrite("f3", &OutputResult::f3)
        .def_readwrite("f4", &OutputResult::f4)
        .def_readwrite("f5", &OutputResult::f5)
        .def_readwrite("f6", &OutputResult::f6);

    py::class_<Checks>(m, "Checks")
        .def(py::init<>())
        .def_readwrite("fitParam", &Checks::fitParam)
        .def_readwrite("fitBackgroundParam", &Checks::fitBackgroundParam)
        .def_readwrite("fitQzshift", &Checks::fitQzshift)
        .def_readwrite("fitScalefactor", &Checks::fitScalefactor)
        .def_readwrite("fitBulkIn", &Checks::fitBulkIn)
        .def_readwrite("fitBulkOut", &Checks::fitBulkOut)
        .def_readwrite("fitResolutionParam", &Checks::fitResolutionParam)
        .def_readwrite("fitDomainRatio", &Checks::fitDomainRatio);

    py::class_<Limits>(m, "Limits")
        .def(py::init<>())
        .def_readwrite("param", &Limits::param)
        .def_readwrite("backgroundParam", &Limits::backgroundParam)
        .def_readwrite("qzshift", &Limits::qzshift)
        .def_readwrite("scalefactor", &Limits::scalefactor)
        .def_readwrite("bulkIn", &Limits::bulkIn)
        .def_readwrite("bulkOut", &Limits::bulkOut)
        .def_readwrite("resolutionParam", &Limits::resolutionParam)
        .def_readwrite("domainRatio", &Limits::domainRatio);

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
        .def_readwrite("priorValues", &Priors::priorValues);

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
        .def_readwrite("f20", &Cells::f20);

    py::class_<Control>(m, "Control")
        .def(py::init<>())
        .def_readwrite("parallel", &Control::parallel)
        .def_readwrite("procedure", &Control::procedure)
        .def_readwrite("display", &Control::display)
        .def_readwrite("tolX", &Control::tolX)
        .def_readwrite("tolFun", &Control::tolFun)
        .def_readwrite("maxFunEvals", &Control::maxFunEvals)
        .def_readwrite("maxIter", &Control::maxIter)
        .def_readwrite("populationSize", &Control::populationSize)
        .def_readwrite("fWeight", &Control::fWeight)  
        .def_readwrite("crossoverProbability", &Control::crossoverProbability)  
        .def_readwrite("targetValue", &Control::targetValue)
        .def_readwrite("numGenerations", &Control::numGenerations)
        .def_readwrite("strategy", &Control::strategy)
        .def_readwrite("Nlive", &Control::Nlive)
        .def_readwrite("Nmcmc", &Control::Nmcmc)
        .def_readwrite("propScale", &Control::propScale)
        .def_readwrite("nsTolerance", &Control::nsTolerance)
        .def_readwrite("calcSldDuringFit", &Control::calcSldDuringFit)
        .def_readwrite("resamPars", &Control::resamPars)
        .def_readwrite("updateFreq", &Control::updateFreq)
        .def_readwrite("updatePlotFreq", &Control::updatePlotFreq)
        .def_readwrite("nSamples", &Control::nSamples)
        .def_readwrite("nChains", &Control::nChains)
        .def_readwrite("jumpProbability", &Control::jumpProbability)
        .def_readwrite("pUnitGamma", &Control::pUnitGamma)
        .def_readwrite("boundHandling", &Control::boundHandling)
        .def_readwrite("adaptPCR", &Control::adaptPCR)
        .def_readwrite("checks", &Control::checks);

    py::class_<ProblemDefinition>(m, "ProblemDefinition")
        .def(py::init<>())
        .def_readwrite("contrastBackgrounds", &ProblemDefinition::contrastBackgrounds)
        .def_readwrite("contrastBackgroundsType", &ProblemDefinition::contrastBackgroundsType)
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
        .def_readwrite("contrastResolutions", &ProblemDefinition::contrastResolutions)
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
        .def_readwrite("otherLimits", &ProblemDefinition::otherLimits);

    m.def("RATMain", &RATMain, "A demo for Python RAT");    
}
