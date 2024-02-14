//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// gelman.cpp
//
// Code generation for function 'gelman'
//

// Include files
#include "gelman.h"
#include "RATMain_types.h"
#include "mean.h"
#include "rt_nonfinite.h"
#include "sqrt1.h"
#include "var.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, real_T in2,
    real_T in3, const ::coder::array<real_T, 2U> &in4, real_T in5, const ::coder::
    array<real_T, 3U> &in6, const ::coder::array<real_T, 2U> &in7);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, real_T in2,
    real_T in3, const ::coder::array<real_T, 2U> &in4, real_T in5, const ::coder::
    array<real_T, 3U> &in6, const ::coder::array<real_T, 2U> &in7)
  {
    real_T c_in6;
    int32_T b_in6;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_1;
    int32_T stride_1_1;
    int32_T stride_2_1;
    b_in6 = in6.size(0);
    c_in6 = (static_cast<real_T>(in6.size(0)) - 1.0) / static_cast<real_T>
      (in6.size(2)) / static_cast<real_T>(in6.size(0));
    if (in4.size(1) == 1) {
      if (in7.size(1) == 1) {
        i = in4.size(1);
      } else {
        i = in7.size(1);
      }
    } else {
      i = in4.size(1);
    }

    in1.set_size(1, i);
    stride_0_1 = (in4.size(1) != 1);
    stride_1_1 = (in7.size(1) != 1);
    stride_2_1 = (in4.size(1) != 1);
    if (in4.size(1) == 1) {
      if (in7.size(1) == 1) {
        loop_ub = in4.size(1);
      } else {
        loop_ub = in7.size(1);
      }
    } else {
      loop_ub = in4.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = in2 * (in3 * in4[i * stride_0_1] + in5 * (static_cast<real_T>
        (b_in6) * in7[i * stride_1_1])) / in4[i * stride_2_1] - c_in6;
    }
  }

  void gelman(const ::coder::array<real_T, 3U> &chain, const struct13_T
              *DREAMPar, ::coder::array<real_T, 2U> &R_stat)
  {
    ::coder::array<real_T, 3U> r1;
    ::coder::array<real_T, 2U> W;
    ::coder::array<real_T, 2U> b_chain;
    ::coder::array<real_T, 2U> r;
    ::coder::array<real_T, 2U> var_chain;
    int32_T b_loop_ub_tmp;
    int32_T i;
    int32_T loop_ub_tmp;

    //  Calculates the R-statistic convergence diagnostic
    //  ----------------------------------------------------
    //  For more information please refer to: Gelman, A. and D.R. Rubin, 1992.
    //  Inference from Iterative Simulation Using Multiple chain,
    //  Statistical Science, Volume 7, Issue 4, 457-472.
    //
    //  Written by Jasper A. Vrugt
    //  Los Alamos, August 2007
    //  ----------------------------------------------------
    //  Compute the dimensions of chain
    loop_ub_tmp = static_cast<int32_T>(DREAMPar->N);
    b_loop_ub_tmp = static_cast<int32_T>(DREAMPar->d);
    var_chain.set_size(loop_ub_tmp, b_loop_ub_tmp);
    for (i = 0; i < b_loop_ub_tmp; i++) {
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        var_chain[i1 + var_chain.size(0) * i] = 0.0;
      }
    }

    if (chain.size(0) < 10) {
      //  Set the R-statistic to a large value
      R_stat.set_size(1, b_loop_ub_tmp);
      for (i = 0; i < b_loop_ub_tmp; i++) {
        R_stat[i] = rtNaN;
      }
    } else {
      real_T a;
      real_T b_a;
      real_T c_a;
      int32_T loop_ub;

      //  Step 1: Determine the _chainuence means
      //  Step 1: Determine the variance between the _chainuence means
      //  Step 2: Compute the variance of the various chain
      for (int32_T zz{0}; zz < loop_ub_tmp; zz++) {
        loop_ub = chain.size(1);
        b_chain.set_size(chain.size(0), chain.size(1));
        for (i = 0; i < loop_ub; i++) {
          int32_T b_loop_ub;
          b_loop_ub = chain.size(0);
          for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
            b_chain[i1 + b_chain.size(0) * i] = chain[(i1 + chain.size(0) * i) +
              chain.size(0) * chain.size(1) * zz];
          }
        }

        coder::var(b_chain, r);
        loop_ub = r.size(1);
        for (i = 0; i < loop_ub; i++) {
          var_chain[zz + var_chain.size(0) * i] = r[i];
        }
      }

      //  Step 2: Calculate the average of the within _chainuence variances
      coder::mean(var_chain, W);

      //  Step 3: Estimate the target mean
      // mu = mean(mean_chain);
      //  Step 4: Estimate the target variance
      a = (static_cast<real_T>(chain.size(0)) - 1.0) / static_cast<real_T>
        (chain.size(0));
      b_a = 1.0 / static_cast<real_T>(chain.size(0));

      //  Step 5: Compute the R-statistic
      c_a = (static_cast<real_T>(chain.size(2)) + 1.0) / static_cast<real_T>
        (chain.size(2));
      coder::mean(chain, r1);
      loop_ub_tmp = chain.size(2);
      b_chain.set_size(chain.size(2), b_loop_ub_tmp);
      for (i = 0; i < b_loop_ub_tmp; i++) {
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          b_chain[i1 + b_chain.size(0) * i] = r1[i + b_loop_ub_tmp * i1];
        }
      }

      coder::var(b_chain, r);
      if (W.size(1) == 1) {
        i = r.size(1);
      } else {
        i = W.size(1);
      }

      if ((W.size(1) == r.size(1)) && (i == W.size(1))) {
        real_T c_chain;
        c_chain = (static_cast<real_T>(chain.size(0)) - 1.0) /
          static_cast<real_T>(chain.size(2)) / static_cast<real_T>(chain.size(0));
        R_stat.set_size(1, W.size(1));
        loop_ub = W.size(1);
        for (i = 0; i < loop_ub; i++) {
          real_T d;
          d = W[i];
          R_stat[i] = c_a * (a * d + b_a * (static_cast<real_T>(chain.size(0)) *
            r[i])) / d - c_chain;
        }
      } else {
        binary_expand_op(R_stat, c_a, a, W, b_a, chain, r);
      }

      coder::c_sqrt(R_stat);
    }
  }
}

// End of code generation (gelman.cpp)
