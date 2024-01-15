//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nest2pos.cpp
//
// Code generation for function 'nest2pos'
//

// Include files
#include "nest2pos.h"
#include "find.h"
#include "minOrMax.h"
#include "rand.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const int32_T
    in4[2], real_T in5, real_T in6);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const int32_T
    in4[2], real_T in5, real_T in6)
  {
    ::coder::array<real_T, 1U> c_in2;
    int32_T b_in2;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    b_in2 = in2.size(1);
    in1.set_size(static_cast<int32_T>(in5) + in4[1]);
    loop_ub = in4[1];
    for (i = 0; i < loop_ub; i++) {
      in1[i] = -in3[i] / in5;
    }

    loop_ub = static_cast<int32_T>(in5);
    for (i = 0; i < loop_ub; i++) {
      in1[i + in4[1]] = -in6 / in5;
    }

    if (in1.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in1.size(0);
    }

    c_in2.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in1.size(0) != 1);
    if (in1.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in1.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      c_in2[i] = in2[i * stride_0_0 + in2.size(0) * (b_in2 - 1)] + in1[i *
        stride_1_0];
    }

    in1.set_size(c_in2.size(0));
    loop_ub = c_in2.size(0);
    for (i = 0; i < loop_ub; i++) {
      in1[i] = c_in2[i];
    }
  }

  void nest2pos(const ::coder::array<real_T, 2U> &nest_samples, real_T Nlive, ::
                coder::array<real_T, 2U> &post_samples)
  {
    ::coder::array<real_T, 2U> b_nest_samples;
    ::coder::array<real_T, 2U> y;
    ::coder::array<real_T, 1U> b_y;
    ::coder::array<real_T, 1U> logWt;
    ::coder::array<int32_T, 1U> idx;
    ::coder::array<boolean_T, 1U> b_logWt;
    real_T b;
    real_T logWtmax;
    int32_T sizes[2];
    int32_T i;
    int32_T k;
    int32_T result;
    int8_T input_sizes_idx_1;
    boolean_T empty_non_axis_sizes;

    //
    //  post_samples = nest2pos(nest_samples, Nlive)
    //
    //  Convert nested samples with Nlive livepoints
    //  to samples from the posterior distribution
    //  (logL values in last column of nest_samples)
    //
    //  John Veitch 2009 (modified by J. Romano 2012)
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //  calculate logWt = log(L*w) = logL + logw = logL - i/Nlive
    b = static_cast<real_T>(nest_samples.size(0)) - Nlive;
    if (std::isnan(b)) {
      sizes[0] = 1;
      sizes[1] = 1;
    } else if (b < 1.0) {
      sizes[0] = 1;
      sizes[1] = 0;
    } else {
      sizes[0] = 1;
      sizes[1] = static_cast<int32_T>(b - 1.0) + 1;
    }

    b = static_cast<real_T>(nest_samples.size(0)) - Nlive;
    if (std::isnan(b)) {
      y.set_size(1, 1);
      y[0] = rtNaN;
    } else if (b < 1.0) {
      y.set_size(1, 0);
    } else {
      y.set_size(1, static_cast<int32_T>(b - 1.0) + 1);
      k = static_cast<int32_T>(b - 1.0);
      for (i = 0; i <= k; i++) {
        y[i] = static_cast<real_T>(i) + 1.0;
      }
    }

    b = static_cast<real_T>(nest_samples.size(0)) - Nlive;
    i = static_cast<int32_T>(Nlive) + sizes[1];
    if (nest_samples.size(0) == i) {
      b_y.set_size(i);
      k = sizes[1];
      for (i = 0; i < k; i++) {
        b_y[i] = -y[i] / Nlive;
      }

      k = static_cast<int32_T>(Nlive);
      for (i = 0; i < k; i++) {
        b_y[i + sizes[1]] = -b / Nlive;
      }

      logWt.set_size(nest_samples.size(0));
      k = nest_samples.size(0);
      for (i = 0; i < k; i++) {
        logWt[i] = nest_samples[i + nest_samples.size(0) * (nest_samples.size(1)
          - 1)] + b_y[i];
      }
    } else {
      binary_expand_op(logWt, nest_samples, y, sizes, Nlive, b);
    }

    //  posterior samples are given by the normalized weight
    logWtmax = coder::internal::maximum(logWt);
    k = logWt.size(0);
    for (i = 0; i < k; i++) {
      logWt[i] = logWt[i] - logWtmax;
    }

    //  Wt -> Wt/Wtmax
    //  accept a nested sample as a posterior sample only if its
    //  value is > than a random number drawn from a unif distribution
    coder::b_rand(static_cast<real_T>(nest_samples.size(0)), b_y);
    i = b_y.size(0);
    for (k = 0; k < i; k++) {
      b_y[k] = std::log(b_y[k]);
    }

    if (logWt.size(0) == b_y.size(0)) {
      b_logWt.set_size(logWt.size(0));
      k = logWt.size(0);
      for (i = 0; i < k; i++) {
        b_logWt[i] = (logWt[i] > b_y[i]);
      }

      coder::eml_find(b_logWt, idx);
    } else {
      binary_expand_op(idx, logWt, b_y);
    }

    //  attach log of posterior probabilities as final column of
    //  the posterior samples
    // post_samples(:,Ncol+1) = logWt(idx);
    if ((idx.size(0) != 0) && (nest_samples.size(1) != 0)) {
      result = idx.size(0);
    } else if (idx.size(0) != 0) {
      result = idx.size(0);
    } else {
      result = 0;
    }

    empty_non_axis_sizes = (result == 0);
    if (empty_non_axis_sizes || ((idx.size(0) != 0) && (nest_samples.size(1) !=
          0))) {
      input_sizes_idx_1 = static_cast<int8_T>(nest_samples.size(1));
    } else {
      input_sizes_idx_1 = 0;
    }

    if (empty_non_axis_sizes || (idx.size(0) != 0)) {
      sizes[1] = 1;
    } else {
      sizes[1] = 0;
    }

    b_nest_samples.set_size(idx.size(0), nest_samples.size(1));
    k = nest_samples.size(1);
    for (i = 0; i < k; i++) {
      int32_T loop_ub;
      loop_ub = idx.size(0);
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_nest_samples[i1 + b_nest_samples.size(0) * i] = nest_samples[(idx[i1]
          + nest_samples.size(0) * i) - 1];
      }
    }

    b_y.set_size(idx.size(0));
    k = idx.size(0);
    for (i = 0; i < k; i++) {
      b_y[i] = logWt[idx[i] - 1];
    }

    post_samples.set_size(result, input_sizes_idx_1 + sizes[1]);
    k = input_sizes_idx_1;
    for (i = 0; i < k; i++) {
      for (int32_T i1{0}; i1 < result; i1++) {
        post_samples[i1 + post_samples.size(0) * i] = b_nest_samples[i1 + result
          * i];
      }
    }

    k = sizes[1];
    for (i = 0; i < k; i++) {
      for (int32_T i1{0}; i1 < result; i1++) {
        post_samples[i1 + post_samples.size(0) * input_sizes_idx_1] = b_y[i1];
      }
    }
  }
}

// End of code generation (nest2pos.cpp)
