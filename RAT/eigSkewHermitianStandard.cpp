//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eigSkewHermitianStandard.cpp
//
// Code generation for function 'eigSkewHermitianStandard'
//

// Include files
#include "eigSkewHermitianStandard.h"
#include "eigRealSkewSymmetricStandard.h"
#include "rt_nonfinite.h"
#include "schur.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void eigSkewHermitianStandard(const ::coder::array<real_T, 2U> &A, ::coder::
      array<creal_T, 2U> &V, ::coder::array<creal_T, 2U> &D)
    {
      ::coder::array<real_T, 2U> T;
      ::coder::array<real_T, 2U> U;
      schur(A, U, T);
      extractEigenValues(T, D);
      extractEigenVectors(U, T, V);
    }
  }
}

// End of code generation (eigSkewHermitianStandard.cpp)
