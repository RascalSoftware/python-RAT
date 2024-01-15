//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runNestedSampler.h
//
// Code generation for function 'runNestedSampler'
//
#ifndef RUNNESTEDSAMPLER_H
#define RUNNESTEDSAMPLER_H

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
  struct struct8_T;
}

// Function Declarations
namespace RAT
{
  void runNestedSampler(struct5_T *problemDef, const cell_14 *problemDefCells,
                        const struct1_T *problemDefLimits, const struct2_T
                        *controls, const struct4_T *inPriors, struct_T
                        *outProblem, cell_wrap_9 result[6], struct8_T
                        *bayesResults);
}

#endif

// End of code generation (runNestedSampler.h)
