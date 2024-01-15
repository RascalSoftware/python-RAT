//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// RATMain_internal_types.h
//
// Code generation for function 'RATMain'
//
#ifndef RATMAIN_INTERNAL_TYPES_H
#define RATMAIN_INTERNAL_TYPES_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Type Definitions
namespace RAT
{
  struct cell_13
  {
    ::coder::array<cell_wrap_11, 1U> f1;
    ::coder::array<cell_wrap_11, 1U> f2;
    ::coder::array<cell_wrap_20, 1U> f3;
    ::coder::array<cell_wrap_8, 2U> f4;
    ::coder::array<cell_wrap_8, 2U> f5;
    ::coder::array<cell_wrap_8, 2U> f6;
  };

  struct struct_T
  {
    ::coder::array<real_T, 1U> ssubs;
    ::coder::array<real_T, 1U> backgroundParams;
    ::coder::array<real_T, 1U> qzshifts;
    ::coder::array<real_T, 1U> scalefactors;
    ::coder::array<real_T, 1U> bulkIn;
    ::coder::array<real_T, 1U> bulkOut;
    ::coder::array<real_T, 1U> resolutionParams;
    struct7_T calculations;
    ::coder::array<real_T, 1U> allSubRough;
    ::coder::array<real_T, 2U> resample;
  };

  struct b_struct_T
  {
    real_T LogZ;
    ::coder::bounded_array<real_T, 2U, 2U> nestSamples;
    ::coder::bounded_array<real_T, 2U, 2U> postSamples;
  };

  struct c_struct_T
  {
    ::coder::array<real_T, 3U> allChains;
    struct13_T dreamOutput;
    b_struct_T nestOutput;
  };

  struct d_struct_T
  {
    struct9_T bestFitsMean;
    struct10_T predlims;
    struct11_T parConfInts;
    ::coder::array<real_T, 2U> bestPars;
    c_struct_T bayesRes;
    ::coder::array<real_T, 2U> chain;
  };

  struct cell_14
  {
    ::coder::array<cell_wrap_2, 2U> f1;
    ::coder::array<cell_wrap_8, 2U> f2;
    ::coder::array<cell_wrap_2, 2U> f3;
    ::coder::array<cell_wrap_2, 2U> f4;
    ::coder::array<cell_wrap_8, 2U> f5;
    ::coder::array<cell_wrap_8, 1U> f6;
    ::coder::array<cell_wrap_1, 2U> f7;
    ::coder::array<cell_wrap_1, 2U> f8;
    ::coder::array<cell_wrap_1, 2U> f9;
    ::coder::array<cell_wrap_1, 2U> f10;
    ::coder::array<cell_wrap_1, 2U> f11;
    ::coder::array<cell_wrap_1, 2U> f12;
    ::coder::array<cell_wrap_1, 2U> f13;
    ::coder::array<cell_wrap_1, 2U> f14;
    ::coder::array<cell_wrap_1, 2U> f15;
    ::coder::array<cell_wrap_1, 2U> f16;
    ::coder::array<cell_wrap_8, 2U> f17;
    ::coder::array<cell_wrap_2, 2U> f18;
    ::coder::array<cell_wrap_8, 2U> f19;
    ::coder::array<cell_wrap_1, 2U> f20;
  };

  struct cell_wrap_15
  {
    ::coder::array<cell_wrap_8, 1U> f1;
  };

  struct cell_wrap_17
  {
    ::coder::array<real_T, 1U> f1;
  };

  struct cell_wrap_36
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_wrap_37
  {
    cell_wrap_8 f1[2];
  };

  struct e_struct_T
  {
    ::coder::array<cell_wrap_11, 1U> ref;
    ::coder::array<cell_wrap_8, 2U> sld;
    real_T chi;
    ::coder::array<cell_wrap_20, 1U> data;
  };

  struct f_struct_T
  {
    ::coder::array<cell_wrap_50, 1U> refPredInts;
    ::coder::array<cell_wrap_50, 1U> sldPredInts;
    ::coder::array<cell_wrap_24, 1U> refXdata;
    ::coder::array<cell_wrap_24, 1U> sldXdata;
    real_T sampleChi[1000];
  };

  struct h_struct_T
  {
    real_T iterations;
    real_T funcCount;
    char_T algorithm[33];
    ::coder::array<char_T, 2U> message;
  };

  struct i_struct_T
  {
    ::coder::bounded_array<real_T, 1U, 1U> I_lentol;
    ::coder::bounded_array<real_T, 50U, 2U> FVr_x;
    ::coder::bounded_array<real_T, 50U, 2U> FVr_lim_up;
    ::coder::bounded_array<real_T, 50U, 2U> FVr_lim_lo;
    real_T I_NP;
    real_T fWeight;
    real_T F_CR;
    real_T I_D;
    ::coder::array<real_T, 2U> FVr_minbound;
    ::coder::array<real_T, 2U> FVr_maxbound;
    real_T I_bnd_constr;
    real_T I_itermax;
    real_T F_VTR;
    real_T I_strategy;
    real_T I_refresh;
    real_T I_plotting;
    ::coder::array<real_T, 2U> FM_pop;
    ::coder::array<real_T, 2U> FVr_bestmem;
  };

  struct cell_27
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct j_struct_T
  {
    ::coder::array<real_T, 1U> LB;
    ::coder::array<real_T, 1U> UB;
    ::coder::array<real_T, 1U> BoundClass;
  };

  struct k_struct_T
  {
    real_T I_no;
    real_T FVr_oa;
  };
}

#endif

// End of code generation (RATMain_internal_types.h)
