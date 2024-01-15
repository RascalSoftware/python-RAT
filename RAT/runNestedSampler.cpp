//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runNestedSampler.cpp
//
// Code generation for function 'runNestedSampler'
//

// Include files
#include "runNestedSampler.h"
#include "RATMain_internal_types.h"
#include "RATMain_rtwutil.h"
#include "RATMain_types.h"
#include "blockedSummation.h"
#include "getFittedPriors.h"
#include "makeEmptyBayesResultsStruct.h"
#include "mean.h"
#include "nestedSampler.h"
#include "packParams.h"
#include "processBayes.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <algorithm>

// Function Definitions
namespace RAT
{
  void runNestedSampler(struct5_T *problemDef, const cell_14 *problemDefCells,
                        const struct1_T *problemDefLimits, const struct2_T
                        *controls, const struct4_T *inPriors, struct_T
                        *outProblem, cell_wrap_9 result[6], struct8_T
                        *bayesResults)
  {
    ::coder::array<cell_wrap_1, 1U> fitNames;
    ::coder::array<cell_wrap_8, 2U> expl_temp_sld;
    ::coder::array<cell_wrap_8, 1U> expl_temp_data;
    ::coder::array<cell_wrap_8, 1U> expl_temp_ref;
    ::coder::array<cell_wrap_8, 1U> t9_predlims_refPredInts;
    ::coder::array<cell_wrap_8, 1U> t9_predlims_refXdata;
    ::coder::array<cell_wrap_8, 1U> t9_predlims_sldPredInts;
    ::coder::array<real_T, 2U> b_expl_temp;
    ::coder::array<real_T, 2U> bayesOutputs_chain;
    ::coder::array<real_T, 2U> chain;
    ::coder::array<real_T, 2U> expl_temp;
    ::coder::array<real_T, 2U> expl_temp_mean;
    ::coder::array<real_T, 2U> expl_temp_par65;
    ::coder::array<real_T, 2U> expl_temp_par95;
    ::coder::array<real_T, 2U> r;
    ::coder::array<real_T, 2U> r1;
    c_struct_T t9_bayesRes;
    cell_13 b_result;
    e_struct_T nestResults_bestFitsMean;
    f_struct_T nestResults_predlims;
    struct5_T b_problemDef;
    real_T t9_predlims_sampleChi_data[1000];
    real_T H;
    real_T expl_temp_chi;
    real_T logZ;
    int32_T b_loop_ub;
    int32_T loop_ub;
    int32_T t9_predlims_sampleChi_size;
    packParams(problemDef, problemDefCells->f7, problemDefCells->f8,
               problemDefCells->f9, problemDefCells->f10, problemDefCells->f11,
               problemDefCells->f12, problemDefCells->f13, problemDefCells->f20,
               problemDefLimits, &controls->checks, fitNames);

    //  Make an empty struct for bayesResults to hold the outputs of the
    //  calculation
    b_makeEmptyBayesResultsStruct(problemDef->numberOfContrasts, coder::internal::
      b_strcmp(problemDef->TF.data, problemDef->TF.size), expl_temp_ref,
      expl_temp_sld, &expl_temp_chi, expl_temp_data, t9_predlims_refPredInts,
      t9_predlims_sldPredInts, t9_predlims_refXdata,
      bayesResults->predlims.sldXdata, t9_predlims_sampleChi_data,
      &t9_predlims_sampleChi_size, expl_temp_par95, expl_temp_par65,
      expl_temp_mean, expl_temp, &t9_bayesRes, b_expl_temp);
    bayesResults->bayesRes.allChains.set_size(t9_bayesRes.allChains.size(0),
      t9_bayesRes.allChains.size(1), t9_bayesRes.allChains.size(2));
    loop_ub = t9_bayesRes.allChains.size(2);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = t9_bayesRes.allChains.size(1);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        t9_predlims_sampleChi_size = t9_bayesRes.allChains.size(0);
        for (int32_T i2{0}; i2 < t9_predlims_sampleChi_size; i2++) {
          bayesResults->bayesRes.allChains[(i2 +
            bayesResults->bayesRes.allChains.size(0) * i1) +
            bayesResults->bayesRes.allChains.size(0) *
            bayesResults->bayesRes.allChains.size(1) * i] =
            t9_bayesRes.allChains[(i2 + t9_bayesRes.allChains.size(0) * i1) +
            t9_bayesRes.allChains.size(0) * t9_bayesRes.allChains.size(1) * i];
        }
      }
    }

    bayesResults->bayesRes.dreamOutput = t9_bayesRes.dreamOutput;

    // Deal with priors.
    // Tuning Parameters
    getFittedPriors(fitNames, inPriors->priorNames, inPriors->priorValues,
                    problemDef->fitLimits, r);
    nestedSampler(problemDef, controls, problemDefCells, controls->Nlive,
                  controls->Nmcmc, controls->nsTolerance, r, &logZ,
                  bayesResults->bayesRes.nestOutput.nestSamples,
                  bayesResults->bayesRes.nestOutput.postSamples, &H);

    //  Process the results...
    //  chain = nest_samples(:,1:end-1);
    if (fitNames.size(0) < 1) {
      loop_ub = 0;
    } else {
      loop_ub = fitNames.size(0);
    }

    chain.set_size(bayesResults->bayesRes.nestOutput.postSamples.size(0),
                   loop_ub);
    bayesOutputs_chain.set_size
      (bayesResults->bayesRes.nestOutput.postSamples.size(0), loop_ub);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = bayesResults->bayesRes.nestOutput.postSamples.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        chain[i1 + chain.size(0) * i] =
          bayesResults->bayesRes.nestOutput.postSamples[i1 +
          bayesResults->bayesRes.nestOutput.postSamples.size(0) * i];
      }

      b_loop_ub = bayesResults->bayesRes.nestOutput.postSamples.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        bayesOutputs_chain[i1 + bayesOutputs_chain.size(0) * i] =
          bayesResults->bayesRes.nestOutput.postSamples[i1 +
          bayesResults->bayesRes.nestOutput.postSamples.size(0) * i];
      }
    }

    int32_T iv[2];
    coder::mean(chain, r1);
    b_problemDef = *problemDef;
    iv[0] = (*(int32_T (*)[2])r1.size())[0];
    iv[1] = (*(int32_T (*)[2])r1.size())[1];
    processBayes((const real_T *)r1.data(), iv, bayesOutputs_chain,
                 &b_problemDef, controls, problemDefCells, problemDef,
                 outProblem, &b_result, &nestResults_bestFitsMean,
                 &nestResults_predlims, &bayesResults->parConfInts);
    result[0].f1.set_size(b_result.f1.size(0), 1);
    t9_predlims_sampleChi_size = b_result.f1.size(0);
    for (int32_T i{0}; i < t9_predlims_sampleChi_size; i++) {
      b_loop_ub = b_result.f1[i].f1.size(0);
      result[0].f1[i].f1.set_size(b_result.f1[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          result[0].f1[i].f1[i2 + result[0].f1[i].f1.size(0) * i1] =
            b_result.f1[i].f1[i2 + b_result.f1[i].f1.size(0) * i1];
        }
      }
    }

    result[1].f1.set_size(b_result.f2.size(0), 1);
    t9_predlims_sampleChi_size = b_result.f2.size(0);
    for (int32_T i{0}; i < t9_predlims_sampleChi_size; i++) {
      b_loop_ub = b_result.f2[i].f1.size(0);
      result[1].f1[i].f1.set_size(b_result.f2[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          result[1].f1[i].f1[i2 + result[1].f1[i].f1.size(0) * i1] =
            b_result.f2[i].f1[i2 + b_result.f2[i].f1.size(0) * i1];
        }
      }
    }

    result[2].f1.set_size(b_result.f3.size(0), 1);
    t9_predlims_sampleChi_size = b_result.f3.size(0);
    for (int32_T i{0}; i < t9_predlims_sampleChi_size; i++) {
      b_loop_ub = b_result.f3[i].f1.size(0);
      result[2].f1[i].f1.set_size(b_result.f3[i].f1.size(0), 3);
      for (int32_T i1{0}; i1 < 3; i1++) {
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          result[2].f1[i].f1[i2 + result[2].f1[i].f1.size(0) * i1] =
            b_result.f3[i].f1[i2 + b_result.f3[i].f1.size(0) * i1];
        }
      }
    }

    result[3].f1.set_size(b_result.f4.size(0), b_result.f4.size(1));
    b_loop_ub = b_result.f4.size(1);
    for (int32_T i{0}; i < b_loop_ub; i++) {
      t9_predlims_sampleChi_size = b_result.f4.size(0);
      for (int32_T i1{0}; i1 < t9_predlims_sampleChi_size; i1++) {
        result[3].f1[i1 + result[3].f1.size(0) * i] = b_result.f4[i1 +
          b_result.f4.size(0) * i];
      }
    }

    result[4].f1.set_size(b_result.f5.size(0), b_result.f5.size(1));
    b_loop_ub = b_result.f5.size(1);
    for (int32_T i{0}; i < b_loop_ub; i++) {
      t9_predlims_sampleChi_size = b_result.f5.size(0);
      for (int32_T i1{0}; i1 < t9_predlims_sampleChi_size; i1++) {
        result[4].f1[i1 + result[4].f1.size(0) * i] = b_result.f5[i1 +
          b_result.f5.size(0) * i];
      }
    }

    result[5].f1.set_size(b_result.f6.size(0), b_result.f6.size(1));
    b_loop_ub = b_result.f6.size(1);
    for (int32_T i{0}; i < b_loop_ub; i++) {
      t9_predlims_sampleChi_size = b_result.f6.size(0);
      for (int32_T i1{0}; i1 < t9_predlims_sampleChi_size; i1++) {
        result[5].f1[i1 + result[5].f1.size(0) * i] = b_result.f6[i1 +
          b_result.f6.size(0) * i];
      }
    }

    cast(nestResults_predlims.refPredInts, bayesResults->predlims.refPredInts);
    cast(nestResults_predlims.sldPredInts, bayesResults->predlims.sldPredInts);
    cast(nestResults_predlims.refXdata, bayesResults->predlims.refXdata);
    bayesResults->predlims.sldXdata.set_size(nestResults_predlims.sldXdata.size
      (0), 1);
    for (int32_T i{0}; i < nestResults_predlims.sldXdata.size(0); i++) {
      bayesResults->predlims.sldXdata[i].f1.set_size(1,
        nestResults_predlims.sldXdata[i].f1.size(1));
      b_loop_ub = nestResults_predlims.sldXdata[i].f1.size(1);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        bayesResults->predlims.sldXdata[i].f1[bayesResults->predlims.sldXdata[i]
          .f1.size(0) * i1] = nestResults_predlims.sldXdata[i].f1[i1];
      }
    }

    bayesResults->predlims.sampleChi.size[0] = 1000;
    std::copy(&nestResults_predlims.sampleChi[0],
              &nestResults_predlims.sampleChi[1000],
              &bayesResults->predlims.sampleChi.data[0]);
    cast(nestResults_bestFitsMean.ref, bayesResults->bestFitsMean.ref);
    bayesResults->bestFitsMean.sld.set_size(nestResults_bestFitsMean.sld.size(0),
      nestResults_bestFitsMean.sld.size(1));
    b_loop_ub = nestResults_bestFitsMean.sld.size(1);
    for (int32_T i{0}; i < b_loop_ub; i++) {
      t9_predlims_sampleChi_size = nestResults_bestFitsMean.sld.size(0);
      for (int32_T i1{0}; i1 < t9_predlims_sampleChi_size; i1++) {
        bayesResults->bestFitsMean.sld[i1 + bayesResults->bestFitsMean.sld.size
          (0) * i] = nestResults_bestFitsMean.sld[i1 +
          nestResults_bestFitsMean.sld.size(0) * i];
      }
    }

    bayesResults->bestFitsMean.chi = nestResults_bestFitsMean.chi;
    cast(nestResults_bestFitsMean.data, bayesResults->bestFitsMean.data);
    coder::blockedSummation(chain,
      bayesResults->bayesRes.nestOutput.postSamples.size(0), r1);
    bayesResults->bestPars.set_size(1, r1.size(1));
    b_loop_ub = r1.size(1);
    for (int32_T i{0}; i < b_loop_ub; i++) {
      bayesResults->bestPars[i] = r1[i] / static_cast<real_T>
        (bayesResults->bayesRes.nestOutput.postSamples.size(0));
    }

    bayesResults->chain.set_size
      (bayesResults->bayesRes.nestOutput.postSamples.size(0), loop_ub);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = bayesResults->bayesRes.nestOutput.postSamples.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        bayesResults->chain[i1 + bayesResults->chain.size(0) * i] =
          bayesResults->bayesRes.nestOutput.postSamples[i1 +
          bayesResults->bayesRes.nestOutput.postSamples.size(0) * i];
      }
    }

    bayesResults->bayesRes.nestOutput.LogZ = logZ;
  }
}

// End of code generation (runNestedSampler.cpp)
