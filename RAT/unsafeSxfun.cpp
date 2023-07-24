//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// unsafeSxfun.cpp
//
// Code generation for function 'unsafeSxfun'
//

// Include files
#include "unsafeSxfun.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::array<
                        real_T, 2U> &in3, const ::coder::array<real_T, 2U> &in4)
  {
    ::coder::array<real_T, 1U> b_in3;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    int32_T stride_2_0;
    if (in3.size(0) == 1) {
      if (in4.size(0) == 1) {
        i = in3.size(0);
      } else {
        i = in4.size(0);
      }
    } else {
      i = in3.size(0);
    }

    b_in3.set_size(i);
    stride_0_0 = (in3.size(0) != 1);
    stride_1_0 = (in4.size(0) != 1);
    stride_2_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      if (in4.size(0) == 1) {
        loop_ub = in3.size(0);
      } else {
        loop_ub = in4.size(0);
      }
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      b_in3[i] = (in3[i * stride_0_0 + in3.size(0)] - in4[i * stride_1_0 +
                  in4.size(0)]) / in3[i * stride_2_0 + in3.size(0) * 2];
    }

    in1.set_size(b_in3.size(0));
    loop_ub = b_in3.size(0);
    for (i = 0; i < loop_ub; i++) {
      real_T varargin_1;
      varargin_1 = b_in3[i];
      in1[i] = varargin_1 * varargin_1;
    }
  }
}

// End of code generation (unsafeSxfun.cpp)
