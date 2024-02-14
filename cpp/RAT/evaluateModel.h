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
  struct struct13_T;
  struct c_struct_T;
  struct cell_11;
  struct struct2_T;
}

// Function Declarations
namespace RAT
{
  void evaluateModel(const ::coder::array<real_T, 2U> &x, const struct13_T
                     *DREAMPar, const c_struct_T *ratInputs_problemStruct, const
                     cell_11 *ratInputs_problemCells, const struct2_T
                     *ratInputs_controls, ::coder::array<real_T, 2U> &fx);
}

#endif

// End of code generation (evaluateModel.h)
