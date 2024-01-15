//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// SLDFunction.h
//
// Code generation for function 'SLDFunction'
//
#ifndef SLDFUNCTION_H
#define SLDFUNCTION_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void b_SLDFunction(real_T x, const ::coder::array<real_T, 2U> &SLD, real_T
                     sldVal_data[], int32_T *sldVal_size);
  void b_SLDFunction(real_T x, const ::coder::array<real_T, 2U> &SLD, ::coder::
                     array<real_T, 1U> &sldVal);
}

#endif

// End of code generation (SLDFunction.h)
