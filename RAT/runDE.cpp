//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runDE.cpp
//
// Code generation for function 'runDE'
//

// Include files
#include "runDE.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "deopt.h"
#include "length.h"
#include "packParams.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "unpackParams.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <algorithm>
#include <stdio.h>

// Function Definitions
namespace RAT
{
  k_struct_T intrafun(const ::coder::array<real_T, 2U> &p, struct5_T *problemDef,
                      const char_T controls_parallel_data[], const int32_T
                      controls_parallel_size[2], const real_T
                      controls_resamPars[2], boolean_T controls_calcSldDuringFit,
                      const struct3_T *controls_checks, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f1, const ::coder::array<
                      cell_wrap_8, 2U> &problemDefCells_f2, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f3, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f4, const ::coder::array<
                      cell_wrap_8, 2U> &problemDefCells_f5, const ::coder::array<
                      cell_wrap_8, 1U> &problemDefCells_f6, const ::coder::array<
                      cell_wrap_1, 2U> &problemDefCells_f14, const ::coder::
                      array<cell_wrap_8, 2U> &problemDefCells_f19)
  {
    cell_13 a__2;
    cell_14 expl_temp;
    k_struct_T S_MSE;
    struct2_T b_expl_temp;
    struct_T b_problemDef;
    int32_T loop_ub;
    problemDef->fitParams.set_size(1, p.size(1));
    loop_ub = p.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      problemDef->fitParams[problemDef->fitParams.size(0) * i] = p[i];
    }

    unpackParams(problemDef, controls_checks->fitParam,
                 controls_checks->fitBackgroundParam,
                 controls_checks->fitQzshift, controls_checks->fitScalefactor,
                 controls_checks->fitBulkIn, controls_checks->fitBulkOut,
                 controls_checks->fitResolutionParam,
                 controls_checks->fitDomainRatio);
    expl_temp.f19.set_size(1, problemDefCells_f19.size(1));
    loop_ub = problemDefCells_f19.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f19[i] = problemDefCells_f19[i];
    }

    expl_temp.f14.set_size(1, problemDefCells_f14.size(1));
    loop_ub = problemDefCells_f14.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f14[i] = problemDefCells_f14[i];
    }

    expl_temp.f6.set_size(problemDefCells_f6.size(0));
    loop_ub = problemDefCells_f6.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f6[i] = problemDefCells_f6[i];
    }

    expl_temp.f5.set_size(1, problemDefCells_f5.size(1));
    loop_ub = problemDefCells_f5.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f5[i] = problemDefCells_f5[i];
    }

    expl_temp.f4.set_size(1, problemDefCells_f4.size(1));
    loop_ub = problemDefCells_f4.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f4[i] = problemDefCells_f4[i];
    }

    expl_temp.f3.set_size(1, problemDefCells_f3.size(1));
    loop_ub = problemDefCells_f3.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f3[i] = problemDefCells_f3[i];
    }

    expl_temp.f2.set_size(1, problemDefCells_f2.size(1));
    loop_ub = problemDefCells_f2.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f2[i] = problemDefCells_f2[i];
    }

    expl_temp.f1.set_size(1, problemDefCells_f1.size(1));
    loop_ub = problemDefCells_f1.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.f1[i] = problemDefCells_f1[i];
    }

    b_expl_temp.checks = *controls_checks;
    b_expl_temp.calcSldDuringFit = controls_calcSldDuringFit;
    b_expl_temp.resamPars[0] = controls_resamPars[0];
    b_expl_temp.resamPars[1] = controls_resamPars[1];
    b_expl_temp.parallel.size[0] = 1;
    b_expl_temp.parallel.size[1] = controls_parallel_size[1];
    loop_ub = controls_parallel_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&controls_parallel_data[0], &controls_parallel_data[loop_ub],
                &b_expl_temp.parallel.data[0]);
    }

    reflectivityCalculation(problemDef, &expl_temp, &b_expl_temp, &b_problemDef,
      &a__2);
    S_MSE.FVr_oa = b_problemDef.calculations.sumChi;

    // no constraints                 THESE FIRST FEW VALS MAY BE WRONG
    // no constraint array
    S_MSE.I_no = 1.0;

    // number of objectives (costs)
    return S_MSE;
  }

  void runDE(struct5_T *problemDef, const cell_14 *problemDefCells, const
             struct1_T *problemDefLimits, const struct2_T *controls, struct_T
             *problem, cell_wrap_9 result[6])
  {
    static const real_T FVr_x[50]{ -1.0, -0.95918367346938771,
      -0.91836734693877542, -0.87755102040816324, -0.836734693877551,
      -0.79591836734693866, -0.75510204081632648, -0.71428571428571419,
      -0.673469387755102, -0.63265306122448972, -0.59183673469387754,
      -0.55102040816326525, -0.51020408163265307, -0.46938775510204078,
      -0.42857142857142855, -0.38775510204081631, -0.34693877551020408,
      -0.30612244897959179, -0.26530612244897955, -0.22448979591836732,
      -0.18367346938775508, -0.14285714285714285, -0.1020408163265306,
      -0.061224489795918366, -0.020408163265306121, 0.020408163265306121,
      0.061224489795918366, 0.1020408163265306, 0.14285714285714285,
      0.18367346938775508, 0.22448979591836732, 0.26530612244897955,
      0.30612244897959179, 0.34693877551020408, 0.38775510204081631,
      0.42857142857142855, 0.46938775510204078, 0.51020408163265307,
      0.55102040816326525, 0.59183673469387754, 0.63265306122448972,
      0.673469387755102, 0.71428571428571419, 0.75510204081632648,
      0.79591836734693866, 0.836734693877551, 0.87755102040816324,
      0.91836734693877542, 0.95918367346938771, 1.0 };

    ::coder::array<cell_wrap_1, 1U> b_problemDef;
    ::coder::array<real_T, 2U> res;
    ::coder::array<int8_T, 2U> S_struct_FM_pop;
    cell_13 b_result;
    i_struct_T expl_temp;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    packParams(problemDef, problemDefCells->f7, problemDefCells->f8,
               problemDefCells->f9, problemDefCells->f10, problemDefCells->f11,
               problemDefCells->f12, problemDefCells->f13, problemDefCells->f20,
               problemDefLimits, &controls->checks, b_problemDef);

    // Check the bounds on all the selected
    //  out = checkBounds(problemDef,controls);
    //  if strcmp(out{1},'fail')
    //      return
    //  end
    // Value to reach
    expl_temp.FVr_minbound.set_size(1, problemDef->fitLimits.size(0));
    loop_ub = problemDef->fitLimits.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.FVr_minbound[i] = problemDef->fitLimits[i];
    }

    expl_temp.FVr_maxbound.set_size(1, problemDef->fitLimits.size(0));
    loop_ub = problemDef->fitLimits.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      expl_temp.FVr_maxbound[i] = problemDef->fitLimits[i +
        problemDef->fitLimits.size(0)];
    }

    // 1: use bounds as bound constraints, 0: no bound constraints
    //  I_NP            number of population members
    //  I_itermax       maximum number of iterations (generations)
    //  fWeight        DE-stepsize fWeight ex [0, 2]
    //  F_CR            crossover probability constant ex [0, 1]
    //  I_strategy     1 --> DE/rand/1:
    //                       the classical version of DE.
    //                 2 --> DE/local-to-best/1:
    //                       a version which has been used by quite a number
    //                       of scientists. Attempts a balance between robustness
    //                       and fast convergence.
    //                 3 --> DE/best/1 with jitter:
    //                       taylored for small population sizes and fast convergence.
    //                       Dimensionality should not be too high.
    //                 4 --> DE/rand/1 with per-vector-dither:
    //                       Classical DE with dither to become even more robust.
    //                 5 --> DE/rand/1 with per-generation-dither:
    //                       Classical DE with dither to become even more robust.
    //                       Choosing fWeight = 0.3 is a good start here.
    //                 6 --> DE/rand/1 either-or-algorithm:
    //                       Alternates between differential mutation and three-point-
    //                       recombination.
    //  I_refresh     intermediate output will be produced after "I_refresh"
    //                iterations. No intermediate output will be produced
    //                if I_refresh is < 1
    //  I_plotting    Will use plotting if set to 1. Will skip plotting otherwise.
    // -----Definition of tolerance scheme--------------------------------------
    // -----The scheme is sampled at I_lentol points----------------------------
    // ordinate running from -1 to +1
    // upper limit is 1
    // lower limit is -1
    // Tell compiler abut variable sizes
    // -----tie all important values to a structure that can be passed along----
    expl_temp.FVr_x.size[0] = 1;
    expl_temp.FVr_x.size[1] = 50;
    std::copy(&FVr_x[0], &FVr_x[50], &expl_temp.FVr_x.data[0]);
    loop_ub_tmp = static_cast<int32_T>(controls->populationSize);
    S_struct_FM_pop.set_size(loop_ub_tmp, 2);
    expl_temp.FVr_bestmem.set_size(1, 2);
    for (int32_T i{0}; i < 2; i++) {
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        S_struct_FM_pop[i1 + S_struct_FM_pop.size(0) * i] = 0;
      }

      expl_temp.FVr_bestmem[i] = 0.0;
    }

    expl_temp.FM_pop.set_size(S_struct_FM_pop.size(0), 2);
    loop_ub = S_struct_FM_pop.size(0);
    for (int32_T i{0}; i < 2; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        expl_temp.FM_pop[i1 + expl_temp.FM_pop.size(0) * i] = 0.0;
      }
    }

    expl_temp.I_plotting = 0.0;
    expl_temp.I_refresh = 1.0;
    expl_temp.I_strategy = 5.0;
    expl_temp.F_VTR = controls->targetValue;
    expl_temp.I_itermax = controls->numGenerations;
    expl_temp.I_bnd_constr = 1.0;
    expl_temp.I_D = coder::internal::intlength(problemDef->fitParams.size(0),
      problemDef->fitParams.size(1));
    expl_temp.F_CR = controls->crossoverProbability;
    expl_temp.fWeight = controls->fWeight;
    expl_temp.I_NP = controls->populationSize;
    expl_temp.FVr_lim_lo.size[0] = 1;
    expl_temp.FVr_lim_lo.size[1] = 50;
    expl_temp.FVr_lim_up.size[0] = 1;
    expl_temp.FVr_lim_up.size[1] = 50;
    for (int32_T i{0}; i < 50; i++) {
      expl_temp.FVr_lim_lo.data[i] = -1.0;
      expl_temp.FVr_lim_up.data[i] = 1.0;
    }

    expl_temp.I_lentol.size[0] = 1;
    expl_temp.I_lentol.data[0] = 50.0;
    deopt(problemDef, problemDefCells->f1, problemDefCells->f2,
          problemDefCells->f3, problemDefCells->f4, problemDefCells->f5,
          problemDefCells->f6, problemDefCells->f14, problemDefCells->f19,
          controls->parallel.data, controls->parallel.size, controls->resamPars,
          controls->calcSldDuringFit, controls->display.data,
          controls->display.size, &controls->checks, &expl_temp, res);
    problemDef->fitParams.set_size(1, res.size(1));
    loop_ub = res.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      problemDef->fitParams[problemDef->fitParams.size(0) * i] = res[i];
    }

    unpackParams(problemDef, controls->checks.fitParam,
                 controls->checks.fitBackgroundParam,
                 controls->checks.fitQzshift, controls->checks.fitScalefactor,
                 controls->checks.fitBulkIn, controls->checks.fitBulkOut,
                 controls->checks.fitResolutionParam,
                 controls->checks.fitDomainRatio);
    reflectivityCalculation(problemDef, problemDefCells, controls, problem,
      &b_result);
    result[0].f1.set_size(b_result.f1.size(0), 1);
    loop_ub_tmp = b_result.f1.size(0);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      loop_ub = b_result.f1[i].f1.size(0);
      result[0].f1[i].f1.set_size(b_result.f1[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[0].f1[i].f1[i2 + result[0].f1[i].f1.size(0) * i1] =
            b_result.f1[i].f1[i2 + b_result.f1[i].f1.size(0) * i1];
        }
      }
    }

    result[1].f1.set_size(b_result.f2.size(0), 1);
    loop_ub_tmp = b_result.f2.size(0);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      loop_ub = b_result.f2[i].f1.size(0);
      result[1].f1[i].f1.set_size(b_result.f2[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[1].f1[i].f1[i2 + result[1].f1[i].f1.size(0) * i1] =
            b_result.f2[i].f1[i2 + b_result.f2[i].f1.size(0) * i1];
        }
      }
    }

    result[2].f1.set_size(b_result.f3.size(0), 1);
    loop_ub_tmp = b_result.f3.size(0);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      loop_ub = b_result.f3[i].f1.size(0);
      result[2].f1[i].f1.set_size(b_result.f3[i].f1.size(0), 3);
      for (int32_T i1{0}; i1 < 3; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[2].f1[i].f1[i2 + result[2].f1[i].f1.size(0) * i1] =
            b_result.f3[i].f1[i2 + b_result.f3[i].f1.size(0) * i1];
        }
      }
    }

    result[3].f1.set_size(b_result.f4.size(0), b_result.f4.size(1));
    loop_ub = b_result.f4.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      loop_ub_tmp = b_result.f4.size(0);
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        result[3].f1[i1 + result[3].f1.size(0) * i] = b_result.f4[i1 +
          b_result.f4.size(0) * i];
      }
    }

    result[4].f1.set_size(b_result.f5.size(0), b_result.f5.size(1));
    loop_ub = b_result.f5.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      loop_ub_tmp = b_result.f5.size(0);
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        result[4].f1[i1 + result[4].f1.size(0) * i] = b_result.f5[i1 +
          b_result.f5.size(0) * i];
      }
    }

    result[5].f1.set_size(b_result.f6.size(0), b_result.f6.size(1));
    loop_ub = b_result.f6.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      loop_ub_tmp = b_result.f6.size(0);
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        result[5].f1[i1 + result[5].f1.size(0) * i] = b_result.f6[i1 +
          b_result.f6.size(0) * i];
      }
    }

    if (!coder::internal::d_strcmp(controls->display.data,
         controls->display.size)) {
      printf("Final chi squared is %g\n", problem->calculations.sumChi);
      fflush(stdout);
    }
  }
}

// End of code generation (runDE.cpp)
