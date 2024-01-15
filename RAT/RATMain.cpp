//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// RATMain.cpp
//
// Code generation for function 'RATMain'
//

// Include files
#include "RATMain.h"
#include "RATMain_data.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "lower.h"
#include "makeEmptyBayesResultsStruct.h"
#include "reflectivityCalculation.h"
#include "repmat.h"
#include "rt_nonfinite.h"
#include "runDE.h"
#include "runDREAM.h"
#include "runNestedSampler.h"
#include "runSimplex.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <algorithm>
#include <stdio.h>

// Function Declarations
namespace RAT
{
  static void cast(const struct0_T *r, struct5_T *r1);
  static void cast(const cell_7 *r, cell_14 *r1);
  static void cast(const ::coder::array<cell_wrap_5, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1);
  static void cast(const ::coder::array<cell_wrap_3, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1);
  static void cast(const ::coder::array<cell_wrap_4, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1);
  static void cast(const ::coder::array<cell_wrap_6, 2U> &r, ::coder::array<
                   cell_wrap_1, 2U> &r1);
}

// Function Definitions
namespace RAT
{
  static void cast(const struct0_T *r, struct5_T *r1)
  {
    int32_T b_loop_ub;
    int32_T loop_ub;
    r1->contrastBackgrounds.set_size(1, r->contrastBackgrounds.size(1));
    loop_ub = r->contrastBackgrounds.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastBackgrounds[i] = r->contrastBackgrounds[i];
    }

    r1->contrastBackgroundsType.set_size(1, r->contrastBackgroundsType.size(1));
    loop_ub = r->contrastBackgroundsType.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastBackgroundsType[i] = r->contrastBackgroundsType[i];
    }

    r1->TF.size[0] = 1;
    r1->TF.size[1] = r->TF.size[1];
    loop_ub = r->TF.size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&r->TF.data[0], &r->TF.data[loop_ub], &r1->TF.data[0]);
    }

    r1->resample.set_size(1, r->resample.size(1));
    loop_ub = r->resample.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->resample[i] = r->resample[i];
    }

    r1->dataPresent.set_size(1, r->dataPresent.size(1));
    loop_ub = r->dataPresent.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->dataPresent[i] = r->dataPresent[i];
    }

    r1->oilChiDataPresent.set_size(1, r->oilChiDataPresent.size(1));
    loop_ub = r->oilChiDataPresent.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->oilChiDataPresent[i] = r->oilChiDataPresent[i];
    }

    r1->numberOfContrasts = r->numberOfContrasts;
    r1->geometry.size[0] = 1;
    r1->geometry.size[1] = r->geometry.size[1];
    loop_ub = r->geometry.size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&r->geometry.data[0], &r->geometry.data[loop_ub],
                &r1->geometry.data[0]);
    }

    r1->useImaginary = r->useImaginary;
    r1->contrastQzshifts.set_size(1, r->contrastQzshifts.size(1));
    loop_ub = r->contrastQzshifts.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastQzshifts[i] = r->contrastQzshifts[i];
    }

    r1->contrastScalefactors.set_size(1, r->contrastScalefactors.size(1));
    loop_ub = r->contrastScalefactors.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastScalefactors[i] = r->contrastScalefactors[i];
    }

    r1->contrastBulkIns.set_size(1, r->contrastBulkIns.size(1));
    loop_ub = r->contrastBulkIns.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastBulkIns[i] = r->contrastBulkIns[i];
    }

    r1->contrastBulkOuts.set_size(1, r->contrastBulkOuts.size(1));
    loop_ub = r->contrastBulkOuts.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastBulkOuts[i] = r->contrastBulkOuts[i];
    }

    r1->contrastResolutions.set_size(1, r->contrastResolutions.size(1));
    loop_ub = r->contrastResolutions.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastResolutions[i] = r->contrastResolutions[i];
    }

    r1->backgroundParams.set_size(1, r->backgroundParams.size(1));
    loop_ub = r->backgroundParams.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->backgroundParams[i] = r->backgroundParams[i];
    }

    r1->qzshifts.set_size(1, r->qzshifts.size(1));
    loop_ub = r->qzshifts.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->qzshifts[i] = r->qzshifts[i];
    }

    r1->scalefactors.set_size(1, r->scalefactors.size(1));
    loop_ub = r->scalefactors.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->scalefactors[i] = r->scalefactors[i];
    }

    r1->bulkIn.set_size(1, r->bulkIn.size(1));
    loop_ub = r->bulkIn.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->bulkIn[i] = r->bulkIn[i];
    }

    r1->bulkOut.set_size(1, r->bulkOut.size(1));
    loop_ub = r->bulkOut.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->bulkOut[i] = r->bulkOut[i];
    }

    r1->resolutionParams.set_size(1, r->resolutionParams.size(1));
    loop_ub = r->resolutionParams.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->resolutionParams[i] = r->resolutionParams[i];
    }

    r1->params.set_size(1, r->params.size(1));
    loop_ub = r->params.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->params[i] = r->params[i];
    }

    r1->numberOfLayers = r->numberOfLayers;
    r1->modelType.size[0] = 1;
    r1->modelType.size[1] = r->modelType.size[1];
    loop_ub = r->modelType.size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&r->modelType.data[0], &r->modelType.data[loop_ub],
                &r1->modelType.data[0]);
    }

    r1->contrastCustomFiles.set_size(1, r->contrastCustomFiles.size(1));
    loop_ub = r->contrastCustomFiles.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastCustomFiles[i] = r->contrastCustomFiles[i];
    }

    r1->contrastDomainRatios.set_size(1, r->contrastDomainRatios.size(1));
    loop_ub = r->contrastDomainRatios.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->contrastDomainRatios[i] = r->contrastDomainRatios[i];
    }

    r1->domainRatio.set_size(1, r->domainRatio.size(1));
    loop_ub = r->domainRatio.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->domainRatio[i] = r->domainRatio[i];
    }

    r1->numberOfDomainContrasts = r->numberOfDomainContrasts;
    r1->fitParams.set_size(r->fitParams.size(0), r->fitParams.size(1));
    loop_ub = r->fitParams.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = r->fitParams.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        r1->fitParams[i1 + r1->fitParams.size(0) * i] = r->fitParams[i1 +
          r->fitParams.size(0) * i];
      }
    }

    r1->otherParams.set_size(r->otherParams.size(0), r->otherParams.size(1));
    loop_ub = r->otherParams.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = r->otherParams.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        r1->otherParams[i1 + r1->otherParams.size(0) * i] = r->otherParams[i1 +
          r->otherParams.size(0) * i];
      }
    }

    r1->fitLimits.set_size(r->fitLimits.size(0), r->fitLimits.size(1));
    loop_ub = r->fitLimits.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = r->fitLimits.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        r1->fitLimits[i1 + r1->fitLimits.size(0) * i] = r->fitLimits[i1 +
          r->fitLimits.size(0) * i];
      }
    }

    r1->otherLimits.set_size(r->otherLimits.size(0), r->otherLimits.size(1));
    loop_ub = r->otherLimits.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      b_loop_ub = r->otherLimits.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        r1->otherLimits[i1 + r1->otherLimits.size(0) * i] = r->otherLimits[i1 +
          r->otherLimits.size(0) * i];
      }
    }
  }

  static void cast(const cell_7 *r, cell_14 *r1)
  {
    int32_T loop_ub;
    r1->f1.set_size(1, r->f1.size(1));
    loop_ub = r->f1.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f1[i] = r->f1[i];
    }

    cast(r->f2, r1->f2);
    r1->f3.set_size(1, r->f3.size(1));
    loop_ub = r->f3.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f3[i] = r->f3[i];
    }

    r1->f4.set_size(1, r->f4.size(1));
    loop_ub = r->f4.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f4[i] = r->f4[i];
    }

    cast(r->f5, r1->f5);
    cast(r->f6, r1->f6);
    cast(r->f7, r1->f7);
    cast(r->f8, r1->f8);
    cast(r->f9, r1->f9);
    cast(r->f10, r1->f10);
    cast(r->f11, r1->f11);
    cast(r->f12, r1->f12);
    cast(r->f13, r1->f13);
    cast(r->f14, r1->f14);
    cast(r->f15, r1->f15);
    cast(r->f16, r1->f16);
    cast(r->f17, r1->f17);
    r1->f18.set_size(1, r->f18.size(1));
    loop_ub = r->f18.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f18[i] = r->f18[i];
    }

    cast(r->f19, r1->f19);
    cast(r->f20, r1->f20);
  }

  static void cast(const ::coder::array<cell_wrap_5, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size[1];
      r1[i1].f1.set_size(r[i1].f1.size[0], r[i1].f1.size[1]);
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size[0];
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[r1[i1].f1.size(0) * i2] = r[i1].f1.data[r[i1].f1.size[0] *
            i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_3, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[r1.size(0) * i1].f1.set_size(r[r.size(0) * i1].f1.size(0), r[r.size(0) *
        i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_4, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[r1.size(0) * i1].f1.set_size(r[r.size(0) * i1].f1.size(0), r[r.size(0) *
        i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[r1[i1].f1.size(0) * i2] = r[i1].f1[r[i1].f1.size(0) * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_6, 2U> &r, ::coder::array<
                   cell_wrap_1, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size[1];
      r1[r1.size(0) * i1].f1.set_size(1, r[r.size(0) * i1].f1.size[1]);
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        r1[i1].f1[i2] = r[i1].f1.data[i2];
      }
    }
  }

  void RATMain(const struct0_T *problemDef, const cell_7 *problemDefCells, const
               struct1_T *problemDefLimits, struct2_T *controls, const struct4_T
               *priors, struct5_T *outProblemDef, struct6_T *problem,
               cell_wrap_9 results[6], struct8_T *bayesResults)
  {
    static d_struct_T b_bayesResults;
    static struct5_T outProblemDef_tmp;
    ::coder::array<int8_T, 1U> t19_calculations_allChis;
    cell_13 b_results;
    cell_14 r;
    struct_T b_problem;
    int32_T switch_expression_size[2];
    int32_T b_loop_ub;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    char_T switch_expression_data[10000];
    coder::repmat(results);
    loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
    t19_calculations_allChis.set_size(loop_ub_tmp);
    problem->ssubs.set_size(loop_ub_tmp);
    problem->backgroundParams.set_size(loop_ub_tmp);
    problem->qzshifts.set_size(loop_ub_tmp);
    problem->scalefactors.set_size(loop_ub_tmp);
    problem->bulkIn.set_size(loop_ub_tmp);
    problem->bulkOut.set_size(loop_ub_tmp);
    problem->resolutionParams.set_size(loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      t19_calculations_allChis[i] = 0;
      problem->ssubs[i] = 0.0;
      problem->backgroundParams[i] = 0.0;
      problem->qzshifts[i] = 0.0;
      problem->scalefactors[i] = 0.0;
      problem->bulkIn[i] = 0.0;
      problem->bulkOut[i] = 0.0;
      problem->resolutionParams[i] = 0.0;
    }

    problem->calculations.allChis.set_size(t19_calculations_allChis.size(0));
    loop_ub = t19_calculations_allChis.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      problem->calculations.allChis[i] = 0.0;
    }

    problem->calculations.sumChi = 0.0;
    problem->allSubRough.set_size(loop_ub_tmp);
    problem->resample.set_size(loop_ub_tmp, 1);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      problem->allSubRough[i] = 0.0;
      problem->resample[i] = 0.0;
    }

    makeEmptyBayesResultsStruct(problemDef->numberOfContrasts, coder::internal::
      b_strcmp(problemDef->TF.data, problemDef->TF.size), controls->nChains,
      bayesResults->bestFitsMean.ref, bayesResults->bestFitsMean.sld,
      &bayesResults->bestFitsMean.chi, bayesResults->bestFitsMean.data,
      bayesResults->predlims.refPredInts, bayesResults->predlims.sldPredInts,
      bayesResults->predlims.refXdata, bayesResults->predlims.sldXdata,
      bayesResults->predlims.sampleChi.data,
      &bayesResults->predlims.sampleChi.size[0], bayesResults->parConfInts.par95,
      bayesResults->parConfInts.par65, bayesResults->parConfInts.mean,
      bayesResults->bestPars, &b_bayesResults.bayesRes, bayesResults->chain);
    bayesResults->bayesRes.allChains.set_size
      (b_bayesResults.bayesRes.allChains.size(0),
       b_bayesResults.bayesRes.allChains.size(1),
       b_bayesResults.bayesRes.allChains.size(2));
    loop_ub = b_bayesResults.bayesRes.allChains.size(2);
    for (int32_T i{0}; i < loop_ub; i++) {
      loop_ub_tmp = b_bayesResults.bayesRes.allChains.size(1);
      for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
        b_loop_ub = b_bayesResults.bayesRes.allChains.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          bayesResults->bayesRes.allChains[(i2 +
            bayesResults->bayesRes.allChains.size(0) * i1) +
            bayesResults->bayesRes.allChains.size(0) *
            bayesResults->bayesRes.allChains.size(1) * i] =
            b_bayesResults.bayesRes.allChains[(i2 +
            b_bayesResults.bayesRes.allChains.size(0) * i1) +
            b_bayesResults.bayesRes.allChains.size(0) *
            b_bayesResults.bayesRes.allChains.size(1) * i];
        }
      }
    }

    bayesResults->bayesRes.dreamOutput = b_bayesResults.bayesRes.dreamOutput;
    bayesResults->bayesRes.nestOutput.LogZ =
      b_bayesResults.bayesRes.nestOutput.LogZ;
    bayesResults->bayesRes.nestOutput.nestSamples.set_size(1, 2);
    bayesResults->bayesRes.nestOutput.postSamples.set_size(1, 2);
    bayesResults->bayesRes.nestOutput.nestSamples[0] =
      b_bayesResults.bayesRes.nestOutput.nestSamples.data[0];
    bayesResults->bayesRes.nestOutput.postSamples[0] =
      b_bayesResults.bayesRes.nestOutput.postSamples.data[0];
    bayesResults->bayesRes.nestOutput.nestSamples
      [bayesResults->bayesRes.nestOutput.nestSamples.size(0)] =
      b_bayesResults.bayesRes.nestOutput.nestSamples.data[1];
    bayesResults->bayesRes.nestOutput.postSamples
      [bayesResults->bayesRes.nestOutput.postSamples.size(0)] =
      b_bayesResults.bayesRes.nestOutput.postSamples.data[1];

    //  bayesResults = struct('bayesRes',[],...
    //      'chain',[],...
    //      's2chain',[],...
    //      'ssChain',[],...
    //      'bestPars_Mean',[],...
    //      'bestFitsMean',[],...
    //      'predLims',[],...
    //      'parConfInts',[]);
    cast(problemDef, &outProblemDef_tmp);
    *outProblemDef = outProblemDef_tmp;

    // Decide what we are doing....
    coder::lower(controls->procedure.data, controls->procedure.size,
                 switch_expression_data, switch_expression_size);
    if (coder::internal::c_strcmp(switch_expression_data, switch_expression_size))
    {
      loop_ub_tmp = 0;
    } else if (coder::internal::e_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 1;
    } else if (coder::internal::f_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 2;
    } else if (coder::internal::g_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 3;
    } else if (coder::internal::h_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 4;
    } else {
      loop_ub_tmp = -1;
    }

    switch (loop_ub_tmp) {
     case 0:
      // Just a single reflectivity calculation
      cast(problemDef, &outProblemDef_tmp);
      cast(problemDefCells, &r);
      reflectivityCalculation(&outProblemDef_tmp, &r, controls, &b_problem,
        &b_results);
      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }

      results[0].f1.set_size(b_results.f1.size(0), 1);
      loop_ub_tmp = b_results.f1.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f1[i].f1.size(0);
        results[0].f1[i].f1.set_size(b_results.f1[i].f1.size(0), 2);
        for (int32_T i1{0}; i1 < 2; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[0].f1[i].f1[i2 + results[0].f1[i].f1.size(0) * i1] =
              b_results.f1[i].f1[i2 + b_results.f1[i].f1.size(0) * i1];
          }
        }
      }

      results[1].f1.set_size(b_results.f2.size(0), 1);
      loop_ub_tmp = b_results.f2.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f2[i].f1.size(0);
        results[1].f1[i].f1.set_size(b_results.f2[i].f1.size(0), 2);
        for (int32_T i1{0}; i1 < 2; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[1].f1[i].f1[i2 + results[1].f1[i].f1.size(0) * i1] =
              b_results.f2[i].f1[i2 + b_results.f2[i].f1.size(0) * i1];
          }
        }
      }

      results[2].f1.set_size(b_results.f3.size(0), 1);
      loop_ub_tmp = b_results.f3.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f3[i].f1.size(0);
        results[2].f1[i].f1.set_size(b_results.f3[i].f1.size(0), 3);
        for (int32_T i1{0}; i1 < 3; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[2].f1[i].f1[i2 + results[2].f1[i].f1.size(0) * i1] =
              b_results.f3[i].f1[i2 + b_results.f3[i].f1.size(0) * i1];
          }
        }
      }

      results[3].f1.set_size(b_results.f4.size(0), b_results.f4.size(1));
      loop_ub = b_results.f4.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f4.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[3].f1[i1 + results[3].f1.size(0) * i] = b_results.f4[i1 +
            b_results.f4.size(0) * i];
        }
      }

      results[4].f1.set_size(b_results.f5.size(0), b_results.f5.size(1));
      loop_ub = b_results.f5.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f5.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[4].f1[i1 + results[4].f1.size(0) * i] = b_results.f5[i1 +
            b_results.f5.size(0) * i];
        }
      }

      results[5].f1.set_size(b_results.f6.size(0), b_results.f6.size(1));
      loop_ub = b_results.f6.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f6.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[5].f1[i1 + results[5].f1.size(0) * i] = b_results.f6[i1 +
            b_results.f6.size(0) * i];
        }
      }
      break;

     case 1:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning simplex\n\n");
        fflush(stdout);
      }

      cast(problemDefCells, &r);
      runSimplex(outProblemDef, &r, problemDefLimits, controls, &b_problem,
                 results);
      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }
      break;

     case 2:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning Differential Evolution\n\n");
        fflush(stdout);
      }

      cast(problemDefCells, &r);
      runDE(outProblemDef, &r, problemDefLimits, controls, &b_problem, results);
      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }
      break;

     case 3:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning Nested Sampler\n\n");
        fflush(stdout);
      }

      cast(problemDefCells, &r);
      runNestedSampler(outProblemDef, &r, problemDefLimits, controls, priors,
                       &b_problem, results, bayesResults);
      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }
      break;

     case 4:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning DREAM\n\n");
        fflush(stdout);
      }

      cast(problemDef, &outProblemDef_tmp);
      cast(problemDefCells, &r);
      runDREAM(&outProblemDef_tmp, &r, problemDefLimits, controls, priors,
               outProblemDef, &b_problem, results, &b_bayesResults);
      bayesResults->bestFitsMean = b_bayesResults.bestFitsMean;
      bayesResults->predlims = b_bayesResults.predlims;
      bayesResults->parConfInts = b_bayesResults.parConfInts;
      bayesResults->bestPars.set_size(1, b_bayesResults.bestPars.size(1));
      loop_ub = b_bayesResults.bestPars.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        bayesResults->bestPars[i] = b_bayesResults.bestPars[i];
      }

      bayesResults->chain.set_size(b_bayesResults.chain.size(0),
        b_bayesResults.chain.size(1));
      loop_ub = b_bayesResults.chain.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_bayesResults.chain.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          bayesResults->chain[i1 + bayesResults->chain.size(0) * i] =
            b_bayesResults.chain[i1 + b_bayesResults.chain.size(0) * i];
        }
      }

      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }

      bayesResults->bayesRes.allChains.set_size
        (b_bayesResults.bayesRes.allChains.size(0),
         b_bayesResults.bayesRes.allChains.size(1),
         b_bayesResults.bayesRes.allChains.size(2));
      loop_ub = b_bayesResults.bayesRes.allChains.size(2);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_bayesResults.bayesRes.allChains.size(1);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          b_loop_ub = b_bayesResults.bayesRes.allChains.size(0);
          for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
            bayesResults->bayesRes.allChains[(i2 +
              bayesResults->bayesRes.allChains.size(0) * i1) +
              bayesResults->bayesRes.allChains.size(0) *
              bayesResults->bayesRes.allChains.size(1) * i] =
              b_bayesResults.bayesRes.allChains[(i2 +
              b_bayesResults.bayesRes.allChains.size(0) * i1) +
              b_bayesResults.bayesRes.allChains.size(0) *
              b_bayesResults.bayesRes.allChains.size(1) * i];
          }
        }
      }

      bayesResults->bayesRes.dreamOutput = b_bayesResults.bayesRes.dreamOutput;
      bayesResults->bayesRes.nestOutput.LogZ =
        b_bayesResults.bayesRes.nestOutput.LogZ;
      bayesResults->bayesRes.nestOutput.nestSamples.set_size(1, 2);
      bayesResults->bayesRes.nestOutput.postSamples.set_size(1, 2);
      bayesResults->bayesRes.nestOutput.nestSamples[0] =
        b_bayesResults.bayesRes.nestOutput.nestSamples.data[0];
      bayesResults->bayesRes.nestOutput.postSamples[0] =
        b_bayesResults.bayesRes.nestOutput.postSamples.data[0];
      bayesResults->bayesRes.nestOutput.nestSamples
        [bayesResults->bayesRes.nestOutput.nestSamples.size(0)] =
        b_bayesResults.bayesRes.nestOutput.nestSamples.data[1];
      bayesResults->bayesRes.nestOutput.postSamples
        [bayesResults->bayesRes.nestOutput.postSamples.size(0)] =
        b_bayesResults.bayesRes.nestOutput.postSamples.data[1];
      break;
    }

    //  Then just do a final calculation to fill in SLD if necessary
    //  (i.e. if calcSLD is no for fit)
    if (!controls->calcSldDuringFit) {
      controls->calcSldDuringFit = true;
      controls->procedure.size[0] = 1;
      controls->procedure.size[1] = 9;
      for (int32_T i{0}; i < 9; i++) {
        controls->procedure.data[i] = cv[i];
      }

      cast(problemDefCells, &r);
      reflectivityCalculation(outProblemDef, &r, controls, &b_problem,
        &b_results);
      problem->ssubs.set_size(b_problem.ssubs.size(0));
      loop_ub = b_problem.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->ssubs[i] = b_problem.ssubs[i];
      }

      problem->backgroundParams.set_size(b_problem.backgroundParams.size(0));
      loop_ub = b_problem.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->backgroundParams[i] = b_problem.backgroundParams[i];
      }

      problem->qzshifts.set_size(b_problem.qzshifts.size(0));
      loop_ub = b_problem.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->qzshifts[i] = b_problem.qzshifts[i];
      }

      problem->scalefactors.set_size(b_problem.scalefactors.size(0));
      loop_ub = b_problem.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->scalefactors[i] = b_problem.scalefactors[i];
      }

      problem->bulkIn.set_size(b_problem.bulkIn.size(0));
      loop_ub = b_problem.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkIn[i] = b_problem.bulkIn[i];
      }

      problem->bulkOut.set_size(b_problem.bulkOut.size(0));
      loop_ub = b_problem.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->bulkOut[i] = b_problem.bulkOut[i];
      }

      problem->resolutionParams.set_size(b_problem.resolutionParams.size(0));
      loop_ub = b_problem.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resolutionParams[i] = b_problem.resolutionParams[i];
      }

      problem->calculations = b_problem.calculations;
      problem->allSubRough.set_size(b_problem.allSubRough.size(0));
      loop_ub = b_problem.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->allSubRough[i] = b_problem.allSubRough[i];
      }

      problem->resample.set_size(1, b_problem.resample.size(1));
      loop_ub = b_problem.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        problem->resample[problem->resample.size(0) * i] = b_problem.resample[i];
      }

      results[0].f1.set_size(b_results.f1.size(0), 1);
      loop_ub_tmp = b_results.f1.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f1[i].f1.size(0);
        results[0].f1[i].f1.set_size(b_results.f1[i].f1.size(0), 2);
        for (int32_T i1{0}; i1 < 2; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[0].f1[i].f1[i2 + results[0].f1[i].f1.size(0) * i1] =
              b_results.f1[i].f1[i2 + b_results.f1[i].f1.size(0) * i1];
          }
        }
      }

      results[1].f1.set_size(b_results.f2.size(0), 1);
      loop_ub_tmp = b_results.f2.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f2[i].f1.size(0);
        results[1].f1[i].f1.set_size(b_results.f2[i].f1.size(0), 2);
        for (int32_T i1{0}; i1 < 2; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[1].f1[i].f1[i2 + results[1].f1[i].f1.size(0) * i1] =
              b_results.f2[i].f1[i2 + b_results.f2[i].f1.size(0) * i1];
          }
        }
      }

      results[2].f1.set_size(b_results.f3.size(0), 1);
      loop_ub_tmp = b_results.f3.size(0);
      for (int32_T i{0}; i < loop_ub_tmp; i++) {
        loop_ub = b_results.f3[i].f1.size(0);
        results[2].f1[i].f1.set_size(b_results.f3[i].f1.size(0), 3);
        for (int32_T i1{0}; i1 < 3; i1++) {
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            results[2].f1[i].f1[i2 + results[2].f1[i].f1.size(0) * i1] =
              b_results.f3[i].f1[i2 + b_results.f3[i].f1.size(0) * i1];
          }
        }
      }

      results[3].f1.set_size(b_results.f4.size(0), b_results.f4.size(1));
      loop_ub = b_results.f4.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f4.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[3].f1[i1 + results[3].f1.size(0) * i] = b_results.f4[i1 +
            b_results.f4.size(0) * i];
        }
      }

      results[4].f1.set_size(b_results.f5.size(0), b_results.f5.size(1));
      loop_ub = b_results.f5.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f5.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[4].f1[i1 + results[4].f1.size(0) * i] = b_results.f5[i1 +
            b_results.f5.size(0) * i];
        }
      }

      results[5].f1.set_size(b_results.f6.size(0), b_results.f6.size(1));
      loop_ub = b_results.f6.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        loop_ub_tmp = b_results.f6.size(0);
        for (int32_T i1{0}; i1 < loop_ub_tmp; i1++) {
          results[5].f1[i1 + results[5].f1.size(0) * i] = b_results.f6[i1 +
            b_results.f6.size(0) * i];
        }
      }
    }
  }
}

// End of code generation (RATMain.cpp)
