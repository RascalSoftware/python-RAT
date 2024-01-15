//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eig.h
//
// Code generation for function 'eig'
//
#ifndef EIG_H
#define EIG_H

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
    void eig(const ::coder::array<real_T, 2U> &A, ::coder::array<creal_T, 2U> &V,
             ::coder::array<creal_T, 2U> &D);
  }
}

#endif

// End of code generation (eig.h)
