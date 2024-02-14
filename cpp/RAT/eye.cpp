//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eye.cpp
//
// Code generation for function 'eye'
//

// Include files
#include "eye.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void eye(int32_T varargin_1, ::coder::array<creal_T, 2U> &b_I)
    {
      b_I.set_size(varargin_1, varargin_1);
      for (int32_T k{0}; k < varargin_1; k++) {
        for (int32_T i{0}; i < varargin_1; i++) {
          b_I[i + b_I.size(0) * k].re = 0.0;
          b_I[i + b_I.size(0) * k].im = 0.0;
        }
      }

      if (varargin_1 > 0) {
        for (int32_T k{0}; k < varargin_1; k++) {
          b_I[k + b_I.size(0) * k].re = 1.0;
          b_I[k + b_I.size(0) * k].im = 0.0;
        }
      }
    }
  }
}

// End of code generation (eye.cpp)
