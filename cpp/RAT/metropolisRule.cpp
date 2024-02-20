//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// metropolisRule.cpp
//
// Code generation for function 'metropolisRule'
//

// Include files
#include "metropolisRule.h"
#include "RATMain_types.h"
#include "find.h"
#include "rand.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  static void e_binary_expand_op(::coder::array<boolean_T, 1U> &in1, const ::
    coder::array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::
    coder::array<real_T, 1U> &in4);
  static void minus(::coder::array<real_T, 1U> &in1, const ::coder::array<real_T,
                    1U> &in2, const ::coder::array<real_T, 1U> &in3);
}

// Function Definitions
namespace RAT
{
  static void e_binary_expand_op(::coder::array<boolean_T, 1U> &in1, const ::
    coder::array<real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3, const ::
    coder::array<real_T, 1U> &in4)
  {
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    int32_T stride_2_0;
    if (in4.size(0) == 1) {
      if (in3.size(0) == 1) {
        i = in2.size(0);
      } else {
        i = in3.size(0);
      }
    } else {
      i = in4.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_2_0 = (in4.size(0) != 1);
    if (in4.size(0) == 1) {
      if (in3.size(0) == 1) {
        loop_ub = in2.size(0);
      } else {
        loop_ub = in3.size(0);
      }
    } else {
      loop_ub = in4.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = (in2[i * stride_0_0] * in3[i * stride_1_0] > in4[i * stride_2_0]);
    }
  }

  static void minus(::coder::array<real_T, 1U> &in1, const ::coder::array<real_T,
                    1U> &in2, const ::coder::array<real_T, 1U> &in3)
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
      in1[i] = in2[i * stride_0_0] - in3[i * stride_1_0];
    }
  }

  void metropolisRule(const struct13_T *DREAMPar, const ::coder::array<real_T,
                      1U> &log_L_xnew, const ::coder::array<real_T, 1U>
                      &log_PR_xnew, const ::coder::array<real_T, 1U> &log_L_xold,
                      const ::coder::array<real_T, 1U> &log_PR_xold, ::coder::
                      array<boolean_T, 1U> &accept, ::coder::array<real_T, 1U>
                      &idx_accept)
  {
    ::coder::array<real_T, 1U> Z;
    ::coder::array<real_T, 1U> r;
    ::coder::array<real_T, 1U> r1;
    ::coder::array<int32_T, 1U> r2;
    ::coder::array<boolean_T, 1U> b_accept;
    int32_T i;
    int32_T k;

    //  Metropolis rule for acceptance or rejection
    //  No ABC --> regular MCMC with prior and likelihood
    //  Calculate the likelihood ratio
    //  Calculate the prior ration
    //  Calculate product of two probabily ratios
    //  Generate random numbers
    coder::b_rand(DREAMPar->N, Z);

    //  Find which alfa's are greater than Z
    if (log_L_xnew.size(0) == log_L_xold.size(0)) {
      r.set_size(log_L_xnew.size(0));
      k = log_L_xnew.size(0);
      for (i = 0; i < k; i++) {
        r[i] = log_L_xnew[i] - log_L_xold[i];
      }
    } else {
      minus(r, log_L_xnew, log_L_xold);
    }

    i = r.size(0);
    for (k = 0; k < i; k++) {
      r[k] = std::exp(r[k]);
    }

    if (log_PR_xnew.size(0) == log_PR_xold.size(0)) {
      r1.set_size(log_PR_xnew.size(0));
      k = log_PR_xnew.size(0);
      for (i = 0; i < k; i++) {
        r1[i] = log_PR_xnew[i] - log_PR_xold[i];
      }
    } else {
      minus(r1, log_PR_xnew, log_PR_xold);
    }

    i = r1.size(0);
    for (k = 0; k < i; k++) {
      r1[k] = std::exp(r1[k]);
    }

    if (r.size(0) == 1) {
      i = r1.size(0);
    } else {
      i = r.size(0);
    }

    if ((r.size(0) == r1.size(0)) && (i == Z.size(0))) {
      accept.set_size(r.size(0));
      k = r.size(0);
      for (i = 0; i < k; i++) {
        accept[i] = (r[i] * r1[i] > Z[i]);
      }
    } else {
      e_binary_expand_op(accept, r, r1, Z);
    }

    //  ABC --> check if summary metrics as prior (diagnostic Bayes) or likelihood (regular ABC)
    //  Now derive which proposals to accept (row numbers of X)
    b_accept.set_size(accept.size(0));
    k = accept.size(0);
    for (i = 0; i < k; i++) {
      b_accept[i] = accept[i];
    }

    coder::eml_find(b_accept, r2);
    idx_accept.set_size(r2.size(0));
    k = r2.size(0);
    for (i = 0; i < k; i++) {
      idx_accept[i] = r2[i];
    }
  }
}

// End of code generation (metropolisRule.cpp)