//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// resampleLayers.h
//
// Code generation for function 'resampleLayers'
//
#ifndef RESAMPLELAYERS_H
#define RESAMPLELAYERS_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void resampleLayers(const ::coder::array<real_T, 2U> &sldProfile, const real_T
                      resamPars[2], ::coder::array<real_T, 2U> &newSLD);
}

#endif

// End of code generation (resampleLayers.h)
