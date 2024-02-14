//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eml_mtimes_helper.h
//
// Code generation for function 'eml_mtimes_helper'
//
#ifndef EML_MTIMES_HELPER_H
#define EML_MTIMES_HELPER_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void binary_expand_op(real_T in1_data[], int32_T in1_size[2], const ::coder::
                        array<real_T, 2U> &in2, const ::coder::array<real_T, 2U>
                        &in3, int32_T in4, const ::coder::array<real_T, 2U> &in5);
  void binary_expand_op(::coder::array<creal_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in2, const ::coder::array<creal_T, 1U> &in3,
                        const ::coder::array<creal_T, 2U> &in4);
  void binary_expand_op(::coder::array<real_T, 2U> &in1, real_T in2, const ::
                        coder::array<real_T, 2U> &in3, const ::coder::array<
                        real_T, 2U> &in4);
  void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3);
  void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in3, const ::coder::array<int32_T, 1U> &in4,
                        const ::coder::array<int32_T, 1U> &in5);
  void c_binary_expand_op(real_T in1_data[], int32_T in1_size[2], const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, int32_T in4,
    const ::coder::array<real_T, 2U> &in5);
}

#endif

// End of code generation (eml_mtimes_helper.h)
