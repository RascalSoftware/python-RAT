//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// rdivide_helper.cpp
//
// Code generation for function 'rdivide_helper'
//

// Include files
#include "rdivide_helper.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void binary_expand_op(::coder::array<int8_T, 1U> &in1, const ::coder::array<
                        real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3)
  {
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = static_cast<int8_T>(in2[i * stride_0_0] == in3[i * stride_1_0]);
    }
  }
}

// End of code generation (rdivide_helper.cpp)
