//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// vAllOrAny.h
//
// Code generation for function 'vAllOrAny'
//
#ifndef VALLORANY_H
#define VALLORANY_H

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
      void b_genloops(const ::coder::array<real_T, 2U> &x, boolean_T *p, int32_T
                      varargin_1, int32_T varargin_2);
      void c_genloops(const real_T x_data[], boolean_T *p);
      boolean_T vAllOrAny(const ::coder::array<real_T, 2U> &x);
    }
  }
}

#endif

// End of code generation (vAllOrAny.h)
