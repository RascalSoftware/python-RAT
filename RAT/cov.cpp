//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// cov.cpp
//
// Code generation for function 'cov'
//

// Include files
#include "cov.h"
#include "isrow.h"
#include "rt_nonfinite.h"
#include "xgemm.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  namespace coder
  {
    static void local_cov(::coder::array<real_T, 2U> &x, ::coder::array<real_T,
                          2U> &c);
    static real_T local_cov(::coder::array<real_T, 1U> &x);
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    static void local_cov(::coder::array<real_T, 2U> &x, ::coder::array<real_T,
                          2U> &c)
    {
      int32_T b_i;
      int32_T j;
      int32_T m;
      int32_T n;
      m = x.size(0);
      n = x.size(1);
      c.set_size(x.size(1), x.size(1));
      j = x.size(1);
      for (int32_T i{0}; i < j; i++) {
        b_i = x.size(1);
        for (int32_T i1{0}; i1 < b_i; i1++) {
          c[i1 + c.size(0) * i] = 0.0;
        }
      }

      if ((x.size(0) == 0) || (x.size(1) == 0)) {
        c.set_size(x.size(1), x.size(1));
        j = x.size(1);
        for (int32_T i{0}; i < j; i++) {
          b_i = x.size(1);
          for (int32_T i1{0}; i1 < b_i; i1++) {
            c[i1 + c.size(0) * i] = rtNaN;
          }
        }
      } else if (x.size(0) >= 2) {
        for (j = 0; j < n; j++) {
          real_T muj;
          muj = 0.0;
          for (b_i = 0; b_i < m; b_i++) {
            muj += x[b_i + x.size(0) * j];
          }

          muj /= static_cast<real_T>(m);
          for (b_i = 0; b_i < m; b_i++) {
            x[b_i + x.size(0) * j] = x[b_i + x.size(0) * j] - muj;
          }
        }

        c.set_size(x.size(1), x.size(1));
        j = x.size(1);
        b_i = x.size(1);
        for (int32_T i{0}; i < j; i++) {
          for (int32_T i1{0}; i1 < b_i; i1++) {
            c[i1 + c.size(0) * i] = 0.0;
          }
        }

        internal::blas::xgemm(x.size(1), x.size(1), x.size(0), 1.0 / (
          static_cast<real_T>(x.size(0)) - 1.0), x, x.size(0), x, x.size(0), c,
                              x.size(1));
      }
    }

    static real_T local_cov(::coder::array<real_T, 1U> &x)
    {
      real_T c;
      int32_T m;
      m = x.size(0) - 1;
      c = 0.0;
      if (x.size(0) == 0) {
        c = rtNaN;
      } else if (x.size(0) >= 2) {
        real_T muj;
        muj = 0.0;
        for (int32_T i{0}; i <= m; i++) {
          muj += x[i];
        }

        muj /= static_cast<real_T>(x.size(0));
        for (int32_T i{0}; i <= m; i++) {
          x[i] = x[i] - muj;
        }

        c = internal::blas::xgemm(x.size(0), 1.0 / (static_cast<real_T>(x.size(0))
          - 1.0), x, x);
      }

      return c;
    }

    void cov(const ::coder::array<real_T, 2U> &x, ::coder::array<real_T, 2U> &xy)
    {
      ::coder::array<real_T, 2U> b_x;
      ::coder::array<real_T, 1U> c_x;
      if (isrow(x)) {
        int32_T loop_ub;
        c_x.set_size(x.size(1));
        loop_ub = x.size(1);
        for (int32_T i{0}; i < loop_ub; i++) {
          c_x[i] = x[x.size(0) * i];
        }

        xy.set_size(1, 1);
        xy[0] = local_cov(c_x);
      } else {
        int32_T loop_ub;
        b_x.set_size(x.size(0), x.size(1));
        loop_ub = x.size(1) - 1;
        for (int32_T i{0}; i <= loop_ub; i++) {
          int32_T b_loop_ub;
          b_loop_ub = x.size(0) - 1;
          for (int32_T i1{0}; i1 <= b_loop_ub; i1++) {
            b_x[i1 + b_x.size(0) * i] = x[i1 + x.size(0) * i];
          }
        }

        local_cov(b_x, xy);
      }
    }
  }
}

// End of code generation (cov.cpp)
