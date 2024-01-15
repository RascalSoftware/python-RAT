//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xztgevc.cpp
//
// Code generation for function 'xztgevc'
//

// Include files
#include "xztgevc.h"
#include "RATMain_data.h"
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
      namespace reflapack
      {
        void xztgevc(const ::coder::array<creal_T, 2U> &A, ::coder::array<
                     creal_T, 2U> &V)
        {
          ::coder::array<creal_T, 1U> work1;
          ::coder::array<creal_T, 1U> work2;
          ::coder::array<real_T, 1U> rworka;
          real_T BIG;
          real_T BIGNUM;
          real_T NSAFMIN;
          real_T SMALL;
          real_T anorm;
          real_T ascale;
          real_T x;
          real_T y;
          int32_T b_i;
          int32_T i;
          int32_T n;
          n = A.size(0);
          work1.set_size(A.size(0));
          i = A.size(0);
          for (b_i = 0; b_i < i; b_i++) {
            work1[b_i].re = 0.0;
            work1[b_i].im = 0.0;
          }

          work2.set_size(A.size(0));
          i = A.size(0);
          for (b_i = 0; b_i < i; b_i++) {
            work2[b_i].re = 0.0;
            work2[b_i].im = 0.0;
          }

          NSAFMIN = 2.2250738585072014E-308 * static_cast<real_T>(A.size(0));
          SMALL = NSAFMIN / 2.2204460492503131E-16;
          BIG = 1.0 / SMALL;
          BIGNUM = 1.0 / NSAFMIN;
          rworka.set_size(A.size(0));
          i = A.size(0);
          for (b_i = 0; b_i < i; b_i++) {
            rworka[b_i] = 0.0;
          }

          anorm = std::abs(A[0].re) + std::abs(A[0].im);
          for (int32_T j{2}; j <= n; j++) {
            for (i = 0; i <= j - 2; i++) {
              rworka[j - 1] = rworka[j - 1] + (std::abs(A[i + A.size(0) * (j - 1)]
                .re) + std::abs(A[i + A.size(0) * (j - 1)].im));
            }

            y = rworka[j - 1] + (std::abs(A[(j + A.size(0) * (j - 1)) - 1].re) +
                                 std::abs(A[(j + A.size(0) * (j - 1)) - 1].im));
            if (y > anorm) {
              anorm = y;
            }
          }

          x = anorm;
          if (anorm < 2.2250738585072014E-308) {
            x = 2.2250738585072014E-308;
          }

          ascale = 1.0 / x;
          for (int32_T je{n}; je >= 1; je--) {
            real_T acoeff;
            real_T dmin;
            real_T salpha_im;
            real_T salpha_re;
            real_T scale;
            real_T temp;
            boolean_T lscalea;
            boolean_T lscaleb;
            NSAFMIN = A[(je + A.size(0) * (je - 1)) - 1].re;
            y = A[(je + A.size(0) * (je - 1)) - 1].im;
            x = (std::abs(NSAFMIN) + std::abs(y)) * ascale;
            if (x < 1.0) {
              x = 1.0;
            }

            temp = 1.0 / x;
            salpha_re = ascale * (temp * NSAFMIN);
            salpha_im = ascale * (temp * y);
            acoeff = temp * ascale;
            if ((temp >= 2.2250738585072014E-308) && (acoeff < SMALL)) {
              lscalea = true;
            } else {
              lscalea = false;
            }

            NSAFMIN = std::abs(salpha_re) + std::abs(salpha_im);
            if ((NSAFMIN >= 2.2250738585072014E-308) && (NSAFMIN < SMALL)) {
              lscaleb = true;
            } else {
              lscaleb = false;
            }

            scale = 1.0;
            if (lscalea) {
              x = anorm;
              if (BIG < anorm) {
                x = BIG;
              }

              scale = SMALL / temp * x;
            }

            if (lscaleb) {
              y = SMALL / NSAFMIN;
              if (y > scale) {
                scale = y;
              }
            }

            if (lscalea || lscaleb) {
              x = acoeff;
              if (acoeff < 1.0) {
                x = 1.0;
              }

              if (NSAFMIN > x) {
                x = NSAFMIN;
              }

              y = 1.0 / (2.2250738585072014E-308 * x);
              if (y < scale) {
                scale = y;
              }

              if (lscalea) {
                acoeff = ascale * (scale * temp);
              } else {
                acoeff *= scale;
              }

              salpha_re *= scale;
              salpha_im *= scale;
            }

            for (int32_T jr{0}; jr < n; jr++) {
              work1[jr].re = 0.0;
              work1[jr].im = 0.0;
            }

            work1[je - 1].re = 1.0;
            work1[je - 1].im = 0.0;
            dmin = 2.2204460492503131E-16 * acoeff * anorm;
            y = 2.2204460492503131E-16 * (std::abs(salpha_re) + std::abs
              (salpha_im));
            if (y > dmin) {
              dmin = y;
            }

            if (dmin < 2.2250738585072014E-308) {
              dmin = 2.2250738585072014E-308;
            }

            for (int32_T jr{0}; jr <= je - 2; jr++) {
              work1[jr].re = acoeff * A[jr + A.size(0) * (je - 1)].re;
              work1[jr].im = acoeff * A[jr + A.size(0) * (je - 1)].im;
            }

            work1[je - 1].re = 1.0;
            work1[je - 1].im = 0.0;
            b_i = je - 1;
            for (int32_T j{b_i}; j >= 1; j--) {
              real_T brm;
              real_T d_im;
              real_T d_re;
              d_re = acoeff * A[(j + A.size(0) * (j - 1)) - 1].re - salpha_re;
              d_im = acoeff * A[(j + A.size(0) * (j - 1)) - 1].im - salpha_im;
              if (std::abs(d_re) + std::abs(d_im) <= dmin) {
                d_re = dmin;
                d_im = 0.0;
              }

              brm = std::abs(d_re);
              y = std::abs(d_im);
              NSAFMIN = brm + y;
              if (NSAFMIN < 1.0) {
                scale = std::abs(work1[j - 1].re) + std::abs(work1[j - 1].im);
                if (scale >= BIGNUM * NSAFMIN) {
                  temp = 1.0 / scale;
                  for (int32_T jr{0}; jr < je; jr++) {
                    work1[jr].re = temp * work1[jr].re;
                    work1[jr].im = temp * work1[jr].im;
                  }
                }
              }

              NSAFMIN = work1[j - 1].re;
              x = -work1[j - 1].re;
              scale = work1[j - 1].im;
              temp = -work1[j - 1].im;
              if (d_im == 0.0) {
                if (-scale == 0.0) {
                  y = -NSAFMIN / d_re;
                  NSAFMIN = 0.0;
                } else if (-NSAFMIN == 0.0) {
                  y = 0.0;
                  NSAFMIN = -scale / d_re;
                } else {
                  y = -NSAFMIN / d_re;
                  NSAFMIN = -scale / d_re;
                }
              } else if (d_re == 0.0) {
                if (-NSAFMIN == 0.0) {
                  y = -scale / d_im;
                  NSAFMIN = 0.0;
                } else if (-scale == 0.0) {
                  y = 0.0;
                  NSAFMIN = -(-NSAFMIN / d_im);
                } else {
                  y = -scale / d_im;
                  NSAFMIN = -(-NSAFMIN / d_im);
                }
              } else if (brm > y) {
                scale = d_im / d_re;
                NSAFMIN = d_re + scale * d_im;
                y = (x + scale * temp) / NSAFMIN;
                NSAFMIN = (temp - scale * x) / NSAFMIN;
              } else if (y == brm) {
                if (d_re > 0.0) {
                  scale = 0.5;
                } else {
                  scale = -0.5;
                }

                if (d_im > 0.0) {
                  NSAFMIN = 0.5;
                } else {
                  NSAFMIN = -0.5;
                }

                y = (x * scale + temp * NSAFMIN) / brm;
                NSAFMIN = (temp * scale - x * NSAFMIN) / brm;
              } else {
                scale = d_re / d_im;
                NSAFMIN = d_im + scale * d_re;
                y = (scale * x + temp) / NSAFMIN;
                NSAFMIN = (scale * temp - x) / NSAFMIN;
              }

              work1[j - 1].re = y;
              work1[j - 1].im = NSAFMIN;
              if (j > 1) {
                NSAFMIN = std::abs(y) + std::abs(NSAFMIN);
                if (NSAFMIN > 1.0) {
                  temp = 1.0 / NSAFMIN;
                  if (acoeff * rworka[j - 1] >= BIGNUM * temp) {
                    for (int32_T jr{0}; jr < je; jr++) {
                      work1[jr].re = temp * work1[jr].re;
                      work1[jr].im = temp * work1[jr].im;
                    }
                  }
                }

                d_re = acoeff * work1[j - 1].re;
                d_im = acoeff * work1[j - 1].im;
                for (int32_T jr{0}; jr <= j - 2; jr++) {
                  NSAFMIN = A[jr + A.size(0) * (j - 1)].im;
                  x = A[jr + A.size(0) * (j - 1)].re;
                  work1[jr].re = work1[jr].re + (d_re * x - d_im * NSAFMIN);
                  work1[jr].im = work1[jr].im + (d_re * NSAFMIN + d_im * x);
                }
              }
            }

            for (int32_T jr{0}; jr < n; jr++) {
              work2[jr].re = 0.0;
              work2[jr].im = 0.0;
            }

            for (i = 0; i < je; i++) {
              for (int32_T jr{0}; jr < n; jr++) {
                NSAFMIN = V[jr + V.size(0) * i].re;
                y = work1[i].im;
                scale = V[jr + V.size(0) * i].im;
                x = work1[i].re;
                work2[jr].re = work2[jr].re + (NSAFMIN * x - scale * y);
                work2[jr].im = work2[jr].im + (NSAFMIN * y + scale * x);
              }
            }

            NSAFMIN = std::abs(work2[0].re) + std::abs(work2[0].im);
            if (n > 1) {
              for (int32_T jr{2}; jr <= n; jr++) {
                y = std::abs(work2[jr - 1].re) + std::abs(work2[jr - 1].im);
                if (y > NSAFMIN) {
                  NSAFMIN = y;
                }
              }
            }

            if (NSAFMIN > 2.2250738585072014E-308) {
              temp = 1.0 / NSAFMIN;
              for (int32_T jr{0}; jr < n; jr++) {
                V[jr + V.size(0) * (je - 1)].re = temp * work2[jr].re;
                V[jr + V.size(0) * (je - 1)].im = temp * work2[jr].im;
              }
            } else {
              for (int32_T jr{0}; jr < n; jr++) {
                V[jr + V.size(0) * (je - 1)].re = 0.0;
                V[jr + V.size(0) * (je - 1)].im = 0.0;
              }
            }
          }
        }
      }
    }
  }
}

// End of code generation (xztgevc.cpp)
