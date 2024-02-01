//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// packParams.h
//
// Code generation for function 'packParams'
//
#ifndef PACKPARAMS_H
#define PACKPARAMS_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct c_struct_T;
}

// Function Declarations
namespace RAT
{
  void packParams(c_struct_T *problemStruct, const ::coder::array<cell_wrap_1,
                  2U> &problemCells_f7, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f8, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f9, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f10, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f11, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f12, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f13, const ::coder::array<cell_wrap_1, 2U>
                  &problemCells_f20, const struct1_T *limits, const struct3_T
                  *checks, ::coder::array<cell_wrap_1, 1U> &fitNames);
}

#endif

// End of code generation (packParams.h)
