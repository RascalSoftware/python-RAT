//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// adaptive.h
//
// Code generation for function 'adaptive'
//
#ifndef ADAPTIVE_H
#define ADAPTIVE_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_24;
}

// Function Declarations
namespace RAT
{
  void adaptive(const real_T sldProfile_data[], const int32_T sldProfile_size[2],
                const real_T startDomain[2], real_T minAngle, real_T nPoints,
                cell_24 *out);
  void adaptive(const ::coder::array<real_T, 2U> &sldProfile, const real_T
                startDomain[2], real_T minAngle, real_T nPoints, cell_24 *out);
}

#endif

// End of code generation (adaptive.h)
