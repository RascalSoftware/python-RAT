//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFStandardLayersParallelPoints.h
//
// Code generation for function 'standardTFStandardLayersParallelPoints'
//
#ifndef STANDARDTFSTANDARDLAYERSPARALLELPOINTS_H
#define STANDARDTFSTANDARDLAYERSPARALLELPOINTS_H

// Include files
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void standardTFStandardLayersParallelPoints(const struct0_T *problemDef, const
    cell_19 *problemDefCells, const struct2_T *controls, ::coder::array<real_T,
    1U> &outSsubs, ::coder::array<real_T, 1U> &backgs, ::coder::array<real_T, 1U>
    &qshifts, ::coder::array<real_T, 1U> &sfs, ::coder::array<real_T, 1U> &nbas,
    ::coder::array<real_T, 1U> &nbss, ::coder::array<real_T, 1U> &resols, ::
    coder::array<real_T, 1U> &chis, ::coder::array<cell_wrap_9, 1U>
    &reflectivity, ::coder::array<cell_wrap_9, 1U> &Simulation, ::coder::array<
    cell_wrap_15, 1U> &shifted_data, ::coder::array<cell_wrap_15, 1U> &layerSlds,
    ::coder::array<cell_wrap_15, 1U> &sldProfiles, ::coder::array<cell_wrap_15,
    1U> &allLayers, ::coder::array<real_T, 1U> &allRoughs);
}

#endif

// End of code generation (standardTFStandardLayersParallelPoints.h)
