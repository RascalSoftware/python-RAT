//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// blockedSummation.h
//
// Code generation for function 'blockedSummation'
//
#ifndef BLOCKEDSUMMATION_H
#define BLOCKEDSUMMATION_H

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
    void blockedSummation(const ::coder::array<real_T, 2U> &x, int32_T vlen, ::
                          coder::array<real_T, 1U> &y);
    real_T nestedIter(const ::coder::array<real_T, 1U> &x, int32_T vlen);
  }
}

#endif

// End of code generation (blockedSummation.h)
