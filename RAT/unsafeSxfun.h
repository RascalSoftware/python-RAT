//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// unsafeSxfun.h
//
// Code generation for function 'unsafeSxfun'
//
#ifndef UNSAFESXFUN_H
#define UNSAFESXFUN_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void binary_expand_op(::coder::array<creal_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in3, const ::coder::array<real_T, 2U> &in4,
                        int32_T in5, const ::coder::array<creal_T, 2U> &in6,
                        const ::coder::array<creal_T, 1U> &in7);
  void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::array<
                        real_T, 1U> &in3, const ::coder::array<real_T, 1U> &in4);
  void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in3, int32_T in4, int32_T in5, const ::
                        coder::array<real_T, 2U> &in6, int32_T in7, int32_T in8,
                        const ::coder::array<real_T, 2U> &in9);
  void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::array<
                        real_T, 2U> &in3, const ::coder::array<real_T, 2U> &in4);
}

#endif

// End of code generation (unsafeSxfun.h)
