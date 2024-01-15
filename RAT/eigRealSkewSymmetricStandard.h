//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eigRealSkewSymmetricStandard.h
//
// Code generation for function 'eigRealSkewSymmetricStandard'
//
#ifndef EIGREALSKEWSYMMETRICSTANDARD_H
#define EIGREALSKEWSYMMETRICSTANDARD_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    void extractEigenValues(const ::coder::array<real_T, 2U> &T, ::coder::array<
      creal_T, 2U> &D);
    void extractEigenVectors(const ::coder::array<real_T, 2U> &U, const ::coder::
      array<real_T, 2U> &T, ::coder::array<creal_T, 2U> &V);
  }
}

#endif

// End of code generation (eigRealSkewSymmetricStandard.h)
