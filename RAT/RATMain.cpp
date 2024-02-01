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
  static void cast(const c_struct_T *r, struct0_T *r1);
  static void cast(const struct0_T *r, c_struct_T *r1);
  static void cast(const cell_7 *r, cell_11 *r1);
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
  static void cast(const c_struct_T *r, struct0_T *r1)
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

  static void cast(const struct0_T *r, c_struct_T *r1)
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

  static void cast(const cell_7 *r, cell_11 *r1)
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

  void RATMain(struct0_T *problemStruct, const cell_7 *problemCells, const
               struct1_T *problemLimits, struct2_T *controls, const struct4_T
               *priors, struct5_T *contrastParams, cell_wrap_9 resultCells[6],
               struct7_T *bayesResults)
  {
    static c_struct_T b_problemStruct;
    static c_struct_T r;
    static g_struct_T b_bayesResults;
    ::coder::array<int8_T, 1U> t19_calculations_allChis;
    cell_11 r1;
    d_struct_T b_contrastParams;
    int32_T switch_expression_size[2];
    int32_T b_loop_ub;
    int32_T loop_ub;
    int32_T problemStruct_idx_0_tmp;
    char_T switch_expression_data[10000];
    coder::repmat(resultCells);
    problemStruct_idx_0_tmp = static_cast<int32_T>
      (problemStruct->numberOfContrasts);
    t19_calculations_allChis.set_size(problemStruct_idx_0_tmp);
    contrastParams->ssubs.set_size(problemStruct_idx_0_tmp);
    contrastParams->backgroundParams.set_size(problemStruct_idx_0_tmp);
    contrastParams->qzshifts.set_size(problemStruct_idx_0_tmp);
    contrastParams->scalefactors.set_size(problemStruct_idx_0_tmp);
    contrastParams->bulkIn.set_size(problemStruct_idx_0_tmp);
    contrastParams->bulkOut.set_size(problemStruct_idx_0_tmp);
    contrastParams->resolutionParams.set_size(problemStruct_idx_0_tmp);
    for (int32_T i{0}; i < problemStruct_idx_0_tmp; i++) {
      t19_calculations_allChis[i] = 0;
      contrastParams->ssubs[i] = 0.0;
      contrastParams->backgroundParams[i] = 0.0;
      contrastParams->qzshifts[i] = 0.0;
      contrastParams->scalefactors[i] = 0.0;
      contrastParams->bulkIn[i] = 0.0;
      contrastParams->bulkOut[i] = 0.0;
      contrastParams->resolutionParams[i] = 0.0;
    }

    contrastParams->calculations.allChis.set_size(t19_calculations_allChis.size
      (0));
    loop_ub = t19_calculations_allChis.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      contrastParams->calculations.allChis[i] = 0.0;
    }

    contrastParams->calculations.sumChi = 0.0;
    contrastParams->allSubRough.set_size(problemStruct_idx_0_tmp);
    contrastParams->resample.set_size(problemStruct_idx_0_tmp, 1);
    for (int32_T i{0}; i < problemStruct_idx_0_tmp; i++) {
      contrastParams->allSubRough[i] = 0.0;
      contrastParams->resample[i] = 0.0;
    }

    makeEmptyBayesResultsStruct(problemStruct->numberOfContrasts, coder::
      internal::b_strcmp(problemStruct->TF.data, problemStruct->TF.size),
      controls->nChains, bayesResults->bestFitsMean.ref,
      bayesResults->bestFitsMean.sld, &bayesResults->bestFitsMean.chi,
      bayesResults->bestFitsMean.data, bayesResults->predlims.refPredInts,
      bayesResults->predlims.sldPredInts, bayesResults->predlims.refXdata,
      bayesResults->predlims.sldXdata, bayesResults->predlims.sampleChi.data,
      &bayesResults->predlims.sampleChi.size[0], bayesResults->parConfInts.par95,
      bayesResults->parConfInts.par65, bayesResults->parConfInts.mean,
      bayesResults->bestPars, &b_bayesResults.bayesRes, bayesResults->chain);
    bayesResults->bayesRes.allChains.set_size
      (b_bayesResults.bayesRes.allChains.size(0),
       b_bayesResults.bayesRes.allChains.size(1),
       b_bayesResults.bayesRes.allChains.size(2));
    loop_ub = b_bayesResults.bayesRes.allChains.size(2);
    for (int32_T i{0}; i < loop_ub; i++) {
      problemStruct_idx_0_tmp = b_bayesResults.bayesRes.allChains.size(1);
      for (int32_T i1{0}; i1 < problemStruct_idx_0_tmp; i1++) {
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

    // Decide what we are doing....
    coder::lower(controls->procedure.data, controls->procedure.size,
                 switch_expression_data, switch_expression_size);
    if (coder::internal::c_strcmp(switch_expression_data, switch_expression_size))
    {
      problemStruct_idx_0_tmp = 0;
    } else if (coder::internal::e_strcmp(switch_expression_data,
                switch_expression_size)) {
      problemStruct_idx_0_tmp = 1;
    } else if (coder::internal::f_strcmp(switch_expression_data,
                switch_expression_size)) {
      problemStruct_idx_0_tmp = 2;
    } else if (coder::internal::g_strcmp(switch_expression_data,
                switch_expression_size)) {
      problemStruct_idx_0_tmp = 3;
    } else if (coder::internal::h_strcmp(switch_expression_data,
                switch_expression_size)) {
      problemStruct_idx_0_tmp = 4;
    } else {
      problemStruct_idx_0_tmp = -1;
    }

    switch (problemStruct_idx_0_tmp) {
     case 0:
      // Just a single reflectivity calculation
      cast(problemStruct, &r);
      cast(problemCells, &r1);
      reflectivityCalculation(&r, &r1, controls, &b_contrastParams, resultCells);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }
      break;

     case 1:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning simplex\n\n");
        fflush(stdout);
      }

      cast(problemStruct, &b_problemStruct);
      cast(problemCells, &r1);
      runSimplex(&b_problemStruct, &r1, problemLimits, controls,
                 &b_contrastParams, resultCells);
      cast(&b_problemStruct, problemStruct);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }
      break;

     case 2:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning Differential Evolution\n\n");
        fflush(stdout);
      }

      cast(problemStruct, &b_problemStruct);
      cast(problemCells, &r1);
      runDE(&b_problemStruct, &r1, problemLimits, controls, &b_contrastParams,
            resultCells);
      cast(&b_problemStruct, problemStruct);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }
      break;

     case 3:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning Nested Sampler\n\n");
        fflush(stdout);
      }

      cast(problemStruct, &b_problemStruct);
      cast(problemCells, &r1);
      runNestedSampler(&b_problemStruct, &r1, problemLimits, controls, priors,
                       &b_contrastParams, resultCells, bayesResults);
      cast(&b_problemStruct, problemStruct);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }
      break;

     case 4:
      if (!coder::internal::d_strcmp(controls->display.data,
           controls->display.size)) {
        printf("\nRunning DREAM\n\n");
        fflush(stdout);
      }

      cast(problemStruct, &r);
      cast(problemCells, &r1);
      runDREAM(&r, &r1, problemLimits, controls, priors, &b_problemStruct,
               &b_contrastParams, resultCells, &b_bayesResults);
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
        problemStruct_idx_0_tmp = b_bayesResults.chain.size(0);
        for (int32_T i1{0}; i1 < problemStruct_idx_0_tmp; i1++) {
          bayesResults->chain[i1 + bayesResults->chain.size(0) * i] =
            b_bayesResults.chain[i1 + b_bayesResults.chain.size(0) * i];
        }
      }

      cast(&b_problemStruct, problemStruct);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }

      bayesResults->bayesRes.allChains.set_size
        (b_bayesResults.bayesRes.allChains.size(0),
         b_bayesResults.bayesRes.allChains.size(1),
         b_bayesResults.bayesRes.allChains.size(2));
      loop_ub = b_bayesResults.bayesRes.allChains.size(2);
      for (int32_T i{0}; i < loop_ub; i++) {
        problemStruct_idx_0_tmp = b_bayesResults.bayesRes.allChains.size(1);
        for (int32_T i1{0}; i1 < problemStruct_idx_0_tmp; i1++) {
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
        controls->procedure.data[i] = cv1[i];
      }

      cast(problemStruct, &r);
      cast(problemCells, &r1);
      reflectivityCalculation(&r, &r1, controls, &b_contrastParams, resultCells);
      contrastParams->ssubs.set_size(b_contrastParams.ssubs.size(0));
      loop_ub = b_contrastParams.ssubs.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->ssubs[i] = b_contrastParams.ssubs[i];
      }

      contrastParams->backgroundParams.set_size
        (b_contrastParams.backgroundParams.size(0));
      loop_ub = b_contrastParams.backgroundParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->backgroundParams[i] =
          b_contrastParams.backgroundParams[i];
      }

      contrastParams->qzshifts.set_size(b_contrastParams.qzshifts.size(0));
      loop_ub = b_contrastParams.qzshifts.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->qzshifts[i] = b_contrastParams.qzshifts[i];
      }

      contrastParams->scalefactors.set_size(b_contrastParams.scalefactors.size(0));
      loop_ub = b_contrastParams.scalefactors.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->scalefactors[i] = b_contrastParams.scalefactors[i];
      }

      contrastParams->bulkIn.set_size(b_contrastParams.bulkIn.size(0));
      loop_ub = b_contrastParams.bulkIn.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkIn[i] = b_contrastParams.bulkIn[i];
      }

      contrastParams->bulkOut.set_size(b_contrastParams.bulkOut.size(0));
      loop_ub = b_contrastParams.bulkOut.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->bulkOut[i] = b_contrastParams.bulkOut[i];
      }

      contrastParams->resolutionParams.set_size
        (b_contrastParams.resolutionParams.size(0));
      loop_ub = b_contrastParams.resolutionParams.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resolutionParams[i] =
          b_contrastParams.resolutionParams[i];
      }

      contrastParams->calculations = b_contrastParams.calculations;
      contrastParams->allSubRough.set_size(b_contrastParams.allSubRough.size(0));
      loop_ub = b_contrastParams.allSubRough.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->allSubRough[i] = b_contrastParams.allSubRough[i];
      }

      contrastParams->resample.set_size(1, b_contrastParams.resample.size(1));
      loop_ub = b_contrastParams.resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        contrastParams->resample[contrastParams->resample.size(0) * i] =
          b_contrastParams.resample[i];
      }
    }
  }
}

// End of code generation (RATMain.cpp)
