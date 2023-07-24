//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// minOrMax.cpp
//
// Code generation for function 'minOrMax'
//

// Include files
#include "minOrMax.h"
#include "relop.h"
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
      real_T b_maximum(const real_T x[2])
      {
        real_T ex;
        if ((x[0] < x[1]) || (std::isnan(x[0]) && (!std::isnan(x[1])))) {
          ex = x[1];
        } else {
          ex = x[0];
        }

        return ex;
      }

      real_T maximum(const real_T x_data[])
      {
        real_T ex;
        int32_T idx;
        int32_T k;
        if (!std::isnan(x_data[0])) {
          idx = 1;
        } else {
          boolean_T exitg1;
          idx = 0;
          k = 2;
          exitg1 = false;
          while ((!exitg1) && (k < 102)) {
            if (!std::isnan(x_data[k - 1])) {
              idx = k;
              exitg1 = true;
            } else {
              k++;
            }
          }
        }

        if (idx == 0) {
          ex = x_data[0];
        } else {
          ex = x_data[idx - 1];
          idx++;
          for (k = idx; k < 102; k++) {
            real_T d;
            d = x_data[k - 1];
            if (ex < d) {
              ex = d;
            }
          }
        }

        return ex;
      }

      void maximum(const ::coder::array<real_T, 2U> &x, real_T ex[2])
      {
        int32_T m;
        m = x.size(0);
        for (int32_T j{0}; j < 2; j++) {
          ex[j] = x[x.size(0) * j];
          for (int32_T i{2}; i <= m; i++) {
            real_T d;
            d = x[(i + x.size(0) * j) - 1];
            if (relop(ex[j], d)) {
              ex[j] = d;
            }
          }
        }
      }

      void minimum(const ::coder::array<real_T, 2U> &x, real_T ex[2])
      {
        int32_T m;
        m = x.size(0);
        for (int32_T j{0}; j < 2; j++) {
          ex[j] = x[x.size(0) * j];
          for (int32_T i{2}; i <= m; i++) {
            real_T d;
            d = x[(i + x.size(0) * j) - 1];
            if (b_relop(ex[j], d)) {
              ex[j] = d;
            }
          }
        }
      }
    }
  }
}

// End of code generation (minOrMax.cpp)
