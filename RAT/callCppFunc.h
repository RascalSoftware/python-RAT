//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// callCppFunc.h
//
// Code generation for function 'callCppFunc'
//
#ifndef CALLCPPFUNC_H
#define CALLCPPFUNC_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void callCppFunc(::coder::array<real_T, 2U> &params, real_T nba, real_T nbs,
                   real_T numberOfContrasts, const char_T libraryName_data[],
                   const int32_T libraryName_size[2], const char_T
                   functionName_data[], const int32_T functionName_size[2],
                   real_T output_data[], int32_T output_size[2], real_T
                   *subRough);
}

#endif

// End of code generation (callCppFunc.h)
