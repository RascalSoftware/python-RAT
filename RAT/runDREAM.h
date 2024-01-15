//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runDREAM.h
//
// Code generation for function 'runDREAM'
//
#ifndef RUNDREAM_H
#define RUNDREAM_H

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
  struct struct4_T;
  struct struct_T;
  struct cell_wrap_9;
  struct d_struct_T;
}

// Function Declarations
namespace RAT
{
  void runDREAM(const struct5_T *problemDef, const cell_14 *problemDefCells,
                const struct1_T *problemDefLimits, const struct2_T *controls,
                const struct4_T *priors, struct5_T *outProblemDef, struct_T
                *outProblem, cell_wrap_9 result[6], d_struct_T *bayesResults);
}

#endif

// End of code generation (runDREAM.h)
