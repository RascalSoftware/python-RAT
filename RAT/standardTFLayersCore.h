//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFLayersCore.h
//
// Code generation for function 'standardTFLayersCore'
//
#ifndef STANDARDTFLAYERSCORE_H
#define STANDARDTFLAYERSCORE_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
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
    *chiSq, real_T *ssubs);
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
    *chiSq, real_T *ssubs);
}

#endif

// End of code generation (standardTFLayersCore.h)
