//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xzgghrd.cpp
//
// Code generation for function 'xzgghrd'
//

// Include files
#include "xzgghrd.h"
#include "eye.h"
#include "rt_nonfinite.h"
#include "xzlartg.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace reflapack
      {
        void xzgghrd(int32_T ilo, int32_T ihi, ::coder::array<creal_T, 2U> &A, ::
                     coder::array<creal_T, 2U> &Z)
        {
          ::coder::array<creal_T, 2U> b_A;
          creal_T s;
          real_T c;
          int32_T n;
          n = A.size(0);
          eye(A.size(0), Z);
          if ((A.size(0) > 1) && (ihi >= ilo + 2)) {
            for (int32_T jcol{ilo - 1}; jcol + 1 < ihi - 1; jcol++) {
              int32_T jcolp1;
              jcolp1 = jcol + 2;
              for (int32_T jrow{ihi - 1}; jrow + 1 > jcol + 2; jrow--) {
                real_T b_s_re_tmp;
                real_T s_re_tmp;
                real_T stemp_im_tmp;
                real_T stemp_re_tmp;
                int32_T j;
                int32_T loop_ub;
                xzlartg(A[(jrow + A.size(0) * jcol) - 1], A[jrow + A.size(0) *
                        jcol], &c, &s, &A[(jrow + A.size(0) * jcol) - 1]);
                A[jrow + A.size(0) * jcol].re = 0.0;
                A[jrow + A.size(0) * jcol].im = 0.0;
                for (j = jcolp1; j <= n; j++) {
                  s_re_tmp = A[jrow + A.size(0) * (j - 1)].im;
                  b_s_re_tmp = A[jrow + A.size(0) * (j - 1)].re;
                  stemp_re_tmp = A[(jrow + A.size(0) * (j - 1)) - 1].re;
                  stemp_im_tmp = A[(jrow + A.size(0) * (j - 1)) - 1].im;
                  A[jrow + A.size(0) * (j - 1)].re = c * b_s_re_tmp - (s.re *
                    stemp_re_tmp + s.im * stemp_im_tmp);
                  A[jrow + A.size(0) * (j - 1)].im = c * A[jrow + A.size(0) * (j
                    - 1)].im - (s.re * stemp_im_tmp - s.im * stemp_re_tmp);
                  A[(jrow + A.size(0) * (j - 1)) - 1].re = c * stemp_re_tmp +
                    (s.re * b_s_re_tmp - s.im * s_re_tmp);
                  A[(jrow + A.size(0) * (j - 1)) - 1].im = c * stemp_im_tmp +
                    (s.re * s_re_tmp + s.im * b_s_re_tmp);
                }

                s.re = -s.re;
                s.im = -s.im;
                for (j = 1; j <= ihi; j++) {
                  s_re_tmp = A[(j + A.size(0) * (jrow - 1)) - 1].im;
                  b_s_re_tmp = A[(j + A.size(0) * (jrow - 1)) - 1].re;
                  stemp_re_tmp = A[(j + A.size(0) * jrow) - 1].re;
                  stemp_im_tmp = A[(j + A.size(0) * jrow) - 1].im;
                  A[(j + A.size(0) * (jrow - 1)) - 1].re = c * b_s_re_tmp -
                    (s.re * stemp_re_tmp + s.im * stemp_im_tmp);
                  A[(j + A.size(0) * (jrow - 1)) - 1].im = c * A[(j + A.size(0) *
                    (jrow - 1)) - 1].im - (s.re * stemp_im_tmp - s.im *
                    stemp_re_tmp);
                  A[(j + A.size(0) * jrow) - 1].re = c * stemp_re_tmp + (s.re *
                    b_s_re_tmp - s.im * s_re_tmp);
                  A[(j + A.size(0) * jrow) - 1].im = c * stemp_im_tmp + (s.re *
                    s_re_tmp + s.im * b_s_re_tmp);
                }

                b_A.set_size(Z.size(0), Z.size(1));
                j = Z.size(1);
                for (int32_T i{0}; i < j; i++) {
                  loop_ub = Z.size(0);
                  for (int32_T i1{0}; i1 < loop_ub; i1++) {
                    b_A[i1 + b_A.size(0) * i] = Z[i1 + Z.size(0) * i];
                  }
                }

                for (j = 1; j <= n; j++) {
                  s_re_tmp = b_A[(j + b_A.size(0) * (jrow - 1)) - 1].im;
                  b_s_re_tmp = b_A[(j + b_A.size(0) * (jrow - 1)) - 1].re;
                  stemp_re_tmp = b_A[(j + b_A.size(0) * jrow) - 1].re;
                  stemp_im_tmp = b_A[(j + b_A.size(0) * jrow) - 1].im;
                  b_A[(j + b_A.size(0) * (jrow - 1)) - 1].re = c * b_s_re_tmp -
                    (s.re * stemp_re_tmp + s.im * stemp_im_tmp);
                  b_A[(j + b_A.size(0) * (jrow - 1)) - 1].im = c * s_re_tmp -
                    (s.re * stemp_im_tmp - s.im * stemp_re_tmp);
                  b_A[(j + b_A.size(0) * jrow) - 1].re = c * stemp_re_tmp +
                    (s.re * b_s_re_tmp - s.im * s_re_tmp);
                  b_A[(j + b_A.size(0) * jrow) - 1].im = c * stemp_im_tmp +
                    (s.re * s_re_tmp + s.im * b_s_re_tmp);
                }

                Z.set_size(b_A.size(0), b_A.size(1));
                j = b_A.size(1);
                for (int32_T i{0}; i < j; i++) {
                  loop_ub = b_A.size(0);
                  for (int32_T i1{0}; i1 < loop_ub; i1++) {
                    Z[i1 + Z.size(0) * i] = b_A[i1 + b_A.size(0) * i];
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

// End of code generation (xzgghrd.cpp)
