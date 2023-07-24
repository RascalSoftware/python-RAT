//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// backSort.h
//
// Code generation for function 'backSort'
//
#ifndef BACKSORT_H
#define BACKSORT_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void backSort(real_T cBacks, real_T cShifts, real_T cScales, real_T cNbas,
                real_T cNbss, real_T cRes, const ::coder::array<real_T, 2U>
                &backs, const ::coder::array<real_T, 2U> &shifts, const ::coder::
                array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba,
                const ::coder::array<real_T, 2U> &nbs, const ::coder::array<
                real_T, 2U> &res, real_T *backg, real_T *qshift, real_T *b_sf,
                real_T *b_nba, real_T *b_nbs, real_T *resol);
}

#endif

// End of code generation (backSort.h)
