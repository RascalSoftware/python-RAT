//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// parseResultToStruct.h
//
// Code generation for function 'parseResultToStruct'
//
#ifndef PARSERESULTTOSTRUCT_H
#define PARSERESULTTOSTRUCT_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct d_struct_T;
}

// Function Declarations
namespace RAT
{
  void parseResultToStruct(const d_struct_T *contrastParams, const cell_wrap_9
    resultCells[6], ::coder::array<cell_wrap_8, 2U> &result_reflectivity, ::
    coder::array<cell_wrap_8, 2U> &result_simulation, ::coder::array<cell_wrap_8,
    2U> &result_shiftedData, ::coder::array<cell_wrap_8, 2U> &result_layerSlds, ::
    coder::array<cell_wrap_8, 2U> &result_sldProfiles, ::coder::array<
    cell_wrap_8, 2U> &result_allLayers, real_T *result_calculationResults_sumChi,
    d_struct_T *result_contrastParams);
}

#endif

// End of code generation (parseResultToStruct.h)
