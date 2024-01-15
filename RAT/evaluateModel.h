//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// evaluateModel.h
//
// Code generation for function 'evaluateModel'
//
#ifndef EVALUATEMODEL_H
#define EVALUATEMODEL_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct struct14_T;
  struct struct5_T;
  struct cell_14;
  struct struct2_T;
}

// Function Declarations
namespace RAT
{
  void evaluateModel(const ::coder::array<real_T, 2U> &x, const struct14_T
                     *DREAMPar, const struct5_T *ratInputs_problemDef, const
                     cell_14 *ratInputs_problemDefCells, const struct2_T
                     *ratInputs_controls, ::coder::array<real_T, 2U> &fx);
}

#endif

// End of code generation (evaluateModel.h)
