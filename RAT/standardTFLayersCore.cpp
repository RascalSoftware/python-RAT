//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFLayersCore.cpp
//
// Code generation for function 'standardTFLayersCore'
//

// Include files
#include "standardTFLayersCore.h"
#include "applyBackgroundCorrection.h"
#include "callReflectivity.h"
#include "chiSquared.h"
#include "groupLayersMod.h"
#include "groupLayersModImaginary.h"
#include "makeSLDProfiles.h"
#include "resampleLayers.h"
#include "resampleLayersReIm.h"
#include "rt_nonfinite.h"
#include "shiftData.h"
#include "coder_array.h"
#include <pybind11/pybind11.h>

namespace py = pybind11;
// Function Definitions
namespace RAT
{
  void b_standardTFLayersCore(const ::coder::array<real_T, 2U> &contrastLayers,
    real_T rough, const char_T geometry_data[], const int32_T geometry_size[2],
    real_T nba, real_T nbs, real_T resample, real_T calcSld, real_T sf, real_T
    qshift, real_T dataPresent, const ::coder::array<real_T, 2U> &data, const
    real_T dataLimits[2], const real_T simLimits_data[], const real_T
    repeatLayers[2], real_T background, real_T resol, real_T backsType, real_T
    params, const real_T resamPars[2], boolean_T useImaginary, ::coder::array<
    real_T, 2U> &sldProfile, ::coder::array<real_T, 2U> &reflect, ::coder::array<
    real_T, 2U> &Simul, ::coder::array<real_T, 2U> &shifted_dat, ::coder::array<
    real_T, 2U> &theseLayers, ::coder::array<real_T, 2U> &resamLayers, real_T
    *chiSq, real_T *ssubs)
  {
    ::coder::array<real_T, 2U> b_data;
    ::coder::array<real_T, 2U> b_theseLayers;
    ::coder::array<real_T, 2U> c_theseLayers;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> sldProfileIm;
    ::coder::array<real_T, 2U> thisSldLays;
    ::coder::array<real_T, 2U> thisSldLaysIm;
    ::coder::array<real_T, 1U> d_theseLayers;
    int32_T b_loop_ub;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;

    //    This is the main reflectivity calculation for all Layers models in the
    //    standard target function.
    //
    //    The function first arranges the
    //    roughness' in the correct order according
    //    to geometry. Then, if required it calculates the SLD profile and if
    //    requested resamples this into a number of zero-roughness layers
    //    (roughness resampling). It the applies any scalefactors and qz shifts
    //    the data requires. This is done before calculating the reflectivity to
    //    ensure that the reflectivity is calculated over the same range in qz as
    //    the shifted datapoints. The main reflectivity calculation is then
    //    called, including the resolution function. The calculation outputs two
    //    profiles - 'reflect' which is the same range as the points, and
    //    'Simulation' which can be a different range to allow extrapolation.
    //    The background correction is the applied, and finally chi-squared is
    //    calculated.
    //
    //  Inputs:
    //    contrastLayers  :
    //    rough           :
    //    geometry        :
    //    nba             :
    //    nbs             :
    //    resample        :
    //    calcSld         :
    //    sf              :
    //    qshift          :
    //    dataPresent     :
    //    data            :
    //    dataLimits      :
    //    simLimits       :
    //    repeatLayers    :
    //    background      :
    //    resol           :
    //    backsType       :
    //    params          :
    //    parallelPoints  :
    //
    //  Outputs:
    //
    //  ------------------------------------------------------------------------
    //
    //        (c) Arwel Hughes  -   12/1/21
    //
    //        Last Modified: 12/1/21 by Arwel Hughes.
    //
    //  ------------------------------------------------------------------------
    //  Pre-definition for Coder..
    thisSldLaysIm.set_size(1, 2);
    sldProfileIm.set_size(1, 2);
    thisSldLaysIm[0] = 0.0;
    sldProfileIm[0] = 0.0;
    thisSldLaysIm[thisSldLaysIm.size(0)] = 0.0;
    sldProfileIm[sldProfileIm.size(0)] = 0.0;

    //  Bulid up the layers matrix for this contrast
    if (!useImaginary) {
      groupLayersMod(contrastLayers, rough, geometry_data, geometry_size, nba,
                     nbs, layerSld, ssubs);
      theseLayers.set_size(layerSld.size(0), 3);
      loop_ub = layerSld.size(0);
      for (i = 0; i < 3; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          theseLayers[i1 + theseLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    } else {
      groupLayersModImaginary(contrastLayers, rough, geometry_data,
        geometry_size, nba, nbs, layerSld, ssubs);
      theseLayers.set_size(layerSld.size(0), 4);
      loop_ub = layerSld.size(0);
      for (i = 0; i < 4; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          theseLayers[i1 + theseLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    }

    //  Make the SLD profiles.
    //  If resampling is needed, then enforce the calcSLD flag, so as to catch
    //  the error af trying to resample a non-existent profile.
    if ((resample == 1.0) && (calcSld == 0.0)) {
      calcSld = 1.0;
    }

    //  If calc SLD flag is set, then calculate the SLD profile
    if (calcSld == 1.0) {
      //  If we need them both, we process real and imaginary parts of the SLD
      //  seperately...
      if (useImaginary) {
        int32_T result;
        int8_T input_sizes_idx_1;
        int8_T sizes_idx_1;
        boolean_T empty_non_axis_sizes;
        if (theseLayers.size(1) < 4) {
          i = 0;
          i1 = 0;
        } else {
          i = 3;
          i1 = 4;
        }

        if (theseLayers.size(0) != 0) {
          result = theseLayers.size(0);
        } else {
          result = 0;
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          input_sizes_idx_1 = 2;
        } else {
          input_sizes_idx_1 = 0;
        }

        if (empty_non_axis_sizes || ((theseLayers.size(0) != 0) && (i1 - i != 0)))
        {
          sizes_idx_1 = static_cast<int8_T>(i1 - i);
        } else {
          sizes_idx_1 = 0;
        }

        b_theseLayers.set_size(theseLayers.size(0), 2);
        loop_ub = theseLayers.size(0);
        for (int32_T i2{0}; i2 < 2; i2++) {
          for (b_loop_ub = 0; b_loop_ub < loop_ub; b_loop_ub++) {
            b_theseLayers[b_loop_ub + b_theseLayers.size(0) * i2] =
              theseLayers[b_loop_ub + theseLayers.size(0) * i2];
          }
        }

        loop_ub = i1 - i;
        c_theseLayers.set_size(theseLayers.size(0), loop_ub);
        for (i1 = 0; i1 < loop_ub; i1++) {
          b_loop_ub = theseLayers.size(0);
          for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
            c_theseLayers[i2 + c_theseLayers.size(0) * i1] = theseLayers[i2 +
              theseLayers.size(0) * (i + i1)];
          }
        }

        thisSldLays.set_size(result, input_sizes_idx_1 + sizes_idx_1);
        loop_ub = input_sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * i] = b_theseLayers[i1 +
              result * i];
          }
        }

        loop_ub = sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * (i + input_sizes_idx_1)] =
              c_theseLayers[i1 + result * i];
          }
        }

        if (theseLayers.size(0) != 0) {
          result = theseLayers.size(0);
        } else {
          result = 0;
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          input_sizes_idx_1 = 1;
        } else {
          input_sizes_idx_1 = 0;
        }

        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          sizes_idx_1 = static_cast<int8_T>(theseLayers.size(1) - 2);
        } else {
          sizes_idx_1 = 0;
        }

        d_theseLayers.set_size(theseLayers.size(0));
        loop_ub = theseLayers.size(0);
        for (i = 0; i < loop_ub; i++) {
          d_theseLayers[i] = theseLayers[i];
        }

        c_theseLayers.set_size(theseLayers.size(0), theseLayers.size(1) - 2);
        loop_ub = theseLayers.size(1);
        b_loop_ub = theseLayers.size(0);
        for (i = 0; i <= loop_ub - 3; i++) {
          for (i1 = 0; i1 < b_loop_ub; i1++) {
            c_theseLayers[i1 + c_theseLayers.size(0) * i] = theseLayers[i1 +
              theseLayers.size(0) * (i + 2)];
          }
        }

        thisSldLaysIm.set_size(result, input_sizes_idx_1 + sizes_idx_1);
        loop_ub = input_sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLaysIm[i1] = d_theseLayers[i1];
          }
        }

        loop_ub = sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLaysIm[i1 + thisSldLaysIm.size(0) * (i + input_sizes_idx_1)] =
              c_theseLayers[i1 + result * i];
          }
        }
      } else {
        thisSldLays.set_size(theseLayers.size(0), theseLayers.size(1));
        loop_ub = theseLayers.size(1);
        b_loop_ub = theseLayers.size(0);
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < b_loop_ub; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * i] = theseLayers[i1 +
              theseLayers.size(0) * i];
          }
        }
      }

      makeSLDProfiles(nba, nbs, thisSldLays, *ssubs, repeatLayers, b_theseLayers);
      sldProfile.set_size(b_theseLayers.size(0), 2);
      loop_ub = b_theseLayers.size(0);
      for (i = 0; i < 2; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          sldProfile[i1 + sldProfile.size(0) * i] = b_theseLayers[i1 +
            b_theseLayers.size(0) * i];
        }
      }

      //  If we have imaginary, we are also
      //  going to need an SLD profile for the imaginary part
      if (useImaginary) {
        //  Note nba and nbs = 0 since there is never any imaginary part for
        //  the bulk phases..
        makeSLDProfiles(thisSldLaysIm, *ssubs, repeatLayers, b_theseLayers);
        sldProfileIm.set_size(b_theseLayers.size(0), 2);
        loop_ub = b_theseLayers.size(0);
        for (i = 0; i < 2; i++) {
          for (i1 = 0; i1 < loop_ub; i1++) {
            sldProfileIm[i1 + sldProfileIm.size(0) * i] = b_theseLayers[i1 +
              b_theseLayers.size(0) * i];
          }
        }
      }
    } else {
      sldProfile.set_size(1, 2);
      sldProfile[0] = 0.0;
      sldProfile[sldProfile.size(0)] = 0.0;
    }

    //  If required, then resample the SLD
    if (resample == 1.0) {
      if (!useImaginary) {
        resampleLayers(sldProfile, resamPars, layerSld);
      } else {
        resampleLayersReIm(sldProfile, sldProfileIm, resamPars, layerSld);
      }

      resamLayers.set_size(layerSld.size(0), layerSld.size(1));
      loop_ub = layerSld.size(1);
      b_loop_ub = layerSld.size(0);
      for (i = 0; i < loop_ub; i++) {
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          resamLayers[i1 + resamLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    } else {
      resamLayers.set_size(1, 3);
      resamLayers[0] = 0.0;
      resamLayers[resamLayers.size(0)] = 0.0;
      resamLayers[resamLayers.size(0) * 2] = 0.0;
    }

    //  Apply scale factors and q shifts to the data
    b_data.set_size(data.size(0), data.size(1));
    loop_ub = data.size(1) - 1;
    for (i = 0; i <= loop_ub; i++) {
      b_loop_ub = data.size(0) - 1;
      for (i1 = 0; i1 <= b_loop_ub; i1++) {
        b_data[i1 + b_data.size(0) * i] = data[i1 + data.size(0) * i];
      }
    }

    shiftData(sf, qshift, dataPresent, b_data, dataLimits, simLimits_data,
              shifted_dat);

    //  Calculate the reflectivity
    b_callReflectivity(nba, nbs, simLimits_data, repeatLayers, shifted_dat,
                       layerSld, *ssubs, resol, useImaginary, reflect, Simul);

    //  Apply background correction, either to the simulation or the data
    applyBackgroundCorrection(reflect, Simul, shifted_dat, background, backsType);

    //  Calculate chi squared.
    *chiSq = chiSquared(shifted_dat, reflect, params);
  }

  void standardTFLayersCore(const ::coder::array<real_T, 2U> &contrastLayers,
    real_T rough, const char_T geometry_data[], const int32_T geometry_size[2],
    real_T nba, real_T nbs, real_T resample, real_T calcSld, real_T sf, real_T
    qshift, real_T dataPresent, const ::coder::array<real_T, 2U> &data, const
    real_T dataLimits[2], const real_T simLimits_data[], const real_T
    repeatLayers[2], real_T background, real_T resol, real_T backsType, real_T
    params, const real_T resamPars[2], boolean_T useImaginary, ::coder::array<
    real_T, 2U> &sldProfile, ::coder::array<real_T, 2U> &reflect, ::coder::array<
    real_T, 2U> &Simul, ::coder::array<real_T, 2U> &shifted_dat, ::coder::array<
    real_T, 2U> &theseLayers, ::coder::array<real_T, 2U> &resamLayers, real_T
    *chiSq, real_T *ssubs)
  {
    ::coder::array<real_T, 2U> b_data;
    ::coder::array<real_T, 2U> b_theseLayers;
    ::coder::array<real_T, 2U> c_theseLayers;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> sldProfileIm;
    ::coder::array<real_T, 2U> thisSldLays;
    ::coder::array<real_T, 2U> thisSldLaysIm;
    ::coder::array<real_T, 1U> d_theseLayers;
    int32_T b_loop_ub;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;

    //    This is the main reflectivity calculation for all Layers models in the
    //    standard target function.
    //
    //    The function first arranges the
    //    roughness' in the correct order according
    //    to geometry. Then, if required it calculates the SLD profile and if
    //    requested resamples this into a number of zero-roughness layers
    //    (roughness resampling). It the applies any scalefactors and qz shifts
    //    the data requires. This is done before calculating the reflectivity to
    //    ensure that the reflectivity is calculated over the same range in qz as
    //    the shifted datapoints. The main reflectivity calculation is then
    //    called, including the resolution function. The calculation outputs two
    //    profiles - 'reflect' which is the same range as the points, and
    //    'Simulation' which can be a different range to allow extrapolation.
    //    The background correction is the applied, and finally chi-squared is
    //    calculated.
    //
    //  Inputs:
    //    contrastLayers  :
    //    rough           :
    //    geometry        :
    //    nba             :
    //    nbs             :
    //    resample        :
    //    calcSld         :
    //    sf              :
    //    qshift          :
    //    dataPresent     :
    //    data            :
    //    dataLimits      :
    //    simLimits       :
    //    repeatLayers    :
    //    background      :
    //    resol           :
    //    backsType       :
    //    params          :
    //    parallelPoints  :
    //
    //  Outputs:
    //
    //  ------------------------------------------------------------------------
    //
    //        (c) Arwel Hughes  -   12/1/21
    //
    //        Last Modified: 12/1/21 by Arwel Hughes.
    //
    //  ------------------------------------------------------------------------
    //  Pre-definition for Coder..
    thisSldLaysIm.set_size(1, 2);
    sldProfileIm.set_size(1, 2);
    thisSldLaysIm[0] = 0.0;
    sldProfileIm[0] = 0.0;
    thisSldLaysIm[thisSldLaysIm.size(0)] = 0.0;
    sldProfileIm[sldProfileIm.size(0)] = 0.0;

    //  Bulid up the layers matrix for this contrast
    if (!useImaginary) {
      groupLayersMod(contrastLayers, rough, geometry_data, geometry_size, nba,
                     nbs, layerSld, ssubs);
      theseLayers.set_size(layerSld.size(0), 3);
      loop_ub = layerSld.size(0);
      for (i = 0; i < 3; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          theseLayers[i1 + theseLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    } else {
      groupLayersModImaginary(contrastLayers, rough, geometry_data,
        geometry_size, nba, nbs, layerSld, ssubs);
      theseLayers.set_size(layerSld.size(0), 4);
      loop_ub = layerSld.size(0);
      for (i = 0; i < 4; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          theseLayers[i1 + theseLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    }

    //  Make the SLD profiles.
    //  If resampling is needed, then enforce the calcSLD flag, so as to catch
    //  the error af trying to resample a non-existent profile.
    if ((resample == 1.0) && (calcSld == 0.0)) {
      calcSld = 1.0;
    }

    //  If calc SLD flag is set, then calculate the SLD profile
    if (calcSld == 1.0) {
      //  If we need them both, we process real and imaginary parts of the SLD
      //  seperately...
      if (useImaginary) {
        int32_T result;
        int8_T input_sizes_idx_1;
        int8_T sizes_idx_1;
        boolean_T empty_non_axis_sizes;
        if (theseLayers.size(1) < 4) {
          i = 0;
          i1 = 0;
        } else {
          i = 3;
          i1 = 4;
        }

        if (theseLayers.size(0) != 0) {
          result = theseLayers.size(0);
        } else {
          result = 0;
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          input_sizes_idx_1 = 2;
        } else {
          input_sizes_idx_1 = 0;
        }

        if (empty_non_axis_sizes || ((theseLayers.size(0) != 0) && (i1 - i != 0)))
        {
          sizes_idx_1 = static_cast<int8_T>(i1 - i);
        } else {
          sizes_idx_1 = 0;
        }

        b_theseLayers.set_size(theseLayers.size(0), 2);
        loop_ub = theseLayers.size(0);
        for (int32_T i2{0}; i2 < 2; i2++) {
          for (b_loop_ub = 0; b_loop_ub < loop_ub; b_loop_ub++) {
            b_theseLayers[b_loop_ub + b_theseLayers.size(0) * i2] =
              theseLayers[b_loop_ub + theseLayers.size(0) * i2];
          }
        }

        loop_ub = i1 - i;
        c_theseLayers.set_size(theseLayers.size(0), loop_ub);
        for (i1 = 0; i1 < loop_ub; i1++) {
          b_loop_ub = theseLayers.size(0);
          for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
            c_theseLayers[i2 + c_theseLayers.size(0) * i1] = theseLayers[i2 +
              theseLayers.size(0) * (i + i1)];
          }
        }

        thisSldLays.set_size(result, input_sizes_idx_1 + sizes_idx_1);
        loop_ub = input_sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * i] = b_theseLayers[i1 +
              result * i];
          }
        }

        loop_ub = sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * (i + input_sizes_idx_1)] =
              c_theseLayers[i1 + result * i];
          }
        }

        if (theseLayers.size(0) != 0) {
          result = theseLayers.size(0);
        } else {
          result = 0;
        }

        empty_non_axis_sizes = (result == 0);
        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          input_sizes_idx_1 = 1;
        } else {
          input_sizes_idx_1 = 0;
        }

        if (empty_non_axis_sizes || (theseLayers.size(0) != 0)) {
          sizes_idx_1 = static_cast<int8_T>(theseLayers.size(1) - 2);
        } else {
          sizes_idx_1 = 0;
        }

        d_theseLayers.set_size(theseLayers.size(0));
        loop_ub = theseLayers.size(0);
        for (i = 0; i < loop_ub; i++) {
          d_theseLayers[i] = theseLayers[i];
        }

        c_theseLayers.set_size(theseLayers.size(0), theseLayers.size(1) - 2);
        loop_ub = theseLayers.size(1);
        b_loop_ub = theseLayers.size(0);
        for (i = 0; i <= loop_ub - 3; i++) {
          for (i1 = 0; i1 < b_loop_ub; i1++) {
            c_theseLayers[i1 + c_theseLayers.size(0) * i] = theseLayers[i1 +
              theseLayers.size(0) * (i + 2)];
          }
        }

        thisSldLaysIm.set_size(result, input_sizes_idx_1 + sizes_idx_1);
        loop_ub = input_sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLaysIm[i1] = d_theseLayers[i1];
          }
        }

        loop_ub = sizes_idx_1;
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < result; i1++) {
            thisSldLaysIm[i1 + thisSldLaysIm.size(0) * (i + input_sizes_idx_1)] =
              c_theseLayers[i1 + result * i];
          }
        }
      } else {
        thisSldLays.set_size(theseLayers.size(0), theseLayers.size(1));
        loop_ub = theseLayers.size(1);
        b_loop_ub = theseLayers.size(0);
        for (i = 0; i < loop_ub; i++) {
          for (i1 = 0; i1 < b_loop_ub; i1++) {
            thisSldLays[i1 + thisSldLays.size(0) * i] = theseLayers[i1 +
              theseLayers.size(0) * i];
          }
        }
      }

      makeSLDProfiles(nba, nbs, thisSldLays, *ssubs, repeatLayers, b_theseLayers);
      sldProfile.set_size(b_theseLayers.size(0), 2);
      loop_ub = b_theseLayers.size(0);
      for (i = 0; i < 2; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          sldProfile[i1 + sldProfile.size(0) * i] = b_theseLayers[i1 +
            b_theseLayers.size(0) * i];
        }
      }

      //  If we have imaginary, we are also
      //  going to need an SLD profile for the imaginary part
      if (useImaginary) {
        //  Note nba and nbs = 0 since there is never any imaginary part for
        //  the bulk phases..
        makeSLDProfiles(thisSldLaysIm, *ssubs, repeatLayers, b_theseLayers);
        sldProfileIm.set_size(b_theseLayers.size(0), 2);
        loop_ub = b_theseLayers.size(0);
        for (i = 0; i < 2; i++) {
          for (i1 = 0; i1 < loop_ub; i1++) {
            sldProfileIm[i1 + sldProfileIm.size(0) * i] = b_theseLayers[i1 +
              b_theseLayers.size(0) * i];
          }
        }
      }
    } else {
      sldProfile.set_size(1, 2);
      sldProfile[0] = 0.0;
      sldProfile[sldProfile.size(0)] = 0.0;
    }

    //  If required, then resample the SLD
    if (resample == 1.0) {
      if (!useImaginary) {
        resampleLayers(sldProfile, resamPars, layerSld);
      } else {
        resampleLayersReIm(sldProfile, sldProfileIm, resamPars, layerSld);
      }

      resamLayers.set_size(layerSld.size(0), layerSld.size(1));
      loop_ub = layerSld.size(1);
      b_loop_ub = layerSld.size(0);
      for (i = 0; i < loop_ub; i++) {
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          resamLayers[i1 + resamLayers.size(0) * i] = layerSld[i1 +
            layerSld.size(0) * i];
        }
      }
    } else {
      resamLayers.set_size(1, 3);
      resamLayers[0] = 0.0;
      resamLayers[resamLayers.size(0)] = 0.0;
      resamLayers[resamLayers.size(0) * 2] = 0.0;
    }

    //  Apply scale factors and q shifts to the data
    b_data.set_size(data.size(0), data.size(1));
    loop_ub = data.size(1) - 1;
    for (i = 0; i <= loop_ub; i++) {
      b_loop_ub = data.size(0) - 1;
      for (i1 = 0; i1 <= b_loop_ub; i1++) {
        b_data[i1 + b_data.size(0) * i] = data[i1 + data.size(0) * i];
      }
    }

    shiftData(sf, qshift, dataPresent, b_data, dataLimits, simLimits_data,
              shifted_dat);

    //  Calculate the reflectivity
    callReflectivity(nba, nbs, simLimits_data, repeatLayers, shifted_dat,
                     layerSld, *ssubs, resol, useImaginary, reflect, Simul);

    //  Apply background correction, either to the simulation or the data
    applyBackgroundCorrection(reflect, Simul, shifted_dat, background, backsType);

    //  Calculate chi squared.
    *chiSq = chiSquared(shifted_dat, reflect, params);
  }
}

// End of code generation (standardTFLayersCore.cpp)
