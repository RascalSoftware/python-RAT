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
#include "RATMain_data.h"
#include "RATMain_rtwutil.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void binary_expand_op(::coder::array<creal_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in3, const ::coder::array<real_T, 2U> &in4,
                        int32_T in5, const ::coder::array<creal_T, 2U> &in6,
                        const ::coder::array<creal_T, 1U> &in7)
  {
    ::coder::array<creal_T, 2U> b_in3;
    ::coder::array<creal_T, 2U> c_in3;
    ::coder::array<creal_T, 2U> d_in3;
    real_T b_in3_re_tmp;
    real_T c_in3_re_tmp;
    real_T d_in3_re_tmp;
    real_T in3_re_tmp;
    int32_T b_loop_ub;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_1;
    int32_T stride_1_1;
    if (in4.size(1) == 1) {
      i = in3.size(1);
    } else {
      i = in4.size(1);
    }

    b_in3.set_size(1, i);
    stride_0_1 = (in3.size(1) != 1);
    stride_1_1 = (in4.size(1) != 1);
    if (in4.size(1) == 1) {
      loop_ub = in3.size(1);
    } else {
      loop_ub = in4.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      b_in3[i].re = in3[i * stride_0_1] - in4[in5 + in4.size(0) * (i *
        stride_1_1)];
      b_in3[i].im = 0.0;
    }

    c_in3.set_size(1, in6.size(1));
    loop_ub = in6.size(1);
    for (i = 0; i < loop_ub; i++) {
      c_in3[i].re = 0.0;
      c_in3[i].im = 0.0;
      b_loop_ub = b_in3.size(1);
      for (stride_0_1 = 0; stride_0_1 < b_loop_ub; stride_0_1++) {
        in3_re_tmp = b_in3[stride_0_1].re;
        b_in3_re_tmp = in6[stride_0_1 + in6.size(0) * i].im;
        c_in3_re_tmp = b_in3[stride_0_1].im;
        d_in3_re_tmp = in6[stride_0_1 + in6.size(0) * i].re;
        c_in3[i].re = c_in3[i].re + (in3_re_tmp * d_in3_re_tmp - c_in3_re_tmp *
          b_in3_re_tmp);
        c_in3[i].im = c_in3[i].im + (in3_re_tmp * b_in3_re_tmp + c_in3_re_tmp *
          d_in3_re_tmp);
      }
    }

    if (in7.size(0) == 1) {
      i = c_in3.size(1);
    } else {
      i = in7.size(0);
    }

    d_in3.set_size(1, i);
    stride_0_1 = (c_in3.size(1) != 1);
    stride_1_1 = (in7.size(0) != 1);
    if (in7.size(0) == 1) {
      loop_ub = c_in3.size(1);
    } else {
      loop_ub = in7.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      real_T ai;
      real_T ar;
      b_loop_ub = i * stride_0_1;
      ar = c_in3[b_loop_ub].re;
      ai = c_in3[b_loop_ub].im;
      b_loop_ub = i * stride_1_1;
      c_in3_re_tmp = in7[b_loop_ub].re;
      d_in3_re_tmp = -in7[b_loop_ub].im;
      if (d_in3_re_tmp == 0.0) {
        if (ai == 0.0) {
          d_in3[i].re = ar / c_in3_re_tmp;
          d_in3[i].im = 0.0;
        } else if (ar == 0.0) {
          d_in3[i].re = 0.0;
          d_in3[i].im = ai / c_in3_re_tmp;
        } else {
          d_in3[i].re = ar / c_in3_re_tmp;
          d_in3[i].im = ai / c_in3_re_tmp;
        }
      } else if (c_in3_re_tmp == 0.0) {
        if (ar == 0.0) {
          d_in3[i].re = ai / d_in3_re_tmp;
          d_in3[i].im = 0.0;
        } else if (ai == 0.0) {
          d_in3[i].re = 0.0;
          d_in3[i].im = -(ar / d_in3_re_tmp);
        } else {
          d_in3[i].re = ai / d_in3_re_tmp;
          d_in3[i].im = -(ar / d_in3_re_tmp);
        }
      } else {
        real_T brm;
        brm = std::abs(c_in3_re_tmp);
        in3_re_tmp = std::abs(d_in3_re_tmp);
        if (brm > in3_re_tmp) {
          b_in3_re_tmp = d_in3_re_tmp / c_in3_re_tmp;
          in3_re_tmp = c_in3_re_tmp + b_in3_re_tmp * d_in3_re_tmp;
          d_in3[i].re = (ar + b_in3_re_tmp * ai) / in3_re_tmp;
          d_in3[i].im = (ai - b_in3_re_tmp * ar) / in3_re_tmp;
        } else if (in3_re_tmp == brm) {
          if (c_in3_re_tmp > 0.0) {
            b_in3_re_tmp = 0.5;
          } else {
            b_in3_re_tmp = -0.5;
          }

          if (d_in3_re_tmp > 0.0) {
            in3_re_tmp = 0.5;
          } else {
            in3_re_tmp = -0.5;
          }

          d_in3[i].re = (ar * b_in3_re_tmp + ai * in3_re_tmp) / brm;
          d_in3[i].im = (ai * b_in3_re_tmp - ar * in3_re_tmp) / brm;
        } else {
          b_in3_re_tmp = c_in3_re_tmp / d_in3_re_tmp;
          in3_re_tmp = d_in3_re_tmp + b_in3_re_tmp * c_in3_re_tmp;
          d_in3[i].re = (b_in3_re_tmp * ar + ai) / in3_re_tmp;
          d_in3[i].im = (b_in3_re_tmp * ai - ar) / in3_re_tmp;
        }
      }
    }

    in1.set_size(1, d_in3.size(1));
    loop_ub = d_in3.size(1);
    for (i = 0; i < loop_ub; i++) {
      creal_T varargin_1;
      creal_T varargout_1;
      varargin_1 = d_in3[i];
      varargout_1.re = varargin_1.re * varargin_1.re - varargin_1.im *
        varargin_1.im;
      in3_re_tmp = varargin_1.re * varargin_1.im;
      varargout_1.im = in3_re_tmp + in3_re_tmp;
      in1[i] = varargout_1;
    }
  }

  void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::array<
                        real_T, 1U> &in3, const ::coder::array<real_T, 1U> &in4)
  {
    ::coder::array<real_T, 1U> b_in1;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    int32_T stride_2_0;
    if (in4.size(0) == 1) {
      if (in3.size(0) == 1) {
        i = in1.size(0);
      } else {
        i = in3.size(0);
      }
    } else {
      i = in4.size(0);
    }

    b_in1.set_size(i);
    stride_0_0 = (in1.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_2_0 = (in4.size(0) != 1);
    if (in4.size(0) == 1) {
      if (in3.size(0) == 1) {
        loop_ub = in1.size(0);
      } else {
        loop_ub = in3.size(0);
      }
    } else {
      loop_ub = in4.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      b_in1[i] = (in1[i * stride_0_0] - in3[i * stride_1_0]) / in4[i *
        stride_2_0];
    }

    in1.set_size(b_in1.size(0));
    loop_ub = b_in1.size(0);
    for (i = 0; i < loop_ub; i++) {
      in1[i] = b_in1[i];
    }

    loop_ub = in1.size(0);
    for (i = 0; i < loop_ub; i++) {
      real_T varargin_1;
      varargin_1 = in1[i];
      in1[i] = varargin_1 * varargin_1;
    }
  }

  void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
                        real_T, 2U> &in3, int32_T in4, int32_T in5, const ::
                        coder::array<real_T, 2U> &in6, int32_T in7, int32_T in8,
                        const ::coder::array<real_T, 2U> &in9)
  {
    ::coder::array<real_T, 2U> b_in3;
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T aux_2_1;
    int32_T b_loop_ub;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    int32_T stride_2_0;
    int32_T stride_2_1;
    if (in9.size(0) == 1) {
      if (in7 + 1 == 1) {
        i = in4 + 1;
      } else {
        i = in7 + 1;
      }
    } else {
      i = in9.size(0);
    }

    if (in9.size(1) == 1) {
      if (in8 + 1 == 1) {
        i1 = in5 + 1;
      } else {
        i1 = in8 + 1;
      }
    } else {
      i1 = in9.size(1);
    }

    b_in3.set_size(i, i1);
    stride_0_0 = (in4 + 1 != 1);
    stride_0_1 = (in5 + 1 != 1);
    stride_1_0 = (in7 + 1 != 1);
    stride_1_1 = (in8 + 1 != 1);
    stride_2_0 = (in9.size(0) != 1);
    stride_2_1 = (in9.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    aux_2_1 = 0;
    if (in9.size(1) == 1) {
      if (in8 + 1 == 1) {
        loop_ub = in5 + 1;
      } else {
        loop_ub = in8 + 1;
      }
    } else {
      loop_ub = in9.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      i1 = in9.size(0);
      if (i1 == 1) {
        if (in7 + 1 == 1) {
          b_loop_ub = in4 + 1;
        } else {
          b_loop_ub = in7 + 1;
        }
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in3[i1 + b_in3.size(0) * i] = (in3[i1 * stride_0_0 + in3.size(0) *
          aux_0_1] - in6[i1 * stride_1_0 + in6.size(0) * aux_1_1]) / in9[i1 *
          stride_2_0 + in9.size(0) * aux_2_1];
      }

      aux_2_1 += stride_2_1;
      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    in1.set_size(b_in3.size(0), b_in3.size(1));
    loop_ub = b_in3.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b_in3.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        real_T varargin_1;
        varargin_1 = b_in3[i1 + b_in3.size(0) * i];
        in1[i1 + in1.size(0) * i] = rt_powd_snf(varargin_1, 2.0);
      }
    }
  }

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
