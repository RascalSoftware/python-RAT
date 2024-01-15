//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// rescaleKernel.cpp
//
// Code generation for function 'rescaleKernel'
//

// Include files
#include "rescaleKernel.h"
#include "abs.h"
#include "bsxfun.h"
#include "div.h"
#include "log2.h"
#include "minOrMax.h"
#include "pow2.h"
#include "rdivide_helper.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::coder::
    array<real_T, 1U> &in4, const ::coder::array<real_T, 1U> &in5, const ::coder::
    array<int8_T, 1U> &in6);
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::coder::
    array<real_T, 1U> &in4, const ::coder::array<real_T, 1U> &in5, const ::coder::
    array<int8_T, 1U> &in6, const ::coder::array<real_T, 1U> &in7);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::coder::
    array<real_T, 1U> &in4, const ::coder::array<real_T, 1U> &in5, const ::coder::
    array<int8_T, 1U> &in6)
  {
    ::coder::array<real_T, 1U> r;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    int32_T stride_2_0;
    int32_T stride_3_0;
    int32_T stride_4_0;
    int32_T stride_5_0;
    int32_T stride_6_0;
    if (in4.size(0) == 1) {
      i = in6.size(0);
      stride_0_0 = in5.size(0);
      if (i == 1) {
        if (stride_0_0 == 1) {
          if (in4.size(0) == 1) {
            i = in3.size(0);
          } else {
            i = in4.size(0);
          }
        } else if (in4.size(0) == 1) {
          i = in5.size(0);
        } else {
          i = in4.size(0);
        }
      } else if (in4.size(0) == 1) {
        i = in6.size(0);
      } else {
        i = in4.size(0);
      }
    } else {
      i = in4.size(0);
    }

    r.set_size(i);
    stride_0_0 = (in3.size(0) != 1);
    stride_1_0 = (in4.size(0) != 1);
    stride_2_0 = (in5.size(0) != 1);
    stride_3_0 = (in4.size(0) != 1);
    stride_4_0 = (in6.size(0) != 1);
    stride_5_0 = (in4.size(0) != 1);
    stride_6_0 = (in4.size(0) != 1);
    if (in4.size(0) == 1) {
      i = in6.size(0);
      if (i == 1) {
        if (in4.size(0) == 1) {
          i = in5.size(0);
        } else {
          i = in4.size(0);
        }

        if (i == 1) {
          if (in4.size(0) == 1) {
            loop_ub = in3.size(0);
          } else {
            loop_ub = in4.size(0);
          }
        } else if (in4.size(0) == 1) {
          loop_ub = in5.size(0);
        } else {
          loop_ub = in4.size(0);
        }
      } else if (in4.size(0) == 1) {
        loop_ub = in6.size(0);
      } else {
        loop_ub = in4.size(0);
      }
    } else {
      loop_ub = in4.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      r[i] = 1.0 / ((in3[i * stride_0_0] / in4[i * stride_1_0] - in5[i *
                     stride_2_0] / in4[i * stride_3_0]) + static_cast<real_T>
                    (in6[i * stride_4_0]) / in4[i * stride_5_0]) / in4[i *
        stride_6_0];
    }

    coder::e_bsxfun(in2, r, in1);
  }

  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::coder::
    array<real_T, 1U> &in4, const ::coder::array<real_T, 1U> &in5, const ::coder::
    array<int8_T, 1U> &in6, const ::coder::array<real_T, 1U> &in7)
  {
    ::coder::array<real_T, 1U> b_in3;
    int32_T i;
    int32_T i1;
    int32_T i2;
    int32_T i3;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_10_0;
    int32_T stride_11_0;
    int32_T stride_1_0;
    int32_T stride_2_0;
    int32_T stride_3_0;
    int32_T stride_4_0;
    int32_T stride_5_0;
    int32_T stride_6_0;
    int32_T stride_7_0;
    int32_T stride_8_0;
    int32_T stride_9_0;
    if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (i == 1) {
      if (in5.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (in4.size(0) == 1) {
      if (in7.size(0) == 1) {
        i1 = in6.size(0);
      } else {
        i1 = in7.size(0);
      }
    } else {
      i1 = in4.size(0);
    }

    if (in3.size(0) == 1) {
      i2 = in5.size(0);
    } else {
      i2 = in3.size(0);
    }

    if (in7.size(0) == 1) {
      i3 = in6.size(0);
    } else {
      i3 = in7.size(0);
    }

    if (i == 1) {
      if (i1 == 1) {
        if (i2 == 1) {
          if (in4.size(0) == 1) {
            i = in1.size(0);
          } else {
            i = in4.size(0);
          }
        } else if (in3.size(0) == 1) {
          i = in5.size(0);
        } else {
          i = in3.size(0);
        }
      } else if (in4.size(0) == 1) {
        if (in7.size(0) == 1) {
          i = in6.size(0);
        } else {
          i = in7.size(0);
        }
      } else {
        i = in4.size(0);
      }
    } else if (i3 == 1) {
      if (in5.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (in7.size(0) == 1) {
      i1 = in6.size(0);
    } else {
      i1 = in7.size(0);
    }

    if (i1 == 1) {
      if (in5.size(0) == 1) {
        i1 = in1.size(0);
      } else {
        i1 = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i1 = in6.size(0);
    } else {
      i1 = in7.size(0);
    }

    if (in4.size(0) == 1) {
      if (in7.size(0) == 1) {
        i2 = in6.size(0);
      } else {
        i2 = in7.size(0);
      }
    } else {
      i2 = in4.size(0);
    }

    if (in3.size(0) == 1) {
      i3 = in5.size(0);
    } else {
      i3 = in3.size(0);
    }

    if (in7.size(0) == 1) {
      loop_ub = in6.size(0);
    } else {
      loop_ub = in7.size(0);
    }

    if (i == 1) {
      i = in3.size(0);
    } else if (i1 == 1) {
      if (i2 == 1) {
        if (i3 == 1) {
          if (in4.size(0) == 1) {
            i = in1.size(0);
          } else {
            i = in4.size(0);
          }
        } else if (in3.size(0) == 1) {
          i = in5.size(0);
        } else {
          i = in3.size(0);
        }
      } else if (in4.size(0) == 1) {
        if (in7.size(0) == 1) {
          i = in6.size(0);
        } else {
          i = in7.size(0);
        }
      } else {
        i = in4.size(0);
      }
    } else if (loop_ub == 1) {
      if (in5.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    b_in3.set_size(i);
    stride_0_0 = (in3.size(0) != 1);
    stride_1_0 = (in1.size(0) != 1);
    stride_2_0 = (in4.size(0) != 1);
    stride_3_0 = (in5.size(0) != 1);
    stride_4_0 = (in3.size(0) != 1);
    stride_5_0 = (in6.size(0) != 1);
    stride_6_0 = (in7.size(0) != 1);
    stride_7_0 = (in4.size(0) != 1);
    stride_8_0 = (in1.size(0) != 1);
    stride_9_0 = (in5.size(0) != 1);
    stride_10_0 = (in6.size(0) != 1);
    stride_11_0 = (in7.size(0) != 1);
    if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (i == 1) {
      if (in5.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (in4.size(0) == 1) {
      if (in7.size(0) == 1) {
        i1 = in6.size(0);
      } else {
        i1 = in7.size(0);
      }
    } else {
      i1 = in4.size(0);
    }

    if (in3.size(0) == 1) {
      i2 = in5.size(0);
    } else {
      i2 = in3.size(0);
    }

    if (in7.size(0) == 1) {
      i3 = in6.size(0);
    } else {
      i3 = in7.size(0);
    }

    if (i == 1) {
      if (i1 == 1) {
        if (i2 == 1) {
          if (in4.size(0) == 1) {
            i = in1.size(0);
          } else {
            i = in4.size(0);
          }
        } else if (in3.size(0) == 1) {
          i = in5.size(0);
        } else {
          i = in3.size(0);
        }
      } else if (in4.size(0) == 1) {
        if (in7.size(0) == 1) {
          i = in6.size(0);
        } else {
          i = in7.size(0);
        }
      } else {
        i = in4.size(0);
      }
    } else if (i3 == 1) {
      if (in5.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in5.size(0);
      }
    } else if (in7.size(0) == 1) {
      i = in6.size(0);
    } else {
      i = in7.size(0);
    }

    if (i == 1) {
      loop_ub = in3.size(0);
    } else {
      if (in7.size(0) == 1) {
        i = in6.size(0);
      } else {
        i = in7.size(0);
      }

      if (i == 1) {
        if (in5.size(0) == 1) {
          i = in1.size(0);
        } else {
          i = in5.size(0);
        }
      } else if (in7.size(0) == 1) {
        i = in6.size(0);
      } else {
        i = in7.size(0);
      }

      if (i == 1) {
        if (in4.size(0) == 1) {
          if (in7.size(0) == 1) {
            i = in6.size(0);
          } else {
            i = in7.size(0);
          }
        } else {
          i = in4.size(0);
        }

        if (i == 1) {
          if (in3.size(0) == 1) {
            i = in5.size(0);
          } else {
            i = in3.size(0);
          }

          if (i == 1) {
            if (in4.size(0) == 1) {
              loop_ub = in1.size(0);
            } else {
              loop_ub = in4.size(0);
            }
          } else if (in3.size(0) == 1) {
            loop_ub = in5.size(0);
          } else {
            loop_ub = in3.size(0);
          }
        } else if (in4.size(0) == 1) {
          if (in7.size(0) == 1) {
            loop_ub = in6.size(0);
          } else {
            loop_ub = in7.size(0);
          }
        } else {
          loop_ub = in4.size(0);
        }
      } else {
        if (in7.size(0) == 1) {
          i = in6.size(0);
        } else {
          i = in7.size(0);
        }

        if (i == 1) {
          if (in5.size(0) == 1) {
            loop_ub = in1.size(0);
          } else {
            loop_ub = in5.size(0);
          }
        } else if (in7.size(0) == 1) {
          loop_ub = in6.size(0);
        } else {
          loop_ub = in7.size(0);
        }
      }
    }

    for (i = 0; i < loop_ub; i++) {
      b_in3[i] = in3[i * stride_0_0] * (((in1[i * stride_1_0] * in4[i *
        stride_2_0] - in5[i * stride_3_0] * (1.0 / in3[i * stride_4_0])) +
        static_cast<real_T>(in6[i * stride_5_0]) / in7[i * stride_6_0] * in4[i *
        stride_7_0]) / ((in1[i * stride_8_0] - in5[i * stride_9_0]) +
                        static_cast<real_T>(in6[i * stride_10_0]) / in7[i *
                        stride_11_0]));
    }

    coder::f_bsxfun(in2, b_in3, in1);
  }

  namespace coder
  {
    void rescaleKernel(::coder::array<real_T, 1U> &A, const ::coder::array<
                       real_T, 1U> &inputMin, const ::coder::array<real_T, 1U>
                       &inputMax)
    {
      ::coder::array<real_T, 1U> b_r1;
      ::coder::array<real_T, 1U> e1;
      ::coder::array<real_T, 1U> iMax;
      ::coder::array<real_T, 1U> iMin;
      ::coder::array<real_T, 1U> r;
      ::coder::array<real_T, 1U> r1;
      ::coder::array<real_T, 1U> r3;
      ::coder::array<real_T, 1U> sigma;
      ::coder::array<int8_T, 1U> x_tmp;
      if (A.size(0) != 0) {
        real_T varargin_2;
        int32_T i;
        int32_T i1;
        int32_T i10;
        int32_T i11;
        int32_T i12;
        int32_T i2;
        int32_T i3;
        int32_T i4;
        int32_T i5;
        int32_T i6;
        int32_T i7;
        int32_T i8;
        int32_T i9;
        int32_T k;
        r.set_size(inputMax.size(0));
        k = inputMax.size(0);
        for (i = 0; i < k; i++) {
          varargin_2 = inputMax[i];
          r[i] = std::fmin(0.0, varargin_2);
        }

        c_bsxfun(r, inputMin, sigma);
        e1.set_size(A.size(0));
        k = A.size(0) - 1;
        for (i = 0; i <= k; i++) {
          e1[i] = A[i];
        }

        d_bsxfun(e1, sigma, A);
        d_bsxfun(inputMin, sigma, iMin);
        d_bsxfun(inputMax, sigma, iMax);
        b_abs(iMax, r);
        b_abs(iMin, e1);
        internal::maximum2(r, e1, r1);
        b_log2(r1, sigma, e1);
        r3.set_size(e1.size(0));
        k = e1.size(0);
        for (i = 0; i < k; i++) {
          r3[i] = e1[i] - 1.0;
        }

        pow2(r3, b_r1);
        k = e1.size(0);
        for (i = 0; i < k; i++) {
          e1[i] = (e1[i] + 1.0) / 2.0;
        }

        i = e1.size(0);
        for (k = 0; k < i; k++) {
          e1[k] = std::trunc(e1[k]);
        }

        k = e1.size(0);
        for (i = 0; i < k; i++) {
          e1[i] = e1[i] - 1.0;
        }

        pow2(e1, r3);
        if (iMax.size(0) == b_r1.size(0)) {
          r.set_size(iMax.size(0));
          k = iMax.size(0);
          for (i = 0; i < k; i++) {
            r[i] = iMax[i] / b_r1[i];
          }
        } else {
          rdivide(r, iMax, b_r1);
        }

        e1.set_size(r3.size(0));
        k = r3.size(0);
        for (i = 0; i < k; i++) {
          e1[i] = 0.0 / r3[i];
        }

        if (iMin.size(0) == b_r1.size(0)) {
          r1.set_size(iMin.size(0));
          k = iMin.size(0);
          for (i = 0; i < k; i++) {
            r1[i] = iMin[i] / b_r1[i];
          }
        } else {
          rdivide(r1, iMin, b_r1);
        }

        if (iMin.size(0) == iMax.size(0)) {
          x_tmp.set_size(iMin.size(0));
          k = iMin.size(0);
          for (i = 0; i < k; i++) {
            x_tmp[i] = static_cast<int8_T>(iMin[i] == iMax[i]);
          }
        } else {
          binary_expand_op(x_tmp, iMin, iMax);
        }

        if (iMax.size(0) == 1) {
          i = r3.size(0);
        } else {
          i = iMax.size(0);
        }

        if (iMin.size(0) == 1) {
          k = r3.size(0);
        } else {
          k = iMin.size(0);
        }

        if (iMax.size(0) == 1) {
          i1 = r3.size(0);
        } else {
          i1 = iMax.size(0);
        }

        if (i1 == 1) {
          if (iMin.size(0) == 1) {
            i1 = r3.size(0);
          } else {
            i1 = iMin.size(0);
          }
        } else if (iMax.size(0) == 1) {
          i1 = r3.size(0);
        } else {
          i1 = iMax.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i2 = r3.size(0);
        } else {
          i2 = x_tmp.size(0);
        }

        if (iMax.size(0) == 1) {
          i3 = r3.size(0);
        } else {
          i3 = iMax.size(0);
        }

        if (i3 == 1) {
          if (iMin.size(0) == 1) {
            i3 = r3.size(0);
          } else {
            i3 = iMin.size(0);
          }
        } else if (iMax.size(0) == 1) {
          i3 = r3.size(0);
        } else {
          i3 = iMax.size(0);
        }

        if (iMax.size(0) == 1) {
          i4 = r3.size(0);
        } else {
          i4 = iMax.size(0);
        }

        if (i3 == 1) {
          if (x_tmp.size(0) == 1) {
            i3 = r3.size(0);
          } else {
            i3 = x_tmp.size(0);
          }
        } else if (i4 == 1) {
          if (iMin.size(0) == 1) {
            i3 = r3.size(0);
          } else {
            i3 = iMin.size(0);
          }
        } else if (iMax.size(0) == 1) {
          i3 = r3.size(0);
        } else {
          i3 = iMax.size(0);
        }

        if ((iMax.size(0) == r3.size(0)) && (iMin.size(0) == r3.size(0)) && (i ==
             k) && (x_tmp.size(0) == r3.size(0)) && (i1 == i2) && (i3 == r3.size
             (0))) {
          k = iMax.size(0);
          for (i = 0; i < k; i++) {
            iMax[i] = 1.0 / ((iMax[i] / r3[i] - iMin[i] / r3[i]) +
                             static_cast<real_T>(x_tmp[i]) / r3[i]) / r3[i];
          }

          e_bsxfun(A, iMax, sigma);
        } else {
          binary_expand_op(sigma, A, iMax, r3, iMin, x_tmp);
        }

        if (r.size(0) == 1) {
          i = e1.size(0);
        } else {
          i = r.size(0);
        }

        if (r1.size(0) == 1) {
          k = r3.size(0);
        } else {
          k = r1.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i1 = b_r1.size(0);
        } else {
          i1 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i2 = e1.size(0);
        } else {
          i2 = r.size(0);
        }

        if (i2 == 1) {
          if (r1.size(0) == 1) {
            i2 = r3.size(0);
          } else {
            i2 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i2 = e1.size(0);
        } else {
          i2 = r.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i3 = b_r1.size(0);
        } else {
          i3 = x_tmp.size(0);
        }

        if (i3 == 1) {
          i3 = e1.size(0);
        } else if (x_tmp.size(0) == 1) {
          i3 = b_r1.size(0);
        } else {
          i3 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i4 = r1.size(0);
        } else {
          i4 = r.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i5 = b_r1.size(0);
        } else {
          i5 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i6 = e1.size(0);
        } else {
          i6 = r.size(0);
        }

        if (i6 == 1) {
          if (r1.size(0) == 1) {
            i6 = r3.size(0);
          } else {
            i6 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i6 = e1.size(0);
        } else {
          i6 = r.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i7 = b_r1.size(0);
        } else {
          i7 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i8 = e1.size(0);
        } else {
          i8 = r.size(0);
        }

        if (i6 == 1) {
          if (i7 == 1) {
            i6 = e1.size(0);
          } else if (x_tmp.size(0) == 1) {
            i6 = b_r1.size(0);
          } else {
            i6 = x_tmp.size(0);
          }
        } else if (i8 == 1) {
          if (r1.size(0) == 1) {
            i6 = r3.size(0);
          } else {
            i6 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i6 = e1.size(0);
        } else {
          i6 = r.size(0);
        }

        if (r.size(0) == 1) {
          i7 = r1.size(0);
        } else {
          i7 = r.size(0);
        }

        if (i7 == 1) {
          if (x_tmp.size(0) == 1) {
            i7 = b_r1.size(0);
          } else {
            i7 = x_tmp.size(0);
          }
        } else if (r.size(0) == 1) {
          i7 = r1.size(0);
        } else {
          i7 = r.size(0);
        }

        if (r.size(0) == 1) {
          i8 = e1.size(0);
        } else {
          i8 = r.size(0);
        }

        if (i8 == 1) {
          if (r1.size(0) == 1) {
            i8 = r3.size(0);
          } else {
            i8 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i8 = e1.size(0);
        } else {
          i8 = r.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i9 = b_r1.size(0);
        } else {
          i9 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i10 = e1.size(0);
        } else {
          i10 = r.size(0);
        }

        if (i8 == 1) {
          if (i9 == 1) {
            i8 = e1.size(0);
          } else if (x_tmp.size(0) == 1) {
            i8 = b_r1.size(0);
          } else {
            i8 = x_tmp.size(0);
          }
        } else if (i10 == 1) {
          if (r1.size(0) == 1) {
            i8 = r3.size(0);
          } else {
            i8 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i8 = e1.size(0);
        } else {
          i8 = r.size(0);
        }

        if (r.size(0) == 1) {
          i9 = r1.size(0);
        } else {
          i9 = r.size(0);
        }

        if (r.size(0) == 1) {
          i10 = e1.size(0);
        } else {
          i10 = r.size(0);
        }

        if (i10 == 1) {
          if (r1.size(0) == 1) {
            i10 = r3.size(0);
          } else {
            i10 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i10 = e1.size(0);
        } else {
          i10 = r.size(0);
        }

        if (x_tmp.size(0) == 1) {
          i11 = b_r1.size(0);
        } else {
          i11 = x_tmp.size(0);
        }

        if (r.size(0) == 1) {
          i12 = e1.size(0);
        } else {
          i12 = r.size(0);
        }

        if (i8 == 1) {
          if (i9 == 1) {
            if (x_tmp.size(0) == 1) {
              i8 = b_r1.size(0);
            } else {
              i8 = x_tmp.size(0);
            }
          } else if (r.size(0) == 1) {
            i8 = r1.size(0);
          } else {
            i8 = r.size(0);
          }
        } else if (i10 == 1) {
          if (i11 == 1) {
            i8 = e1.size(0);
          } else if (x_tmp.size(0) == 1) {
            i8 = b_r1.size(0);
          } else {
            i8 = x_tmp.size(0);
          }
        } else if (i12 == 1) {
          if (r1.size(0) == 1) {
            i8 = r3.size(0);
          } else {
            i8 = r1.size(0);
          }
        } else if (r.size(0) == 1) {
          i8 = e1.size(0);
        } else {
          i8 = r.size(0);
        }

        if ((r.size(0) == e1.size(0)) && (r1.size(0) == r3.size(0)) && (i == k) &&
            (x_tmp.size(0) == b_r1.size(0)) && (i1 == e1.size(0)) && (i2 == i3) &&
            (r.size(0) == r1.size(0)) && (x_tmp.size(0) == b_r1.size(0)) && (i4 ==
             i5) && (i6 == i7) && (r3.size(0) == i8)) {
          k = r3.size(0);
          for (i = 0; i < k; i++) {
            varargin_2 = static_cast<real_T>(x_tmp[i]) / b_r1[i];
            r3[i] = r3[i] * (((r[i] * e1[i] - r1[i] * (1.0 / r3[i])) +
                              varargin_2 * e1[i]) / ((r[i] - r1[i]) + varargin_2));
          }

          f_bsxfun(sigma, r3, r);
        } else {
          binary_expand_op(r, sigma, r3, e1, r1, x_tmp, b_r1);
        }

        bsxfun(r, e1);
        b_bsxfun(e1, r);
        c_bsxfun(r, A);
      }
    }
  }
}

// End of code generation (rescaleKernel.cpp)
