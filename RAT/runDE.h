//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runDE.h
//
// Code generation for function 'runDE'
//
#ifndef RUNDE_H
#define RUNDE_H

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
  struct l_struct_T;
  struct cell_11;
  struct d_struct_T;
}

// Function Declarations
namespace RAT
{
  l_struct_T intrafun(const ::coder::array<real_T, 2U> &p, c_struct_T
                      *problemStruct, const char_T controls_parallel_data[],
                      const int32_T controls_parallel_size[2], const real_T
                      controls_resamPars[2], boolean_T controls_calcSldDuringFit,
                      const struct3_T *controls_checks, const ::coder::array<
                      cell_wrap_2, 2U> &problemCells_f1, const ::coder::array<
                      cell_wrap_8, 2U> &problemCells_f2, const ::coder::array<
                      cell_wrap_2, 2U> &problemCells_f3, const ::coder::array<
                      cell_wrap_2, 2U> &problemCells_f4, const ::coder::array<
                      cell_wrap_8, 2U> &problemCells_f5, const ::coder::array<
                      cell_wrap_8, 1U> &problemCells_f6, const ::coder::array<
                      cell_wrap_1, 2U> &problemCells_f14, const ::coder::array<
                      cell_wrap_8, 2U> &problemCells_f19);
  void runDE(c_struct_T *problemStruct, const cell_11 *problemCells, const
             struct1_T *problemLimits, const struct2_T *controls, d_struct_T
             *contrastParams, cell_wrap_9 result[6]);
}

#endif

// End of code generation (runDE.h)
