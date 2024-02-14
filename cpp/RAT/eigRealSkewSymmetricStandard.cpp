//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eigRealSkewSymmetricStandard.cpp
//
// Code generation for function 'eigRealSkewSymmetricStandard'
//

// Include files
#include "eigRealSkewSymmetricStandard.h"
#include "RATMain_data.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void extractEigenValues(const ::coder::array<real_T, 2U> &T, ::coder::array<
      creal_T, 2U> &D)
    {
      int32_T i;
      int32_T n;
      n = T.size(0);
      D.set_size(T.size(0), T.size(0));
      i = T.size(0);
      for (int32_T b_i{0}; b_i < i; b_i++) {
        int32_T loop_ub;
        loop_ub = T.size(0);
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          D[i1 + D.size(0) * b_i].re = 0.0;
          D[i1 + D.size(0) * b_i].im = 0.0;
        }
      }

      i = 1;
      int32_T exitg1;
      do {
        exitg1 = 0;
        if (i <= n) {
          if (i != n) {
            real_T lambda;
            lambda = T[i + T.size(0) * (i - 1)];
            if (lambda != 0.0) {
              lambda = std::abs(lambda);
              D[(i + D.size(0) * (i - 1)) - 1].re = 0.0;
              D[(i + D.size(0) * (i - 1)) - 1].im = lambda;
              D[i + D.size(0) * i].re = 0.0;
              D[i + D.size(0) * i].im = -lambda;
              i += 2;
            } else {
              i++;
            }
          } else {
            i++;
          }
        } else {
          exitg1 = 1;
        }
      } while (exitg1 == 0);
    }

    void extractEigenVectors(const ::coder::array<real_T, 2U> &U, const ::coder::
      array<real_T, 2U> &T, ::coder::array<creal_T, 2U> &V)
    {
      int32_T j;
      int32_T n;
      int32_T sgn;
      V.set_size(U.size(0), U.size(1));
      j = U.size(1);
      for (sgn = 0; sgn < j; sgn++) {
        n = U.size(0);
        for (int32_T i{0}; i < n; i++) {
          V[i + V.size(0) * sgn].re = U[i + U.size(0) * sgn];
          V[i + V.size(0) * sgn].im = 0.0;
        }
      }

      j = 1;
      n = T.size(0);
      while (j <= n) {
        if ((j != n) && (T[j + T.size(0) * (j - 1)] != 0.0)) {
          if (T[j + T.size(0) * (j - 1)] < 0.0) {
            sgn = 1;
          } else {
            sgn = -1;
          }

          for (int32_T i{0}; i < n; i++) {
            real_T ai;
            real_T ar;
            ar = V[i + V.size(0) * (j - 1)].re;
            ai = static_cast<real_T>(sgn) * V[i + V.size(0) * j].re;
            if (ai == 0.0) {
              V[i + V.size(0) * (j - 1)].re = ar / 1.4142135623730951;
              V[i + V.size(0) * (j - 1)].im = 0.0;
            } else if (ar == 0.0) {
              V[i + V.size(0) * (j - 1)].re = 0.0;
              V[i + V.size(0) * (j - 1)].im = ai / 1.4142135623730951;
            } else {
              V[i + V.size(0) * (j - 1)].re = ar / 1.4142135623730951;
              V[i + V.size(0) * (j - 1)].im = ai / 1.4142135623730951;
            }

            V[i + V.size(0) * j].re = V[i + V.size(0) * (j - 1)].re;
            V[i + V.size(0) * j].im = -V[i + V.size(0) * (j - 1)].im;
          }

          j += 2;
        } else {
          j++;
        }
      }
    }
  }
}

// End of code generation (eigRealSkewSymmetricStandard.cpp)
