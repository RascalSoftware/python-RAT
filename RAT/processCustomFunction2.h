//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processCustomFunction2.h
//
// Code generation for function 'processCustomFunction2'
//
#ifndef PROCESSCUSTOMFUNCTION2_H
#define PROCESSCUSTOMFUNCTION2_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace domainsTF
  {
    namespace customLayers
    {
      void processCustomFunction(const ::coder::array<real_T, 2U>
        &contrastBackgrounds, const ::coder::array<real_T, 2U> &contrastQzshifts,
        const ::coder::array<real_T, 2U> &contrastScalefactors, const ::coder::
        array<real_T, 2U> &contrastBulkIns, const ::coder::array<real_T, 2U>
        &contrastBulkOuts, const ::coder::array<real_T, 2U> &contrastResolutions,
        const ::coder::array<real_T, 2U> &backgroundParams, const ::coder::array<
        real_T, 2U> &qzshifts, const ::coder::array<real_T, 2U> &scalefactor,
        const ::coder::array<real_T, 2U> &bulkIn, const ::coder::array<real_T,
        2U> &bulkOut, const ::coder::array<real_T, 2U> &resolutionParams, const ::
        coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts, const ::
        coder::array<cell_wrap_1, 2U> &customFiles, const ::coder::array<real_T,
        2U> &params, boolean_T useImaginary, ::coder::array<cell_wrap_21, 2U>
        &allLayers, ::coder::array<real_T, 1U> &allRoughs);
    }
  }
}

#endif

// End of code generation (processCustomFunction2.h)
