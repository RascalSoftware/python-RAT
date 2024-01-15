//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runDREAM.cpp
//
// Code generation for function 'runDREAM'
//

// Include files
#include "runDREAM.h"
#include "RATMain_internal_types.h"
#include "RATMain_rtwutil.h"
#include "RATMain_types.h"
#include "getFittedPriors.h"
#include "makeEmptyBayesResultsStruct.h"
#include "mean.h"
#include "packParams.h"
#include "processBayes.h"
#include "ratDREAM.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <algorithm>
#include <cmath>

// Function Definitions
namespace RAT
{
  void runDREAM(const struct5_T *problemDef, const cell_14 *problemDefCells,
                const struct1_T *problemDefLimits, const struct2_T *controls,
                const struct4_T *priors, struct5_T *outProblemDef, struct_T
                *outProblem, cell_wrap_9 result[6], d_struct_T *bayesResults)
  {
    ::coder::array<cell_wrap_1, 1U> fitParamNames;
    ::coder::array<real_T, 2U> Par_info_max;
    ::coder::array<real_T, 2U> Par_info_min;
    ::coder::array<real_T, 2U> a__1;
    ::coder::array<real_T, 2U> b_bayesResults;
    ::coder::array<real_T, 2U> c_bayesResults;
    ::coder::array<real_T, 2U> r;
    cell_13 b_result;
    e_struct_T dreamResults_bestFitsMean;
    f_struct_T dreamResults_predlims;
    struct5_T b_problemDef;
    int32_T b_loop_ub;
    int32_T c_result;
    int32_T i;
    int32_T i1;
    int32_T i2;
    int32_T loop_ub;

    //  Make an empty struct for bayesResults to hold the outputs of the
    //  calculation
    makeEmptyBayesResultsStruct(problemDef->numberOfContrasts, coder::internal::
      b_strcmp(problemDef->TF.data, problemDef->TF.size), controls->nChains,
      bayesResults->bestFitsMean.ref, bayesResults->bestFitsMean.sld,
      &bayesResults->bestFitsMean.chi, bayesResults->bestFitsMean.data,
      bayesResults->predlims.refPredInts, bayesResults->predlims.sldPredInts,
      bayesResults->predlims.refXdata, bayesResults->predlims.sldXdata,
      bayesResults->predlims.sampleChi.data,
      &bayesResults->predlims.sampleChi.size[0], bayesResults->parConfInts.par95,
      bayesResults->parConfInts.par65, bayesResults->parConfInts.mean,
      bayesResults->bestPars, &bayesResults->bayesRes, bayesResults->chain);

    //  Pre-allocation
    b_problemDef = *problemDef;
    packParams(&b_problemDef, problemDefCells->f7, problemDefCells->f8,
               problemDefCells->f9, problemDefCells->f10, problemDefCells->f11,
               problemDefCells->f12, problemDefCells->f13, problemDefCells->f20,
               problemDefLimits, &controls->checks, fitParamNames);

    //  Get the priors for the fitted parameters...
    //  Put all the RAT parameters together into one array...
    //
    //  Get the parameters from the user
    //  Total number of generations
    //  Number of chains
    //  Set the relevant parameters for the DREAM sampler....
    //  Dimension of the problem
    //  Number of Markov Chains
    //  Number of generations per chain
    // DREAMPar.lik = 1;                              % Model output is likelihood
    //  Parallel or not...
    //  Jump probabilities...
    //  This will change...
    //  Initial sampling and parameter range
    Par_info_min.set_size(1, b_problemDef.fitLimits.size(0));
    loop_ub = b_problemDef.fitLimits.size(0);
    for (i = 0; i < loop_ub; i++) {
      Par_info_min[i] = b_problemDef.fitLimits[i];
    }

    Par_info_max.set_size(1, b_problemDef.fitLimits.size(0));
    loop_ub = b_problemDef.fitLimits.size(0);
    for (i = 0; i < loop_ub; i++) {
      Par_info_max[i] = b_problemDef.fitLimits[i + b_problemDef.fitLimits.size(0)];
    }

    // if dreamC.prior
    // end
    //  Run the sampler....
    // [chain,output,fx] = rat_DREAM(DREAMPar,Par_info,[],ratInputs);
    // Func_name = @DREAMWrapper;
    getFittedPriors(fitParamNames, priors->priorNames, priors->priorValues,
                    b_problemDef.fitLimits, r);
    ratDREAM(static_cast<real_T>(fitParamNames.size(0)), controls->nChains, std::
             ceil(controls->nSamples / controls->nChains),
             controls->jumpProbability, controls->pUnitGamma, controls->adaptPCR,
             Par_info_min, Par_info_max, controls->boundHandling.data,
             controls->boundHandling.size, &b_problemDef, problemDefCells,
             controls, r, bayesResults->bayesRes.allChains,
             &bayesResults->bayesRes.dreamOutput, a__1);

    //  Combine all chains....
    bayesResults->chain.set_size(0, 0);
    i = static_cast<int32_T>(controls->nChains);
    if (i - 1 >= 0) {
      int32_T cutoff;
      if (fitParamNames.size(0) < 1) {
        b_loop_ub = 0;
      } else {
        b_loop_ub = fitParamNames.size(0);
      }

      cutoff = static_cast<int32_T>(std::floor(static_cast<real_T>
        (bayesResults->bayesRes.allChains.size(0)) * 0.25));
      if (cutoff > bayesResults->bayesRes.allChains.size(0)) {
        i1 = 0;
        i2 = 0;
      } else {
        i1 = cutoff - 1;
        i2 = bayesResults->bayesRes.allChains.size(0);
      }
    }

    for (int32_T b_i{0}; b_i < i; b_i++) {
      int32_T c_loop_ub;
      int32_T sizes_idx_0;
      boolean_T empty_non_axis_sizes;

      //  Keep only the last 75% of the chain..
      //  Combine the parallel chains into one....
      if ((bayesResults->chain.size(0) != 0) && (bayesResults->chain.size(1) !=
           0)) {
        c_result = bayesResults->chain.size(1);
      } else if ((i2 - i1 != 0) && (b_loop_ub != 0)) {
        c_result = b_loop_ub;
      } else {
        c_result = bayesResults->chain.size(1);
        if (b_loop_ub > bayesResults->chain.size(1)) {
          c_result = b_loop_ub;
        }
      }

      empty_non_axis_sizes = (c_result == 0);
      if (empty_non_axis_sizes || ((bayesResults->chain.size(0) != 0) &&
           (bayesResults->chain.size(1) != 0))) {
        loop_ub = bayesResults->chain.size(0);
      } else {
        loop_ub = 0;
      }

      if (empty_non_axis_sizes || ((i2 - i1 != 0) && (b_loop_ub != 0))) {
        sizes_idx_0 = i2 - i1;
      } else {
        sizes_idx_0 = 0;
      }

      c_loop_ub = i2 - i1;
      b_bayesResults.set_size(c_loop_ub, b_loop_ub);
      for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
        for (int32_T i4{0}; i4 < c_loop_ub; i4++) {
          b_bayesResults[i4 + b_bayesResults.size(0) * i3] =
            bayesResults->bayesRes.allChains[((i1 + i4) +
            bayesResults->bayesRes.allChains.size(0) * i3) +
            bayesResults->bayesRes.allChains.size(0) *
            bayesResults->bayesRes.allChains.size(1) * b_i];
        }
      }

      c_bayesResults.set_size(loop_ub + sizes_idx_0, c_result);
      for (int32_T i3{0}; i3 < c_result; i3++) {
        for (int32_T i4{0}; i4 < loop_ub; i4++) {
          c_bayesResults[i4 + c_bayesResults.size(0) * i3] = bayesResults->
            chain[i4 + loop_ub * i3];
        }
      }

      for (int32_T i3{0}; i3 < c_result; i3++) {
        for (int32_T i4{0}; i4 < sizes_idx_0; i4++) {
          c_bayesResults[(i4 + loop_ub) + c_bayesResults.size(0) * i3] =
            b_bayesResults[i4 + sizes_idx_0 * i3];
        }
      }

      bayesResults->chain.set_size(c_bayesResults.size(0), c_bayesResults.size(1));
      loop_ub = c_bayesResults.size(1);
      for (int32_T i3{0}; i3 < loop_ub; i3++) {
        c_loop_ub = c_bayesResults.size(0);
        for (int32_T i4{0}; i4 < c_loop_ub; i4++) {
          bayesResults->chain[i4 + bayesResults->chain.size(0) * i3] =
            c_bayesResults[i4 + c_bayesResults.size(0) * i3];
        }
      }
    }

    coder::mean(bayesResults->chain, bayesResults->bestPars);
    processBayes(bayesResults->bestPars, bayesResults->chain, &b_problemDef,
                 controls, problemDefCells, outProblemDef, outProblem, &b_result,
                 &dreamResults_bestFitsMean, &dreamResults_predlims,
                 &bayesResults->parConfInts);
    result[0].f1.set_size(b_result.f1.size(0), 1);
    c_result = b_result.f1.size(0);
    for (i = 0; i < c_result; i++) {
      loop_ub = b_result.f1[i].f1.size(0);
      result[0].f1[i].f1.set_size(b_result.f1[i].f1.size(0), 2);
      for (i1 = 0; i1 < 2; i1++) {
        for (i2 = 0; i2 < loop_ub; i2++) {
          result[0].f1[i].f1[i2 + result[0].f1[i].f1.size(0) * i1] =
            b_result.f1[i].f1[i2 + b_result.f1[i].f1.size(0) * i1];
        }
      }
    }

    result[1].f1.set_size(b_result.f2.size(0), 1);
    c_result = b_result.f2.size(0);
    for (i = 0; i < c_result; i++) {
      loop_ub = b_result.f2[i].f1.size(0);
      result[1].f1[i].f1.set_size(b_result.f2[i].f1.size(0), 2);
      for (i1 = 0; i1 < 2; i1++) {
        for (i2 = 0; i2 < loop_ub; i2++) {
          result[1].f1[i].f1[i2 + result[1].f1[i].f1.size(0) * i1] =
            b_result.f2[i].f1[i2 + b_result.f2[i].f1.size(0) * i1];
        }
      }
    }

    result[2].f1.set_size(b_result.f3.size(0), 1);
    c_result = b_result.f3.size(0);
    for (i = 0; i < c_result; i++) {
      loop_ub = b_result.f3[i].f1.size(0);
      result[2].f1[i].f1.set_size(b_result.f3[i].f1.size(0), 3);
      for (i1 = 0; i1 < 3; i1++) {
        for (i2 = 0; i2 < loop_ub; i2++) {
          result[2].f1[i].f1[i2 + result[2].f1[i].f1.size(0) * i1] =
            b_result.f3[i].f1[i2 + b_result.f3[i].f1.size(0) * i1];
        }
      }
    }

    result[3].f1.set_size(b_result.f4.size(0), b_result.f4.size(1));
    loop_ub = b_result.f4.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b_result.f4.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        result[3].f1[i1 + result[3].f1.size(0) * i] = b_result.f4[i1 +
          b_result.f4.size(0) * i];
      }
    }

    result[4].f1.set_size(b_result.f5.size(0), b_result.f5.size(1));
    loop_ub = b_result.f5.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b_result.f5.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        result[4].f1[i1 + result[4].f1.size(0) * i] = b_result.f5[i1 +
          b_result.f5.size(0) * i];
      }
    }

    result[5].f1.set_size(b_result.f6.size(0), b_result.f6.size(1));
    loop_ub = b_result.f6.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = b_result.f6.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        result[5].f1[i1 + result[5].f1.size(0) * i] = b_result.f6[i1 +
          b_result.f6.size(0) * i];
      }
    }

    //  Populate the output struct
    //  bayesResults.bayesRes.allChains = chain;
    cast(dreamResults_predlims.refPredInts, bayesResults->predlims.refPredInts);
    cast(dreamResults_predlims.sldPredInts, bayesResults->predlims.sldPredInts);
    cast(dreamResults_predlims.refXdata, bayesResults->predlims.refXdata);
    bayesResults->predlims.sldXdata.set_size(dreamResults_predlims.sldXdata.size
      (0), 1);
    for (i = 0; i < dreamResults_predlims.sldXdata.size(0); i++) {
      bayesResults->predlims.sldXdata[i].f1.set_size(1,
        dreamResults_predlims.sldXdata[i].f1.size(1));
      loop_ub = dreamResults_predlims.sldXdata[i].f1.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        bayesResults->predlims.sldXdata[i].f1[bayesResults->predlims.sldXdata[i]
          .f1.size(0) * i1] = dreamResults_predlims.sldXdata[i].f1[i1];
      }
    }

    bayesResults->predlims.sampleChi.size[0] = 1000;
    std::copy(&dreamResults_predlims.sampleChi[0],
              &dreamResults_predlims.sampleChi[1000],
              &bayesResults->predlims.sampleChi.data[0]);
    cast(dreamResults_bestFitsMean.ref, bayesResults->bestFitsMean.ref);
    bayesResults->bestFitsMean.sld.set_size(dreamResults_bestFitsMean.sld.size(0),
      dreamResults_bestFitsMean.sld.size(1));
    loop_ub = dreamResults_bestFitsMean.sld.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = dreamResults_bestFitsMean.sld.size(0);
      for (i1 = 0; i1 < b_loop_ub; i1++) {
        bayesResults->bestFitsMean.sld[i1 + bayesResults->bestFitsMean.sld.size
          (0) * i] = dreamResults_bestFitsMean.sld[i1 +
          dreamResults_bestFitsMean.sld.size(0) * i];
      }
    }

    bayesResults->bestFitsMean.chi = dreamResults_bestFitsMean.chi;
    cast(dreamResults_bestFitsMean.data, bayesResults->bestFitsMean.data);

    //  These are not defined in makeEmptyBayesResultsStruct
    //  bayesResults.bayesRes.DREAMPar = DREAMPar;
    //  bayesResults.bayesRes.Meas_info = Meas_info;
    //  bayesResults.bayesRes.dreamOutput = output;
  }
}

// End of code generation (runDREAM.cpp)
