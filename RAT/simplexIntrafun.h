//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// simplexIntrafun.h
//
// Code generation for function 'simplexIntrafun'
//
#ifndef SIMPLEXINTRAFUN_H
#define SIMPLEXINTRAFUN_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct j_struct_T;
  struct struct_T;
  struct cell_13;
}

// Function Declarations
namespace RAT
{
  void simplexIntrafun(const ::coder::array<real_T, 1U> &x, const struct5_T
                       *problemDef, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f1, const ::coder::array<cell_wrap_8, 2U>
                       &problemDefCells_f2, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f3, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f4, const ::coder::array<cell_wrap_8, 2U>
                       &problemDefCells_f5, const ::coder::array<cell_wrap_8, 1U>
                       &problemDefCells_f6, const ::coder::array<cell_wrap_1, 2U>
                       &problemDefCells_f14, const ::coder::array<cell_wrap_8,
                       2U> &problemDefCells_f19, const struct2_T *controls,
                       const j_struct_T *params, real_T *fval, struct_T
                       *b_problemDef, cell_13 *result);
}

#endif

// End of code generation (simplexIntrafun.h)
