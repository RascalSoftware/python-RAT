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
    real_T logZErr;
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
    py::list params;
    py::list backgroundParams;
    py::list scalefactors;
    py::list qzshifts;
    py::list bulkIns;
    py::list bulkOuts;
    py::list resolutionParams;
    py::list domainRatios;
    py::list priorNames;
    py::array_t<real_T> priorValues;
};

struct Checks {
    py::array_t<real_T> params;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> qzshifts;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIns;
    py::array_t<real_T> bulkOuts;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> domainRatios;
};

struct Calculation
{
    py::array_t<real_T> chiValues;
    real_T sumChi;
};

struct ContrastParams
{
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
    py::list backgrounds;
    py::list layerSlds;
    py::list sldProfiles;
    py::list resampledLayers;
    Calculation calculationResults {};
    ContrastParams contrastParams {};
    py::array_t<real_T> fitParams;
    py::list fitNames;
};

struct Limits {
    py::array_t<real_T> params;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> qzshifts;
    py::array_t<real_T> bulkIns;
    py::array_t<real_T> bulkOuts;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> domainRatios;
};

struct NameStore {
    py::list params;
    py::list backgroundParams;
    py::list scalefactors;
    py::list qzshifts;
    py::list bulkIns;
    py::list bulkOuts;
    py::list resolutionParams;
    py::list domainRatios;
    py::list contrasts;
};

struct ProblemDefinition {
    std::string  TF {};
    py::array_t<real_T> resample;
    py::list data;
    py::array_t<real_T> dataPresent;
    py::list dataLimits;
    py::list simulationLimits;
    py::array_t<real_T> oilChiDataPresent;
    real_T numberOfContrasts;
    std::string  geometry {};
    boolean_T useImaginary {};
    py::list repeatLayers;
    py::list contrastBackgroundParams;
    py::list contrastBackgroundTypes;
    py::list contrastBackgroundActions;
    py::array_t<real_T> contrastQzshifts;
    py::array_t<real_T> contrastScalefactors;
    py::array_t<real_T> contrastBulkIns;
    py::array_t<real_T> contrastBulkOuts;
    py::array_t<real_T> contrastResolutionParams;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> qzshifts;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIns;
    py::array_t<real_T> bulkOuts;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> params;
    real_T numberOfLayers {};
    py::list contrastLayers;
    py::list layersDetails;
    py::object customFiles;
    std::string  modelType {};
    py::array_t<real_T> contrastCustomFiles;
    py::array_t<real_T> contrastDomainRatios;
    py::array_t<real_T> domainRatios;
    real_T numberOfDomainContrasts {};
    py::list domainContrastLayers;
    py::array_t<real_T> fitParams;
    py::array_t<real_T> otherParams;
    py::array_t<real_T> fitLimits;
    py::array_t<real_T> otherLimits;
    NameStore names;
    Checks checks {};
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
    std::string IPCFilePath {};
};


void stringToRatBoundedArray(std::string value, char_T result_data[], int32_T result_size[2])
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

void stringFromRatBoundedArray(const char_T array_data[], const int32_T array_size[2], std::string& result) 
{
    result.resize(array_size[1]);
    memcpy(&result[0], array_data, array_size[1]);
}

coder::array<real_T, 2U> pyArrayToRatRowArray1d(py::array_t<real_T> value)
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

coder::bounded_array<real_T, 5U, 2U> pyArrayToRatBoundedArray3(py::array_t<real_T> value)
{
    coder::bounded_array<real_T, 5U, 2U> result {};

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

coder::array<RAT::cell_6, 1U> pyListToUnboundedCell0(py::list values)
{
    coder::array<RAT::cell_6, 1U> result;
    result.set_size(values.size());
    int32_T idx {0};
    for (py::handle list: values)
    { 
        py::list value = py::cast<py::list>(list);
        if (py::len(list) != 4 || !py::isinstance<py::str>(value[0]) || !py::isinstance<py::str>(value[1]) ||
                !py::isinstance<py::float_>(value[2]) || !py::isinstance<py::float_>(value[3]))
            throw std::runtime_error("Expects a 2D list where each row must contain 4 elements. " 
                                     "Columns 1 and 2 must be strings and Columns 3 and 4 must be numeric arrays");

        stringToRatBoundedArray(value[0].cast<std::string>(), result[idx].f1.data, result[idx].f1.size);
        stringToRatBoundedArray(value[1].cast<std::string>(), result[idx].f2.data, result[idx].f2.size);
        result[idx].f3 = value[2].cast<real_T>();
        result[idx].f4 = value[3].cast<real_T>();
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_7, 1U> pyListToUnboundedCell1(py::list values)
{
    coder::array<RAT::cell_wrap_7, 1U> result;
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
coder::array<RAT::cell_wrap_1, 2U> pyListToRatCellWrap1(py::list values)
{
    coder::array<RAT::cell_wrap_1, 2U> result;
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
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatBoundedArray3, casted_array);
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
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatRowArray1d, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_5, 2U> pyListToRatCellWrap5(py::list values)
{
    coder::array<RAT::cell_wrap_5, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatBoundedArray, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_0, 2U> pyListToRatCellWrap0(py::list values)
{
    coder::array<RAT::cell_wrap_0, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        if (py::isinstance<py::str>(array)) {
            std::string name = py::cast<std::string>(array);
            stringToRatBoundedArray(name, result[idx].f1.data, result[idx].f1.size);
            idx++;
        }
        else
            throw std::runtime_error("Expects a 1D list of strings");
    }

    return result;
}

coder::array<RAT::cell_wrap_0, 2U> py_function_array_to_rat_cell_wrap_0(py::object values)
{
    auto handles = py::cast<py::list>(values);
    coder::array<RAT::cell_wrap_0, 2U> result;
    result.set_size(1, handles.size());
    int32_T idx {0};
    for (py::handle array: handles)
    { 
        auto func = py::cast<py::function>(array);
        std::string func_ptr = convertPtr2String<CallbackInterface>(new Library(func));
        stringToRatBoundedArray(func_ptr, result[idx].f1.data, result[idx].f1.size);
        idx++;
    }

    return result;
}

RAT::struct1_T createStruct1(const NameStore& names)
{
    RAT::struct1_T names_struct;
    names_struct.params = customCaller("NameStore.params", pyListToRatCellWrap0, names.params);
    names_struct.backgroundParams = customCaller("NameStore.backgroundParams", pyListToRatCellWrap0, names.backgroundParams);
    names_struct.scalefactors = customCaller("NameStore.scalefactors", pyListToRatCellWrap0, names.scalefactors);
    names_struct.qzshifts = customCaller("NameStore.qzshifts", pyListToRatCellWrap0, names.qzshifts);
    names_struct.bulkIns = customCaller("NameStore.bulkIns", pyListToRatCellWrap0, names.bulkIns);
    names_struct.bulkOuts = customCaller("NameStore.bulkOuts", pyListToRatCellWrap0, names.bulkOuts);
    names_struct.resolutionParams = customCaller("NameStore.resolutionParams", pyListToRatCellWrap0, names.resolutionParams);
    names_struct.domainRatios = customCaller("NameStore.domainRatios", pyListToRatCellWrap0, names.domainRatios);
    names_struct.contrasts = customCaller("NameStore.contrasts", pyListToRatCellWrap0, names.contrasts);
    
    return names_struct;
}

RAT::struct2_T createStruct2(const Checks& checks)
{
    RAT::struct2_T checks_struct;
    checks_struct.params = customCaller("Checks.params", pyArrayToRatRowArray1d, checks.params);
    checks_struct.backgroundParams = customCaller("Checks.backgroundParams", pyArrayToRatRowArray1d, checks.backgroundParams);
    checks_struct.scalefactors = customCaller("Checks.scalefactors", pyArrayToRatRowArray1d, checks.scalefactors);
    checks_struct.qzshifts = customCaller("Checks.qzshifts", pyArrayToRatRowArray1d, checks.qzshifts);
    checks_struct.bulkIns = customCaller("Checks.bulkIns", pyArrayToRatRowArray1d, checks.bulkIns);
    checks_struct.bulkOuts = customCaller("Checks.bulkOuts", pyArrayToRatRowArray1d, checks.bulkOuts);
    checks_struct.resolutionParams = customCaller("Checks.resolutionParams", pyArrayToRatRowArray1d, checks.resolutionParams);
    checks_struct.domainRatios = customCaller("Checks.domainRatios", pyArrayToRatRowArray1d, checks.domainRatios);
    
    return checks_struct;
}

RAT::struct0_T createStruct0(const ProblemDefinition& problem)
{
    RAT::struct0_T problem_struct;
    
    stringToRatBoundedArray(problem.TF, problem_struct.TF.data, problem_struct.TF.size);
    problem_struct.resample = customCaller("Problem.resample", pyArrayToRatRowArray1d, problem.resample);
    problem_struct.data = customCaller("Problem.data", pyListToRatCellWrap1, problem.data);
    problem_struct.dataPresent = customCaller("Problem.dataPresent", pyArrayToRatRowArray1d, problem.dataPresent);
    problem_struct.dataLimits = customCaller("Problem.dataLimits", pyListToRatCellWrap2, problem.dataLimits);
    problem_struct.simulationLimits = customCaller("Problem.simulationLimits", pyListToRatCellWrap2, problem.simulationLimits);   
    problem_struct.oilChiDataPresent = customCaller("Problem.oilChiDataPresent", pyArrayToRatRowArray1d, problem.oilChiDataPresent);
    problem_struct.numberOfContrasts = problem.numberOfContrasts;
    stringToRatBoundedArray(problem.geometry, problem_struct.geometry.data, problem_struct.geometry.size);
    problem_struct.useImaginary = problem.useImaginary;
    problem_struct.repeatLayers = customCaller("Problem.repeatLayers", pyListToRatCellWrap2, problem.repeatLayers);
    problem_struct.contrastBackgroundParams = customCaller("Problem.contrastBackgroundParams", pyListToRatCellWrap3, problem.contrastBackgroundParams); 
    problem_struct.contrastBackgroundTypes = customCaller("Problem.contrastBackgroundTypes", pyListToRatCellWrap0, problem.contrastBackgroundTypes);
    problem_struct.contrastBackgroundActions = customCaller("Problem.contrastBackgroundActions", pyListToRatCellWrap0, problem.contrastBackgroundActions);
    problem_struct.contrastQzshifts = customCaller("Problem.contrastQzshifts", pyArrayToRatRowArray1d, problem.contrastQzshifts);
    problem_struct.contrastScalefactors = customCaller("Problem.contrastScalefactors", pyArrayToRatRowArray1d, problem.contrastScalefactors);
    problem_struct.contrastBulkIns = customCaller("Problem.contrastBulkIns", pyArrayToRatRowArray1d, problem.contrastBulkIns);
    problem_struct.contrastBulkOuts = customCaller("Problem.contrastBulkOuts", pyArrayToRatRowArray1d, problem.contrastBulkOuts);
    problem_struct.contrastResolutionParams = customCaller("Problem.contrastResolutionParams", pyArrayToRatRowArray1d, problem.contrastResolutionParams);
    problem_struct.backgroundParams = customCaller("Problem.backgroundParams", pyArrayToRatRowArray1d, problem.backgroundParams);
    problem_struct.qzshifts = customCaller("Problem.qzshifts", pyArrayToRatRowArray1d, problem.qzshifts);
    problem_struct.scalefactors = customCaller("Problem.scalefactors", pyArrayToRatRowArray1d, problem.scalefactors);
    problem_struct.bulkIns = customCaller("Problem.bulkIns", pyArrayToRatRowArray1d, problem.bulkIns);
    problem_struct.bulkOuts = customCaller("Problem.bulkOuts", pyArrayToRatRowArray1d, problem.bulkOuts);
    problem_struct.resolutionParams = customCaller("Problem.resolutionParams", pyArrayToRatRowArray1d, problem.resolutionParams);
    problem_struct.params = customCaller("Problem.params", pyArrayToRatRowArray1d, problem.params);
    problem_struct.numberOfLayers = problem.numberOfLayers;
    problem_struct.contrastLayers = customCaller("Problem.contrastLayers", pyListToRatCellWrap4, problem.contrastLayers);
    problem_struct.layersDetails = customCaller("Problem.layersDetails", pyListToRatCellWrap5, problem.layersDetails);
    problem_struct.customFiles = customCaller("Problem.customFiles", py_function_array_to_rat_cell_wrap_0, problem.customFiles);   
    stringToRatBoundedArray(problem.modelType, problem_struct.modelType.data, problem_struct.modelType.size);
    problem_struct.contrastCustomFiles = customCaller("Problem.contrastCustomFiles", pyArrayToRatRowArray1d, problem.contrastCustomFiles);
    problem_struct.contrastDomainRatios = customCaller("Problem.contrastDomainRatios", pyArrayToRatRowArray1d, problem.contrastDomainRatios);
    problem_struct.domainRatios = customCaller("Problem.domainRatios", pyArrayToRatRowArray1d, problem.domainRatios);
    problem_struct.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_struct.domainContrastLayers = customCaller("Problem.domainContrastLayers", pyListToRatCellWrap4, problem.domainContrastLayers);
    problem_struct.fitParams =  customCaller("Problem.fitParams", pyArrayToRatRowArray1d, problem.fitParams);
    problem_struct.otherParams =  customCaller("Problem.otherParams", pyArrayToRatRowArray1d, problem.otherParams);
    problem_struct.fitLimits =  customCaller("Problem.fitLimits", pyArrayToRatArray2d, problem.fitLimits);
    problem_struct.otherLimits =  customCaller("Problem.otherLimits", pyArrayToRatArray2d, problem.otherLimits);

    problem_struct.names = createStruct1(problem.names);
    problem_struct.checks = createStruct2(problem.checks);


    return problem_struct;
}


RAT::struct3_T createStruct3(const Limits& limits)
{
    RAT::struct3_T limits_struct;
    limits_struct.params = customCaller("Limits.params", pyArrayToRatArray2d, limits.params);
    limits_struct.backgroundParams = customCaller("Limits.backgroundParams", pyArrayToRatArray2d, limits.backgroundParams);
    limits_struct.scalefactors = customCaller("Limits.scalefactors", pyArrayToRatArray2d, limits.scalefactors);
    limits_struct.qzshifts = customCaller("Limits.qzshifts", pyArrayToRatArray2d, limits.qzshifts);
    limits_struct.bulkIns = customCaller("Limits.bulkIns", pyArrayToRatArray2d, limits.bulkIns);
    limits_struct.bulkOuts = customCaller("Limits.bulkOuts", pyArrayToRatArray2d, limits.bulkOuts);
    limits_struct.resolutionParams = customCaller("Limits.resolutionParams", pyArrayToRatArray2d, limits.resolutionParams);
    limits_struct.domainRatios = customCaller("Limits.domainRatios", pyArrayToRatArray2d, limits.domainRatios);
    
    return limits_struct;
}

RAT::struct5_T createStruct5(const Priors& priors)
{
    RAT::struct5_T priors_struct;
    priors_struct.params = customCaller("Priors.params", pyListToUnboundedCell0, priors.params);
    priors_struct.backgroundParams = customCaller("Priors.backgroundParams", pyListToUnboundedCell0, priors.backgroundParams);
    priors_struct.scalefactors = customCaller("Priors.scalefactors", pyListToUnboundedCell0, priors.scalefactors);
    priors_struct.qzshifts = customCaller("Priors.qzshifts", pyListToUnboundedCell0, priors.qzshifts);
    priors_struct.bulkIns = customCaller("Priors.bulkIns", pyListToUnboundedCell0, priors.bulkIns);
    priors_struct.bulkOuts = customCaller("Priors.bulkOuts", pyListToUnboundedCell0, priors.bulkOuts);
    priors_struct.resolutionParams = customCaller("Priors.resolutionParams", pyListToUnboundedCell0, priors.resolutionParams);
    priors_struct.domainRatios = customCaller("Priors.domainRatios", pyListToUnboundedCell0, priors.domainRatios);
    priors_struct.priorNames = customCaller("Priors.priorNames", pyListToUnboundedCell1, priors.priorNames);
    priors_struct.priorValues =  customCaller("Priors.priorValues", pyArrayToRatArray2d, priors.priorValues);
    
    return priors_struct;
}

RAT::struct4_T createStruct4(const Control& control)
{
    RAT::struct4_T control_struct;
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
    stringToRatBoundedArray(control.parallel, control_struct.parallel.data, control_struct.parallel.size);
    stringToRatBoundedArray(control.procedure, control_struct.procedure.data, control_struct.procedure.size);
    stringToRatBoundedArray(control.display, control_struct.display.data, control_struct.display.size);
    control_struct.xTolerance = control.xTolerance;
    control_struct.resampleMinAngle = control.resampleMinAngle;
    control_struct.resampleNPoints = control.resampleNPoints;
    stringToRatBoundedArray(control.boundHandling, control_struct.boundHandling.data, control_struct.boundHandling.size);
    control_struct.adaptPCR = control.adaptPCR;
    stringToRatBoundedArray(control.IPCFilePath, control_struct.IPCFilePath.data, control_struct.IPCFilePath.size);

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

py::list pyListFromRatCellWrap0(coder::array<RAT::cell_wrap_0, 2U> values)
{
    py::list result;
    for (int32_T idx0{0}; idx0 < values.size(1); idx0++) {
        std::string tmp;
        stringToRatBoundedArray(tmp, values[idx0].f1.data, values[idx0].f1.size);
        result.append(tmp);
    }

    return result;
}

py::list pyListFromRatCellWrap2(coder::array<RAT::cell_wrap_2, 2U> values)
{
    py::list result;
    
    for (int32_T idx0{0}; idx0 < values.size(1); idx0++) {
        py::list inner = py::make_tuple(values[idx0].f1[0], values[idx0].f1[1]);
        result.append(inner);
    }

    return result;
}

template <typename T> 
py::list pyList1DFromRatCellWrap2D(const T& values)
{
    py::list result;
    
    for (int32_T idx0{0}; idx0 < values.size(1); idx0++) {
        result.append(pyArrayFromRatArray2d(values[idx0].f1));
    }

    return result;
}

template <typename T>
py::list pyList1DFromRatCellWrap1D(const T& values)
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
py::list pyListFromBoundedCellWrap(const T& values)
{
    py::list result;
    
    for (int32_T idx0{0}; idx0 < values.size(1); idx0++) {
        auto array = py::array_t<real_T, py::array::f_style>({values[idx0].f1.size[0]});
        std::memcpy(array.request().ptr, values[idx0].f1.data, array.nbytes());
    
        result.append(array);
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

OutputResult OutputResultFromStruct6T(const RAT::struct6_T result)
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
    
    stringFromRatBoundedArray(problem.TF.data, problem.TF.size, problem_def.TF);    
    problem_def.resample = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resample);
    problem_def.data = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_1, 2U>>(problem.data);
    problem_def.dataPresent = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.dataPresent);
    problem_def.dataLimits =  pyListFromRatCellWrap2(problem.dataLimits);
    problem_def.simulationLimits =  pyListFromRatCellWrap2(problem.simulationLimits);
    problem_def.oilChiDataPresent = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.oilChiDataPresent);
    problem_def.numberOfContrasts = problem.numberOfContrasts;
    stringFromRatBoundedArray(problem.geometry.data, problem.geometry.size, problem_def.geometry);
    problem_def.useImaginary = problem.useImaginary;       
    problem_def.repeatLayers = pyListFromRatCellWrap2(problem.repeatLayers);
    problem_def.contrastBackgroundParams = pyListFromBoundedCellWrap<coder::array<RAT::cell_wrap_3, 2U>>(problem.contrastBackgroundParams);
    problem_def.contrastBackgroundTypes = pyListFromRatCellWrap0(problem.contrastBackgroundTypes);
    problem_def.contrastBackgroundActions = pyListFromRatCellWrap0(problem.contrastBackgroundActions);    
    problem_def.contrastQzshifts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastQzshifts);
    problem_def.contrastScalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastScalefactors);
    problem_def.contrastBulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkIns);
    problem_def.contrastBulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastBulkOuts);
    problem_def.contrastResolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastResolutionParams);
    problem_def.backgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.backgroundParams);
    problem_def.qzshifts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.qzshifts);
    problem_def.scalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.scalefactors);
    problem_def.bulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkIns);
    problem_def.bulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.bulkOuts);
    problem_def.resolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.resolutionParams);
    problem_def.params = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.params);
    problem_def.numberOfLayers = problem.numberOfLayers;
    problem_def.contrastLayers = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_4, 2U>>(problem.contrastLayers);
    problem_def.layersDetails = pyListFromBoundedCellWrap<coder::array<RAT::cell_wrap_5, 2U>>(problem.layersDetails);
    // problem_def.customFiles is not set here since the object has been converted to function handles 
    stringFromRatBoundedArray(problem.modelType.data, problem.modelType.size, problem_def.modelType);
    problem_def.contrastCustomFiles = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastCustomFiles);
    problem_def.contrastDomainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.contrastDomainRatios);
    problem_def.domainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.domainRatios);
    problem_def.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_def.domainContrastLayers = pyList1DFromRatCellWrap2D<coder::array<RAT::cell_wrap_4, 2U>>(problem.domainContrastLayers);
    problem_def.fitParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.fitParams);
    problem_def.otherParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.otherParams);
    problem_def.fitLimits = pyArrayFromRatArray2d(problem.fitLimits);
    problem_def.otherLimits = pyArrayFromRatArray2d(problem.otherLimits);
    
    problem_def.names.params = pyListFromRatCellWrap0(problem.names.params);
    problem_def.names.backgroundParams = pyListFromRatCellWrap0(problem.names.backgroundParams);
    problem_def.names.scalefactors = pyListFromRatCellWrap0(problem.names.scalefactors);
    problem_def.names.qzshifts = pyListFromRatCellWrap0(problem.names.qzshifts);
    problem_def.names.bulkIns = pyListFromRatCellWrap0(problem.names.bulkIns);
    problem_def.names.bulkOuts = pyListFromRatCellWrap0(problem.names.bulkOuts);
    problem_def.names.resolutionParams = pyListFromRatCellWrap0(problem.names.resolutionParams);
    problem_def.names.domainRatios = pyListFromRatCellWrap0(problem.names.domainRatios);
    problem_def.names.contrasts = pyListFromRatCellWrap0(problem.names.contrasts);
    
    problem_def.checks.params = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.params);
    problem_def.checks.backgroundParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.backgroundParams);
    problem_def.checks.scalefactors = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.scalefactors);
    problem_def.checks.qzshifts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.qzshifts);
    problem_def.checks.bulkIns = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.bulkIns);
    problem_def.checks.bulkOuts = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.bulkOuts);
    problem_def.checks.resolutionParams = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.resolutionParams);
    problem_def.checks.domainRatios = pyArrayFromRatArray1d<coder::array<real_T, 2U>>(problem.checks.domainRatios);

    return problem_def;
}

BayesResults bayesResultsFromStruct9T(const RAT::struct9_T results)
{
    BayesResults bayesResults;

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

py::tuple RATMain(const ProblemDefinition& problem_def, const Limits& limits, const Control& control, const Priors& priors)
{
    RAT::struct0_T problem_def_struct = createStruct0(problem_def);
    RAT::struct3_T limits_struct = createStruct3(limits);
    RAT::struct4_T control_struct = createStruct4(control);
    RAT::struct5_T priors_struct = createStruct5(priors);
    // Output
    RAT::struct6_T results;
    RAT::struct9_T bayesResults;
    // Call the entry-point
    RAT::RATMain(&problem_def_struct, &limits_struct, &control_struct, &priors_struct, &results, &bayesResults);
    // Copy result to output
    auto out_problem_def = problemDefinitionFromStruct0T(problem_def_struct);
    out_problem_def.customFiles = problem_def.customFiles.attr("copy")(); 
    return py::make_tuple(out_problem_def, 
                          OutputResultFromStruct6T(results), 
                          bayesResultsFromStruct9T(bayesResults));    
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
        .def_readwrite("logZErr", &NestedSamplerOutput::logZErr)
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
        .def_readwrite("backgrounds", &OutputResult::backgrounds)
        .def_readwrite("layerSlds", &OutputResult::layerSlds)
        .def_readwrite("sldProfiles", &OutputResult::sldProfiles)
        .def_readwrite("resampledLayers", &OutputResult::resampledLayers)
        .def_readwrite("calculationResults", &OutputResult::calculationResults)
        .def_readwrite("contrastParams", &OutputResult::contrastParams)        
        .def_readwrite("fitParams", &OutputResult::fitParams)
        .def_readwrite("fitNames", &OutputResult::fitNames);

    py::class_<NameStore>(m, "NameStore")
        .def(py::init<>())
        .def_readwrite("params", &NameStore::params)
        .def_readwrite("backgroundParams", &NameStore::backgroundParams)
        .def_readwrite("scalefactors", &NameStore::scalefactors)
        .def_readwrite("qzshifts", &NameStore::qzshifts)
        .def_readwrite("bulkIns", &NameStore::bulkIns)
        .def_readwrite("bulkOuts", &NameStore::bulkOuts)
        .def_readwrite("resolutionParams", &NameStore::resolutionParams)
        .def_readwrite("domainRatios", &NameStore::domainRatios)
        .def_readwrite("contrasts", &NameStore::contrasts)
        .def(py::pickle(
            [](const NameStore &names) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(names.params, names.backgroundParams, names.scalefactors, names.qzshifts, names.bulkIns, names.bulkOuts, names.resolutionParams, 
                                      names.domainRatios, names.contrasts);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 9)
                    throw std::runtime_error("Encountered invalid state unpickling NameStore object!");

                /* Create a new C++ instance */
                NameStore names;

                names.params = t[0].cast<py::list>(); 
                names.backgroundParams = t[1].cast<py::list>(); 
                names.scalefactors = t[2].cast<py::list>();
                names.qzshifts = t[3].cast<py::list>();
                names.bulkIns = t[4].cast<py::list>();
                names.bulkOuts = t[5].cast<py::list>();
                names.resolutionParams = t[6].cast<py::list>();
                names.domainRatios = t[7].cast<py::list>();
                names.contrasts = t[8].cast<py::list>();

                return names;
            }));

    py::class_<Checks>(m, "Checks")
        .def(py::init<>())
        .def_readwrite("params", &Checks::params)
        .def_readwrite("backgroundParams", &Checks::backgroundParams)
        .def_readwrite("scalefactors", &Checks::scalefactors)
        .def_readwrite("qzshifts", &Checks::qzshifts)
        .def_readwrite("bulkIns", &Checks::bulkIns)
        .def_readwrite("bulkOuts", &Checks::bulkOuts)
        .def_readwrite("resolutionParams", &Checks::resolutionParams)
        .def_readwrite("domainRatios", &Checks::domainRatios)
        .def(py::pickle(
            [](const Checks &chk) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(chk.params, chk.backgroundParams, chk.scalefactors, chk.qzshifts, chk.bulkIns, chk.bulkOuts, 
                                      chk.resolutionParams, chk.domainRatios);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 8)
                    throw std::runtime_error("Encountered invalid state unpickling Checks object!");

                /* Create a new C++ instance */
                Checks chk;

                chk.params = t[0].cast<py::array_t<real_T>>(); 
                chk.backgroundParams = t[1].cast<py::array_t<real_T>>(); 
                chk.scalefactors = t[2].cast<py::array_t<real_T>>();
                chk.qzshifts = t[3].cast<py::array_t<real_T>>(); 
                chk.bulkIns = t[4].cast<py::array_t<real_T>>(); 
                chk.bulkOuts = t[5].cast<py::array_t<real_T>>(); 
                chk.resolutionParams = t[6].cast<py::array_t<real_T>>(); 
                chk.domainRatios = t[7].cast<py::array_t<real_T>>();

                return chk;
            }));

    py::class_<Limits>(m, "Limits")
        .def(py::init<>())
        .def_readwrite("params", &Limits::params)
        .def_readwrite("backgroundParams", &Limits::backgroundParams)
        .def_readwrite("scalefactors", &Limits::scalefactors)
        .def_readwrite("qzshifts", &Limits::qzshifts)
        .def_readwrite("bulkIns", &Limits::bulkIns)
        .def_readwrite("bulkOuts", &Limits::bulkOuts)
        .def_readwrite("resolutionParams", &Limits::resolutionParams)
        .def_readwrite("domainRatios", &Limits::domainRatios)
        .def(py::pickle(
            [](const Limits &lim) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(lim.params, lim.backgroundParams, lim.scalefactors, lim.qzshifts, lim.bulkIns, lim.bulkOuts, 
                                      lim.resolutionParams, lim.domainRatios);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 8)
                    throw std::runtime_error("Encountered invalid state unpickling Limits object!");

                /* Create a new C++ instance */
                Limits lim;

                lim.params = t[0].cast<py::array_t<real_T>>(); 
                lim.backgroundParams = t[1].cast<py::array_t<real_T>>(); 
                lim.scalefactors = t[2].cast<py::array_t<real_T>>(); 
                lim.qzshifts = t[3].cast<py::array_t<real_T>>(); 
                lim.bulkIns = t[4].cast<py::array_t<real_T>>(); 
                lim.bulkOuts = t[5].cast<py::array_t<real_T>>(); 
                lim.resolutionParams = t[6].cast<py::array_t<real_T>>(); 
                lim.domainRatios = t[7].cast<py::array_t<real_T>>();

                return lim;
            }));

    py::class_<Priors>(m, "Priors")
        .def(py::init<>())
        .def_readwrite("params", &Priors::params)
        .def_readwrite("backgroundParams", &Priors::backgroundParams)
        .def_readwrite("scalefactors", &Priors::scalefactors)
        .def_readwrite("qzshifts", &Priors::qzshifts)
        .def_readwrite("bulkIns", &Priors::bulkIns)
        .def_readwrite("bulkOuts", &Priors::bulkOuts)
        .def_readwrite("resolutionParams", &Priors::resolutionParams)
        .def_readwrite("domainRatios", &Priors::domainRatios)
        .def_readwrite("priorNames", &Priors::priorNames)
        .def_readwrite("priorValues", &Priors::priorValues)
        .def(py::pickle(
            [](const Priors &prior) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(prior.params, prior.backgroundParams, prior.scalefactors, prior.qzshifts, prior.bulkIns, 
                                      prior.bulkOuts, prior.resolutionParams, prior.domainRatios, prior.priorNames, prior.priorValues);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 10)
                    throw std::runtime_error("Encountered invalid state unpickling Limits object!");

                /* Create a new C++ instance */
                Priors prior;

                prior.params = t[0].cast<py::list>();
                prior.backgroundParams = t[1].cast<py::list>();
                prior.scalefactors = t[2].cast<py::list>();
                prior.qzshifts = t[3].cast<py::list>();
                prior.bulkIns = t[4].cast<py::list>();
                prior.bulkOuts = t[5].cast<py::list>();
                prior.resolutionParams = t[6].cast<py::list>();
                prior.domainRatios = t[7].cast<py::list>();
                prior.priorNames = t[8].cast<py::list>();
                prior.priorValues = t[9].cast<py::array_t<real_T>>();

                return prior;
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
        .def_readwrite("IPCFilePath", &Control::IPCFilePath)
        .def(py::pickle(
            [](const Control &ctrl) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(ctrl.parallel, ctrl.procedure, ctrl.display, ctrl.xTolerance, ctrl.funcTolerance, 
                                      ctrl.maxFuncEvals, ctrl.maxIterations, ctrl.populationSize, ctrl.fWeight, ctrl.crossoverProbability, 
                                      ctrl.targetValue, ctrl.numGenerations, ctrl.strategy, ctrl.nLive, ctrl.nMCMC, ctrl.propScale, 
                                      ctrl.nsTolerance, ctrl.calcSldDuringFit, ctrl.resampleMinAngle, ctrl.resampleNPoints,
                                      ctrl.updateFreq, ctrl.updatePlotFreq, ctrl.nSamples, ctrl.nChains, ctrl.jumpProbability, ctrl.pUnitGamma, 
                                      ctrl.boundHandling, ctrl.adaptPCR, ctrl.IPCFilePath);
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

                return ctrl;
            }));

    py::class_<ProblemDefinition>(m, "ProblemDefinition")
        .def(py::init<>())
        .def_readwrite("TF", &ProblemDefinition::TF)
        .def_readwrite("resample", &ProblemDefinition::resample)
        .def_readwrite("data", &ProblemDefinition::data)
        .def_readwrite("dataPresent", &ProblemDefinition::dataPresent)
        .def_readwrite("dataLimits", &ProblemDefinition::dataLimits)
        .def_readwrite("simulationLimits", &ProblemDefinition::simulationLimits)
        .def_readwrite("oilChiDataPresent", &ProblemDefinition::oilChiDataPresent)
        .def_readwrite("numberOfContrasts", &ProblemDefinition::numberOfContrasts)
        .def_readwrite("geometry", &ProblemDefinition::geometry)
        .def_readwrite("useImaginary", &ProblemDefinition::useImaginary)
        .def_readwrite("repeatLayers", &ProblemDefinition::repeatLayers)
        .def_readwrite("contrastBackgroundParams", &ProblemDefinition::contrastBackgroundParams)
        .def_readwrite("contrastBackgroundTypes", &ProblemDefinition::contrastBackgroundTypes)
        .def_readwrite("contrastBackgroundActions", &ProblemDefinition::contrastBackgroundActions)
        .def_readwrite("contrastQzshifts", &ProblemDefinition::contrastQzshifts)
        .def_readwrite("contrastScalefactors", &ProblemDefinition::contrastScalefactors)
        .def_readwrite("contrastBulkIns", &ProblemDefinition::contrastBulkIns)
        .def_readwrite("contrastBulkOuts", &ProblemDefinition::contrastBulkOuts)
        .def_readwrite("contrastResolutionParams", &ProblemDefinition::contrastResolutionParams)
        .def_readwrite("backgroundParams", &ProblemDefinition::backgroundParams)
        .def_readwrite("qzshifts", &ProblemDefinition::qzshifts)
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
        .def_readwrite("otherParams", &ProblemDefinition::otherParams)
        .def_readwrite("fitLimits", &ProblemDefinition::fitLimits)
        .def_readwrite("otherLimits", &ProblemDefinition::otherLimits)
        .def_readwrite("names", &ProblemDefinition::names)
        .def_readwrite("checks", &ProblemDefinition::checks)
        .def(py::pickle(
            [](const ProblemDefinition &p) { // __getstate__
                /* Return a tuple that fully encodes the state of the object */
                return py::make_tuple(p.TF, p.resample, p.data, p.dataPresent, p.dataLimits, p.simulationLimits, p.oilChiDataPresent, 
                                      p.numberOfContrasts, p.geometry, p.useImaginary, p.repeatLayers, 
                                      p.contrastBackgroundParams, p.contrastBackgroundTypes, p.contrastBackgroundActions,  
                                      p.contrastQzshifts, p.contrastScalefactors, p.contrastBulkIns, p.contrastBulkOuts, 
                                      p.contrastResolutionParams, p.backgroundParams, p.qzshifts, p.scalefactors, p.bulkIns, 
                                      p.bulkOuts, p.resolutionParams, p.params, p.numberOfLayers, p.contrastLayers, p.layersDetails, 
                                      p.customFiles, p.modelType, p.contrastCustomFiles, p.contrastDomainRatios, p.domainRatios, 
                                      p.numberOfDomainContrasts, p.domainContrastLayers, p.fitParams, p.otherParams, p.fitLimits, 
                                      p.otherLimits, p.names.backgroundParams, p.names.scalefactors, p.names.qzshifts, p.names.bulkIns, 
                                      p.names.bulkOuts, p.names.resolutionParams, p.names.domainRatios, p.names.contrasts, 
                                      p.checks.params, p.checks.backgroundParams, p.checks.scalefactors, p.checks.qzshifts,  
                                      p.checks.bulkIns, p.checks.bulkOuts, p.checks.resolutionParams, p.checks.domainRatios);
            },
            [](py::tuple t) { // __setstate__
                if (t.size() != 56)
                    throw std::runtime_error("Encountered invalid state unpickling ProblemDefinition object!");

                /* Create a new C++ instance */
                ProblemDefinition p;

                p.TF = t[0].cast<std::string>(); 
                p.resample = t[1].cast<py::array_t<real_T>>(); 
                p.data = t[2].cast<py::list>(); 
                p.dataPresent = t[3].cast<py::array_t<real_T>>(); 
                p.dataLimits = t[4].cast<py::list>(); 
                p.simulationLimits = t[5].cast<py::list>(); 
                p.oilChiDataPresent = t[6].cast<py::array_t<real_T>>(); 
                p.numberOfContrasts = t[7].cast<real_T>();
                p.geometry = t[8].cast<std::string>(); 
                p.useImaginary = t[9].cast<bool>();
                p.repeatLayers = t[10].cast<py::list>();
                p.contrastBackgroundParams = t[11].cast<py::list>(); 
                p.contrastBackgroundTypes = t[12].cast<py::list>(); 
                p.contrastBackgroundActions = t[13].cast<py::list>(); 
                p.contrastQzshifts = t[14].cast<py::array_t<real_T>>();
                p.contrastScalefactors = t[15].cast<py::array_t<real_T>>();
                p.contrastBulkIns = t[16].cast<py::array_t<real_T>>();
                p.contrastBulkOuts = t[17].cast<py::array_t<real_T>>();
                p.contrastResolutionParams = t[18].cast<py::array_t<real_T>>();
                p.backgroundParams = t[19].cast<py::array_t<real_T>>();
                p.qzshifts = t[20].cast<py::array_t<real_T>>();
                p.scalefactors = t[21].cast<py::array_t<real_T>>();
                p.bulkIns = t[22].cast<py::array_t<real_T>>();
                p.bulkOuts = t[23].cast<py::array_t<real_T>>();
                p.resolutionParams = t[24].cast<py::array_t<real_T>>();
                p.params = t[25].cast<py::array_t<real_T>>();
                p.numberOfLayers = t[26].cast<real_T>();
                p.contrastLayers = t[27].cast<py::list>(); 
                p.layersDetails = t[28].cast<py::list>(); 
                p.customFiles = t[29].cast<py::object>(); 
                p.modelType = t[30].cast<std::string>(); 
                p.contrastCustomFiles = t[31].cast<py::array_t<real_T>>(); 
                p.contrastDomainRatios = t[32].cast<py::array_t<real_T>>();
                p.domainRatios = t[33].cast<py::array_t<real_T>>();
                p.numberOfDomainContrasts = t[34].cast<real_T>();
                p.domainContrastLayers = t[35].cast<py::list>();
                p.fitParams = t[36].cast<py::array_t<real_T>>();
                p.otherParams = t[37].cast<py::array_t<real_T>>();
                p.fitLimits = t[38].cast<py::array_t<real_T>>();
                p.otherLimits = t[39].cast<py::array_t<real_T>>();
                
                p.names.backgroundParams = t[40].cast<py::list>(); 
                p.names.scalefactors = t[41].cast<py::list>();
                p.names.qzshifts = t[42].cast<py::list>();
                p.names.bulkIns = t[43].cast<py::list>();
                p.names.bulkOuts = t[44].cast<py::list>();
                p.names.resolutionParams = t[45].cast<py::list>();
                p.names.domainRatios = t[46].cast<py::list>();
                p.names.contrasts = t[47].cast<py::list>(); 

                p.checks.params = t[48].cast<py::array_t<real_T>>(); 
                p.checks.backgroundParams = t[49].cast<py::array_t<real_T>>(); 
                p.checks.scalefactors = t[50].cast<py::array_t<real_T>>();
                p.checks.qzshifts = t[51].cast<py::array_t<real_T>>(); 
                p.checks.bulkIns = t[52].cast<py::array_t<real_T>>(); 
                p.checks.bulkOuts = t[53].cast<py::array_t<real_T>>(); 
                p.checks.resolutionParams = t[54].cast<py::array_t<real_T>>(); 
                p.checks.domainRatios = t[55].cast<py::array_t<real_T>>();

                return p;
            }));

    m.def("RATMain", &RATMain, "Entry point for the main reflectivity computation.");

    m.def("makeSLDProfileXY", &makeSLDProfileXY, "Creates the profiles for the SLD plots");
}
