//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xdlanv2.h
//
// Code generation for function 'xdlanv2'
//
#ifndef XDLANV2_H
#define XDLANV2_H

// Include files
#include "rtwtypes.h"
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
        void xdlanv2(real_T *a, real_T *b, real_T *c, real_T *d, real_T *rt1r,
                     real_T *rt1i, real_T *rt2r, real_T *rt2i, real_T *cs,
                     real_T *sn);
      }
    }
  }
}

#endif

// End of code generation (xdlanv2.h)
