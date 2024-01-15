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
  struct struct5_T;
  struct cell_14;
  struct struct2_T;
  struct struct_T;
  struct cell_13;
}

// Function Declarations
namespace RAT
{
  void reflectivityCalculation(const struct5_T *problemDef, const cell_14
    *problemDefCells, const struct2_T *controls, struct_T *problem, cell_13
    *result);
}

#endif

// End of code generation (reflectivityCalculation.h)
