//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// unpackParams.h
//
// Code generation for function 'unpackParams'
//
#ifndef UNPACKPARAMS_H
#define UNPACKPARAMS_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct c_struct_T;
}

// Function Declarations
namespace RAT
{
  void unpackParams(c_struct_T *problemStruct, const ::coder::array<real_T, 2U>
                    &controls_checks_fitParam, const ::coder::array<real_T, 2U>
                    &controls_checks_fitBackgroundParam, const ::coder::array<
                    real_T, 2U> &controls_checks_fitQzshift, const ::coder::
                    array<real_T, 2U> &controls_checks_fitScalefactor, const ::
                    coder::array<real_T, 2U> &controls_checks_fitBulkIn, const ::
                    coder::array<real_T, 2U> &controls_checks_fitBulkOut, const ::
                    coder::array<real_T, 2U> &controls_checks_fitResolutionParam,
                    const ::coder::array<real_T, 2U>
                    &controls_checks_fitDomainRatio);
}

#endif

// End of code generation (unpackParams.h)
