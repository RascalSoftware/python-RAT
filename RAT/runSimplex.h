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
  struct struct5_T;
  struct cell_14;
  struct struct1_T;
  struct struct2_T;
  struct struct_T;
  struct cell_wrap_9;
}

// Function Declarations
namespace RAT
{
  void runSimplex(struct5_T *problemDef, const cell_14 *problemDefCells, const
                  struct1_T *problemDefLimits, const struct2_T *controls,
                  struct_T *problem, cell_wrap_9 result[6]);
}

#endif

// End of code generation (runSimplex.h)
