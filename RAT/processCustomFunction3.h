//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processCustomFunction3.h
//
// Code generation for function 'processCustomFunction3'
//
#ifndef PROCESSCUSTOMFUNCTION3_H
#define PROCESSCUSTOMFUNCTION3_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace domainsTF
  {
    namespace customXY
    {
      void processCustomFunction(const ::coder::array<real_T, 2U>
        &contrastBulkIns, const ::coder::array<real_T, 2U> &contrastBulkOuts,
        const ::coder::array<real_T, 2U> &bulkIn, const ::coder::array<real_T,
        2U> &bulkOut, const ::coder::array<real_T, 2U> &cCustFiles, real_T
        numberOfContrasts, const ::coder::array<cell_wrap_1, 2U> &customFiles,
        const ::coder::array<real_T, 2U> &params, ::coder::array<cell_wrap_8, 2U>
        &allSLDs, ::coder::array<real_T, 1U> &allRoughs);
    }
  }
}

#endif

// End of code generation (processCustomFunction3.h)
