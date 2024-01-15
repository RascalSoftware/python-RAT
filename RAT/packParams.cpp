//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// packParams.cpp
//
// Code generation for function 'packParams'
//

// Include files
#include "packParams.h"
#include "RATMain_types.h"
#include "rt_nonfinite.h"
#include "sum.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void packParams(struct5_T *problemDef, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f7, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f8, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f9, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f10, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f11, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f12, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f13, const ::coder::array<cell_wrap_1, 2U>
                  &problemDefCells_f20, const struct1_T *limits, const struct3_T
                  *checks, ::coder::array<cell_wrap_1, 1U> &fitNames)
  {
    ::coder::array<real_T, 2U> fitLimits;
    ::coder::array<real_T, 2U> fitParams;
    ::coder::array<real_T, 2U> otherLimits;
    ::coder::array<real_T, 1U> otherParams;
    real_T numberOfFitted;
    int32_T b_loop_ub;
    int32_T fitCounter;
    int32_T i;
    int32_T loop_ub;
    int32_T numberOfTotal;
    int32_T otherCounter;

    // We need to pack the parameters into separate vectors
    // of those that are being fitted, and those that are
    // held constant.
    numberOfFitted = ((((((coder::sum(checks->fitParam) + coder::sum
      (checks->fitBackgroundParam)) + coder::sum(checks->fitScalefactor)) +
                         coder::sum(checks->fitQzshift)) + coder::sum
                        (checks->fitBulkIn)) + coder::sum(checks->fitBulkOut)) +
                      coder::sum(checks->fitResolutionParam)) + coder::sum
      (checks->fitDomainRatio);
    numberOfTotal = ((((((problemDef->params.size(1) +
                          problemDef->backgroundParams.size(1)) +
                         problemDef->scalefactors.size(1)) +
                        problemDef->qzshifts.size(1)) + problemDef->bulkIn.size
                       (1)) + problemDef->bulkOut.size(1)) +
                     problemDef->resolutionParams.size(1)) +
      problemDef->domainRatio.size(1);
    fitParams.set_size(problemDef->fitParams.size(0), problemDef->fitParams.size
                       (1));
    loop_ub = problemDef->fitParams.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = problemDef->fitParams.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        fitParams[i1 + fitParams.size(0) * i] = problemDef->fitParams[i1 +
          problemDef->fitParams.size(0) * i];
      }
    }

    // zeros(numberOfFitted,1);
    b_loop_ub = static_cast<int32_T>(static_cast<real_T>(numberOfTotal) -
      numberOfFitted);
    otherParams.set_size(b_loop_ub);
    for (i = 0; i < b_loop_ub; i++) {
      otherParams[i] = 0.0;
    }

    loop_ub = static_cast<int32_T>(numberOfFitted);
    fitLimits.set_size(loop_ub, 2);
    otherLimits.set_size(b_loop_ub, 2);
    for (i = 0; i < 2; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        fitLimits[i1 + fitLimits.size(0) * i] = 0.0;
      }

      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        otherLimits[i1 + otherLimits.size(0) * i] = 0.0;
      }
    }

    // limits = problemDef.limits;
    fitNames.set_size(loop_ub);
    for (int32_T b_i{0}; b_i < loop_ub; b_i++) {
      fitNames[b_i].f1.set_size(1, 0);
    }

    fitCounter = 0;
    otherCounter = 0;
    i = checks->fitParam.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitParam[n] == 1.0) {
        fitParams[fitCounter] = problemDef->params[n];
        fitLimits[fitCounter] = limits->param[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->param[n +
          limits->param.size(0)];
        loop_ub = problemDefCells_f7[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f7[problemDefCells_f7.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f7[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->params[n];
        otherLimits[otherCounter] = limits->param[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->param[n +
          limits->param.size(0)];
        otherCounter++;
      }
    }

    // Also do the same for backgrounds...
    i = checks->fitBackgroundParam.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitBackgroundParam[n] == 1.0) {
        fitParams[fitCounter] = problemDef->backgroundParams[n];
        fitLimits[fitCounter] = limits->backgroundParam[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->backgroundParam[n +
          limits->backgroundParam.size(0)];
        loop_ub = problemDefCells_f8[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f8[problemDefCells_f8.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f8[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->backgroundParams[n];
        otherLimits[otherCounter] = limits->backgroundParam[n];
        otherLimits[otherCounter + otherLimits.size(0)] =
          limits->backgroundParam[n + limits->backgroundParam.size(0)];
        otherCounter++;
      }
    }

    // ..also for the scale factors
    i = checks->fitScalefactor.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitScalefactor[n] == 1.0) {
        fitParams[fitCounter] = problemDef->scalefactors[n];
        fitLimits[fitCounter] = limits->scalefactor[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->scalefactor[n +
          limits->scalefactor.size(0)];
        loop_ub = problemDefCells_f9[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f9[problemDefCells_f9.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f9[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->scalefactors[n];
        otherLimits[otherCounter] = limits->scalefactor[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->scalefactor[n
          + limits->scalefactor.size(0)];
        otherCounter++;
      }
    }

    // Need qzshifts
    i = checks->fitQzshift.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitQzshift[n] == 1.0) {
        fitParams[fitCounter] = problemDef->qzshifts[n];
        fitLimits[fitCounter] = limits->qzshift[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->qzshift[n +
          limits->qzshift.size(0)];
        loop_ub = problemDefCells_f10[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f10[problemDefCells_f10.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f10[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->qzshifts[n];
        otherLimits[otherCounter] = limits->qzshift[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->qzshift[n +
          limits->qzshift.size(0)];
        otherCounter++;
      }
    }

    // Bulk In
    i = checks->fitBulkIn.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitBulkIn[n] == 1.0) {
        fitParams[fitCounter] = problemDef->bulkIn[n];
        fitLimits[fitCounter] = limits->bulkIn[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->bulkIn[n +
          limits->bulkIn.size(0)];
        loop_ub = problemDefCells_f11[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f11[problemDefCells_f11.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f11[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->bulkIn[n];
        otherLimits[otherCounter] = limits->bulkIn[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->bulkIn[n +
          limits->bulkIn.size(0)];
        otherCounter++;
      }
    }

    // Bulk Out
    i = checks->fitBulkOut.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitBulkOut[n] == 1.0) {
        fitParams[fitCounter] = problemDef->bulkOut[n];
        fitLimits[fitCounter] = limits->bulkOut[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->bulkOut[n +
          limits->bulkOut.size(0)];
        loop_ub = problemDefCells_f12[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f12[problemDefCells_f12.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f12[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->bulkOut[n];
        otherLimits[otherCounter] = limits->bulkOut[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->bulkOut[n +
          limits->bulkOut.size(0)];
        otherCounter++;
      }
    }

    // Resolution.....
    i = checks->fitResolutionParam.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitResolutionParam[n] == 1.0) {
        fitParams[fitCounter] = problemDef->resolutionParams[n];
        fitLimits[fitCounter] = limits->resolutionParam[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->resolutionParam[n +
          limits->resolutionParam.size(0)];
        loop_ub = problemDefCells_f13[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f13[problemDefCells_f13.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f13[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->resolutionParams[n];
        otherLimits[otherCounter] = limits->resolutionParam[n];
        otherLimits[otherCounter + otherLimits.size(0)] =
          limits->resolutionParam[n + limits->resolutionParam.size(0)];
        otherCounter++;
      }
    }

    //  Domain Ratio
    i = checks->fitDomainRatio.size(1);
    for (int32_T n{0}; n < i; n++) {
      if (checks->fitDomainRatio[n] == 1.0) {
        fitParams[fitCounter] = problemDef->domainRatio[n];
        fitLimits[fitCounter] = limits->domainRatio[n];
        fitLimits[fitCounter + fitLimits.size(0)] = limits->domainRatio[n +
          limits->domainRatio.size(0)];
        loop_ub = problemDefCells_f20[n].f1.size(1);
        fitNames[fitCounter].f1.set_size(1,
          problemDefCells_f20[problemDefCells_f20.size(0) * n].f1.size(1));
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          fitNames[fitCounter].f1[i1] = problemDefCells_f20[n].f1[i1];
        }

        fitCounter++;
      } else {
        otherParams[otherCounter] = problemDef->domainRatio[n];
        otherLimits[otherCounter] = limits->domainRatio[n];
        otherLimits[otherCounter + otherLimits.size(0)] = limits->domainRatio[n
          + limits->domainRatio.size(0)];
        otherCounter++;
      }
    }

    problemDef->fitParams.set_size(fitParams.size(0), fitParams.size(1));
    loop_ub = fitParams.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = fitParams.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        problemDef->fitParams[i1 + problemDef->fitParams.size(0) * i] =
          fitParams[i1 + fitParams.size(0) * i];
      }
    }

    b_loop_ub = otherParams.size(0);
    problemDef->otherParams.set_size(otherParams.size(0), 1);
    for (i = 0; i < b_loop_ub; i++) {
      problemDef->otherParams[i] = otherParams[i];
    }

    problemDef->fitLimits.set_size(fitLimits.size(0), 2);
    problemDef->otherLimits.set_size(otherLimits.size(0), 2);
    loop_ub = fitLimits.size(0);
    b_loop_ub = otherLimits.size(0);
    for (i = 0; i < 2; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        problemDef->fitLimits[i1 + problemDef->fitLimits.size(0) * i] =
          fitLimits[i1 + fitLimits.size(0) * i];
      }

      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        problemDef->otherLimits[i1 + problemDef->otherLimits.size(0) * i] =
          otherLimits[i1 + otherLimits.size(0) * i];
      }
    }
  }
}

// End of code generation (packParams.cpp)
