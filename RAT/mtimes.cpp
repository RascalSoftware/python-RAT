//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// mtimes.cpp
//
// Code generation for function 'mtimes'
//

// Include files
#include "mtimes.h"
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
        void b_mtimes(const ::coder::array<real_T, 2U> &A, const ::coder::array<
                      real_T, 2U> &B, ::coder::array<real_T, 2U> &C)
        {
          int32_T inner;
          int32_T mc;
          int32_T nc;
          mc = A.size(1) - 1;
          inner = A.size(0);
          nc = B.size(1);
          C.set_size(A.size(1), B.size(1));
          for (int32_T j{0}; j < nc; j++) {
            for (int32_T i{0}; i <= mc; i++) {
              C[i + C.size(0) * j] = 0.0;
            }

            for (int32_T k{0}; k < inner; k++) {
              real_T bkj;
              bkj = B[k + B.size(0) * j];
              for (int32_T i{0}; i <= mc; i++) {
                C[i + C.size(0) * j] = C[i + C.size(0) * j] + A[k + A.size(0) *
                  i] * bkj;
              }
            }
          }
        }

        void mtimes(const ::coder::array<real_T, 2U> &A, const ::coder::array<
                    real_T, 2U> &B, real_T C_data[], int32_T C_size[2])
        {
          int32_T inner;
          int32_T mc;
          int32_T nc;
          mc = A.size(0) - 1;
          inner = A.size(1);
          nc = B.size(0);
          C_size[0] = A.size(0);
          C_size[1] = B.size(0);
          for (int32_T j{0}; j < nc; j++) {
            if (mc >= 0) {
              C_data[0] = 0.0;
            }

            for (int32_T k{0}; k < inner; k++) {
              real_T bkj;
              bkj = B[B.size(0) * k];
              for (int32_T i{0}; i <= mc; i++) {
                C_data[0] += A[A.size(0) * k] * bkj;
              }
            }
          }
        }

        void mtimes(const real_T A_data[], const int32_T A_size[2], const real_T
                    B_data[], const int32_T B_size[2], real_T C_data[], int32_T
                    C_size[2])
        {
          int32_T inner;
          int32_T mc;
          int32_T nc;
          mc = A_size[0] - 1;
          inner = A_size[1];
          nc = B_size[1];
          C_size[0] = A_size[0];
          C_size[1] = B_size[1];
          for (int32_T j{0}; j < nc; j++) {
            if (mc >= 0) {
              C_data[0] = 0.0;
            }

            for (int32_T k{0}; k < inner; k++) {
              real_T bkj;
              bkj = B_data[0];
              for (int32_T i{0}; i <= mc; i++) {
                C_data[0] += A_data[0] * bkj;
              }
            }
          }
        }

        void mtimes(const ::coder::array<creal_T, 2U> &A, const ::coder::array<
                    creal_T, 2U> &B, ::coder::array<creal_T, 2U> &C)
        {
          int32_T inner;
          int32_T n;
          inner = A.size(1);
          n = B.size(0);
          C.set_size(1, B.size(0));
          for (int32_T j{0}; j < n; j++) {
            real_T s_im;
            real_T s_re;
            s_re = 0.0;
            s_im = 0.0;
            for (int32_T k{0}; k < inner; k++) {
              real_T A_re_tmp;
              real_T B_im;
              real_T B_re;
              real_T b_A_re_tmp;
              B_re = B[j + B.size(0) * k].re;
              B_im = -B[j + B.size(0) * k].im;
              A_re_tmp = A[k].re;
              b_A_re_tmp = A[k].im;
              s_re += A_re_tmp * B_re - b_A_re_tmp * B_im;
              s_im += A_re_tmp * B_im + b_A_re_tmp * B_re;
            }

            C[j].re = s_re;
            C[j].im = s_im;
          }
        }

        void mtimes(const ::coder::array<real_T, 2U> &A, const ::coder::array<
                    real_T, 2U> &B, ::coder::array<real_T, 2U> &C)
        {
          int32_T inner;
          int32_T nc;
          inner = A.size(1);
          nc = B.size(0);
          C.set_size(1, B.size(0));
          for (int32_T j{0}; j < nc; j++) {
            C[j] = 0.0;
            for (int32_T k{0}; k < inner; k++) {
              C[j] = C[j] + A[k] * B[j + B.size(0) * k];
            }
          }
        }

        void mtimes(const ::coder::array<real_T, 2U> &A, const ::coder::array<
                    real_T, 1U> &B, ::coder::array<real_T, 1U> &C)
        {
          int32_T inner;
          int32_T mc;
          mc = A.size(0) - 1;
          inner = A.size(1);
          C.set_size(A.size(0));
          for (int32_T i{0}; i <= mc; i++) {
            C[i] = 0.0;
          }

          for (int32_T k{0}; k < inner; k++) {
            for (int32_T i{0}; i <= mc; i++) {
              C[i] = C[i] + A[i + A.size(0) * k] * B[k];
            }
          }
        }
      }
    }
  }
}

// End of code generation (mtimes.cpp)
