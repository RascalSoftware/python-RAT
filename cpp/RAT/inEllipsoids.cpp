//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// inEllipsoids.cpp
//
// Code generation for function 'inEllipsoids'
//

// Include files
#include "inEllipsoids.h"
#include "RATMain_data.h"
#include "diag.h"
#include "eig.h"
#include "rt_nonfinite.h"
#include "sqrt.h"
#include "sum.h"
#include "unsafeSxfun.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  real_T inEllipsoids(const ::coder::array<real_T, 2U> &pnt, const ::coder::
                      array<real_T, 2U> &Bs, const ::coder::array<real_T, 2U>
                      &mus)
  {
    ::coder::array<creal_T, 2U> E;
    ::coder::array<creal_T, 2U> V;
    ::coder::array<creal_T, 2U> b_pnt;
    ::coder::array<creal_T, 2U> c_pnt;
    ::coder::array<creal_T, 1U> r;
    ::coder::array<real_T, 2U> b_Bs;
    creal_T varargin_1;
    real_T N;
    int32_T b_loop_ub;
    int32_T i;
    int32_T loop_ub;
    int32_T ndims;

    //  function N = inEllipsoids(pnt, Bs, mus)
    //
    //  This function works out how many of the ellipsoids (defined by the
    //  bounding matrices Bs and centroids mus) contain the point pnt.
    //  This number is returned in N.
    //
    //  Bs is a [(Kxndims) x ndims] array, where K=total number of ellipsoids
    //  and ndims = dimension of the parameter space.
    //  mus is a [K x ndims] array.
    //  pnt is a ndims-dimensional vector.
    //
    //  NOTE: in the future it may be quicker to input precalculated eigenvalues
    //  and eigenvectors into this function rather than the bounding matrices
    //
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    N = 0.0;

    //  total number of ellipsoids and number of dimensions
    ndims = mus.size(1);

    //  loop over number of ellipsiods and work out whether it contains the point
    i = mus.size(0);
    if (i - 1 >= 0) {
      loop_ub = Bs.size(1);
      b_loop_ub = pnt.size(1);
    }

    for (int32_T k{0}; k < i; k++) {
      int32_T b_k;
      int32_T c_loop_ub;
      int32_T i1;
      int32_T i2;

      //  set the point to have the same origin as the ellipsoid
      //  extract the bounding matrix
      i1 = k * ndims;
      i2 = (k + 1) * ndims;
      if (i1 + 1 > i2) {
        i1 = 0;
        i2 = 0;
      }

      //  get the eigenvalues and eigenvectors of the ellipsoid
      c_loop_ub = i2 - i1;
      b_Bs.set_size(c_loop_ub, Bs.size(1));
      for (i2 = 0; i2 < loop_ub; i2++) {
        for (b_k = 0; b_k < c_loop_ub; b_k++) {
          b_Bs[b_k + b_Bs.size(0) * i2] = Bs[(i1 + b_k) + Bs.size(0) * i2];
        }
      }

      coder::eig(b_Bs, V, E);

      //  V is matrix of eigenvectors (as columns)
      //  rotate points to be on coordinate axes of the ellipsiod
      //  scale points so that it's equivalent to having unit hyper-spheroids
      //  rather than ellipsiods
      //  get distance to point from centre of hyper-sphere
      coder::diag(E, r);
      i1 = r.size(0);
      for (b_k = 0; b_k < i1; b_k++) {
        coder::internal::scalar::d_sqrt(&r[b_k]);
      }

      if ((pnt.size(1) == mus.size(1)) && (r.size(0) == V.size(1))) {
        real_T pnt_re_tmp;
        c_pnt.set_size(1, pnt.size(1));
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          c_pnt[i1].re = pnt[i1] - mus[k + mus.size(0) * i1];
          c_pnt[i1].im = 0.0;
        }

        c_loop_ub = V.size(1);
        b_pnt.set_size(1, V.size(1));
        for (i1 = 0; i1 < c_loop_ub; i1++) {
          real_T b_pnt_re_tmp;
          real_T c_pnt_re_tmp;
          real_T d_pnt_re_tmp;
          real_T im;
          real_T re;
          re = 0.0;
          im = 0.0;
          b_k = c_pnt.size(1);
          for (i2 = 0; i2 < b_k; i2++) {
            pnt_re_tmp = c_pnt[i2].re;
            d_pnt_re_tmp = V[i2 + V.size(0) * i1].im;
            b_pnt_re_tmp = c_pnt[i2].im;
            c_pnt_re_tmp = V[i2 + V.size(0) * i1].re;
            re += pnt_re_tmp * c_pnt_re_tmp - b_pnt_re_tmp * d_pnt_re_tmp;
            im += pnt_re_tmp * d_pnt_re_tmp + b_pnt_re_tmp * c_pnt_re_tmp;
          }

          b_pnt_re_tmp = r[i1].re;
          c_pnt_re_tmp = -r[i1].im;
          if (c_pnt_re_tmp == 0.0) {
            if (im == 0.0) {
              b_pnt[i1].re = re / b_pnt_re_tmp;
              b_pnt[i1].im = 0.0;
            } else if (re == 0.0) {
              b_pnt[i1].re = 0.0;
              b_pnt[i1].im = im / b_pnt_re_tmp;
            } else {
              b_pnt[i1].re = re / b_pnt_re_tmp;
              b_pnt[i1].im = im / b_pnt_re_tmp;
            }
          } else if (b_pnt_re_tmp == 0.0) {
            if (re == 0.0) {
              b_pnt[i1].re = im / c_pnt_re_tmp;
              b_pnt[i1].im = 0.0;
            } else if (im == 0.0) {
              b_pnt[i1].re = 0.0;
              b_pnt[i1].im = -(re / c_pnt_re_tmp);
            } else {
              b_pnt[i1].re = im / c_pnt_re_tmp;
              b_pnt[i1].im = -(re / c_pnt_re_tmp);
            }
          } else {
            real_T brm;
            brm = std::abs(b_pnt_re_tmp);
            pnt_re_tmp = std::abs(c_pnt_re_tmp);
            if (brm > pnt_re_tmp) {
              d_pnt_re_tmp = c_pnt_re_tmp / b_pnt_re_tmp;
              pnt_re_tmp = b_pnt_re_tmp + d_pnt_re_tmp * c_pnt_re_tmp;
              b_pnt[i1].re = (re + d_pnt_re_tmp * im) / pnt_re_tmp;
              b_pnt[i1].im = (im - d_pnt_re_tmp * re) / pnt_re_tmp;
            } else if (pnt_re_tmp == brm) {
              if (b_pnt_re_tmp > 0.0) {
                d_pnt_re_tmp = 0.5;
              } else {
                d_pnt_re_tmp = -0.5;
              }

              if (c_pnt_re_tmp > 0.0) {
                pnt_re_tmp = 0.5;
              } else {
                pnt_re_tmp = -0.5;
              }

              b_pnt[i1].re = (re * d_pnt_re_tmp + im * pnt_re_tmp) / brm;
              b_pnt[i1].im = (im * d_pnt_re_tmp - re * pnt_re_tmp) / brm;
            } else {
              d_pnt_re_tmp = b_pnt_re_tmp / c_pnt_re_tmp;
              pnt_re_tmp = c_pnt_re_tmp + d_pnt_re_tmp * b_pnt_re_tmp;
              b_pnt[i1].re = (d_pnt_re_tmp * re + im) / pnt_re_tmp;
              b_pnt[i1].im = (d_pnt_re_tmp * im - re) / pnt_re_tmp;
            }
          }
        }

        b_pnt.set_size(1, b_pnt.size(1));
        c_loop_ub = b_pnt.size(1);
        for (i1 = 0; i1 < c_loop_ub; i1++) {
          creal_T varargout_1;
          varargin_1 = b_pnt[i1];
          varargout_1.re = varargin_1.re * varargin_1.re - varargin_1.im *
            varargin_1.im;
          pnt_re_tmp = varargin_1.re * varargin_1.im;
          varargout_1.im = pnt_re_tmp + pnt_re_tmp;
          b_pnt[i1] = varargout_1;
        }
      } else {
        binary_expand_op(b_pnt, pnt, mus, k, V, r);
      }

      varargin_1 = coder::sum(b_pnt);
      coder::internal::scalar::d_sqrt(&varargin_1);
      if (varargin_1.re <= 1.0) {
        //  values is within the ellipsiod
        N++;
      }
    }

    return N;
  }
}

// End of code generation (inEllipsoids.cpp)
