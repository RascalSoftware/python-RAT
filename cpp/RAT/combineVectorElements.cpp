//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// combineVectorElements.cpp
//
// Code generation for function 'combineVectorElements'
//

// Include files
#include "combineVectorElements.h"
#include "blockedSummation.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3,
                        const ::coder::array<real_T, 1U> &in4, int32_T in5,
                        int32_T in6, int32_T in7)
  {
    ::coder::array<real_T, 2U> b_in2;
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    if (in3.size(1) == 1) {
      i1 = in2.size(1);
    } else {
      i1 = in3.size(1);
    }

    b_in2.set_size(i, i1);
    stride_0_0 = (in2.size(0) != 1);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_1_1 = (in3.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    if (in3.size(1) == 1) {
      loop_ub = in2.size(1);
    } else {
      loop_ub = in3.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      i1 = in3.size(0);
      if (i1 == 1) {
        b_loop_ub = in2.size(0);
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = in2[i1 * stride_0_0 + in2.size(0) *
          aux_0_1] - in3[i1 * stride_1_0 + in3.size(0) * aux_1_1];
      }

      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    if (static_cast<int32_T>(in4[in5]) == 1) {
      i = in6 - in7;
    } else {
      i = static_cast<int32_T>(in4[in5]);
    }

    coder::blockedSummation(b_in2, i, in1);
  }

  void c_binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
    real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const ::coder::
    array<real_T, 1U> &in4, int32_T in5, int32_T in6, int32_T in7)
  {
    ::coder::array<real_T, 2U> b_in2;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    b_in2.set_size(i, in2.size(1));
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    loop_ub = in2.size(1);
    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      int32_T i1;
      i1 = in3.size(0);
      if (i1 == 1) {
        b_loop_ub = in2.size(0);
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = in2[i1 * stride_0_0 + in2.size(0) * i] -
          in3[i1 * stride_1_0 + in3.size(0) * i];
      }
    }

    if (static_cast<int32_T>(in4[in5]) == 1) {
      i = in6 - in7;
    } else {
      i = static_cast<int32_T>(in4[in5]);
    }

    coder::blockedSummation(b_in2, i, in1);
  }

  namespace coder
  {
    real_T b_combineVectorElements(const ::coder::array<real_T, 1U> &x, int32_T
      vlen)
    {
      real_T y;
      if ((x.size(0) == 0) || (vlen == 0)) {
        y = 0.0;
      } else {
        y = nestedIter(x, vlen);
      }

      return y;
    }

    int32_T c_combineVectorElements(const ::coder::array<boolean_T, 1U> &x)
    {
      int32_T vlen;
      int32_T y;
      vlen = x.size(0);
      if (x.size(0) == 0) {
        y = 0;
      } else {
        y = x[0];
        for (int32_T k{2}; k <= vlen; k++) {
          if (vlen >= 2) {
            y += x[k - 1];
          }
        }
      }

      return y;
    }

    int32_T combineVectorElements(const ::coder::array<boolean_T, 2U> &x)
    {
      int32_T vlen;
      int32_T y;
      vlen = x.size(1);
      if (x.size(1) == 0) {
        y = 0;
      } else {
        y = x[0];
        for (int32_T k{2}; k <= vlen; k++) {
          if (vlen >= 2) {
            y += x[k - 1];
          }
        }
      }

      return y;
    }
  }
}

// End of code generation (combineVectorElements.cpp)
