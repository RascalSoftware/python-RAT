//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// div.h
//
// Code generation for function 'div'
//
#ifndef DIV_H
#define DIV_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void binary_expand_op(::coder::array<real_T, 2U> &in1, int32_T in2, const ::
                        coder::array<real_T, 1U> &in3, const ::coder::array<
                        real_T, 2U> &in4, const ::coder::array<real_T, 1U> &in5);
  void rdivide(::coder::array<real_T, 1U> &in1, const ::coder::array<real_T, 1U>
               &in2, const ::coder::array<real_T, 1U> &in3);
}

#endif

// End of code generation (div.h)
