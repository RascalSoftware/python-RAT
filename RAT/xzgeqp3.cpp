//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// xzgeqp3.cpp
//
// Code generation for function 'xzgeqp3'
//

// Include files
#include "xzgeqp3.h"
#include "ixamax.h"
#include "rt_nonfinite.h"
#include "xnrm2.h"
#include "xswap.h"
#include "xzlarf.h"
#include "xzlarfg.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace reflapack
      {
        static int32_T qrpf(::coder::array<real_T, 1U> &A, int32_T m, real_T
                            tau_data[]);
      }
    }
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace reflapack
      {
        static int32_T qrpf(::coder::array<real_T, 1U> &A, int32_T m, real_T
                            tau_data[])
        {
          real_T atmp;
          int32_T jpvt;
          int32_T y;
          jpvt = 1;
          if (m <= 1) {
            y = m;
          } else {
            y = 1;
          }

          y = static_cast<uint8_T>(y);
          for (int32_T i{0}; i < y; i++) {
            if (m > 1) {
              atmp = A[0];
              tau_data[0] = xzlarfg(m, &atmp, A);
              A[0] = atmp;
            } else {
              tau_data[0] = 0.0;
            }
          }

          return jpvt;
        }

        void qrpf(::coder::array<real_T, 2U> &A, int32_T m, int32_T n, ::coder::
                  array<real_T, 1U> &tau, ::coder::array<int32_T, 2U> &jpvt)
        {
          ::coder::array<real_T, 1U> vn1;
          ::coder::array<real_T, 1U> vn2;
          ::coder::array<real_T, 1U> work;
          real_T d;
          real_T temp1;
          int32_T itemp;
          int32_T j;
          int32_T ma;
          int32_T minmn;
          ma = A.size(0);
          if (m <= n) {
            minmn = m;
          } else {
            minmn = n;
          }

          itemp = A.size(1);
          work.set_size(itemp);
          for (j = 0; j < itemp; j++) {
            work[j] = 0.0;
          }

          itemp = A.size(1);
          vn1.set_size(itemp);
          for (j = 0; j < itemp; j++) {
            vn1[j] = 0.0;
          }

          itemp = A.size(1);
          vn2.set_size(itemp);
          for (j = 0; j < itemp; j++) {
            vn2[j] = 0.0;
          }

          for (j = 0; j < n; j++) {
            d = blas::xnrm2(m, A, j * ma + 1);
            vn1[j] = d;
            vn2[j] = d;
          }

          for (int32_T i{0}; i < minmn; i++) {
            int32_T ii;
            int32_T ip1;
            int32_T mmi;
            int32_T nmi;
            int32_T pvt;
            ip1 = i + 2;
            j = i * ma;
            ii = j + i;
            nmi = n - i;
            mmi = m - i;
            pvt = (i + blas::ixamax(nmi, vn1, i + 1)) - 1;
            if (pvt + 1 != i + 1) {
              blas::xswap(m, A, pvt * ma + 1, j + 1);
              itemp = jpvt[pvt];
              jpvt[pvt] = jpvt[i];
              jpvt[i] = itemp;
              vn1[pvt] = vn1[i];
              vn2[pvt] = vn2[i];
            }

            if (i + 1 < m) {
              temp1 = A[ii];
              d = xzlarfg(mmi, &temp1, A, ii + 2);
              tau[i] = d;
              A[ii] = temp1;
            } else {
              d = 0.0;
              tau[i] = 0.0;
            }

            if (i + 1 < n) {
              temp1 = A[ii];
              A[ii] = 1.0;
              xzlarf(mmi, nmi - 1, ii + 1, d, A, (ii + ma) + 1, ma, work);
              A[ii] = temp1;
            }

            for (j = ip1; j <= n; j++) {
              itemp = i + (j - 1) * ma;
              d = vn1[j - 1];
              if (d != 0.0) {
                real_T temp2;
                temp1 = std::abs(A[itemp]) / d;
                temp1 = 1.0 - temp1 * temp1;
                if (temp1 < 0.0) {
                  temp1 = 0.0;
                }

                temp2 = d / vn2[j - 1];
                temp2 = temp1 * (temp2 * temp2);
                if (temp2 <= 1.4901161193847656E-8) {
                  if (i + 1 < m) {
                    d = blas::xnrm2(mmi - 1, A, itemp + 2);
                    vn1[j - 1] = d;
                    vn2[j - 1] = d;
                  } else {
                    vn1[j - 1] = 0.0;
                    vn2[j - 1] = 0.0;
                  }
                } else {
                  vn1[j - 1] = d * std::sqrt(temp1);
                }
              }
            }
          }
        }

        void xzgeqp3(::coder::array<real_T, 1U> &A, int32_T m, real_T tau_data[],
                     int32_T *tau_size, int32_T *jpvt)
        {
          *tau_size = A.size(0);
          if (*tau_size > 1) {
            *tau_size = 1;
          }

          if (*tau_size - 1 >= 0) {
            tau_data[0] = 0.0;
          }

          if (A.size(0) == 0) {
            *jpvt = 1;
          } else {
            int32_T y;
            if (m <= 1) {
              y = m;
            } else {
              y = 1;
            }

            if (y < 1) {
              *jpvt = 1;
            } else {
              qrpf(A, m, tau_data);
              *jpvt = 1;
            }
          }
        }
      }
    }
  }
}

// End of code generation (xzgeqp3.cpp)
