//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// sortIdx.cpp
//
// Code generation for function 'sortIdx'
//

// Include files
#include "sortIdx.h"
#include "mergesort.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      static void merge(::coder::array<int32_T, 2U> &idx, ::coder::array<real_T,
                        2U> &x, int32_T offset, int32_T np, int32_T nq, ::coder::
                        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U>
                        &xwork);
      static void merge(::coder::array<int32_T, 1U> &idx, ::coder::array<real_T,
                        1U> &x, int32_T offset, int32_T np, int32_T nq, ::coder::
                        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U>
                        &xwork);
      static void merge_block(::coder::array<int32_T, 1U> &idx, ::coder::array<
        real_T, 1U> &x, int32_T offset, int32_T n, int32_T preSortLevel, ::coder::
        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U> &xwork);
      static void merge_pow2_block(::coder::array<int32_T, 1U> &idx, ::coder::
        array<real_T, 1U> &x, int32_T offset);
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
      static void merge(::coder::array<int32_T, 2U> &idx, ::coder::array<real_T,
                        2U> &x, int32_T offset, int32_T np, int32_T nq, ::coder::
                        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U>
                        &xwork)
      {
        if (nq != 0) {
          int32_T iout;
          int32_T n_tmp;
          int32_T p;
          int32_T q;
          n_tmp = np + nq;
          for (int32_T j{0}; j < n_tmp; j++) {
            iout = offset + j;
            iwork[j] = idx[iout];
            xwork[j] = x[iout];
          }

          p = 0;
          q = np;
          iout = offset - 1;
          int32_T exitg1;
          do {
            exitg1 = 0;
            iout++;
            if (xwork[p] <= xwork[q]) {
              idx[iout] = iwork[p];
              x[iout] = xwork[p];
              if (p + 1 < np) {
                p++;
              } else {
                exitg1 = 1;
              }
            } else {
              idx[iout] = iwork[q];
              x[iout] = xwork[q];
              if (q + 1 < n_tmp) {
                q++;
              } else {
                q = iout - p;
                for (int32_T j{p + 1}; j <= np; j++) {
                  iout = q + j;
                  idx[iout] = iwork[j - 1];
                  x[iout] = xwork[j - 1];
                }

                exitg1 = 1;
              }
            }
          } while (exitg1 == 0);
        }
      }

      static void merge(::coder::array<int32_T, 1U> &idx, ::coder::array<real_T,
                        1U> &x, int32_T offset, int32_T np, int32_T nq, ::coder::
                        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U>
                        &xwork)
      {
        if (nq != 0) {
          int32_T iout;
          int32_T n_tmp;
          int32_T p;
          int32_T q;
          n_tmp = np + nq;
          for (int32_T j{0}; j < n_tmp; j++) {
            iout = offset + j;
            iwork[j] = idx[iout];
            xwork[j] = x[iout];
          }

          p = 0;
          q = np;
          iout = offset - 1;
          int32_T exitg1;
          do {
            exitg1 = 0;
            iout++;
            if (xwork[p] <= xwork[q]) {
              idx[iout] = iwork[p];
              x[iout] = xwork[p];
              if (p + 1 < np) {
                p++;
              } else {
                exitg1 = 1;
              }
            } else {
              idx[iout] = iwork[q];
              x[iout] = xwork[q];
              if (q + 1 < n_tmp) {
                q++;
              } else {
                q = iout - p;
                for (int32_T j{p + 1}; j <= np; j++) {
                  iout = q + j;
                  idx[iout] = iwork[j - 1];
                  x[iout] = xwork[j - 1];
                }

                exitg1 = 1;
              }
            }
          } while (exitg1 == 0);
        }
      }

      static void merge_block(::coder::array<int32_T, 1U> &idx, ::coder::array<
        real_T, 1U> &x, int32_T offset, int32_T n, int32_T preSortLevel, ::coder::
        array<int32_T, 1U> &iwork, ::coder::array<real_T, 1U> &xwork)
      {
        int32_T bLen;
        int32_T nPairs;
        nPairs = n >> preSortLevel;
        bLen = 1 << preSortLevel;
        while (nPairs > 1) {
          int32_T nTail;
          int32_T tailOffset;
          if ((nPairs & 1) != 0) {
            nPairs--;
            tailOffset = bLen * nPairs;
            nTail = n - tailOffset;
            if (nTail > bLen) {
              merge(idx, x, offset + tailOffset, bLen, nTail - bLen, iwork,
                    xwork);
            }
          }

          tailOffset = bLen << 1;
          nPairs >>= 1;
          for (nTail = 0; nTail < nPairs; nTail++) {
            merge(idx, x, offset + nTail * tailOffset, bLen, bLen, iwork, xwork);
          }

          bLen = tailOffset;
        }

        if (n > bLen) {
          merge(idx, x, offset, bLen, n - bLen, iwork, xwork);
        }
      }

      static void merge_pow2_block(::coder::array<int32_T, 1U> &idx, ::coder::
        array<real_T, 1U> &x, int32_T offset)
      {
        real_T xwork[256];
        int32_T iwork[256];
        for (int32_T b{0}; b < 6; b++) {
          int32_T bLen;
          int32_T bLen2;
          int32_T i;
          bLen = 1 << (b + 2);
          bLen2 = bLen << 1;
          i = 256 >> (b + 3);
          for (int32_T k{0}; k < i; k++) {
            int32_T blockOffset;
            int32_T iout;
            int32_T p;
            int32_T q;
            blockOffset = offset + k * bLen2;
            for (int32_T j{0}; j < bLen2; j++) {
              iout = blockOffset + j;
              iwork[j] = idx[iout];
              xwork[j] = x[iout];
            }

            p = 0;
            q = bLen;
            iout = blockOffset - 1;
            int32_T exitg1;
            do {
              exitg1 = 0;
              iout++;
              if (xwork[p] <= xwork[q]) {
                idx[iout] = iwork[p];
                x[iout] = xwork[p];
                if (p + 1 < bLen) {
                  p++;
                } else {
                  exitg1 = 1;
                }
              } else {
                idx[iout] = iwork[q];
                x[iout] = xwork[q];
                if (q + 1 < bLen2) {
                  q++;
                } else {
                  iout -= p;
                  for (int32_T j{p + 1}; j <= bLen; j++) {
                    q = iout + j;
                    idx[q] = iwork[j - 1];
                    x[q] = xwork[j - 1];
                  }

                  exitg1 = 1;
                }
              }
            } while (exitg1 == 0);
          }
        }
      }

      void b_sortIdx(::coder::array<real_T, 1U> &x, ::coder::array<int32_T, 1U>
                     &idx)
      {
        ::coder::array<real_T, 1U> b_x;
        ::coder::array<real_T, 1U> xwork;
        ::coder::array<int32_T, 1U> b_idx;
        ::coder::array<int32_T, 1U> iwork;
        int32_T i1;
        int32_T ib;
        uint32_T unnamed_idx_0;
        unnamed_idx_0 = static_cast<uint32_T>(x.size(0));
        idx.set_size(static_cast<int32_T>(unnamed_idx_0));
        ib = static_cast<int32_T>(unnamed_idx_0);
        for (i1 = 0; i1 < ib; i1++) {
          idx[i1] = 0;
        }

        if (x.size(0) != 0) {
          real_T x4[4];
          int32_T idx4[4];
          int32_T i3;
          int32_T i4;
          int32_T n;
          int32_T nNaNs;
          int32_T quartetOffset;
          ib = static_cast<int32_T>(unnamed_idx_0);
          b_idx.set_size(static_cast<int32_T>(unnamed_idx_0));
          for (i1 = 0; i1 < ib; i1++) {
            b_idx[i1] = 0;
          }

          b_x.set_size(x.size(0));
          ib = x.size(0);
          for (i1 = 0; i1 < ib; i1++) {
            b_x[i1] = x[i1];
          }

          n = x.size(0);
          x4[0] = 0.0;
          idx4[0] = 0;
          x4[1] = 0.0;
          idx4[1] = 0;
          x4[2] = 0.0;
          idx4[2] = 0;
          x4[3] = 0.0;
          idx4[3] = 0;
          iwork.set_size(static_cast<int32_T>(unnamed_idx_0));
          ib = static_cast<int32_T>(unnamed_idx_0);
          for (i1 = 0; i1 < ib; i1++) {
            iwork[i1] = 0;
          }

          ib = x.size(0);
          xwork.set_size(ib);
          for (i1 = 0; i1 < ib; i1++) {
            xwork[i1] = 0.0;
          }

          nNaNs = 0;
          ib = 0;
          for (int32_T k{0}; k < n; k++) {
            if (std::isnan(b_x[k])) {
              i3 = (n - nNaNs) - 1;
              b_idx[i3] = k + 1;
              xwork[i3] = b_x[k];
              nNaNs++;
            } else {
              ib++;
              idx4[ib - 1] = k + 1;
              x4[ib - 1] = b_x[k];
              if (ib == 4) {
                real_T d;
                real_T d1;
                int8_T b_i1;
                int8_T b_i3;
                int8_T i;
                int8_T i2;
                quartetOffset = k - nNaNs;
                if (x4[0] <= x4[1]) {
                  i1 = 1;
                  ib = 2;
                } else {
                  i1 = 2;
                  ib = 1;
                }

                if (x4[2] <= x4[3]) {
                  i3 = 3;
                  i4 = 4;
                } else {
                  i3 = 4;
                  i4 = 3;
                }

                d = x4[i1 - 1];
                d1 = x4[i3 - 1];
                if (d <= d1) {
                  d = x4[ib - 1];
                  if (d <= d1) {
                    i = static_cast<int8_T>(i1);
                    b_i1 = static_cast<int8_T>(ib);
                    i2 = static_cast<int8_T>(i3);
                    b_i3 = static_cast<int8_T>(i4);
                  } else if (d <= x4[i4 - 1]) {
                    i = static_cast<int8_T>(i1);
                    b_i1 = static_cast<int8_T>(i3);
                    i2 = static_cast<int8_T>(ib);
                    b_i3 = static_cast<int8_T>(i4);
                  } else {
                    i = static_cast<int8_T>(i1);
                    b_i1 = static_cast<int8_T>(i3);
                    i2 = static_cast<int8_T>(i4);
                    b_i3 = static_cast<int8_T>(ib);
                  }
                } else {
                  d1 = x4[i4 - 1];
                  if (d <= d1) {
                    if (x4[ib - 1] <= d1) {
                      i = static_cast<int8_T>(i3);
                      b_i1 = static_cast<int8_T>(i1);
                      i2 = static_cast<int8_T>(ib);
                      b_i3 = static_cast<int8_T>(i4);
                    } else {
                      i = static_cast<int8_T>(i3);
                      b_i1 = static_cast<int8_T>(i1);
                      i2 = static_cast<int8_T>(i4);
                      b_i3 = static_cast<int8_T>(ib);
                    }
                  } else {
                    i = static_cast<int8_T>(i3);
                    b_i1 = static_cast<int8_T>(i4);
                    i2 = static_cast<int8_T>(i1);
                    b_i3 = static_cast<int8_T>(ib);
                  }
                }

                b_idx[quartetOffset - 3] = idx4[i - 1];
                b_idx[quartetOffset - 2] = idx4[b_i1 - 1];
                b_idx[quartetOffset - 1] = idx4[i2 - 1];
                b_idx[quartetOffset] = idx4[b_i3 - 1];
                b_x[quartetOffset - 3] = x4[i - 1];
                b_x[quartetOffset - 2] = x4[b_i1 - 1];
                b_x[quartetOffset - 1] = x4[i2 - 1];
                b_x[quartetOffset] = x4[b_i3 - 1];
                ib = 0;
              }
            }
          }

          i4 = b_x.size(0) - nNaNs;
          if (ib > 0) {
            int8_T perm[4];
            perm[1] = 0;
            perm[2] = 0;
            perm[3] = 0;
            if (ib == 1) {
              perm[0] = 1;
            } else if (ib == 2) {
              if (x4[0] <= x4[1]) {
                perm[0] = 1;
                perm[1] = 2;
              } else {
                perm[0] = 2;
                perm[1] = 1;
              }
            } else if (x4[0] <= x4[1]) {
              if (x4[1] <= x4[2]) {
                perm[0] = 1;
                perm[1] = 2;
                perm[2] = 3;
              } else if (x4[0] <= x4[2]) {
                perm[0] = 1;
                perm[1] = 3;
                perm[2] = 2;
              } else {
                perm[0] = 3;
                perm[1] = 1;
                perm[2] = 2;
              }
            } else if (x4[0] <= x4[2]) {
              perm[0] = 2;
              perm[1] = 1;
              perm[2] = 3;
            } else if (x4[1] <= x4[2]) {
              perm[0] = 2;
              perm[1] = 3;
              perm[2] = 1;
            } else {
              perm[0] = 3;
              perm[1] = 2;
              perm[2] = 1;
            }

            i1 = static_cast<uint8_T>(ib);
            for (int32_T k{0}; k < i1; k++) {
              i3 = perm[k] - 1;
              quartetOffset = (i4 - ib) + k;
              b_idx[quartetOffset] = idx4[i3];
              b_x[quartetOffset] = x4[i3];
            }
          }

          ib = nNaNs >> 1;
          for (int32_T k{0}; k < ib; k++) {
            quartetOffset = i4 + k;
            i1 = b_idx[quartetOffset];
            i3 = (n - k) - 1;
            b_idx[quartetOffset] = b_idx[i3];
            b_idx[i3] = i1;
            b_x[quartetOffset] = xwork[i3];
            b_x[i3] = xwork[quartetOffset];
          }

          if ((nNaNs & 1) != 0) {
            ib += i4;
            b_x[ib] = xwork[ib];
          }

          i1 = b_x.size(0) - nNaNs;
          ib = 2;
          if (i1 > 1) {
            if (b_x.size(0) >= 256) {
              quartetOffset = i1 >> 8;
              if (quartetOffset > 0) {
                for (ib = 0; ib < quartetOffset; ib++) {
                  merge_pow2_block(b_idx, b_x, ib << 8);
                }

                ib = quartetOffset << 8;
                quartetOffset = i1 - ib;
                if (quartetOffset > 0) {
                  merge_block(b_idx, b_x, ib, quartetOffset, 2, iwork, xwork);
                }

                ib = 8;
              }
            }

            merge_block(b_idx, b_x, 0, i1, ib, iwork, xwork);
          }

          ib = b_idx.size(0);
          for (i1 = 0; i1 < ib; i1++) {
            idx[i1] = b_idx[i1];
          }

          ib = b_x.size(0);
          for (i1 = 0; i1 < ib; i1++) {
            x[i1] = b_x[i1];
          }
        }
      }

      void merge_block(::coder::array<int32_T, 2U> &idx, ::coder::array<real_T,
                       2U> &x, int32_T offset, int32_T n, int32_T preSortLevel, ::
                       coder::array<int32_T, 1U> &iwork, ::coder::array<real_T,
                       1U> &xwork)
      {
        int32_T bLen;
        int32_T nPairs;
        nPairs = n >> preSortLevel;
        bLen = 1 << preSortLevel;
        while (nPairs > 1) {
          int32_T nTail;
          int32_T tailOffset;
          if ((nPairs & 1) != 0) {
            nPairs--;
            tailOffset = bLen * nPairs;
            nTail = n - tailOffset;
            if (nTail > bLen) {
              merge(idx, x, offset + tailOffset, bLen, nTail - bLen, iwork,
                    xwork);
            }
          }

          tailOffset = bLen << 1;
          nPairs >>= 1;
          for (nTail = 0; nTail < nPairs; nTail++) {
            merge(idx, x, offset + nTail * tailOffset, bLen, bLen, iwork, xwork);
          }

          bLen = tailOffset;
        }

        if (n > bLen) {
          merge(idx, x, offset, bLen, n - bLen, iwork, xwork);
        }
      }

      void merge_pow2_block(::coder::array<int32_T, 2U> &idx, ::coder::array<
                            real_T, 2U> &x, int32_T offset)
      {
        real_T xwork[256];
        int32_T iwork[256];
        for (int32_T b{0}; b < 6; b++) {
          int32_T bLen;
          int32_T bLen2;
          int32_T i;
          bLen = 1 << (b + 2);
          bLen2 = bLen << 1;
          i = 256 >> (b + 3);
          for (int32_T k{0}; k < i; k++) {
            int32_T blockOffset;
            int32_T iout;
            int32_T p;
            int32_T q;
            blockOffset = offset + k * bLen2;
            for (int32_T j{0}; j < bLen2; j++) {
              iout = blockOffset + j;
              iwork[j] = idx[iout];
              xwork[j] = x[iout];
            }

            p = 0;
            q = bLen;
            iout = blockOffset - 1;
            int32_T exitg1;
            do {
              exitg1 = 0;
              iout++;
              if (xwork[p] <= xwork[q]) {
                idx[iout] = iwork[p];
                x[iout] = xwork[p];
                if (p + 1 < bLen) {
                  p++;
                } else {
                  exitg1 = 1;
                }
              } else {
                idx[iout] = iwork[q];
                x[iout] = xwork[q];
                if (q + 1 < bLen2) {
                  q++;
                } else {
                  iout -= p;
                  for (int32_T j{p + 1}; j <= bLen; j++) {
                    q = iout + j;
                    idx[q] = iwork[j - 1];
                    x[q] = xwork[j - 1];
                  }

                  exitg1 = 1;
                }
              }
            } while (exitg1 == 0);
          }
        }
      }

      void sortIdx(const ::coder::array<real_T, 2U> &x, ::coder::array<int32_T,
                   2U> &idx)
      {
        int32_T loop_ub;
        idx.set_size(1, x.size(1));
        loop_ub = x.size(1);
        for (int32_T i{0}; i < loop_ub; i++) {
          idx[i] = 0;
        }

        if (x.size(1) != 0) {
          b_mergesort(idx, x, x.size(1));
        }
      }

      void sortIdx(const ::coder::array<real_T, 1U> &x, ::coder::array<int32_T,
                   1U> &idx)
      {
        ::coder::array<real_T, 1U> b_x;
        ::coder::array<int32_T, 1U> r;
        int32_T loop_ub;
        idx.set_size(x.size(0));
        loop_ub = x.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          idx[i] = 0;
        }

        loop_ub = x.size(0);
        r.set_size(x.size(0));
        for (int32_T i{0}; i < loop_ub; i++) {
          r[i] = 0;
        }

        b_x.set_size(x.size(0));
        loop_ub = x.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          b_x[i] = x[i];
        }

        b_mergesort(r, b_x, x.size(0));
        loop_ub = r.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          idx[i] = r[i];
        }
      }

      void sortIdx(const ::coder::array<real_T, 2U> &x, const int32_T col_data[],
                   ::coder::array<int32_T, 1U> &idx)
      {
        int32_T k;
        int32_T n;
        n = x.size(0);
        idx.set_size(x.size(0));
        k = x.size(0);
        for (int32_T i{0}; i < k; i++) {
          idx[i] = 0;
        }

        if (x.size(0) == 0) {
          for (k = 0; k < n; k++) {
            idx[k] = k + 1;
          }
        } else {
          b_mergesort(idx, x, col_data, x.size(0));
        }
      }
    }
  }
}

// End of code generation (sortIdx.cpp)
