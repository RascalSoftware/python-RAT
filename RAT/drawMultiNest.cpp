//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// drawMultiNest.cpp
//
// Code generation for function 'drawMultiNest'
//

// Include files
#include "drawMultiNest.h"
#include "RATMain_data.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "drawEllipsoidPoints.h"
#include "inEllipsoids.h"
#include "nsIntraFun.h"
#include "rand.h"
#include "rescaleParameters.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <stdio.h>

// Function Definitions
namespace RAT
{
  void drawMultiNest(const ::coder::array<real_T, 1U> &fracvol, const ::coder::
                     array<real_T, 2U> &Bs, const ::coder::array<real_T, 2U>
                     &mus, real_T logLmin, const ::coder::array<real_T, 2U>
                     &prior, const struct5_T *data_f1, const struct2_T *data_f2,
                     const cell_14 *data_f4, ::coder::array<real_T, 2U> &sample,
                     real_T *logL)
  {
    ::coder::array<real_T, 2U> B;
    ::coder::array<real_T, 2U> mu;
    ::coder::array<real_T, 2U> pnt;
    ::coder::array<real_T, 1U> r;
    int32_T b_loop_ub;
    int32_T loop_ub;
    int32_T ndims;

    //  This function draws a multi-dimensional sample from the prior volume
    //  for use in the nested sampling algorithm. The new point will have a
    //  likelihood greater than the value logLmin. The new point will be found by
    //  drawing a random multi-dimensional sample from within the set of optimal
    //  ellipsoids constructed using the MultiNest algorithm.  The bounding
    //  ellipsoids are defined by their bounding matrices Bs and centroids mus.
    //  extraparvals is a vector of additional parameters needed by the model.
    //
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    //  extra number of ellipsoids, number of dimensions
    ndims = mus.size(1);
    pnt.set_size(1, 2);
    pnt[0] = 1.0;
    pnt[1] = 1.0;
    sample.set_size(1, 0);
    loop_ub = Bs.size(1);
    b_loop_ub = mus.size(1);
    real_T inN;
    real_T rval;
    int32_T k;
    int32_T k0;
    boolean_T exitg1;
    do {
      int32_T c_loop_ub;
      int32_T i;
      int32_T i1;

      //  find the ellipsoid from which to draw a new point
      rval = coder::b_rand();
      k0 = 0;
      k = 0;
      exitg1 = false;
      while ((!exitg1) && (k <= mus.size(0) - 1)) {
        k0 = k;
        if (rval < fracvol[k]) {
          k++;
        } else {
          exitg1 = true;
        }
      }

      //  extract bounding matrix and centroid for that ellipsoid
      i = k0 * ndims;
      i1 = (k0 + 1) * ndims;
      if (i + 1 > i1) {
        i = 0;
        i1 = 0;
      }

      c_loop_ub = i1 - i;
      B.set_size(c_loop_ub, Bs.size(1));
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (int32_T i2{0}; i2 < c_loop_ub; i2++) {
          B[i2 + B.size(0) * i1] = Bs[(i + i2) + Bs.size(0) * i1];
        }
      }

      mu.set_size(1, mus.size(1));
      for (i = 0; i < b_loop_ub; i++) {
        mu[i] = mus[k0 + mus.size(0) * i];
      }

      //  draw points from that ellipsoid until logL >= logLmin
      *logL = rtMinusInf;
      while (*logL < logLmin) {
        int32_T in_range;
        in_range = 1;

        //  default value
        //  draw one point from the ellipsoid
        drawEllipsoidPoints(B, mu, pnt);

        //  make sure that the point lies in unit hypercube
        for (int32_T ii{0}; ii < ndims; ii++) {
          real_T d;
          d = pnt[ii];
          if ((d < 0.0) || (d > 1.0)) {
            in_range = 0;
            if (DEBUG != 0.0) {
              printf("new point not in range!!!!\n");
              fflush(stdout);
            }
          }
        }

        if (in_range != 0) {
          //  assign as candidate replacement live point
          sample.set_size(1, pnt.size(1));
          c_loop_ub = pnt.size(1);
          for (i = 0; i < c_loop_ub; i++) {
            sample[sample.size(0) * i] = pnt[i];
          }

          //  rescale point back to full range
          //  get new likelihood
          //  logL = likelihood(data, model, parnames, loopCell(rescaledpnt));
          rescaleParameters(prior, pnt, r);
          *logL = nsIntraFun(data_f1, data_f2, data_f4, r);
        }
      }

      //  check how many ellipsoids this point lies in
      inN = inEllipsoids(pnt, Bs, mus);

      //  only accept sample with 1/inN probability
    } while (!(coder::b_rand() < 1.0 / inN));
  }
}

// End of code generation (drawMultiNest.cpp)
