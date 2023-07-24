//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation_types.h
//
// Code generation for function 'reflectivityCalculation'
//
#ifndef REFLECTIVITYCALCULATION_TYPES_H
#define REFLECTIVITYCALCULATION_TYPES_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#define MAX_THREADS                    omp_get_max_threads()

// Type Definitions
namespace RAT
{
  struct cell_wrap_0
  {
    real_T f1[2];
  };

  struct struct0_T
  {
    ::coder::array<real_T, 2U> contrastBacks;
    ::coder::array<real_T, 2U> contrastBacksType;
    ::coder::bounded_array<char_T, 10000U, 2U> TF;
    ::coder::array<real_T, 2U> resample;
    ::coder::array<real_T, 2U> dataPresent;
    ::coder::array<real_T, 2U> oilChiDataPresent;
    real_T numberOfContrasts;
    ::coder::bounded_array<char_T, 10000U, 2U> geometry;
    boolean_T useImaginary;
    ::coder::array<real_T, 2U> contrastShifts;
    ::coder::array<real_T, 2U> contrastScales;
    ::coder::array<real_T, 2U> contrastNbas;
    ::coder::array<real_T, 2U> contrastNbss;
    ::coder::array<real_T, 2U> contrastRes;
    ::coder::array<real_T, 2U> backs;
    ::coder::array<real_T, 2U> shifts;
    ::coder::array<real_T, 2U> sf;
    ::coder::array<real_T, 2U> nba;
    ::coder::array<real_T, 2U> nbs;
    ::coder::array<real_T, 2U> res;
    ::coder::array<real_T, 2U> params;
    real_T numberOfLayers;
    ::coder::bounded_array<char_T, 10000U, 2U> modelType;
    ::coder::array<real_T, 2U> contrastCustomFiles;
    ::coder::array<real_T, 2U> contrastDomainRatios;
    ::coder::array<real_T, 2U> domainRatio;
    real_T numberOfDomainContrasts;
    ::coder::array<real_T, 2U> domainContrastCustomFiles;
    ::coder::array<real_T, 2U> fitpars;
    ::coder::array<real_T, 2U> otherpars;
    ::coder::array<real_T, 2U> fitconstr;
    ::coder::array<real_T, 2U> otherconstr;
  };

  struct cell_wrap_1
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_wrap_2
  {
    ::coder::bounded_array<real_T, 2U, 2U> f1;
  };

  struct cell_wrap_3
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_wrap_4
  {
    ::coder::bounded_array<real_T, 10U, 2U> f1;
  };

  struct cell_wrap_5
  {
    ::coder::bounded_array<char_T, 10000U, 2U> f1;
  };

  struct cell_wrap_6
  {
    ::coder::bounded_array<char_T, 10000U, 2U> f1;
  };

  struct cell_wrap_7
  {
    cell_wrap_6 f1[3];
  };

  struct cell_8
  {
    ::coder::array<cell_wrap_0, 2U> f1;
    ::coder::array<cell_wrap_1, 2U> f2;
    ::coder::array<cell_wrap_0, 2U> f3;
    ::coder::array<cell_wrap_2, 2U> f4;
    ::coder::array<cell_wrap_3, 2U> f5;
    ::coder::array<cell_wrap_4, 1U> f6;
    ::coder::array<cell_wrap_5, 2U> f7;
    ::coder::array<cell_wrap_6, 2U> f8;
    ::coder::array<cell_wrap_6, 2U> f9;
    ::coder::array<cell_wrap_6, 2U> f10;
    ::coder::array<cell_wrap_6, 2U> f11;
    ::coder::array<cell_wrap_6, 2U> f12;
    ::coder::array<cell_wrap_6, 2U> f13;
    ::coder::array<cell_wrap_7, 2U> f14;
    ::coder::array<cell_wrap_5, 2U> f15;
    ::coder::array<cell_wrap_5, 2U> f16;
    ::coder::array<cell_wrap_1, 2U> f17;
    ::coder::array<cell_wrap_0, 2U> f18;
    ::coder::array<cell_wrap_3, 2U> f19;
    ::coder::array<cell_wrap_6, 2U> f20;
  };

  struct struct1_T
  {
    ::coder::array<real_T, 2U> params;
    ::coder::array<real_T, 2U> backs;
    ::coder::array<real_T, 2U> scales;
    ::coder::array<real_T, 2U> shifts;
    ::coder::array<real_T, 2U> nba;
    ::coder::array<real_T, 2U> nbs;
    ::coder::array<real_T, 2U> res;
    ::coder::array<real_T, 2U> domainRatio;
  };

  struct struct3_T
  {
    ::coder::array<real_T, 2U> params_fitYesNo;
    ::coder::array<real_T, 2U> backs_fitYesNo;
    ::coder::array<real_T, 2U> shifts_fitYesNo;
    ::coder::array<real_T, 2U> scales_fitYesNo;
    ::coder::array<real_T, 2U> nbairs_fitYesNo;
    ::coder::array<real_T, 2U> nbsubs_fitYesNo;
    ::coder::array<real_T, 2U> resol_fitYesNo;
    ::coder::array<real_T, 2U> domainRatio_fitYesNo;
  };

  struct struct2_T
  {
    ::coder::bounded_array<char_T, 10000U, 2U> para;
    ::coder::bounded_array<char_T, 10000U, 2U> proc;
    ::coder::bounded_array<char_T, 10000U, 2U> display;
    real_T tolX;
    real_T tolFun;
    real_T maxFunEvals;
    real_T maxIter;
    real_T populationSize;
    real_T fWeight;
    real_T F_CR;
    real_T VTR;
    real_T numGen;
    real_T strategy;
    real_T Nlive;
    real_T nmcmc;
    real_T propScale;
    real_T nsTolerance;
    real_T calcSld;
    real_T resamPars[2];
    real_T updateFreq;
    real_T updatePlotFreq;
    real_T nSamples;
    real_T nChains;
    real_T lambda;
    real_T pUnitGamma;
    ::coder::bounded_array<char_T, 8U, 2U> boundHandling;
    struct3_T checks;
  };

  struct struct5_T
  {
    ::coder::array<real_T, 1U> all_chis;
    real_T sum_chi;
  };

  struct struct4_T
  {
    ::coder::array<real_T, 1U> ssubs;
    ::coder::array<real_T, 1U> backgrounds;
    ::coder::array<real_T, 1U> qshifts;
    ::coder::array<real_T, 1U> scalefactors;
    ::coder::array<real_T, 1U> nbairs;
    ::coder::array<real_T, 1U> nbsubs;
    ::coder::array<real_T, 1U> resolutions;
    struct5_T calculations;
    ::coder::array<real_T, 1U> allSubRough;
    ::coder::array<real_T, 2U> resample;
  };

  struct cell_wrap_9
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_wrap_13
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_11
  {
    ::coder::array<cell_wrap_9, 1U> f1;
    ::coder::array<cell_wrap_9, 1U> f2;
    ::coder::array<cell_wrap_13, 1U> f3;
    ::coder::array<cell_wrap_13, 1U> f4;
    ::coder::array<cell_wrap_9, 1U> f5;
    ::coder::array<cell_wrap_13, 1U> f6;
  };
}

#endif

// End of code generation (reflectivityCalculation_types.h)
