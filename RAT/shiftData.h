//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// shiftData.h
//
// Code generation for function 'shiftData'
//
#ifndef SHIFTDATA_H
#define SHIFTDATA_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void shiftData(real_T scalefac, real_T horshift, real_T dataPresent, ::coder::
                 array<real_T, 2U> &data, const real_T dataLimits[2], const
                 real_T simLimits_data[], ::coder::array<real_T, 2U>
                 &shifted_data);
}

#endif

// End of code generation (shiftData.h)
