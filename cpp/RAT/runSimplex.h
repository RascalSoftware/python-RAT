//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runSimplex.h
//
// Code generation for function 'runSimplex'
//
#ifndef RUNSIMPLEX_H
#define RUNSIMPLEX_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct c_struct_T;
  struct cell_11;
  struct struct1_T;
  struct struct2_T;
  struct d_struct_T;
  struct cell_wrap_9;
}

// Function Declarations
namespace RAT
{
  void runSimplex(c_struct_T *problemStruct, const cell_11 *problemCells, const
                  struct1_T *problemLimits, const struct2_T *controls,
                  d_struct_T *contrastParams, cell_wrap_9 result[6]);
}

#endif

// End of code generation (runSimplex.h)
