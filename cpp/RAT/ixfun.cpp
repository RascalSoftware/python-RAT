//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// ixfun.cpp
//
// Code generation for function 'ixfun'
//

// Include files
#include "ixfun.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      void expand_max(const ::coder::array<real_T, 1U> &a, const ::coder::array<
                      real_T, 1U> &b, ::coder::array<real_T, 1U> &c)
      {
        int32_T csz_idx_0;
        int32_T k;
        int32_T u0;
        u0 = a.size(0);
        k = b.size(0);
        if (u0 <= k) {
          k = u0;
        }

        if (b.size(0) == 1) {
          csz_idx_0 = a.size(0);
        } else if (a.size(0) == 1) {
          csz_idx_0 = b.size(0);
        } else {
          csz_idx_0 = k;
        }

        u0 = a.size(0);
        k = b.size(0);
        if (u0 <= k) {
          k = u0;
        }

        if (b.size(0) == 1) {
          k = a.size(0);
        } else if (a.size(0) == 1) {
          k = b.size(0);
        }

        c.set_size(k);
        if (csz_idx_0 != 0) {
          boolean_T b1;
          boolean_T b_b;
          b_b = (a.size(0) != 1);
          b1 = (b.size(0) != 1);
          u0 = csz_idx_0 - 1;
          for (k = 0; k <= u0; k++) {
            c[k] = std::fmax(a[b_b * k], b[b1 * k]);
          }
        }
      }
    }
  }
}

// End of code generation (ixfun.cpp)
