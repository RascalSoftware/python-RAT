//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// makeSLDProfiles.h
//
// Code generation for function 'makeSLDProfiles'
//
#ifndef MAKESLDPROFILES_H
#define MAKESLDPROFILES_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void makeSLDProfiles(real_T nbair, real_T nbsub, const ::coder::array<real_T,
                       2U> &sld, real_T ssub, const real_T repeats[2], ::coder::
                       array<real_T, 2U> &sldProfile);
  void makeSLDProfiles(const ::coder::array<real_T, 2U> &sld, real_T ssub, const
                       real_T repeats[2], ::coder::array<real_T, 2U> &sldProfile);
}

#endif

// End of code generation (makeSLDProfiles.h)
