//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nestedSampler.h
//
// Code generation for function 'nestedSampler'
//
#ifndef NESTEDSAMPLER_H
#define NESTEDSAMPLER_H

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
  void nestedSampler(const struct5_T *data_f1, const struct2_T *data_f2, const
                     cell_14 *data_f4, real_T Nlive, real_T Nmcmc, real_T
                     tolerance, const ::coder::array<real_T, 2U> &prior, real_T *
                     logZ, ::coder::array<real_T, 2U> &nest_samples, ::coder::
                     array<real_T, 2U> &post_samples, real_T *H);
}

#endif

// End of code generation (nestedSampler.h)
