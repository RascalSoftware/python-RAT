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
  struct c_struct_T;
  struct cell_11;
  struct struct2_T;
}

// Function Declarations
namespace RAT
{
  real_T DREAMWrapper(const ::coder::array<real_T, 2U> &pars, const c_struct_T
                      *ratInputs_problemStruct, const cell_11
                      *ratInputs_problemCells, const struct2_T
                      *ratInputs_controls);
}

#endif

// End of code generation (DREAMWrapper.h)
