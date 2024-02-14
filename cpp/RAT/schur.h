//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// schur.h
//
// Code generation for function 'schur'
//
#ifndef SCHUR_H
#define SCHUR_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    void schur(const ::coder::array<real_T, 2U> &A, ::coder::array<real_T, 2U>
               &V, ::coder::array<real_T, 2U> &T);
  }
}

#endif

// End of code generation (schur.h)
