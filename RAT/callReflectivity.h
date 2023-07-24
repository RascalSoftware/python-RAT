//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// callReflectivity.h
//
// Code generation for function 'callReflectivity'
//
#ifndef CALLREFLECTIVITY_H
#define CALLREFLECTIVITY_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void b_callReflectivity(real_T nbairs, real_T nbsubs, const real_T
    simLimits_data[], const real_T repeatLayers[2], const ::coder::array<real_T,
    2U> &this_data, ::coder::array<real_T, 2U> &layers, real_T ssubs, real_T res,
    boolean_T useImaginary, ::coder::array<real_T, 2U> &reflectivity, ::coder::
    array<real_T, 2U> &Simulation);
  void callReflectivity(real_T nbairs, real_T nbsubs, const real_T
                        simLimits_data[], const real_T repeatLayers[2], const ::
                        coder::array<real_T, 2U> &this_data, ::coder::array<
                        real_T, 2U> &layers, real_T ssubs, real_T res, boolean_T
                        useImaginary, ::coder::array<real_T, 2U> &reflectivity, ::
                        coder::array<real_T, 2U> &Simulation);
}

#endif

// End of code generation (callReflectivity.h)
