//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// unpackParams.cpp
//
// Code generation for function 'unpackParams'
//

// Include files
#include "unpackParams.h"
#include "RATMain_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void unpackParams(struct5_T *problemDef, const ::coder::array<real_T, 2U>
                    &controls_checks_fitParam, const ::coder::array<real_T, 2U>
                    &controls_checks_fitBackgroundParam, const ::coder::array<
                    real_T, 2U> &controls_checks_fitQzshift, const ::coder::
                    array<real_T, 2U> &controls_checks_fitScalefactor, const ::
                    coder::array<real_T, 2U> &controls_checks_fitBulkIn, const ::
                    coder::array<real_T, 2U> &controls_checks_fitBulkOut, const ::
                    coder::array<real_T, 2U> &controls_checks_fitResolutionParam,
                    const ::coder::array<real_T, 2U>
                    &controls_checks_fitDomainRatio)
  {
    ::coder::array<real_T, 2U> uppars;
    int32_T i;
    int32_T unnamed_idx_1;
    int32_T uppars_counter;
    uint32_T packed_counter;
    uint32_T unpacked_counter;

    // Unpack the params out of the fitParams and otherParams arrays
    // back into problem.params
    // problem = getappdata(0,'problem');
    unpacked_counter = 1U;
    packed_counter = 1U;
    uppars_counter = 0;
    unnamed_idx_1 = problemDef->params.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    i = controls_checks_fitParam.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitParam[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->params.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->params[i] = uppars[i];
    }

    // Also the backgrounds
    unnamed_idx_1 = problemDef->backgroundParams.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitBackgroundParam.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitBackgroundParam[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->backgroundParams.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->backgroundParams[i] = uppars[i];
    }

    // Scalefactors
    unnamed_idx_1 = problemDef->scalefactors.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitScalefactor.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitScalefactor[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->scalefactors.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->scalefactors[i] = uppars[i];
    }

    // qzshifts
    unnamed_idx_1 = problemDef->qzshifts.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitQzshift.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitQzshift[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->qzshifts.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->qzshifts[i] = uppars[i];
    }

    // Bulk In
    unnamed_idx_1 = problemDef->bulkIn.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitBulkIn.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitBulkIn[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->bulkIn.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->bulkIn[i] = uppars[i];
    }

    // Bulk Out
    unnamed_idx_1 = problemDef->bulkOut.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitBulkOut.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitBulkOut[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->bulkOut.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->bulkOut[i] = uppars[i];
    }

    // Resolutions
    unnamed_idx_1 = problemDef->resolutionParams.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitResolutionParam.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitResolutionParam[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->resolutionParams.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->resolutionParams[i] = uppars[i];
    }

    // Domain Ratios
    unnamed_idx_1 = problemDef->domainRatio.size(1);
    uppars.set_size(1, unnamed_idx_1);
    for (i = 0; i < unnamed_idx_1; i++) {
      uppars[i] = 0.0;
    }

    uppars_counter = 0;
    i = controls_checks_fitDomainRatio.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      if (controls_checks_fitDomainRatio[b_i] == 1.0) {
        uppars[uppars_counter] = problemDef->fitParams[static_cast<int32_T>
          (unpacked_counter) - 1];
        unpacked_counter++;
        uppars_counter++;
      } else {
        uppars[uppars_counter] = problemDef->otherParams[static_cast<int32_T>
          (packed_counter) - 1];
        packed_counter++;
        uppars_counter++;
      }
    }

    problemDef->domainRatio.set_size(1, uppars.size(1));
    unnamed_idx_1 = uppars.size(1);
    for (i = 0; i < unnamed_idx_1; i++) {
      problemDef->domainRatio[i] = uppars[i];
    }
  }
}

// End of code generation (unpackParams.cpp)
