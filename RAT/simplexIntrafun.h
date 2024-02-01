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
  struct c_struct_T;
  struct k_struct_T;
  struct d_struct_T;
}

// Function Declarations
namespace RAT
{
  void simplexIntrafun(const ::coder::array<real_T, 1U> &x, const c_struct_T
                       *problemStruct, const ::coder::array<cell_wrap_2, 2U>
                       &problemCells_f1, const ::coder::array<cell_wrap_8, 2U>
                       &problemCells_f2, const ::coder::array<cell_wrap_2, 2U>
                       &problemCells_f3, const ::coder::array<cell_wrap_2, 2U>
                       &problemCells_f4, const ::coder::array<cell_wrap_8, 2U>
                       &problemCells_f5, const ::coder::array<cell_wrap_8, 1U>
                       &problemCells_f6, const ::coder::array<cell_wrap_1, 2U>
                       &problemCells_f14, const ::coder::array<cell_wrap_8, 2U>
                       &problemCells_f19, const struct2_T *controls, const
                       k_struct_T *params, real_T *fval, d_struct_T
                       *b_problemStruct, cell_wrap_9 result[6]);
}

#endif

// End of code generation (simplexIntrafun.h)
