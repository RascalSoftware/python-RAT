//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xtrsm.cpp
//
// Code generation for function 'xtrsm'
//

// Include files
#include "xtrsm.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace blas
      {
        void b_xtrsm(int32_T m, int32_T n, const ::coder::array<real_T, 2U> &A,
                     int32_T lda, ::coder::array<real_T, 2U> &B, int32_T ldb)
        {
          if ((n != 0) && ((B.size(0) != 0) && (B.size(1) != 0))) {
            for (int32_T j{n}; j >= 1; j--) {
              int32_T i;
              int32_T jAcol;
              int32_T jBcol;
              jBcol = ldb * (j - 1);
              jAcol = lda * (j - 1) - 1;
              i = j + 1;
              for (int32_T k{i}; k <= n; k++) {
                int32_T i1;
                int32_T kBcol;
                kBcol = ldb * (k - 1);
                i1 = k + jAcol;
                if (A[i1] != 0.0) {
                  int32_T i2;
                  i2 = static_cast<uint8_T>(m);
                  for (int32_T b_i{0}; b_i < i2; b_i++) {
                    B[jBcol] = B[jBcol] - A[i1] * B[kBcol];
                  }
                }
              }
            }
          }
        }

        void xtrsm(int32_T m, int32_T n, const ::coder::array<real_T, 2U> &A,
                   int32_T lda, ::coder::array<real_T, 2U> &B, int32_T ldb)
        {
          if ((n != 0) && ((B.size(0) != 0) && (B.size(1) != 0))) {
            int32_T i;
            i = static_cast<uint8_T>(m);
            for (int32_T j{0}; j < n; j++) {
              real_T temp;
              int32_T jAcol;
              int32_T jBcol;
              jBcol = ldb * j;
              jAcol = lda * j;
              for (int32_T k{0}; k < j; k++) {
                int32_T i1;
                int32_T kBcol;
                kBcol = ldb * k;
                i1 = k + jAcol;
                if (A[i1] != 0.0) {
                  for (int32_T b_i{0}; b_i < i; b_i++) {
                    B[jBcol] = B[jBcol] - A[i1] * B[kBcol];
                  }
                }
              }

              temp = 1.0 / A[j + jAcol];
              for (int32_T b_i{0}; b_i < i; b_i++) {
                B[jBcol] = temp * B[jBcol];
              }
            }
          }
        }
      }
    }
  }
}

// End of code generation (xtrsm.cpp)
