//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// deopt.h
//
// Code generation for function 'deopt'
//
#ifndef DEOPT_H
#define DEOPT_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct i_struct_T;
}

// Function Declarations
namespace RAT
{
  void deopt(const struct5_T *problem, const ::coder::array<cell_wrap_2, 2U>
             &problemDefCells_f1, const ::coder::array<cell_wrap_8, 2U>
             &problemDefCells_f2, const ::coder::array<cell_wrap_2, 2U>
             &problemDefCells_f3, const ::coder::array<cell_wrap_2, 2U>
             &problemDefCells_f4, const ::coder::array<cell_wrap_8, 2U>
             &problemDefCells_f5, const ::coder::array<cell_wrap_8, 1U>
             &problemDefCells_f6, const ::coder::array<cell_wrap_1, 2U>
             &problemDefCells_f14, const ::coder::array<cell_wrap_8, 2U>
             &problemDefCells_f19, const char_T controls_parallel_data[], const
             int32_T controls_parallel_size[2], const real_T controls_resamPars
             [2], boolean_T controls_calcSldDuringFit, const char_T
             controls_display_data[], const int32_T controls_display_size[2],
             const struct3_T *controls_checks, const i_struct_T *S_struct, ::
             coder::array<real_T, 2U> &FVr_bestmem);
}

#endif

// End of code generation (deopt.h)
