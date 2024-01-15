//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xungorghr.cpp
//
// Code generation for function 'xungorghr'
//

// Include files
#include "xungorghr.h"
#include "rt_nonfinite.h"
#include "xzungqr.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace lapack
      {
        void xungorghr(int32_T n, int32_T ihi, ::coder::array<real_T, 2U> &A,
                       int32_T lda, const ::coder::array<real_T, 1U> &tau)
        {
          if (n != 0) {
            int32_T b_i;
            int32_T ia;
            for (int32_T j{ihi}; j >= 2; j--) {
              int32_T iajm1;
              ia = (j - 1) * lda - 1;
              for (int32_T i{0}; i <= j - 2; i++) {
                A[(ia + i) + 1] = 0.0;
              }

              iajm1 = ia - lda;
              b_i = j + 1;
              for (int32_T i{b_i}; i <= ihi; i++) {
                A[ia + i] = A[iajm1 + i];
              }

              b_i = ihi + 1;
              for (int32_T i{b_i}; i <= n; i++) {
                A[ia + i] = 0.0;
              }
            }

            for (int32_T i{0}; i < n; i++) {
              A[i] = 0.0;
            }

            A[0] = 1.0;
            b_i = ihi + 1;
            for (int32_T j{b_i}; j <= n; j++) {
              ia = (j - 1) * lda;
              for (int32_T i{0}; i < n; i++) {
                A[ia + i] = 0.0;
              }

              A[(ia + j) - 1] = 1.0;
            }

            reflapack::xzungqr(ihi - 1, ihi - 1, ihi - 1, A, lda + 2, lda, tau);
          }
        }
      }
    }
  }
}

// End of code generation (xungorghr.cpp)
