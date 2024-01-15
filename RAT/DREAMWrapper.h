//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// DREAMWrapper.h
//
// Code generation for function 'DREAMWrapper'
//
#ifndef DREAMWRAPPER_H
#define DREAMWRAPPER_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct struct5_T;
  struct cell_14;
  struct struct2_T;
}

// Function Declarations
namespace RAT
{
  real_T DREAMWrapper(const ::coder::array<real_T, 2U> &pars, const struct5_T
                      *ratInputs_problemDef, const cell_14
                      *ratInputs_problemDefCells, const struct2_T
                      *ratInputs_controls);
}

#endif

// End of code generation (DREAMWrapper.h)
