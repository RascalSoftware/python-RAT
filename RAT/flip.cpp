//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// flip.cpp
//
// Code generation for function 'flip'
//

// Include files
#include "flip.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void flip(::coder::array<real_T, 1U> &x)
    {
      if (x.size(0) > 1) {
        int32_T i;
        i = x.size(0) >> 1;
        for (int32_T k{0}; k < i; k++) {
          real_T tmp;
          int16_T i1;
          i1 = static_cast<int16_T>(x.size(0) - k);
          tmp = x[k];
          x[k] = x[i1 - 1];
          x[i1 - 1] = tmp;
        }
      }
    }
  }
}

// End of code generation (flip.cpp)
