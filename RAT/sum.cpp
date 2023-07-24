//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// sum.cpp
//
// Code generation for function 'sum'
//

// Include files
#include "sum.h"
#include "blockedSummation.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    real_T sum(const ::coder::array<real_T, 1U> &x)
    {
      real_T y;
      if (x.size(0) == 0) {
        y = 0.0;
      } else {
        y = nestedIter(x, x.size(0));
      }

      return y;
    }
  }
}

// End of code generation (sum.cpp)
