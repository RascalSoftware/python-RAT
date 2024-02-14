//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// linspace.cpp
//
// Code generation for function 'linspace'
//

// Include files
#include "linspace.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void linspace(real_T d1, real_T d2, real_T n, ::coder::array<real_T, 2U> &y)
    {
      if (!(n >= 0.0)) {
        y.set_size(1, 0);
      } else {
        real_T delta1;
        delta1 = std::floor(n);
        y.set_size(1, static_cast<int32_T>(delta1));
        if (static_cast<int32_T>(delta1) >= 1) {
          y[static_cast<int32_T>(delta1) - 1] = d2;
          if (y.size(1) >= 2) {
            y[0] = d1;
            if (y.size(1) >= 3) {
              if (d1 == -d2) {
                int32_T i;
                delta1 = d2 / (static_cast<real_T>(y.size(1)) - 1.0);
                i = y.size(1) - 1;
                for (int32_T k{2}; k <= i; k++) {
                  y[k - 1] = static_cast<real_T>(((k << 1) - y.size(1)) - 1) *
                    delta1;
                }

                if ((y.size(1) & 1) == 1) {
                  y[y.size(1) >> 1] = 0.0;
                }
              } else if (((d1 < 0.0) != (d2 < 0.0)) && ((std::abs(d1) >
                           8.9884656743115785E+307) || (std::abs(d2) >
                           8.9884656743115785E+307))) {
                real_T delta2;
                int32_T i;
                delta1 = d1 / (static_cast<real_T>(y.size(1)) - 1.0);
                delta2 = d2 / (static_cast<real_T>(y.size(1)) - 1.0);
                i = y.size(1);
                for (int32_T k{0}; k <= i - 3; k++) {
                  y[k + 1] = (d1 + delta2 * (static_cast<real_T>(k) + 1.0)) -
                    delta1 * (static_cast<real_T>(k) + 1.0);
                }
              } else {
                int32_T i;
                delta1 = (d2 - d1) / (static_cast<real_T>(y.size(1)) - 1.0);
                i = y.size(1);
                for (int32_T k{0}; k <= i - 3; k++) {
                  y[k + 1] = d1 + (static_cast<real_T>(k) + 1.0) * delta1;
                }
              }
            }
          }
        }
      }
    }

    void linspace(real_T d1, real_T d2, real_T y[500])
    {
      y[499] = d2;
      y[0] = d1;
      if (d1 == -d2) {
        real_T delta1;
        delta1 = d2 / 499.0;
        for (int32_T k{0}; k < 498; k++) {
          y[k + 1] = (2.0 * (static_cast<real_T>(k) + 2.0) - 501.0) * delta1;
        }
      } else if (((d1 < 0.0) != (d2 < 0.0)) && ((std::abs(d1) >
                   8.9884656743115785E+307) || (std::abs(d2) >
                   8.9884656743115785E+307))) {
        real_T delta1;
        real_T delta2;
        delta1 = d1 / 499.0;
        delta2 = d2 / 499.0;
        for (int32_T k{0}; k < 498; k++) {
          y[k + 1] = (d1 + delta2 * (static_cast<real_T>(k) + 1.0)) - delta1 * (
            static_cast<real_T>(k) + 1.0);
        }
      } else {
        real_T delta1;
        delta1 = (d2 - d1) / 499.0;
        for (int32_T k{0}; k < 498; k++) {
          y[k + 1] = d1 + (static_cast<real_T>(k) + 1.0) * delta1;
        }
      }
    }
  }
}

// End of code generation (linspace.cpp)
