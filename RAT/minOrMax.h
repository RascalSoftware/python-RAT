//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// minOrMax.h
//
// Code generation for function 'minOrMax'
//
#ifndef MINORMAX_H
#define MINORMAX_H

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
    namespace internal
    {
      real_T b_maximum(const real_T x[2]);
      real_T maximum(const real_T x_data[]);
      void maximum(const ::coder::array<real_T, 2U> &x, real_T ex[2]);
      void minimum(const ::coder::array<real_T, 2U> &x, real_T ex[2]);
    }
  }
}

#endif

// End of code generation (minOrMax.h)
