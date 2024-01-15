//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xzggbak.h
//
// Code generation for function 'xzggbak'
//
#ifndef XZGGBAK_H
#define XZGGBAK_H

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
    namespace internal
    {
      namespace reflapack
      {
        void xzggbak(::coder::array<creal_T, 2U> &V, int32_T ilo, int32_T ihi,
                     const ::coder::array<int32_T, 1U> &rscale);
      }
    }
  }
}

#endif

// End of code generation (xzggbak.h)
