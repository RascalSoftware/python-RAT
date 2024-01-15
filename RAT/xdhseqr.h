//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xdhseqr.h
//
// Code generation for function 'xdhseqr'
//
#ifndef XDHSEQR_H
#define XDHSEQR_H

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
        int32_T eml_dlahqr(::coder::array<real_T, 2U> &h, ::coder::array<real_T,
                           2U> &z);
      }
    }
  }
}

#endif

// End of code generation (xdhseqr.h)
