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
  struct k_struct_T;
  struct cell_14;
  struct struct_T;
}

// Function Declarations
namespace RAT
{
  k_struct_T intrafun(const ::coder::array<real_T, 2U> &p, struct5_T *problemDef,
                      const char_T controls_parallel_data[], const int32_T
                      controls_parallel_size[2], const real_T
                      controls_resamPars[2], boolean_T controls_calcSldDuringFit,
                      const struct3_T *controls_checks, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f1, const ::coder::array<
                      cell_wrap_8, 2U> &problemDefCells_f2, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f3, const ::coder::array<
                      cell_wrap_2, 2U> &problemDefCells_f4, const ::coder::array<
                      cell_wrap_8, 2U> &problemDefCells_f5, const ::coder::array<
                      cell_wrap_8, 1U> &problemDefCells_f6, const ::coder::array<
                      cell_wrap_1, 2U> &problemDefCells_f14, const ::coder::
                      array<cell_wrap_8, 2U> &problemDefCells_f19);
  void runDE(struct5_T *problemDef, const cell_14 *problemDefCells, const
             struct1_T *problemDefLimits, const struct2_T *controls, struct_T
             *problem, cell_wrap_9 result[6]);
}

#endif

// End of code generation (runDE.h)
