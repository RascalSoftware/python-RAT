//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// resampleLayersReIm.cpp
//
// Code generation for function 'resampleLayersReIm'
//

// Include files
#include "resampleLayersReIm.h"
#include "adaptive.h"
#include "interp1.h"
#include "reflectivityCalculation_internal_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <algorithm>

// Function Definitions
namespace RAT
{
  void resampleLayersReIm(const ::coder::array<real_T, 2U> &sldProfile, const ::
    coder::array<real_T, 2U> &sldProfileIm, const real_T resamPars[2], ::coder::
    array<real_T, 2U> &newSLD)
  {
    ::coder::array<real_T, 1U> b_expl_temp;
    ::coder::array<real_T, 1U> b_sldProfileIm;
    ::coder::array<real_T, 1U> c_sldProfileIm;
    ::coder::array<real_T, 1U> newYIm;
    cell_24 expl_temp;
    real_T b_sldProfile[2];
    int32_T i;
    int32_T loop_ub;

    //  Resample the SLD profile. In this case we have an imaginary SLD also, and
    //  so we resample that onto the same points as the real one..
    //  Function handle for adaptive resampling
    //  f = @(x) SLDFunction(x);
    //
    //  Keep points and minangle as constants for now
    //  will fix later
    // newX = linspace(xstart,xend,100);
    b_sldProfile[0] = sldProfile[0];
    b_sldProfile[1] = sldProfile[sldProfile.size(0) - 1];
    adaptive(sldProfile, b_sldProfile, resamPars[0] * 3.1415926535897931,
             resamPars[1], &expl_temp);

    //  Now interpolate the imaginary profile so that it is on the same x points
    //  as the resampled real one....
    b_sldProfileIm.set_size(sldProfileIm.size(0));
    loop_ub = sldProfileIm.size(0);
    for (i = 0; i < loop_ub; i++) {
      b_sldProfileIm[i] = sldProfileIm[i];
    }

    c_sldProfileIm.set_size(sldProfileIm.size(0));
    loop_ub = sldProfileIm.size(0);
    for (i = 0; i < loop_ub; i++) {
      c_sldProfileIm[i] = sldProfileIm[i + sldProfileIm.size(0)];
    }

    b_expl_temp.set_size(expl_temp.f1.size(0));
    loop_ub = expl_temp.f1.size(0);
    for (i = 0; i < loop_ub; i++) {
      b_expl_temp[i] = expl_temp.f1[i];
    }

    coder::interp1(b_sldProfileIm, c_sldProfileIm, b_expl_temp, newYIm);
    newSLD.set_size(expl_temp.f1.size(0) - 1, 4);
    loop_ub = expl_temp.f1.size(0) - 1;
    for (i = 0; i < 4; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        newSLD[i1 + newSLD.size(0) * i] = 0.0;
      }
    }

    //  Now build a layer model from these resampled points
    i = expl_temp.f1.size(0);
    for (int32_T n{0}; n <= i - 2; n++) {
      real_T d;
      real_T d1;
      real_T thisLayRho;
      real_T thisLayRhoIm;
      d = expl_temp.f1[(n + expl_temp.f1.size(0)) + 1];
      d1 = expl_temp.f1[n + expl_temp.f1.size(0)];
      if (d > d1) {
        thisLayRho = (d - d1) / 2.0 + d1;
      } else {
        thisLayRho = (d1 - d) / 2.0 + d;
      }

      d = newYIm[n + 1];
      if (d > newYIm[n]) {
        thisLayRhoIm = (d - newYIm[n]) / 2.0 + newYIm[n];
      } else {
        thisLayRhoIm = (newYIm[n] - d) / 2.0 + d;
      }

      newSLD[n] = expl_temp.f1[n + 1] - expl_temp.f1[n];
      newSLD[n + newSLD.size(0)] = thisLayRho;
      newSLD[n + newSLD.size(0) * 2] = thisLayRhoIm;
      newSLD[n + newSLD.size(0) * 3] = 2.2204460492503131E-16;
    }
  }

  void resampleLayersReIm(const real_T sldProfile_data[], const int32_T
    sldProfile_size[2], const real_T sldProfileIm_data[], const int32_T
    sldProfileIm_size[2], const real_T resamPars[2], ::coder::array<real_T, 2U>
    &newSLD)
  {
    ::coder::array<real_T, 1U> b_expl_temp;
    ::coder::array<real_T, 1U> d_sldProfileIm_data;
    ::coder::array<real_T, 1U> e_sldProfileIm_data;
    ::coder::array<real_T, 1U> newYIm;
    cell_24 expl_temp;
    real_T b_sldProfileIm_data[1000];
    real_T c_sldProfileIm_data[1000];
    real_T sldProfile[2];
    int32_T i;
    int32_T loop_ub;

    //  Resample the SLD profile. In this case we have an imaginary SLD also, and
    //  so we resample that onto the same points as the real one..
    //  Function handle for adaptive resampling
    //  f = @(x) SLDFunction(x);
    //
    //  Keep points and minangle as constants for now
    //  will fix later
    // newX = linspace(xstart,xend,100);
    sldProfile[0] = sldProfile_data[0];
    sldProfile[1] = sldProfile_data[sldProfile_size[0] - 1];
    adaptive(sldProfile_data, sldProfile_size, sldProfile, resamPars[0] *
             3.1415926535897931, resamPars[1], &expl_temp);

    //  Now interpolate the imaginary profile so that it is on the same x points
    //  as the resampled real one....
    loop_ub = sldProfileIm_size[0];
    if (loop_ub - 1 >= 0) {
      std::copy(&sldProfileIm_data[0], &sldProfileIm_data[loop_ub],
                &b_sldProfileIm_data[0]);
    }

    loop_ub = sldProfileIm_size[0];
    for (i = 0; i < loop_ub; i++) {
      c_sldProfileIm_data[i] = sldProfileIm_data[i + sldProfileIm_size[0]];
    }

    b_expl_temp.set_size(expl_temp.f1.size(0));
    loop_ub = expl_temp.f1.size(0);
    for (i = 0; i < loop_ub; i++) {
      b_expl_temp[i] = expl_temp.f1[i];
    }

    d_sldProfileIm_data.set(&b_sldProfileIm_data[0], sldProfileIm_size[0]);
    e_sldProfileIm_data.set(&c_sldProfileIm_data[0], sldProfileIm_size[0]);
    coder::interp1(d_sldProfileIm_data, e_sldProfileIm_data, b_expl_temp, newYIm);
    newSLD.set_size(expl_temp.f1.size(0) - 1, 4);
    loop_ub = expl_temp.f1.size(0) - 1;
    for (i = 0; i < 4; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        newSLD[i1 + newSLD.size(0) * i] = 0.0;
      }
    }

    //  Now build a layer model from these resampled points
    i = expl_temp.f1.size(0);
    for (int32_T n{0}; n <= i - 2; n++) {
      real_T d;
      real_T d1;
      real_T thisLayRho;
      real_T thisLayRhoIm;
      d = expl_temp.f1[(n + expl_temp.f1.size(0)) + 1];
      d1 = expl_temp.f1[n + expl_temp.f1.size(0)];
      if (d > d1) {
        thisLayRho = (d - d1) / 2.0 + d1;
      } else {
        thisLayRho = (d1 - d) / 2.0 + d;
      }

      d = newYIm[n + 1];
      if (d > newYIm[n]) {
        thisLayRhoIm = (d - newYIm[n]) / 2.0 + newYIm[n];
      } else {
        thisLayRhoIm = (newYIm[n] - d) / 2.0 + d;
      }

      newSLD[n] = expl_temp.f1[n + 1] - expl_temp.f1[n];
      newSLD[n + newSLD.size(0)] = thisLayRho;
      newSLD[n + newSLD.size(0) * 2] = thisLayRhoIm;
      newSLD[n + newSLD.size(0) * 3] = 2.2204460492503131E-16;
    }
  }
}

// End of code generation (resampleLayersReIm.cpp)