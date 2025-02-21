#ifndef RAT_FUNCTIONS_H
#define RAT_FUNCTIONS_H

#include <cstdlib>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "../RAT/classHandle.hpp"

namespace py = pybind11;

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

    // Backgrounds
    void invoke(std::vector<double>& xdata, std::vector<double>& params, std::vector<double>& output)
    {
        auto f = py::cast<std::function<py::tuple(py::list, py::list)>>(this->function);
        auto result = f(py::cast(xdata), py::cast(params));
        for (py::handle rowHandle : result)
        {   
            if (py::isinstance<py::list>(rowHandle))
                output.push_back(py::cast<double>(py::cast<py::list>(rowHandle)[0]));  
            else
                output.push_back(py::cast<double>(rowHandle));
            
        }
    };
        
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
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatBoundedArray3, casted_array);
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
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatRowArray1d, casted_array);
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
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = customCaller("$id[" + std::to_string(idx) +"]", pyArrayToRatBoundedArray, casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_0, 1U> pyListToRatCellWrap01d(py::list values)
{
    coder::array<RAT::cell_wrap_0, 1U> result;
    result.set_size(values.size());
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

coder::array<RAT::cell_wrap_0, 2U> pyListToRatCellWrap02d(py::list values)
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

py::list pyListFromRatCellWrap01d(coder::array<RAT::cell_wrap_0, 1U> values)
{
    py::list result;
    for (int32_T idx0{0}; idx0 < values.size(0); idx0++) {
        std::string tmp;
        stringFromRatBoundedArray(values[idx0].f1.data, values[idx0].f1.size, tmp);
        result.append(tmp);
    }

    return result;
}

py::list pyListFromRatCellWrap02d(coder::array<RAT::cell_wrap_0, 2U> values)
{
    py::list result;
    for (int32_T idx0{0}; idx0 < values.size(1); idx0++) {
        std::string tmp;
        stringFromRatBoundedArray(values[idx0].f1.data, values[idx0].f1.size, tmp);
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

#endif  // RAT_FUNCTIONS_H