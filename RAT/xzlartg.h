//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xzlartg.h
//
// Code generation for function 'xzlartg'
//
#ifndef XZLARTG_H
#define XZLARTG_H

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
        void xzlartg(const creal_T f, const creal_T g, real_T *cs, creal_T *sn);
        void xzlartg(const creal_T f, const creal_T g, real_T *cs, creal_T *sn,
                     creal_T *r);
      }
    }
  }
}

#endif

// End of code generation (xzlartg.h)
