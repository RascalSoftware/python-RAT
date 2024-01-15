//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// norm.cpp
//
// Code generation for function 'norm'
//

// Include files
#include "norm.h"
#include "rt_nonfinite.h"
#include "xnrm2.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    static real_T mat1norm(const ::coder::array<real_T, 2U> &x);
    static real_T vecpnorm(const ::coder::array<real_T, 2U> &x);
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    static real_T mat1norm(const ::coder::array<real_T, 2U> &x)
    {
      real_T y;
      int32_T j;
      boolean_T exitg1;
      y = 0.0;
      j = 0;
      exitg1 = false;
      while ((!exitg1) && (j <= x.size(1) - 1)) {
        real_T s;
        int32_T i;
        s = 0.0;
        i = x.size(0);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          s += std::abs(x[b_i + x.size(0) * j]);
        }

        if (std::isnan(s)) {
          y = rtNaN;
          exitg1 = true;
        } else {
          if (s > y) {
            y = s;
          }

          j++;
        }
      }

      return y;
    }

    static real_T vecpnorm(const ::coder::array<real_T, 2U> &x)
    {
      real_T y;
      int32_T i;
      y = 0.0;
      i = x.size(0) * x.size(1);
      for (int32_T k{0}; k < i; k++) {
        y += std::abs(x[k]);
      }

      return y;
    }

    real_T b_norm(const ::coder::array<real_T, 2U> &x)
    {
      real_T y;
      boolean_T MATRIX_INPUT_AND_P_IS_ONE;
      boolean_T VECTOR_INPUT_AND_P_IS_NUMERIC;
      VECTOR_INPUT_AND_P_IS_NUMERIC = false;
      MATRIX_INPUT_AND_P_IS_ONE = false;
      if ((x.size(0) == 1) || (x.size(1) == 1)) {
        VECTOR_INPUT_AND_P_IS_NUMERIC = true;
      } else {
        MATRIX_INPUT_AND_P_IS_ONE = true;
      }

      if ((x.size(0) == 0) || (x.size(1) == 0)) {
        y = 0.0;
      } else if (MATRIX_INPUT_AND_P_IS_ONE) {
        y = mat1norm(x);
      } else if (VECTOR_INPUT_AND_P_IS_NUMERIC) {
        y = vecpnorm(x);
      } else {
        y = rtNaN;
      }

      return y;
    }

    real_T b_norm(const ::coder::array<real_T, 1U> &x)
    {
      real_T y;
      int32_T i;
      y = 0.0;
      i = x.size(0);
      for (int32_T k{0}; k < i; k++) {
        y += std::abs(x[k]);
      }

      return y;
    }

    real_T c_norm(const ::coder::array<real_T, 2U> &x)
    {
      real_T y;
      if (x.size(1) == 0) {
        y = 0.0;
      } else {
        y = internal::blas::xnrm2(x.size(1), x);
      }

      return y;
    }
  }
}

// End of code generation (norm.cpp)
