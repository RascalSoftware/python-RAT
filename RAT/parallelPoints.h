//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// parallelPoints.h
//
// Code generation for function 'parallelPoints'
//
#ifndef PARALLELPOINTS_H
#define PARALLELPOINTS_H

// Include files
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace standardLayers
    {
      void parallelPoints(const c_struct_T *problemStruct, const cell_11
                          *problemCells, const struct2_T *controls, ::coder::
                          array<real_T, 1U> &outSsubs, ::coder::array<real_T, 1U>
                          &backgroundParams, ::coder::array<real_T, 1U>
                          &qzshifts, ::coder::array<real_T, 1U> &scalefactors, ::
                          coder::array<real_T, 1U> &bulkIns, ::coder::array<
                          real_T, 1U> &bulkOuts, ::coder::array<real_T, 1U>
                          &resolutionParams, ::coder::array<real_T, 1U> &chis, ::
                          coder::array<cell_wrap_20, 1U> &reflectivity, ::coder::
                          array<cell_wrap_20, 1U> &simulation, ::coder::array<
                          cell_wrap_8, 1U> &shiftedData, ::coder::array<
                          cell_wrap_8, 1U> &layerSlds, ::coder::array<
                          cell_wrap_8, 1U> &sldProfiles, ::coder::array<
                          cell_wrap_8, 1U> &allLayers, ::coder::array<real_T, 1U>
                          &allRoughs);
    }
  }
}

#endif

// End of code generation (parallelPoints.h)
