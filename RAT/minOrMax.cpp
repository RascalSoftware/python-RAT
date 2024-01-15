//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// minOrMax.cpp
//
// Code generation for function 'minOrMax'
//

// Include files
#include "minOrMax.h"
#include "ixfun.h"
#include "relop.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void c_binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::array<
    real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    ::coder::array<real_T, 2U> b_in2;
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    if (in3.size(1) == 1) {
      i1 = in2.size(1);
    } else {
      i1 = in3.size(1);
    }

    b_in2.set_size(i, i1);
    stride_0_0 = (in2.size(0) != 1);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_1_1 = (in3.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    if (in3.size(1) == 1) {
      loop_ub = in2.size(1);
    } else {
      loop_ub = in3.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      i1 = in3.size(0);
      if (i1 == 1) {
        b_loop_ub = in2.size(0);
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = in2[i1 * stride_0_0 + in2.size(0) *
          aux_0_1] - in3[i1 * stride_1_0 + in3.size(0) * aux_1_1];
      }

      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    coder::internal::maximum(b_in2, in1);
  }

  namespace coder
  {
    namespace internal
    {
      real_T b_maximum(const real_T x[3])
      {
        real_T ex;
        int32_T idx;
        int32_T k;
        if (!std::isnan(x[0])) {
          idx = 1;
        } else {
          boolean_T exitg1;
          idx = 0;
          k = 2;
          exitg1 = false;
          while ((!exitg1) && (k <= 3)) {
            if (!std::isnan(x[k - 1])) {
              idx = k;
              exitg1 = true;
            } else {
              k++;
            }
          }
        }

        if (idx == 0) {
          ex = x[0];
        } else {
          ex = x[idx - 1];
          idx++;
          for (k = idx; k < 4; k++) {
            real_T d;
            d = x[k - 1];
            if (ex < d) {
              ex = d;
            }
          }
        }

        return ex;
      }

      void maximum(const ::coder::array<real_T, 2U> &x, ::coder::array<real_T,
                   2U> &ex)
      {
        int32_T m;
        int32_T n;
        m = x.size(0);
        n = x.size(1);
        ex.set_size(1, x.size(1));
        if (x.size(1) >= 1) {
          for (int32_T j{0}; j < n; j++) {
            ex[j] = x[x.size(0) * j];
            for (int32_T i{2}; i <= m; i++) {
              real_T d;
              d = x[(i + x.size(0) * j) - 1];
              if (relop(ex[j], d)) {
                ex[j] = d;
              }
            }
          }
        }
      }

      real_T maximum(const ::coder::array<real_T, 1U> &x)
      {
        real_T ex;
        int32_T last;
        last = x.size(0);
        if (x.size(0) <= 2) {
          if (x.size(0) == 1) {
            ex = x[0];
          } else if ((x[0] < x[x.size(0) - 1]) || (std::isnan(x[0]) && (!std::
                       isnan(x[x.size(0) - 1])))) {
            ex = x[x.size(0) - 1];
          } else {
            ex = x[0];
          }
        } else {
          int32_T idx;
          int32_T k;
          if (!std::isnan(x[0])) {
            idx = 1;
          } else {
            boolean_T exitg1;
            idx = 0;
            k = 2;
            exitg1 = false;
            while ((!exitg1) && (k <= last)) {
              if (!std::isnan(x[k - 1])) {
                idx = k;
                exitg1 = true;
              } else {
                k++;
              }
            }
          }

          if (idx == 0) {
            ex = x[0];
          } else {
            ex = x[idx - 1];
            idx++;
            for (k = idx; k <= last; k++) {
              real_T d;
              d = x[k - 1];
              if (ex < d) {
                ex = d;
              }
            }
          }
        }

        return ex;
      }

      real_T maximum(const ::coder::array<real_T, 2U> &x)
      {
        real_T ex;
        int32_T last;
        last = x.size(1);
        if (x.size(1) <= 2) {
          if (x.size(1) == 1) {
            ex = x[0];
          } else if ((x[0] < x[x.size(1) - 1]) || (std::isnan(x[0]) && (!std::
                       isnan(x[x.size(1) - 1])))) {
            ex = x[x.size(1) - 1];
          } else {
            ex = x[0];
          }
        } else {
          int32_T idx;
          int32_T k;
          if (!std::isnan(x[0])) {
            idx = 1;
          } else {
            boolean_T exitg1;
            idx = 0;
            k = 2;
            exitg1 = false;
            while ((!exitg1) && (k <= last)) {
              if (!std::isnan(x[k - 1])) {
                idx = k;
                exitg1 = true;
              } else {
                k++;
              }
            }
          }

          if (idx == 0) {
            ex = x[0];
          } else {
            ex = x[idx - 1];
            idx++;
            for (k = idx; k <= last; k++) {
              real_T d;
              d = x[k - 1];
              if (ex < d) {
                ex = d;
              }
            }
          }
        }

        return ex;
      }

      void maximum(const ::coder::array<real_T, 2U> &x, real_T ex[2])
      {
        int32_T m;
        m = x.size(0);
        for (int32_T j{0}; j < 2; j++) {
          ex[j] = x[x.size(0) * j];
          for (int32_T i{2}; i <= m; i++) {
            real_T d;
            d = x[(i + x.size(0) * j) - 1];
            if (relop(ex[j], d)) {
              ex[j] = d;
            }
          }
        }
      }

      real_T maximum(const real_T x[2])
      {
        real_T ex;
        if ((x[0] < x[1]) || (std::isnan(x[0]) && (!std::isnan(x[1])))) {
          ex = x[1];
        } else {
          ex = x[0];
        }

        return ex;
      }

      void maximum2(const ::coder::array<real_T, 1U> &x, const ::coder::array<
                    real_T, 1U> &y, ::coder::array<real_T, 1U> &ex)
      {
        if (x.size(0) == y.size(0)) {
          int32_T loop_ub;
          ex.set_size(x.size(0));
          loop_ub = x.size(0);
          for (int32_T i{0}; i < loop_ub; i++) {
            real_T varargin_1;
            real_T varargin_2;
            varargin_1 = x[i];
            varargin_2 = y[i];
            ex[i] = std::fmax(varargin_1, varargin_2);
          }
        } else {
          expand_max(x, y, ex);
        }
      }

      void minimum(const ::coder::array<real_T, 1U> &x, real_T *ex, int32_T *idx)
      {
        int32_T last;
        last = x.size(0);
        if (x.size(0) <= 2) {
          if (x.size(0) == 1) {
            *ex = x[0];
            *idx = 1;
          } else if ((x[0] > x[x.size(0) - 1]) || (std::isnan(x[0]) && (!std::
                       isnan(x[x.size(0) - 1])))) {
            *ex = x[x.size(0) - 1];
            *idx = x.size(0);
          } else {
            *ex = x[0];
            *idx = 1;
          }
        } else {
          int32_T k;
          if (!std::isnan(x[0])) {
            *idx = 1;
          } else {
            boolean_T exitg1;
            *idx = 0;
            k = 2;
            exitg1 = false;
            while ((!exitg1) && (k <= last)) {
              if (!std::isnan(x[k - 1])) {
                *idx = k;
                exitg1 = true;
              } else {
                k++;
              }
            }
          }

          if (*idx == 0) {
            *ex = x[0];
            *idx = 1;
          } else {
            int32_T i;
            *ex = x[*idx - 1];
            i = *idx + 1;
            for (k = i; k <= last; k++) {
              real_T d;
              d = x[k - 1];
              if (*ex > d) {
                *ex = d;
                *idx = k;
              }
            }
          }
        }
      }

      real_T minimum(const real_T x[2])
      {
        real_T ex;
        if ((x[0] > x[1]) || (std::isnan(x[0]) && (!std::isnan(x[1])))) {
          ex = x[1];
        } else {
          ex = x[0];
        }

        return ex;
      }

      void minimum(const real_T x[50], real_T *ex, int32_T *idx)
      {
        int32_T k;
        if (!std::isnan(x[0])) {
          *idx = 1;
        } else {
          boolean_T exitg1;
          *idx = 0;
          k = 2;
          exitg1 = false;
          while ((!exitg1) && (k < 51)) {
            if (!std::isnan(x[k - 1])) {
              *idx = k;
              exitg1 = true;
            } else {
              k++;
            }
          }
        }

        if (*idx == 0) {
          *ex = x[0];
          *idx = 1;
        } else {
          int32_T i;
          *ex = x[*idx - 1];
          i = *idx + 1;
          for (k = i; k < 51; k++) {
            real_T d;
            d = x[k - 1];
            if (*ex > d) {
              *ex = d;
              *idx = k;
            }
          }
        }
      }

      void minimum(const ::coder::array<real_T, 2U> &x, real_T ex[2])
      {
        int32_T m;
        m = x.size(0);
        for (int32_T j{0}; j < 2; j++) {
          ex[j] = x[x.size(0) * j];
          for (int32_T i{2}; i <= m; i++) {
            real_T d;
            d = x[(i + x.size(0) * j) - 1];
            if (b_relop(ex[j], d)) {
              ex[j] = d;
            }
          }
        }
      }
    }
  }
}

// End of code generation (minOrMax.cpp)
