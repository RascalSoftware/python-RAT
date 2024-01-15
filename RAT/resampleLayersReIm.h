//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// resampleLayersReIm.h
//
// Code generation for function 'resampleLayersReIm'
//
#ifndef RESAMPLELAYERSREIM_H
#define RESAMPLELAYERSREIM_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void b_resampleLayersReIm(const ::coder::array<real_T, 2U> &sldProfile, const ::
    coder::array<real_T, 2U> &sldProfileIm, const real_T resamPars[2], ::coder::
    array<real_T, 2U> &newSLD);
  void c_resampleLayersReIm(const ::coder::array<real_T, 2U> &sldProfile, const ::
    coder::array<real_T, 2U> &sldProfileIm, const real_T resamPars[2], ::coder::
    array<real_T, 2U> &newSLD);
}

#endif

// End of code generation (resampleLayersReIm.h)
