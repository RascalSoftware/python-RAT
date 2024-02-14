//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// norm.h
//
// Code generation for function 'norm'
//
#ifndef NORM_H
#define NORM_H

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
    real_T b_norm(const ::coder::array<real_T, 2U> &x);
    real_T b_norm(const ::coder::array<real_T, 1U> &x);
    real_T c_norm(const ::coder::array<real_T, 2U> &x);
  }
}

#endif

// End of code generation (norm.h)
