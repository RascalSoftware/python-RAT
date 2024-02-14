//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// allocateLayersForContrast.h
//
// Code generation for function 'allocateLayersForContrast'
//
#ifndef ALLOCATELAYERSFORCONTRAST_H
#define ALLOCATELAYERSFORCONTRAST_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void allocateLayersForContrast(const ::coder::array<real_T, 2U>
    &contrastLayers, const ::coder::array<cell_wrap_22, 2U>
    &outParameterisedLayers, boolean_T useImaginary, real_T
    thisContrastLayers_data[], int32_T thisContrastLayers_size[2]);
}

#endif

// End of code generation (allocateLayersForContrast.h)
