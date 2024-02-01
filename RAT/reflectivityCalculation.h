//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation.h
//
// Code generation for function 'reflectivityCalculation'
//
#ifndef REFLECTIVITYCALCULATION_H
#define REFLECTIVITYCALCULATION_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct c_struct_T;
  struct cell_11;
  struct struct2_T;
  struct d_struct_T;
  struct cell_wrap_9;
}

// Function Declarations
namespace RAT
{
  void reflectivityCalculation(const c_struct_T *problemStruct, const cell_11
    *problemCells, const struct2_T *controls, d_struct_T *contrastParams,
    cell_wrap_9 resultCells[6]);
}

#endif

// End of code generation (reflectivityCalculation.h)
