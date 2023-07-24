//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFStandardLayersReflectivityCalculation.h
//
// Code generation for function 'standardTFStandardLayersReflectivityCalculation'
//
#ifndef STANDARDTFSTANDARDLAYERSREFLECTIVITYCALCULATION_H
#define STANDARDTFSTANDARDLAYERSREFLECTIVITYCALCULATION_H

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
  void standardTFStandardLayersReflectivityCalculation(const struct0_T
    *problemDef, const cell_19 *problemDefCells, const struct2_T *controls,
    struct4_T *problem, ::coder::array<cell_wrap_9, 1U> &reflectivity, ::coder::
    array<cell_wrap_9, 1U> &Simulation, ::coder::array<cell_wrap_15, 1U>
    &shifted_data, ::coder::array<cell_wrap_15, 1U> &layerSlds, ::coder::array<
    cell_wrap_15, 1U> &sldProfiles, ::coder::array<cell_wrap_15, 1U> &allLayers);
}

#endif

// End of code generation (standardTFStandardLayersReflectivityCalculation.h)
