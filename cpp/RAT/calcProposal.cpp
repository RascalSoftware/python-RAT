//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// calcProposal.cpp
//
// Code generation for function 'calcProposal'
//

// Include files
#include "calcProposal.h"
#include "RATMain_types.h"
#include "blockedSummation.h"
#include "boundaryHandling.h"
#include "combineVectorElements.h"
#include "find.h"
#include "rand.h"
#include "randn.h"
#include "randperm.h"
#include "randsample.h"
#include "rt_nonfinite.h"
#include "sort.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, int32_T in2,
    const ::coder::array<real_T, 1U> &in3, const ::coder::array<real_T, 2U> &in4,
    real_T in5, const ::coder::array<real_T, 2U> &in6, const ::coder::array<
    real_T, 2U> &in7);
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, int32_T in2,
    const ::coder::array<real_T, 2U> &in4, int32_T in5, const ::coder::array<
    real_T, 2U> &in6, const ::coder::array<real_T, 2U> &in7, int32_T in8);
  static void plus(::coder::array<real_T, 2U> &in1, const ::coder::array<real_T,
                   2U> &in2, const ::coder::array<real_T, 2U> &in3);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 2U> &in1, int32_T in2,
    const ::coder::array<real_T, 1U> &in3, const ::coder::array<real_T, 2U> &in4,
    real_T in5, const ::coder::array<real_T, 2U> &in6, const ::coder::array<
    real_T, 2U> &in7)
  {
    int32_T loop_ub;
    int32_T stride_0_1;
    int32_T stride_1_1;
    int32_T stride_2_1;
    stride_0_1 = (in3.size(0) != 1);
    stride_1_1 = (in6.size(1) != 1);
    stride_2_1 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      if (in6.size(1) == 1) {
        loop_ub = in3.size(0);
      } else {
        loop_ub = in6.size(1);
      }
    } else {
      loop_ub = in3.size(0);
    }

    for (int32_T i{0}; i < loop_ub; i++) {
      in1[in2 + in1.size(0) * (static_cast<int32_T>(in3[i]) - 1)] = (in4[in2 +
        in4.size(0) * (static_cast<int32_T>(in3[i * stride_0_1]) - 1)] + 1.0) *
        in5 * in6[i * stride_1_1] + in7[in2 + in7.size(0) * (static_cast<int32_T>
        (in3[i * stride_2_1]) - 1)];
    }
  }

  static void binary_expand_op(::coder::array<real_T, 2U> &in1, int32_T in2,
    const ::coder::array<real_T, 2U> &in4, int32_T in5, const ::coder::array<
    real_T, 2U> &in6, const ::coder::array<real_T, 2U> &in7, int32_T in8)
  {
    int32_T loop_ub;
    int32_T stride_0_1;
    int32_T stride_1_1;
    int32_T stride_2_1;
    stride_0_1 = (in5 + 1 != 1);
    stride_1_1 = (in6.size(1) != 1);
    stride_2_1 = (in8 + 1 != 1);
    if (in8 + 1 == 1) {
      if (in6.size(1) == 1) {
        loop_ub = in5 + 1;
      } else {
        loop_ub = in6.size(1);
      }
    } else {
      loop_ub = in8 + 1;
    }

    for (int32_T i{0}; i < loop_ub; i++) {
      in1[in2 + in1.size(0) * i] = (in4[in2 + in4.size(0) * (i * stride_0_1)] +
        1.0) * in6[i * stride_1_1] + in7[in2 + in7.size(0) * (i * stride_2_1)];
    }
  }

  static void plus(::coder::array<real_T, 2U> &in1, const ::coder::array<real_T,
                   2U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
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

    in1.set_size(i, i1);
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
        in1[i1 + in1.size(0) * i] = in2[i1 * stride_0_0 + in2.size(0) * aux_0_1]
          + in3[i1 * stride_1_0 + in3.size(0) * aux_1_1];
      }

      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }
  }

  void calcProposal(const ::coder::array<real_T, 2U> &X, real_T CR_data[], const
                    struct13_T *DREAMPar, const ::coder::array<real_T, 2U>
                    &Table_gamma, const ::coder::array<real_T, 2U> &Par_info_min,
                    const ::coder::array<real_T, 2U> &Par_info_max, const char_T
                    Par_info_boundhandling_data[], const int32_T
                    Par_info_boundhandling_size[2], ::coder::array<real_T, 2U>
                    &x_new)
  {
    ::coder::array<real_T, 2U> A;
    ::coder::array<real_T, 2U> a;
    ::coder::array<real_T, 2U> b;
    ::coder::array<real_T, 2U> b_gamma;
    ::coder::array<real_T, 2U> dx;
    ::coder::array<real_T, 2U> eps;
    ::coder::array<real_T, 2U> r;
    ::coder::array<real_T, 2U> r1;
    ::coder::array<real_T, 2U> r4;
    ::coder::array<real_T, 2U> r5;
    ::coder::array<real_T, 2U> r6;
    ::coder::array<real_T, 2U> rnd_cr;
    ::coder::array<real_T, 2U> rnd_jump;
    ::coder::array<real_T, 1U> DE_pairs;
    ::coder::array<real_T, 1U> r3;
    ::coder::array<int32_T, 2U> draw;
    ::coder::array<int32_T, 2U> r2;
    ::coder::array<boolean_T, 2U> b_rnd_cr;
    int32_T b_loop_ub;
    int32_T b_loop_ub_tmp;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T loop_ub_tmp;

    //  Calculate candidate points using discrete proposal distribution
    //  % % % Calculate the ergodicity perturbation
    //  % % eps = DREAMPar.zeta * randn(DREAMPar.N,DREAMPar.d);
    //  % %
    //  % % % Determine which sequences to evolve with what DE strategy
    //  % % DE_pairs = randsample( [1:DREAMPar.delta ] , DREAMPar.N , true , [ 1/DREAMPar.delta*ones(1,DREAMPar.delta) ])';
    //  % %
    //  % % % Generate series of permutations of chains
    //  % % [dummy,tt] = sort(rand(DREAMPar.N-1,DREAMPar.N));
    //  % %
    //  % % % Generate uniform random numbers for each chain to determine which dimension to update
    //  % % D = rand(DREAMPar.N,DREAMPar.d);
    //  % %
    //  % % % Ergodicity for each individual chain
    //  % % noise_x = DREAMPar.lambda * (2 * rand(DREAMPar.N,DREAMPar.d) - 1);
    //  % %
    //  % % % Initialize the delta update to zero
    //  % % delta_x = zeros(DREAMPar.N,DREAMPar.d);
    //  % %
    //  % % % Each chain evolves using information from other chains to create offspring
    //  % % for qq = 1:DREAMPar.N,
    //  % %
    //  % %     % Define ii and remove current member as an option
    //  % %     ii = ones(DREAMPar.N,1); ii(qq) = 0; idx = find(ii > 0);
    //  % %
    //  % %     % randomly select two members of ii that have value == 1
    //  % %     rr = idx(tt(1:2*DE_pairs(qq,1),qq));
    //  % %
    //  % %     % --- WHICH DIMENSIONS TO UPDATE? DO SOMETHING WITH CROSSOVER ----
    //  % %     [i] = find(D(qq,1:DREAMPar.d) > (1-CR(qq,1)));
    //  % %
    //  % %     % Update at least one dimension
    //  % %     if isempty(i), i = randperm(DREAMPar.d); i = i(1); end;
    //  % %     % ----------------------------------------------------------------
    //  % %
    //  % %     % Determine the associated JumpRate and compute the jump
    //  % %     if (rand < (1 - DREAMPar.pJumpRate_one)),
    //  % %
    //  % %         %         % Now determine gamma, the jump factor
    //  % %         %         if ~DREAMPar.ABC
    //  % %         %
    //  % %         % Select the JumpRate (dependent of NrDim and number of pairs)
    //  % %         NrDim = size(i,2); JumpRate = Table_gamma(NrDim,DE_pairs(qq,1));
    //  % %
    //  % %         %           else
    //  % %         %
    //  % %         %                 % Turner (2012) paper -- CU[0.5,1] but needs scaling if
    //  % %         %                 % more than 1 pair is used!
    //  % %         %                 JumpRate = (0.5 + rand/2) * sqrt(1/DREAMPar.delta);
    //  % %
    //  % %         %        end;
    //  % %
    //  % %         % Produce the difference of the pairs used for population evolution
    //  % %         delta = sum(X(rr(1:DE_pairs(qq,1)),1:DREAMPar.d) - X(rr(DE_pairs(qq,1)+1:2*DE_pairs(qq,1)),1:DREAMPar.d),1);
    //  % %
    //  % %         % Then fill update the dimension
    //  % %         delta_x(qq,i) = (1 + noise_x(qq,i)) * JumpRate.*delta(1,i);
    //  % %
    //  % %     else
    //  % %
    //  % %         % Set the JumpRate to 1 and overwrite CR and DE_pairs
    //  % %         JumpRate = 1; CR(qq,1) = -1;
    //  % %
    //  % %         % Compute delta from one pair
    //  % %         delta = X(rr(1),1:DREAMPar.d) - X(rr(2),1:DREAMPar.d);
    //  % %
    //  % %         % Now jumprate to facilitate jumping from one mode to the other in all dimensions
    //  % %         delta_x(qq,1:DREAMPar.d) = JumpRate * delta;
    //  % %
    //  % %     end;
    //  % %
    //  % %     % Check this line to avoid that jump = 0 and x_new is similar to X
    //  % %     if (sum(delta_x(qq,1:DREAMPar.d).^2,2) == 0),
    //  % %
    //  % %         % Compute the Cholesky Decomposition of X
    //  % %         R = (2.38/sqrt(DREAMPar.d)) * chol(cov(X(1:end,1:DREAMPar.d)) + 1e-5*eye(DREAMPar.d));
    //  % %
    //  % %         % Generate jump using multinormal distribution
    //  % %         delta_x(qq,1:DREAMPar.d) = randn(1,DREAMPar.d) * R;
    //  % %         disp('hello');
    //  % %     end;
    //  % %
    //  % % end;
    //  % %
    //  % % % Generate candidate points by perturbing the current X values with jump and eps
    //  % % x_new = X + delta_x + eps;
    //  % %
    //  % % % If specified do boundary handling ( "Bound","Reflect","Fold")
    //  % % if isfield(Par_info,'boundhandling'),
    //  % %     [x_new] = BoundaryHandling(x_new,Par_info,Par_info.boundhandling);
    //  % % end;
    //  % %
    //  ##################################################
    //  Calculate the ergodicity perturbation
    coder::randn(DREAMPar->N, DREAMPar->d, b);
    eps.set_size(b.size(0), b.size(1));
    loop_ub = b.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        eps[i1 + eps.size(0) * i] = 1.0E-12 * b[i1 + b.size(0) * i];
      }
    }

    real_T tmp_data[3];

    //  Determine how many chain pairs to use for each individual chain
    r.set_size(1, 3);
    r[0] = 1.0;
    tmp_data[0] = 0.33333333333333331;
    r[1] = 2.0;
    tmp_data[1] = 0.33333333333333331;
    r[2] = 3.0;
    tmp_data[2] = 0.33333333333333331;
    coder::randsample((const real_T *)r.data(), DREAMPar->N, tmp_data, r1);
    DE_pairs.set_size(r1.size(1));
    loop_ub = r1.size(1);
    for (i = 0; i < loop_ub; i++) {
      DE_pairs[i] = r1[i];
    }

    //  Generate uniform random numbers for each chain to determine which dimension to update
    coder::b_rand(DREAMPar->N, DREAMPar->d, rnd_cr);

    //  Ergodicity for each individual chain
    coder::b_rand(DREAMPar->N, DREAMPar->d, b);
    rnd_jump.set_size(b.size(0), b.size(1));
    loop_ub = b.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        rnd_jump[i1 + rnd_jump.size(0) * i] = DREAMPar->lambda * (2.0 * b[i1 +
          b.size(0) * i] - 1.0);
      }
    }

    // rnd_jump = DREAMPar.lambda * (2 * rand(DREAMPar.N,1) - 1);
    //  Randomly permute numbers [1,...,N-1] N times
    coder::b_rand(DREAMPar->N - 1.0, DREAMPar->N, b);
    coder::internal::b_sort(b, draw);

    //  Set jump vectors equal to zero
    loop_ub_tmp = static_cast<int32_T>(DREAMPar->N);
    b_loop_ub_tmp = static_cast<int32_T>(DREAMPar->d);
    dx.set_size(loop_ub_tmp, b_loop_ub_tmp);
    for (i = 0; i < b_loop_ub_tmp; i++) {
      for (i1 = 0; i1 < loop_ub_tmp; i1++) {
        dx[i1 + dx.size(0) * i] = 0.0;
      }
    }

    real_T b_dv[2];

    //  Determine when jumprate is 1
    b_dv[0] = 1.0 - DREAMPar->pUnitGamma;
    b_dv[1] = DREAMPar->pUnitGamma;
    coder::randsample(DREAMPar->N, b_dv, b_gamma);

    //  Create N proposals
    for (int32_T b_i{0}; b_i < loop_ub_tmp; b_i++) {
      real_T r2_data[6];
      real_T r1_data[3];
      real_T CR;
      int32_T D;
      int32_T i2;

      //  Derive vector r1
      b_loop_ub_tmp = static_cast<int32_T>(DE_pairs[b_i]);
      for (i = 0; i < b_loop_ub_tmp; i++) {
        r1_data[i] = DREAMPar->R[b_i + DREAMPar->R.size(0) * (draw[i + draw.size
          (0) * b_i] - 1)];
      }

      //  Derive vector r2
      CR = 2.0 * DE_pairs[b_i];
      if (DE_pairs[b_i] + 1.0 > CR) {
        i = 0;
        i1 = 0;
      } else {
        i = static_cast<int32_T>(DE_pairs[b_i] + 1.0) - 1;
        i1 = static_cast<int32_T>(CR);
      }

      loop_ub = i1 - i;
      for (i2 = 0; i2 < loop_ub; i2++) {
        r2_data[i2] = DREAMPar->R[b_i + DREAMPar->R.size(0) * (draw[(i + i2) +
          draw.size(0) * b_i] - 1)];
      }

      //  Derive subset A with dimensions to sample
      if (DREAMPar->d < 1.0) {
        b_loop_ub = 0;
      } else {
        b_loop_ub = static_cast<int32_T>(DREAMPar->d);
      }

      CR = CR_data[b_i];
      b_rnd_cr.set_size(1, b_loop_ub);
      for (i2 = 0; i2 < b_loop_ub; i2++) {
        b_rnd_cr[i2] = (rnd_cr[b_i + rnd_cr.size(0) * i2] < CR);
      }

      coder::d_eml_find(b_rnd_cr, r2);
      A.set_size(1, r2.size(1));
      b_loop_ub = r2.size(1);
      for (i2 = 0; i2 < b_loop_ub; i2++) {
        A[i2] = r2[i2];
      }

      //  How many dimensions are sampled?
      D = A.size(1);

      //  Make sure that at least one dimension is selected!
      if (A.size(1) == 0) {
        coder::randperm(DREAMPar->d, a);
        A.set_size(1, 1);
        A[0] = a[0];
        D = 1;
      }

      //  Which gamma to use?
      if (b_gamma[b_i] == 1.0) {
        int32_T c_loop_ub;
        int32_T dx_tmp;

        //  Calculate direct jump
        if (DREAMPar->d < 1.0) {
          b_loop_ub = 0;
          c_loop_ub = 0;
          dx_tmp = 0;
        } else {
          b_loop_ub = static_cast<int32_T>(DREAMPar->d);
          c_loop_ub = static_cast<int32_T>(DREAMPar->d);
          dx_tmp = static_cast<int32_T>(DREAMPar->d);
        }

        b.set_size(b_loop_ub_tmp, c_loop_ub);
        for (i2 = 0; i2 < c_loop_ub; i2++) {
          for (int32_T i3{0}; i3 < b_loop_ub_tmp; i3++) {
            b[i3 + b.size(0) * i2] = X[(static_cast<int32_T>(r1_data[i3]) +
              X.size(0) * i2) - 1];
          }
        }

        r5.set_size(loop_ub, dx_tmp);
        for (i2 = 0; i2 < dx_tmp; i2++) {
          for (int32_T i3{0}; i3 < loop_ub; i3++) {
            r5[i3 + r5.size(0) * i2] = X[(static_cast<int32_T>(r2_data[i3]) +
              X.size(0) * i2) - 1];
          }
        }

        if (DREAMPar->d < 1.0) {
          i2 = 0;
        } else {
          i2 = static_cast<int32_T>(DREAMPar->d);
        }

        if ((b.size(0) == r5.size(0)) && (b.size(1) == r5.size(1))) {
          loop_ub = b.size(1);
          for (int32_T i3{0}; i3 < loop_ub; i3++) {
            c_loop_ub = b.size(0);
            for (dx_tmp = 0; dx_tmp < c_loop_ub; dx_tmp++) {
              b[dx_tmp + b.size(0) * i3] = b[dx_tmp + b.size(0) * i3] -
                r5[dx_tmp + r5.size(0) * i3];
            }
          }

          if (static_cast<int32_T>(DE_pairs[b_i]) == 1) {
            i = i1 - i;
          } else {
            i = static_cast<int32_T>(DE_pairs[b_i]);
          }

          coder::blockedSummation(b, i, r1);
        } else {
          binary_expand_op(r1, b, r5, DE_pairs, b_i, i1, i);
        }

        if (b_loop_ub == 1) {
          i = r1.size(1);
        } else {
          i = b_loop_ub;
        }

        if ((b_loop_ub == r1.size(1)) && (i == i2)) {
          for (i = 0; i < b_loop_ub; i++) {
            dx[b_i + dx.size(0) * i] = (rnd_jump[b_i + rnd_jump.size(0) * i] +
              1.0) * r1[i] + eps[b_i + eps.size(0) * i];
          }
        } else {
          binary_expand_op(dx, b_i, rnd_jump, b_loop_ub - 1, r1, eps, i2 - 1);
        }

        //  Set CR to -1 so that this jump does not count for calculation of pCR
        CR_data[b_i] = -1.0;
      } else {
        real_T gamma_D;

        //  Unpack jump rate
        gamma_D = Table_gamma[(D + Table_gamma.size(0) * (b_loop_ub_tmp - 1)) -
          1];

        //  Calculate jump
        r3.set_size(A.size(1));
        b_loop_ub = A.size(1);
        for (i2 = 0; i2 < b_loop_ub; i2++) {
          r3[i2] = A[i2];
        }

        r4.set_size(b_loop_ub_tmp, r3.size(0));
        b_loop_ub = r3.size(0);
        for (i2 = 0; i2 < b_loop_ub; i2++) {
          for (int32_T i3{0}; i3 < b_loop_ub_tmp; i3++) {
            r4[i3 + r4.size(0) * i2] = X[(static_cast<int32_T>(r1_data[i3]) +
              X.size(0) * (static_cast<int32_T>(r3[i2]) - 1)) - 1];
          }
        }

        r6.set_size(loop_ub, r3.size(0));
        b_loop_ub = r3.size(0);
        for (i2 = 0; i2 < b_loop_ub; i2++) {
          for (int32_T i3{0}; i3 < loop_ub; i3++) {
            r6[i3 + r6.size(0) * i2] = X[(static_cast<int32_T>(r2_data[i3]) +
              X.size(0) * (static_cast<int32_T>(r3[i2]) - 1)) - 1];
          }
        }

        if (r4.size(0) == r6.size(0)) {
          r5.set_size(r4.size(0), r4.size(1));
          loop_ub = r4.size(1);
          b_loop_ub = r4.size(0);
          for (i2 = 0; i2 < loop_ub; i2++) {
            for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
              r5[i3 + r5.size(0) * i2] = r4[i3 + r4.size(0) * i2] - r6[i3 +
                r6.size(0) * i2];
            }
          }

          if (static_cast<int32_T>(DE_pairs[b_i]) == 1) {
            i = i1 - i;
          } else {
            i = static_cast<int32_T>(DE_pairs[b_i]);
          }

          coder::blockedSummation(r5, i, r1);
        } else {
          c_binary_expand_op(r1, r4, r6, DE_pairs, b_i, i1, i);
        }

        if (r3.size(0) == 1) {
          i = r1.size(1);
        } else {
          i = r3.size(0);
        }

        if ((r3.size(0) == r1.size(1)) && (i == r3.size(0))) {
          loop_ub = r3.size(0);
          for (i = 0; i < loop_ub; i++) {
            int32_T dx_tmp;
            dx_tmp = static_cast<int32_T>(r3[i]) - 1;
            dx[b_i + dx.size(0) * dx_tmp] = (rnd_jump[b_i + rnd_jump.size(0) *
              dx_tmp] + 1.0) * gamma_D * r1[i] + eps[b_i + eps.size(0) * dx_tmp];
          }
        } else {
          binary_expand_op(dx, b_i, r3, rnd_jump, gamma_D, r1, eps);
        }
      }
    }

    //  Generate candidate points by perturbing the current X values with jump and eps
    //  If specified do boundary handling ( "Bound","Reflect","Fold")
    if ((X.size(0) == dx.size(0)) && (X.size(1) == dx.size(1))) {
      x_new.set_size(X.size(0), X.size(1));
      loop_ub = X.size(1);
      for (i = 0; i < loop_ub; i++) {
        b_loop_ub = X.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          x_new[i1 + x_new.size(0) * i] = X[i1 + X.size(0) * i] + dx[i1 +
            dx.size(0) * i];
        }
      }
    } else {
      plus(x_new, X, dx);
    }

    boundaryHandling(x_new, Par_info_min, Par_info_max,
                     Par_info_boundhandling_data, Par_info_boundhandling_size);
  }
}

// End of code generation (calcProposal.cpp)
