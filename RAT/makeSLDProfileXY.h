//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// makeSLDProfileXY.h
//
// Code generation for function 'makeSLDProfileXY'
//
#ifndef MAKESLDPROFILEXY_H
#define MAKESLDPROFILEXY_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void makeSLDProfileXY(real_T nbair, real_T nbsub, real_T ssub, const ::coder::
                        array<real_T, 2U> &layers, real_T numberOfLayers, real_T
                        nrepeats, ::coder::array<real_T, 2U> &out);
  void makeSLDProfileXY(real_T ssub, const ::coder::array<real_T, 2U> &layers,
                        real_T numberOfLayers, real_T nrepeats, ::coder::array<
                        real_T, 2U> &out);
}

#endif

// End of code generation (makeSLDProfileXY.h)
