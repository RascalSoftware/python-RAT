//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// calculate5.h
//
// Code generation for function 'calculate5'
//
#ifndef CALCULATE5_H
#define CALCULATE5_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_14;
  struct struct_T;
}

// Function Declarations
namespace RAT
{
  namespace domainsTF
  {
    namespace customXY
    {
      void calculate(const struct5_T *problemDef, const cell_14 *problemDefCells,
                     const struct2_T *controls, struct_T *problem, ::coder::
                     array<cell_wrap_11, 1U> &reflectivity, ::coder::array<
                     cell_wrap_11, 1U> &simulation, ::coder::array<cell_wrap_8,
                     1U> &shiftedData, ::coder::array<cell_wrap_8, 2U>
                     &layerSlds, ::coder::array<cell_wrap_8, 2U>
                     &domainSldProfiles, ::coder::array<cell_wrap_8, 2U>
                     &allLayers);
    }
  }
}

#endif

// End of code generation (calculate5.h)
