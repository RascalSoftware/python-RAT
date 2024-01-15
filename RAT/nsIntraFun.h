//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nsIntraFun.h
//
// Code generation for function 'nsIntraFun'
//
#ifndef NSINTRAFUN_H
#define NSINTRAFUN_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct struct5_T;
  struct struct2_T;
  struct cell_14;
}

// Function Declarations
namespace RAT
{
  real_T nsIntraFun(const struct5_T *data_f1, const struct2_T *data_f2, const
                    cell_14 *data_f4, const ::coder::array<real_T, 2U> &p);
  real_T nsIntraFun(const struct5_T *data_f1, const struct2_T *data_f2, const
                    cell_14 *data_f4, const ::coder::array<real_T, 1U> &p);
}

#endif

// End of code generation (nsIntraFun.h)
