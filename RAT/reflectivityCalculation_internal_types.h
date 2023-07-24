//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation_internal_types.h
//
// Code generation for function 'reflectivityCalculation'
//
#ifndef REFLECTIVITYCALCULATION_INTERNAL_TYPES_H
#define REFLECTIVITYCALCULATION_INTERNAL_TYPES_H

// Include files
#include "reflectivityCalculation_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Type Definitions
namespace RAT
{
  struct cell_wrap_15
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_19
  {
    ::coder::array<cell_wrap_0, 2U> f1;
    ::coder::array<cell_wrap_15, 2U> f2;
    ::coder::array<cell_wrap_0, 2U> f3;
    ::coder::array<cell_wrap_15, 2U> f4;
    ::coder::array<cell_wrap_15, 2U> f5;
    ::coder::array<cell_wrap_15, 1U> f6;
    ::coder::array<cell_wrap_5, 2U> f7;
    ::coder::array<cell_wrap_6, 2U> f8;
    ::coder::array<cell_wrap_6, 2U> f9;
    ::coder::array<cell_wrap_6, 2U> f10;
    ::coder::array<cell_wrap_6, 2U> f11;
    ::coder::array<cell_wrap_6, 2U> f12;
    ::coder::array<cell_wrap_6, 2U> f13;
    ::coder::array<cell_wrap_7, 2U> f14;
    ::coder::array<cell_wrap_5, 2U> f15;
    ::coder::array<cell_wrap_5, 2U> f16;
    ::coder::array<cell_wrap_15, 2U> f17;
    ::coder::array<cell_wrap_0, 2U> f18;
    ::coder::array<cell_wrap_15, 2U> f19;
    ::coder::array<cell_wrap_6, 2U> f20;
  };

  struct cell_wrap_20
  {
    ::coder::bounded_array<real_T, 10U, 2U> f1;
  };

  struct cell_24
  {
    ::coder::array<real_T, 2U> f1;
  };
}

#endif

// End of code generation (reflectivityCalculation_internal_types.h)
