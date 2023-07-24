/*<%
cfg['include_dirs'] = ['D:/Anaconda/lib/site-packages/pybind11/include']
import glob
cfg['sources'] = glob.glob('RAT/*.cpp')
cfg['parallel'] = True
setup_pybind11(cfg)
%>*/

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "RAT/reflectivityCalculation.h"
#include "RAT/reflectivityCalculation_types.h"

namespace py = pybind11;


struct Checks {
    py::array_t<real_T> params_fitYesNo;
    py::array_t<real_T> backs_fitYesNo;
    py::array_t<real_T> shifts_fitYesNo;
    py::array_t<real_T> scales_fitYesNo;
    py::array_t<real_T> nbairs_fitYesNo;
    py::array_t<real_T> nbsubs_fitYesNo;
    py::array_t<real_T> resol_fitYesNo;
    py::array_t<real_T> domainRatio_fitYesNo;
};

struct Calculation
{
    py::array_t<real_T> all_chis;
    real_T sum_chi;
};

struct OutputProblem
{
    py::array_t<real_T> ssubs;
    py::array_t<real_T> backgrounds;
    py::array_t<real_T> qshifts;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> nbairs;
    py::array_t<real_T> nbsubs;
    py::array_t<real_T> resolutions;
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
    py::array_t<real_T> params;
    py::array_t<real_T> backs;
    py::array_t<real_T> scales;
    py::array_t<real_T> shifts;
    py::array_t<real_T> nba;
    py::array_t<real_T> nbs;
    py::array_t<real_T> res;
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
    py::array_t<real_T> contrastBacks;
    py::array_t<real_T> contrastBacksType;
    std::string TF {};
    py::array_t<real_T> resample;
    py::array_t<real_T> dataPresent;
    py::array_t<real_T> oilChiDataPresent;
    real_T numberOfContrasts;
    std::string geometry {};
    bool useImaginary {};
    py::array_t<real_T> contrastShifts;
    py::array_t<real_T> contrastScales;
    py::array_t<real_T> contrastNbas;
    py::array_t<real_T> contrastNbss;
    py::array_t<real_T> contrastRes;
    py::array_t<real_T> backs;
    py::array_t<real_T> shifts;
    py::array_t<real_T> sf;
    py::array_t<real_T> nba;
    py::array_t<real_T> nbs;
    py::array_t<real_T> res;
    py::array_t<real_T> params;
    real_T numberOfLayers {};
    std::string modelType {};
    py::array_t<real_T> contrastCustomFiles;
    py::array_t<real_T> contrastDomainRatios;
    py::array_t<real_T> domainRatio;
    real_T numberOfDomainContrasts {};
    py::array_t<real_T> domainContrastCustomFiles;
    py::array_t<real_T> fitpars;
    py::array_t<real_T> otherpars;
    py::array_t<real_T> fitconstr;
    py::array_t<real_T> otherconstr;
};

struct Control {
    std::string para {};
    std::string proc {};
    std::string display {};
    real_T tolX {};
    real_T tolFun {};
    real_T maxFunEvals {};
    real_T maxIter {};
    real_T populationSize {};
    real_T fWeight {};
    real_T F_CR {};
    real_T VTR {};
    real_T numGen {};
    real_T strategy {};
    real_T Nlive {};
    real_T nmcmc {};
    real_T propScale {};
    real_T nsTolerance {};
    real_T calcSld {};
    py::array_t<real_T> resamPars;
    real_T updateFreq {};
    real_T updatePlotFreq {};
    real_T nSamples {};
    real_T nChains {};
    real_T lambda {};
    real_T pUnitGamma {};
    std::string boundHandling {};
    Checks checks {};
};


void string_to_rat_array(std::string value, char_T result_data[], int32_T result_size[2])
{
    result_size[0] = 1;
    result_size[1] = value.length();

    // Loop over the array to initialize each element.
    for (int32_T idx1{0}; idx1 < value.length(); idx1++) {
        result_data[idx1] = value[idx1];
    }
}

coder::array<real_T, 2U> py_array_to_rat_1d_array(py::array_t<real_T> value)
{
    coder::array<real_T, 2U> result;

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Number of dimensions must be 1");

    result.set_size(1, buffer_info.shape[0]);
    // Loop over the array to initialize each element.
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result[idx0] = value.at(idx0);
    }

    return result;
}

coder::bounded_array<real_T, 2U, 2U> py_array_to_rat_bounded_array2(py::array_t<real_T> value)
{
    coder::bounded_array<real_T, 2U, 2U> result {};

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Number of dimensions must be 1");

    result.size[0] = 1;
    result.size[1] = buffer_info.shape[0];
    // Loop over the array to initialize each element.
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result.data[idx0] = value.at(idx0);
    }

    return result;
}

coder::bounded_array<real_T, 10U, 2U> py_array_to_rat_bounded_array1(py::array_t<real_T> value)
{
    coder::bounded_array<real_T, 10U, 2U> result {};

    py::buffer_info buffer_info = value.request();
    
    if (buffer_info.size == 0)
        return result;
    
    if (buffer_info.ndim != 1)
        throw std::runtime_error("Number of dimensions must be 1");
    
    result.size[0] = 1;
    result.size[1] = buffer_info.shape[0];
    // Loop over the array to initialize each element.
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        result.data[idx0] = value.at(idx0);
    }

    return result;
}

coder::array<real_T, 2U> py_array_to_rat_2d_array(py::array_t<real_T> value)
{
    coder::array<real_T, 2U> result;

    py::buffer_info buffer_info = value.request();

    if (buffer_info.size == 0)
        return result;

    if (buffer_info.ndim != 2)
        throw std::runtime_error("Number of dimensions must be 2");
    
    result.set_size(buffer_info.shape[0], buffer_info.shape[1]);
    
    int32_T idx {0};
    // Loop over the array to initialize each element.
    for (int32_T idx0{0}; idx0 < buffer_info.shape[0]; idx0++) {
        for (int32_T idx1{0}; idx1 < buffer_info.shape[1]; idx1++) {
            idx  = idx0 + result.size(0) * idx1;  
            result[idx] = value.at(idx0, idx1);
        }
    }

    return result;
}

RAT::struct0_T create_struct0_T(const ProblemDefinition& problem)
{
    RAT::struct0_T problem_struct;
    
    problem_struct.useImaginary = problem.useImaginary;
    
    problem_struct.numberOfLayers = problem.numberOfLayers;
    problem_struct.numberOfDomainContrasts = problem.numberOfDomainContrasts;
    problem_struct.numberOfContrasts = problem.numberOfContrasts;

    string_to_rat_array(problem.modelType, problem_struct.modelType.data, problem_struct.modelType.size);
    string_to_rat_array(problem.geometry, problem_struct.geometry.data, problem_struct.geometry.size);
    string_to_rat_array(problem.TF, problem_struct.TF.data, problem_struct.TF.size);
    
    problem_struct.contrastBacks = py_array_to_rat_1d_array(problem.contrastBacks);
    problem_struct.contrastBacksType = py_array_to_rat_1d_array(problem.contrastBacksType);
    problem_struct.resample = py_array_to_rat_1d_array(problem.resample);
    problem_struct.dataPresent = py_array_to_rat_1d_array(problem.dataPresent);
    problem_struct.oilChiDataPresent = py_array_to_rat_1d_array(problem.oilChiDataPresent);
    problem_struct.contrastShifts = py_array_to_rat_1d_array(problem.contrastShifts);
    problem_struct.contrastScales = py_array_to_rat_1d_array(problem.contrastScales);
    problem_struct.contrastNbas = py_array_to_rat_1d_array(problem.contrastNbas);
    problem_struct.contrastNbss = py_array_to_rat_1d_array(problem.contrastNbss);
    problem_struct.contrastRes = py_array_to_rat_1d_array(problem.contrastRes);
    problem_struct.backs = py_array_to_rat_1d_array(problem.backs);
    problem_struct.shifts = py_array_to_rat_1d_array(problem.shifts);
    problem_struct.sf = py_array_to_rat_1d_array(problem.sf);
    problem_struct.nba = py_array_to_rat_1d_array(problem.nba);
    problem_struct.nbs = py_array_to_rat_1d_array(problem.nbs);
    problem_struct.res = py_array_to_rat_1d_array(problem.res);
    problem_struct.params = py_array_to_rat_1d_array(problem.params);

    problem_struct.contrastCustomFiles = py_array_to_rat_1d_array(problem.contrastCustomFiles);
    problem_struct.contrastDomainRatios = py_array_to_rat_1d_array(problem.contrastDomainRatios);
    problem_struct.domainRatio = py_array_to_rat_1d_array(problem.domainRatio);
    problem_struct.domainContrastCustomFiles = py_array_to_rat_1d_array(problem.domainContrastCustomFiles);

    problem_struct.fitpars =  py_array_to_rat_1d_array(problem.fitpars);
    problem_struct.otherpars =  py_array_to_rat_1d_array(problem.otherpars);
    problem_struct.fitconstr =  py_array_to_rat_2d_array(problem.fitconstr);
    problem_struct.otherconstr =  py_array_to_rat_2d_array(problem.otherconstr);

    return problem_struct;
}

RAT::struct1_T create_struct1_T(const Limits& limits)
{
    RAT::struct1_T limits_struct;
    limits_struct.params = py_array_to_rat_2d_array(limits.params);
    limits_struct.backs = py_array_to_rat_2d_array(limits.backs);
    limits_struct.shifts = py_array_to_rat_2d_array(limits.shifts);
    limits_struct.scales = py_array_to_rat_2d_array(limits.scales);
    limits_struct.nba = py_array_to_rat_2d_array(limits.nba);
    limits_struct.nbs = py_array_to_rat_2d_array(limits.nbs);
    limits_struct.res = py_array_to_rat_2d_array(limits.res);
    limits_struct.domainRatio = py_array_to_rat_2d_array(limits.domainRatio);
    
    return limits_struct;
}

RAT::struct3_T create_struct3_T(const Checks& checks)
{
    RAT::struct3_T checks_struct;
    checks_struct.params_fitYesNo = py_array_to_rat_1d_array(checks.params_fitYesNo);
    checks_struct.backs_fitYesNo = py_array_to_rat_1d_array(checks.backs_fitYesNo);
    checks_struct.shifts_fitYesNo = py_array_to_rat_1d_array(checks.shifts_fitYesNo);
    checks_struct.scales_fitYesNo = py_array_to_rat_1d_array(checks.scales_fitYesNo);
    checks_struct.nbairs_fitYesNo = py_array_to_rat_1d_array(checks.nbairs_fitYesNo);
    checks_struct.nbsubs_fitYesNo = py_array_to_rat_1d_array(checks.nbsubs_fitYesNo);
    checks_struct.resol_fitYesNo = py_array_to_rat_1d_array(checks.resol_fitYesNo);
    checks_struct.domainRatio_fitYesNo = py_array_to_rat_1d_array(checks.domainRatio_fitYesNo);
    
    return checks_struct;
}

coder::array<RAT::cell_wrap_0, 2U> py_array_to_rat_cell_wrap_0(py::list values)
{
    coder::array<RAT::cell_wrap_0, 2U> result;
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

coder::array<RAT::cell_wrap_1, 2U> py_array_to_rat_cell_wrap_1(py::list values)
{
    coder::array<RAT::cell_wrap_1, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = py_array_to_rat_2d_array(casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_2, 2U> py_array_to_rat_cell_wrap_2(py::list values)
{
    coder::array<RAT::cell_wrap_2, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = py_array_to_rat_bounded_array2(casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_3, 2U> py_array_to_rat_cell_wrap_3(py::list values)
{
    coder::array<RAT::cell_wrap_3, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        RAT::cell_wrap_3 temp;
        result[idx].f1 = py_array_to_rat_1d_array(casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_4, 1U> py_array_to_rat_cell_wrap_4(py::list values)
{
    coder::array<RAT::cell_wrap_4, 1U> result;
    result.set_size(values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        py::array_t<real_T> casted_array = py::cast<py::array>(array);
        result[idx].f1 = py_array_to_rat_bounded_array1(casted_array);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_5, 2U> py_array_to_rat_cell_wrap_5(py::list values)
{
    coder::array<RAT::cell_wrap_5, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        std::string name = py::cast<std::string>(array);
        string_to_rat_array(name, result[idx].f1.data, result[idx].f1.size);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_6, 2U> py_array_to_rat_cell_wrap_6(py::list values)
{
    coder::array<RAT::cell_wrap_6, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle array: values)
    { 
        std::string name = py::cast<std::string>(array);
        string_to_rat_array(name, result[idx].f1.data, result[idx].f1.size);
        idx++;
    }

    return result;
}

coder::array<RAT::cell_wrap_7, 2U> py_array_to_rat_cell_wrap_7(py::list values)
{
    coder::array<RAT::cell_wrap_7, 2U> result;
    result.set_size(1, values.size());
    int32_T idx {0};
    for (py::handle value: values)
    { 
        py::list casted_array = py::cast<py::list>(value);
        
        int32_T idx1 {0};
        for (py::handle array: casted_array)
        { 
            std::string name = py::cast<std::string>(array);
            string_to_rat_array(name, result[idx].f1[idx1].f1.data, result[idx].f1[idx1].f1.size);
            idx1++;
        }
        idx++;
    }

    return result;
}

RAT::cell_8 create_cell_8(const Cells& cells)
{
    RAT::cell_8 cells_struct;
    cells_struct.f1 = py_array_to_rat_cell_wrap_0(cells.f1);
    cells_struct.f2 = py_array_to_rat_cell_wrap_1(cells.f2);
    cells_struct.f3 = py_array_to_rat_cell_wrap_0(cells.f3);
    cells_struct.f4 = py_array_to_rat_cell_wrap_2(cells.f4);
    cells_struct.f5 = py_array_to_rat_cell_wrap_3(cells.f5);
    cells_struct.f6 = py_array_to_rat_cell_wrap_4(cells.f6);
    cells_struct.f7 = py_array_to_rat_cell_wrap_5(cells.f7);
    cells_struct.f8 = py_array_to_rat_cell_wrap_6(cells.f8);
    cells_struct.f9 = py_array_to_rat_cell_wrap_6(cells.f9);
    cells_struct.f10 = py_array_to_rat_cell_wrap_6(cells.f10);
    cells_struct.f11 = py_array_to_rat_cell_wrap_6(cells.f11);
    cells_struct.f12 = py_array_to_rat_cell_wrap_6(cells.f12);
    cells_struct.f13 = py_array_to_rat_cell_wrap_6(cells.f13);
    cells_struct.f14 = py_array_to_rat_cell_wrap_7(cells.f14);
    cells_struct.f15 = py_array_to_rat_cell_wrap_5(cells.f15);
    cells_struct.f16 = py_array_to_rat_cell_wrap_5(cells.f16);
    cells_struct.f17 = py_array_to_rat_cell_wrap_1(cells.f17);
    cells_struct.f18 = py_array_to_rat_cell_wrap_0(cells.f18);
    cells_struct.f19 = py_array_to_rat_cell_wrap_3(cells.f19);
    cells_struct.f20 = py_array_to_rat_cell_wrap_6(cells.f20);

    return cells_struct;
}

RAT::struct2_T create_struct2_T(const Control& control)
{
    RAT::struct2_T control_struct;
    control_struct.tolFun = control.tolFun;
    control_struct.maxFunEvals = control.maxFunEvals;
    control_struct.maxIter = control.maxIter;
    control_struct.populationSize = control.populationSize;
    control_struct.fWeight = control.fWeight;
    control_struct.F_CR = control.F_CR;
    control_struct.VTR = control.VTR;
    control_struct.numGen = control.numGen;
    control_struct.strategy = control.strategy;
    control_struct.Nlive = control.Nlive;
    control_struct.nmcmc = control.nmcmc;
    control_struct.propScale = control.propScale;
    control_struct.nsTolerance = control.nsTolerance;
    control_struct.calcSld = control.calcSld;
    control_struct.updateFreq = control.updateFreq;
    control_struct.updatePlotFreq = control.updatePlotFreq;
    control_struct.nSamples = control.nSamples;
    control_struct.nChains = control.nChains;
    control_struct.lambda = control.lambda;
    control_struct.pUnitGamma = control.pUnitGamma;
    string_to_rat_array(control.para, control_struct.para.data, control_struct.para.size);
    string_to_rat_array(control.proc, control_struct.proc.data, control_struct.proc.size);
    string_to_rat_array(control.display, control_struct.display.data, control_struct.display.size);
    control_struct.tolX = control.tolX;
    control_struct.resamPars[0] = control.resamPars.at(0);
    control_struct.resamPars[1] = control.resamPars.at(1);
    string_to_rat_array(control.boundHandling, control_struct.boundHandling.data, control_struct.boundHandling.size);
    control_struct.checks = create_struct3_T(control.checks);

    return control_struct;
}

py::tuple reflectivityCalculation(const ProblemDefinition& problem_def, const Cells& cells, const Limits& limits, const Control& control)
{
    RAT::struct0_T problem_def_struct = create_struct0_T(problem_def);
    RAT::cell_8 cells_struct = create_cell_8(cells);
    RAT::struct1_T limits_struct = create_struct1_T(limits);
    RAT::struct2_T control_struct = create_struct2_T(control);

    RAT::cell_11 result;
    RAT::struct4_T problem;

    // Call the entry-point 'reflectivityCalculation'.
    RAT::reflectivityCalculation(&problem_def_struct, &cells_struct, &limits_struct, &control_struct, &problem, &result);
    
    // Copy problem to output
    OutputProblem output_problem;
    output_problem.ssubs = py::array_t<real_T>(problem.ssubs.size(0));
    auto buffer = output_problem.ssubs.request();
    std::memcpy(buffer.ptr, problem.ssubs.data(), output_problem.ssubs.size()*sizeof(real_T));

    output_problem.backgrounds = py::array_t<real_T>(problem.backgrounds.size(0));
    buffer = output_problem.backgrounds.request();
    std::memcpy(buffer.ptr, problem.backgrounds.data(), output_problem.backgrounds.size()*sizeof(real_T));

    output_problem.qshifts = py::array_t<real_T>(problem.qshifts.size(0));
    buffer = output_problem.qshifts.request();
    std::memcpy(buffer.ptr, problem.qshifts.data(), output_problem.qshifts.size()*sizeof(real_T));

    output_problem.scalefactors = py::array_t<real_T>(problem.scalefactors.size(0));
    buffer = output_problem.scalefactors.request();
    std::memcpy(buffer.ptr, problem.scalefactors.data(), output_problem.scalefactors.size()*sizeof(real_T));

    output_problem.nbairs = py::array_t<real_T>(problem.nbairs.size(0));
    buffer = output_problem.nbairs.request();
    std::memcpy(buffer.ptr, problem.nbairs.data(), output_problem.nbairs.size()*sizeof(real_T));

    output_problem.nbsubs = py::array_t<real_T>(problem.nbsubs.size(0));
    buffer = output_problem.nbsubs.request();
    std::memcpy(buffer.ptr, problem.nbsubs.data(), output_problem.nbsubs.size()*sizeof(real_T));

    output_problem.resolutions = py::array_t<real_T>(problem.resolutions.size(0));
    buffer = output_problem.resolutions.request();
    std::memcpy(buffer.ptr, problem.resolutions.data(), output_problem.resolutions.size()*sizeof(real_T));

    output_problem.calculations.sum_chi = problem.calculations.sum_chi;
    output_problem.calculations.all_chis = py::array_t<real_T>(problem.calculations.all_chis.size(0));
    buffer = output_problem.calculations.all_chis.request();
    std::memcpy(buffer.ptr, problem.calculations.all_chis.data(), output_problem.calculations.all_chis.size()*sizeof(real_T));

    output_problem.allSubRough = py::array_t<real_T>(problem.allSubRough.size(0));
    buffer = output_problem.allSubRough.request();
    std::memcpy(buffer.ptr, problem.allSubRough.data(), output_problem.allSubRough.size()*sizeof(real_T));

    output_problem.resample = py::array_t<real_T>(problem.resample.size(1));
    buffer = output_problem.resample.request();
    std::memcpy(buffer.ptr, problem.resample.data(), output_problem.resample.size()*sizeof(real_T));

    // Copy result to output
    OutputResult output_result;

    for (int32_T idx{0}; idx < result.f1.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f1[idx].f1.size(0), result.f1[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f1[idx].f1.data(), array.nbytes());
        output_result.f1.append(array);
    }

    for (int32_T idx{0}; idx < result.f2.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f2[idx].f1.size(0), result.f2[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f2[idx].f1.data(), array.nbytes());
        output_result.f2.append(array);
    }

    for (int32_T idx{0}; idx < result.f3.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f3[idx].f1.size(0), result.f3[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f3[idx].f1.data(), array.nbytes());
        output_result.f3.append(array);
    }

    for (int32_T idx{0}; idx < result.f4.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f4[idx].f1.size(0), result.f4[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f4[idx].f1.data(), array.nbytes());
        output_result.f4.append(array);
    }

    for (int32_T idx{0}; idx < result.f5.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f5[idx].f1.size(0), result.f5[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f5[idx].f1.data(), array.nbytes());
        output_result.f5.append(array);
    }

    for (int32_T idx{0}; idx < result.f6.size(0); idx++) {
        auto array = py::array_t<real_T, py::array::f_style>({result.f6[idx].f1.size(0), result.f6[idx].f1.size(1)});
        std::memcpy(array.request().ptr, result.f6[idx].f1.data(), array.nbytes());
        output_result.f6.append(array);
    }

    return py::make_tuple(output_problem, output_result);
}

PYBIND11_MODULE(rat, m) {
    py::class_<Calculation>(m, "Calculation")
        .def(py::init<>())
        .def_readwrite("all_chis", &Calculation::all_chis)
        .def_readwrite("sum_chi", &Calculation::sum_chi);

    py::class_<OutputProblem>(m, "OutputProblem")
        .def(py::init<>())
        .def_readwrite("ssubs", &OutputProblem::ssubs)
        .def_readwrite("backgrounds", &OutputProblem::backgrounds)
        .def_readwrite("qshifts", &OutputProblem::qshifts)
        .def_readwrite("scalefactors", &OutputProblem::scalefactors)
        .def_readwrite("nbairs", &OutputProblem::nbairs)
        .def_readwrite("nbsubs", &OutputProblem::nbsubs)
        .def_readwrite("resolutions", &OutputProblem::resolutions)
        .def_readwrite("calculations", &OutputProblem::calculations)
        .def_readwrite("allSubRough", &OutputProblem::allSubRough)
        .def_readwrite("resample", &OutputProblem::resample);

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
        .def_readwrite("params_fitYesNo", &Checks::params_fitYesNo)
        .def_readwrite("backs_fitYesNo", &Checks::backs_fitYesNo)
        .def_readwrite("shifts_fitYesNo", &Checks::shifts_fitYesNo)
        .def_readwrite("scales_fitYesNo", &Checks::scales_fitYesNo)
        .def_readwrite("nbairs_fitYesNo", &Checks::nbairs_fitYesNo)
        .def_readwrite("nbsubs_fitYesNo", &Checks::nbsubs_fitYesNo)
        .def_readwrite("resol_fitYesNo", &Checks::resol_fitYesNo)
        .def_readwrite("domainRatio_fitYesNo", &Checks::domainRatio_fitYesNo);

    py::class_<Limits>(m, "Limits")
        .def(py::init<>())
        .def_readwrite("params", &Limits::params)
        .def_readwrite("backs", &Limits::backs)
        .def_readwrite("shifts", &Limits::shifts)
        .def_readwrite("scales", &Limits::scales)
        .def_readwrite("nba", &Limits::nba)
        .def_readwrite("nbs", &Limits::nbs)
        .def_readwrite("res", &Limits::res)
        .def_readwrite("domainRatio", &Limits::domainRatio);

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
        .def_readwrite("para", &Control::para)
        .def_readwrite("proc", &Control::proc)
        .def_readwrite("display", &Control::display)
        .def_readwrite("tolX", &Control::tolX)
        .def_readwrite("tolFun", &Control::tolFun)
        .def_readwrite("maxFunEvals", &Control::maxFunEvals)
        .def_readwrite("maxIter", &Control::maxIter)
        .def_readwrite("populationSize", &Control::populationSize)
        .def_readwrite("fWeight", &Control::fWeight)  
        .def_readwrite("F_CR", &Control::F_CR)  
        .def_readwrite("VTR", &Control::VTR)
        .def_readwrite("numGen", &Control::numGen)
        .def_readwrite("strategy", &Control::strategy)
        .def_readwrite("Nlive", &Control::Nlive)
        .def_readwrite("nmcmc", &Control::nmcmc)
        .def_readwrite("propScale", &Control::propScale)
        .def_readwrite("nsTolerance", &Control::nsTolerance)
        .def_readwrite("calcSld", &Control::calcSld)
        .def_readwrite("resamPars", &Control::resamPars)
        .def_readwrite("updateFreq", &Control::updateFreq)
        .def_readwrite("updatePlotFreq", &Control::updatePlotFreq)
        .def_readwrite("nSamples", &Control::nSamples)
        .def_readwrite("nChains", &Control::nChains)
        .def_readwrite("lambda", &Control::lambda)
        .def_readwrite("pUnitGamma", &Control::pUnitGamma)
        .def_readwrite("boundHandling", &Control::boundHandling)
        .def_readwrite("checks", &Control::checks);

    py::class_<ProblemDefinition>(m, "ProblemDefinition")
        .def(py::init<>())
        .def_readwrite("contrastBacks", &ProblemDefinition::contrastBacks)
        .def_readwrite("contrastBacksType", &ProblemDefinition::contrastBacksType)
        .def_readwrite("TF", &ProblemDefinition::TF)
        .def_readwrite("resample", &ProblemDefinition::resample)
        .def_readwrite("dataPresent", &ProblemDefinition::dataPresent)
        .def_readwrite("oilChiDataPresent", &ProblemDefinition::oilChiDataPresent)
        .def_readwrite("numberOfContrasts", &ProblemDefinition::numberOfContrasts)
        .def_readwrite("geometry", &ProblemDefinition::geometry)
        .def_readwrite("useImaginary", &ProblemDefinition::useImaginary)  
        .def_readwrite("contrastShifts", &ProblemDefinition::contrastShifts)  
        .def_readwrite("contrastScales", &ProblemDefinition::contrastScales)
        .def_readwrite("contrastNbas", &ProblemDefinition::contrastNbas)
        .def_readwrite("contrastNbss", &ProblemDefinition::contrastNbss)
        .def_readwrite("contrastRes", &ProblemDefinition::contrastRes)
        .def_readwrite("backs", &ProblemDefinition::backs)
        .def_readwrite("shifts", &ProblemDefinition::shifts)
        .def_readwrite("sf", &ProblemDefinition::sf)
        .def_readwrite("nba", &ProblemDefinition::nba)
        .def_readwrite("nbs", &ProblemDefinition::nbs)
        .def_readwrite("res", &ProblemDefinition::res)
        .def_readwrite("params", &ProblemDefinition::params)
        .def_readwrite("numberOfLayers", &ProblemDefinition::numberOfLayers)
        .def_readwrite("modelType", &ProblemDefinition::modelType)
        .def_readwrite("contrastCustomFiles", &ProblemDefinition::contrastCustomFiles)
        .def_readwrite("contrastDomainRatios", &ProblemDefinition::contrastDomainRatios)
        .def_readwrite("domainRatio", &ProblemDefinition::domainRatio)
        .def_readwrite("numberOfDomainContrasts", &ProblemDefinition::numberOfDomainContrasts)
        .def_readwrite("domainContrastCustomFiles", &ProblemDefinition::domainContrastCustomFiles)
        .def_readwrite("fitpars", &ProblemDefinition::fitpars)
        .def_readwrite("otherpars", &ProblemDefinition::otherpars)
        .def_readwrite("fitconstr", &ProblemDefinition::fitconstr)
        .def_readwrite("otherconstr", &ProblemDefinition::otherconstr);

    m.def("reflectivityCalculation", &reflectivityCalculation, "A demo for Python RAT");
}
