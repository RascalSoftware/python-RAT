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
  void SLDFunction(real_T x, const real_T SLD_data[], const int32_T SLD_size[2],
                   real_T sldVal_data[], int32_T *sldVal_size);
  void SLDFunction(real_T x, const ::coder::array<real_T, 2U> &SLD, ::coder::
                   array<real_T, 1U> &sldVal);
}

#endif

// End of code generation (SLDFunction.h)
