//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// applyBackgroundCorrection.h
//
// Code generation for function 'applyBackgroundCorrection'
//
#ifndef APPLYBACKGROUNDCORRECTION_H
#define APPLYBACKGROUNDCORRECTION_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void applyBackgroundCorrection(::coder::array<real_T, 2U> &reflect, ::coder::
    array<real_T, 2U> &Simul, ::coder::array<real_T, 2U> &shifted_dat, real_T
    backg, real_T backsType);
}

#endif

// End of code generation (applyBackgroundCorrection.h)
