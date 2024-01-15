//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// initializeDREAM.cpp
//
// Code generation for function 'initializeDREAM'
//

// Include files
#include "initializeDREAM.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "boundaryHandling.h"
#include "calcDensity.h"
#include "drawCR.h"
#include "evaluateModel.h"
#include "rand.h"
#include "repmat.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const
    struct14_T *in4);
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const ::coder::
    array<real_T, 2U> &in4);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const
    struct14_T *in4)
  {
    ::coder::array<real_T, 2U> b_in2;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_1;
    int32_T stride_1_1;
    if (in3.size(1) == 1) {
      i = in2.size(1);
    } else {
      i = in3.size(1);
    }

    b_in2.set_size(1, i);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_1 = (in3.size(1) != 1);
    if (in3.size(1) == 1) {
      loop_ub = in2.size(1);
    } else {
      loop_ub = in3.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      b_in2[i] = in2[i * stride_0_1] - in3[i * stride_1_1];
    }

    coder::repmat(b_in2, in4->N, in1);
  }

  static void binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3, const ::coder::
    array<real_T, 2U> &in4)
  {
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T aux_2_1;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    int32_T stride_2_0;
    int32_T stride_2_1;
    if (in4.size(0) == 1) {
      i = in3.size(0);
    } else {
      i = in4.size(0);
    }

    if (i == 1) {
      i = in2.size(0);
    } else if (in4.size(0) == 1) {
      i = in3.size(0);
    } else {
      i = in4.size(0);
    }

    if (in4.size(1) == 1) {
      i1 = in3.size(1);
    } else {
      i1 = in4.size(1);
    }

    if (i1 == 1) {
      i1 = in2.size(1);
    } else if (in4.size(1) == 1) {
      i1 = in3.size(1);
    } else {
      i1 = in4.size(1);
    }

    in1.set_size(i, i1);
    stride_0_0 = (in2.size(0) != 1);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_1_1 = (in3.size(1) != 1);
    stride_2_0 = (in4.size(0) != 1);
    stride_2_1 = (in4.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    aux_2_1 = 0;
    if (in4.size(1) == 1) {
      i = in3.size(1);
    } else {
      i = in4.size(1);
    }

    if (i == 1) {
      loop_ub = in2.size(1);
    } else if (in4.size(1) == 1) {
      loop_ub = in3.size(1);
    } else {
      loop_ub = in4.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      int32_T i2;
      i1 = in4.size(0);
      b_loop_ub = in3.size(0);
      if (i1 == 1) {
        i2 = b_loop_ub;
      } else {
        i2 = i1;
      }

      if (i2 == 1) {
        b_loop_ub = in2.size(0);
      } else if (i1 != 1) {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        in1[i1 + in1.size(0) * i] = in2[i1 * stride_0_0 + in2.size(0) * aux_0_1]
          + in3[i1 * stride_1_0 + in3.size(0) * aux_1_1] * in4[i1 * stride_2_0 +
          in4.size(0) * aux_2_1];
      }

      aux_2_1 += stride_2_1;
      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }
  }

  void initializeDREAM(const struct14_T *DREAMPar, const ::coder::array<real_T,
                       2U> &Par_info_min, const ::coder::array<real_T, 2U>
                       &Par_info_max, const char_T Par_info_boundhandling_data[],
                       const int32_T Par_info_boundhandling_size[2], ::coder::
                       array<real_T, 3U> &chain, struct13_T *output, ::coder::
                       array<real_T, 2U> &log_L, const struct5_T
                       *ratInputs_problemDef, const cell_14
                       *ratInputs_problemDefCells, const struct2_T
                       *ratInputs_controls, const ::coder::array<real_T, 2U>
                       &ratInputs_priors, ::coder::array<real_T, 2U> &X, ::coder::
                       array<real_T, 2U> &fx, ::coder::array<real_T, 2U> &CR,
                       real_T pCR_data[], int32_T pCR_size[2], real_T lCR_data[],
                       int32_T lCR_size[2], real_T delta_tot_data[], int32_T
                       delta_tot_size[2])
  {
    ::coder::array<real_T, 2U> b_Par_info_max;
    ::coder::array<real_T, 2U> b_X;
    ::coder::array<real_T, 2U> r;
    ::coder::array<real_T, 2U> r1;
    ::coder::array<real_T, 2U> x;
    ::coder::array<real_T, 1U> log_L_x;
    ::coder::array<real_T, 1U> log_PR_x;
    int32_T b_loop_ub;
    int32_T i;
    int32_T i1;
    int32_T input_sizes_idx_1;
    int32_T loop_ub;
    int8_T b_input_sizes_idx_1;
    int8_T sizes_idx_1;
    boolean_T empty_non_axis_sizes;

    //  Initializes the starting positions of the Markov chains
    //  Create the initial positions of the chains
    //  switch Par_info.prior
    //
    //      case {'uniform'}
    //  Random sampling
    coder::b_rand(DREAMPar->N, DREAMPar->d, b_X);
    if (Par_info_max.size(1) == Par_info_min.size(1)) {
      b_Par_info_max.set_size(1, Par_info_max.size(1));
      loop_ub = Par_info_max.size(1);
      for (i = 0; i < loop_ub; i++) {
        b_Par_info_max[i] = Par_info_max[i] - Par_info_min[i];
      }

      coder::repmat(b_Par_info_max, DREAMPar->N, r);
    } else {
      binary_expand_op(r, Par_info_max, Par_info_min, DREAMPar);
    }

    coder::repmat(Par_info_min, DREAMPar->N, r1);

    //      case {'latin'}
    //          % Initialize chains with latinHypercubeSampling hypercube sampling
    //          if isfield(Par_info,'min_initial') && isfield(Par_info,'max_initial')
    //              [x] = latinHypercubeSampling(Par_info.min_initial,Par_info.max_initial,DREAMPar.N);
    //          else
    //              [x] = latinHypercubeSampling(Par_info.min,Par_info.max,DREAMPar.N);
    //          end
    //      case {'normal'}
    //
    //          % Initialize chains with (multi)-normal distribution
    //          [x] = repmat(Par_info.mu,DREAMPar.N,1) + randn(DREAMPar.N,DREAMPar.d) * chol(Par_info.cov);
    //
    //      case {'prior'}
    //
    //          % Create the initial position of each chain by drawing each parameter individually from the prior
    //          for qq = 1:DREAMPar.d
    //              for zz = 1:DREAMPar.N
    //                  x(zz,qq) = eval(char(Par_info.prior_marginal(qq)));
    //              end
    //          end
    //
    //      otherwise
    //
    //          error('unknown initial sampling method');
    //  end
    //  If specified do boundary handling ( "Bound","Reflect","Fold")
    if (b_X.size(0) == 1) {
      i = r.size(0);
    } else {
      i = b_X.size(0);
    }

    if (b_X.size(1) == 1) {
      i1 = r.size(1);
    } else {
      i1 = b_X.size(1);
    }

    if ((b_X.size(0) == r.size(0)) && (b_X.size(1) == r.size(1)) && (r1.size(0) ==
         i) && (r1.size(1) == i1)) {
      x.set_size(r1.size(0), r1.size(1));
      loop_ub = r1.size(1);
      for (i = 0; i < loop_ub; i++) {
        b_loop_ub = r1.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          x[i1 + x.size(0) * i] = r1[i1 + r1.size(0) * i] + b_X[i1 + b_X.size(0)
            * i] * r[i1 + r.size(0) * i];
        }
      }
    } else {
      binary_expand_op(x, r1, b_X, r);
    }

    boundaryHandling(x, Par_info_min, Par_info_max, Par_info_boundhandling_data,
                     Par_info_boundhandling_size);

    //  Now evaluate the model ( = pdf ) and return fx
    evaluateModel(x, DREAMPar, ratInputs_problemDef, ratInputs_problemDefCells,
                  ratInputs_controls, fx);

    //  Calculate the log-likelihood and log-prior of x (fx)
    calcDensity(x, fx, DREAMPar, ratInputs_problemDef->fitLimits,
                ratInputs_priors, log_L_x, log_PR_x);

    //  Define starting x values, corresponding density, log densty and simulations (Xfx)
    if ((x.size(0) != 0) && (x.size(1) != 0)) {
      b_loop_ub = x.size(0);
    } else if (log_PR_x.size(0) != 0) {
      b_loop_ub = log_PR_x.size(0);
    } else if (log_L_x.size(0) != 0) {
      b_loop_ub = log_L_x.size(0);
    } else {
      b_loop_ub = x.size(0);
    }

    empty_non_axis_sizes = (b_loop_ub == 0);
    if (empty_non_axis_sizes || ((x.size(0) != 0) && (x.size(1) != 0))) {
      input_sizes_idx_1 = x.size(1);
    } else {
      input_sizes_idx_1 = 0;
    }

    if (empty_non_axis_sizes || (log_PR_x.size(0) != 0)) {
      b_input_sizes_idx_1 = 1;
    } else {
      b_input_sizes_idx_1 = 0;
    }

    if (empty_non_axis_sizes || (log_L_x.size(0) != 0)) {
      sizes_idx_1 = 1;
    } else {
      sizes_idx_1 = 0;
    }

    X.set_size(b_loop_ub, (input_sizes_idx_1 + b_input_sizes_idx_1) +
               sizes_idx_1);
    for (i = 0; i < input_sizes_idx_1; i++) {
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        X[i1 + X.size(0) * i] = x[i1 + b_loop_ub * i];
      }
    }

    loop_ub = b_input_sizes_idx_1;
    for (i = 0; i < loop_ub; i++) {
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        X[i1 + X.size(0) * input_sizes_idx_1] = log_PR_x[i1];
      }
    }

    loop_ub = sizes_idx_1;
    for (i = 0; i < loop_ub; i++) {
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        X[i1 + X.size(0) * (input_sizes_idx_1 + b_input_sizes_idx_1)] =
          log_L_x[i1];
      }
    }

    //  Store the model simulations (if appropriate)
    //  storeDREAMResults(DREAMPar,fx,Meas_info,'w+');
    //  Set the first point of each of the DREAMPar.N chain equal to the initial X values
    b_X.set_size(X.size(1), X.size(0));
    loop_ub = X.size(0);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = X.size(1);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_X[i1 + b_X.size(0) * i] = X[i + X.size(0) * i1];
      }
    }

    b_loop_ub = static_cast<int32_T>(DREAMPar->d + 2.0);
    loop_ub = static_cast<int32_T>(DREAMPar->N);
    for (i = 0; i < loop_ub; i++) {
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        chain[chain.size(0) * i1 + chain.size(0) * chain.size(1) * i] = b_X[i1 +
          b_loop_ub * i];
      }
    }

    int32_T iv[2];

    //  Define selection probability of each crossover
    b_Par_info_max.set_size(1, 3);
    pCR_size[0] = 1;
    pCR_size[1] = 3;
    b_Par_info_max[0] = 0.33333333333333331;
    pCR_data[0] = 0.33333333333333331;
    b_Par_info_max[1] = 0.33333333333333331;
    pCR_data[1] = 0.33333333333333331;
    b_Par_info_max[2] = 0.33333333333333331;
    pCR_data[2] = 0.33333333333333331;

    //  Generate the actula CR value, lCR and delta_tot
    iv[0] = (*(int32_T (*)[2])b_Par_info_max.size())[0];
    iv[1] = (*(int32_T (*)[2])b_Par_info_max.size())[1];
    drawCR(DREAMPar, (const real_T *)b_Par_info_max.data(), iv, CR);

    // coder.varsize('CR',[100 1e4],[1 1]);
    lCR_size[0] = 1;
    lCR_size[1] = 3;
    delta_tot_size[0] = 1;
    delta_tot_size[1] = 3;
    lCR_data[0] = 0.0;
    delta_tot_data[0] = 0.0;
    lCR_data[1] = 0.0;
    delta_tot_data[1] = 0.0;
    lCR_data[2] = 0.0;
    delta_tot_data[2] = 0.0;

    //  Save pCR values in memory
    output->CR[0] = DREAMPar->N;
    for (i = 0; i < 3; i++) {
      output->CR[output->CR.size(0) * (i + 1)] = 0.33333333333333331;
    }

    //  Save history log density of individual chains
    log_L[0] = DREAMPar->N;
    loop_ub = log_L_x.size(0);
    for (i = 0; i < loop_ub; i++) {
      log_L[log_L.size(0) * (i + 1)] = log_L_x[i];
    }

    //  Compute the R-statistic
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
    //  Set the R-statistic to a large value
    output->R_stat[0] = DREAMPar->N;
    loop_ub = static_cast<int32_T>(DREAMPar->d);
    for (i = 0; i < loop_ub; i++) {
      output->R_stat[output->R_stat.size(0) * (i + 1)] = rtNaN;
    }
  }
}

// End of code generation (initializeDREAM.cpp)
