//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// ratDREAM.h
//
// Code generation for function 'ratDREAM'
//
#ifndef RATDREAM_H
#define RATDREAM_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct struct5_T;
  struct cell_14;
  struct struct2_T;
  struct struct13_T;
}

// Function Declarations
namespace RAT
{
  void ratDREAM(real_T dreamVariables_d, real_T dreamVariables_N, real_T
                dreamVariables_T, real_T dreamVariables_lambda, real_T
                dreamVariables_pUnitGamma, boolean_T dreamVariables_adaptPCR,
                const ::coder::array<real_T, 2U> &Par_info_min, const ::coder::
                array<real_T, 2U> &Par_info_max, const char_T
                Par_info_boundhandling_data[], const int32_T
                Par_info_boundhandling_size[2], const struct5_T
                *ratInputs_problemDef, const cell_14 *ratInputs_problemDefCells,
                const struct2_T *ratInputs_controls, const ::coder::array<real_T,
                2U> &ratInputs_priors, ::coder::array<real_T, 3U> &chain,
                struct13_T *output, ::coder::array<real_T, 2U> &fx);
}

#endif

// End of code generation (ratDREAM.h)
