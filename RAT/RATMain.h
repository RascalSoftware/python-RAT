//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// RATMain.h
//
// Code generation for function 'RATMain'
//
#ifndef RATMAIN_H
#define RATMAIN_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  extern void RATMain(const struct0_T *problemDef, const cell_7 *problemDefCells,
                      const struct1_T *problemDefLimits, struct2_T *controls,
                      const struct4_T *priors, struct5_T *outProblemDef,
                      struct6_T *problem, cell_wrap_9 results[6], struct8_T
                      *bayesResults);
}

#endif

// End of code generation (RATMain.h)
