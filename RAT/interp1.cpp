//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// interp1.cpp
//
// Code generation for function 'interp1'
//

// Include files
#include "interp1.h"
#include "bsearch.h"
#include "flip.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "omp.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    static void interp1Linear(const ::coder::array<real_T, 1U> &y, const ::coder::
      array<real_T, 1U> &xi, ::coder::array<real_T, 1U> &yi, const ::coder::
      array<real_T, 1U> &varargin_1);
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    static void interp1Linear(const ::coder::array<real_T, 1U> &y, const ::coder::
      array<real_T, 1U> &xi, ::coder::array<real_T, 1U> &yi, const ::coder::
      array<real_T, 1U> &varargin_1)
    {
      real_T maxx;
      real_T minx;
      real_T r;
      int32_T n;
      int32_T ub_loop;
      minx = varargin_1[0];
      maxx = varargin_1[varargin_1.size(0) - 1];
      ub_loop = xi.size(0) - 1;

#pragma omp parallel for \
 num_threads(omp_get_max_threads()) \
 private(n,r)

      for (int32_T k = 0; k <= ub_loop; k++) {
        if (std::isnan(xi[k])) {
          yi[k] = rtNaN;
        } else if ((!(xi[k] > maxx)) && (!(xi[k] < minx))) {
          n = internal::b_bsearch(varargin_1, xi[k]) - 1;
          r = (xi[k] - varargin_1[n]) / (varargin_1[n + 1] - varargin_1[n]);
          if (r == 0.0) {
            yi[k] = y[n];
          } else if (r == 1.0) {
            yi[k] = y[n + 1];
          } else if (y[n] == y[n + 1]) {
            yi[k] = y[n];
          } else {
            yi[k] = (1.0 - r) * y[n] + r * y[n + 1];
          }
        }
      }
    }

    void interp1(const ::coder::array<real_T, 1U> &varargin_1, const ::coder::
                 array<real_T, 1U> &varargin_2, const ::coder::array<real_T, 1U>
                 &varargin_3, ::coder::array<real_T, 1U> &Vq)
    {
      ::coder::array<real_T, 1U> x;
      ::coder::array<real_T, 1U> y;
      int32_T i;
      int32_T k;
      int32_T nx;
      y.set_size(varargin_2.size(0));
      k = varargin_2.size(0);
      for (i = 0; i < k; i++) {
        y[i] = varargin_2[i];
      }

      x.set_size(varargin_1.size(0));
      k = varargin_1.size(0);
      for (i = 0; i < k; i++) {
        x[i] = varargin_1[i];
      }

      nx = varargin_1.size(0) - 1;
      Vq.set_size(varargin_3.size(0));
      k = varargin_3.size(0);
      for (i = 0; i < k; i++) {
        Vq[i] = rtNaN;
      }

      if (varargin_3.size(0) != 0) {
        k = 0;
        int32_T exitg1;
        do {
          exitg1 = 0;
          if (k <= nx) {
            if (std::isnan(varargin_1[k])) {
              exitg1 = 1;
            } else {
              k++;
            }
          } else {
            if (varargin_1[1] < varargin_1[0]) {
              i = (nx + 1) >> 1;
              for (int32_T b_j1{0}; b_j1 < i; b_j1++) {
                real_T xtmp;
                xtmp = x[b_j1];
                k = nx - b_j1;
                x[b_j1] = x[k];
                x[k] = xtmp;
              }

              flip(y);
            }

            interp1Linear(y, varargin_3, Vq, x);
            exitg1 = 1;
          }
        } while (exitg1 == 0);
      }
    }
  }
}

// End of code generation (interp1.cpp)