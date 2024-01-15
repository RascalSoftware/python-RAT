//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// repmat.h
//
// Code generation for function 'repmat'
//
#ifndef REPMAT_H
#define REPMAT_H

// Include files
#include "RATMain_internal_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_wrap_9;
}

// Function Declarations
namespace RAT
{
  namespace coder
  {
    void b_repmat(real_T varargin_2, ::coder::array<char_T, 2U> &b);
    void repmat(cell_wrap_9 b[6]);
    void repmat(real_T varargin_1, ::coder::array<k_struct_T, 1U> &b);
    void repmat(const ::coder::array<real_T, 2U> &a, real_T varargin_1, ::coder::
                array<real_T, 2U> &b);
    void repmat(real_T varargin_2, ::coder::array<char_T, 2U> &b);
    void repmat(const real_T a[2], real_T varargin_1, ::coder::array<real_T, 2U>
                &b);
  }
}

#endif

// End of code generation (repmat.h)
