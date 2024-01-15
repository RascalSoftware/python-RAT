//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// calcDensity.h
//
// Code generation for function 'calcDensity'
//
#ifndef CALCDENSITY_H
#define CALCDENSITY_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct struct14_T;
}

// Function Declarations
namespace RAT
{
  void calcDensity(const ::coder::array<real_T, 2U> &x, const ::coder::array<
                   real_T, 2U> &fx, const struct14_T *DREAMPar, const ::coder::
                   array<real_T, 2U> &ratInputs_problemDef_fitLimits, const ::
                   coder::array<real_T, 2U> &ratInputs_priors, ::coder::array<
                   real_T, 1U> &log_L, ::coder::array<real_T, 1U> &log_PR);
}

#endif

// End of code generation (calcDensity.h)
