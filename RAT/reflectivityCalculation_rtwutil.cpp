//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation_rtwutil.cpp
//
// Code generation for function 'reflectivityCalculation_rtwutil'
//

// Include files
#include "reflectivityCalculation_rtwutil.h"
#include "rt_nonfinite.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  real_T rt_hypotd_snf(real_T u0, real_T u1)
  {
    real_T a;
    real_T b;
    real_T y;
    a = std::abs(u0);
    b = std::abs(u1);
    if (a < b) {
      a /= b;
      y = b * std::sqrt(a * a + 1.0);
    } else if (a > b) {
      b /= a;
      y = a * std::sqrt(b * b + 1.0);
    } else if (std::isnan(b)) {
      y = rtNaN;
    } else {
      y = a * 1.4142135623730951;
    }

    return y;
  }
}

// End of code generation (reflectivityCalculation_rtwutil.cpp)
