//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// parallelContrasts.h
//
// Code generation for function 'parallelContrasts'
//
#ifndef PARALLELCONTRASTS_H
#define PARALLELCONTRASTS_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_14;
}

// Function Declarations
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace standardLayers
    {
      void c_parallelContrasts(const struct5_T *problemDef, const cell_14
        *problemDefCells, const struct2_T *controls, ::coder::array<real_T, 1U>
        &outSsubs, ::coder::array<real_T, 1U> &backgroundParams, ::coder::array<
        real_T, 1U> &qzshifts, ::coder::array<real_T, 1U> &scalefactors, ::coder::
        array<real_T, 1U> &bulkIns, ::coder::array<real_T, 1U> &bulkOuts, ::
        coder::array<real_T, 1U> &resolutionParams, ::coder::array<real_T, 1U>
        &chis, ::coder::array<cell_wrap_11, 1U> &reflectivity, ::coder::array<
        cell_wrap_11, 1U> &simulation, ::coder::array<cell_wrap_8, 1U>
        &shiftedData, ::coder::array<cell_wrap_8, 1U> &layerSlds, ::coder::array<
        cell_wrap_8, 1U> &sldProfiles, ::coder::array<cell_wrap_8, 1U>
        &allLayers, ::coder::array<real_T, 1U> &allRoughs);
    }
  }
}

#endif

// End of code generation (parallelContrasts.h)
