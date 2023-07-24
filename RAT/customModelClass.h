//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// customModelClass.h
//
// Code generation for function 'customModelClass'
//
#ifndef CUSTOMMODELCLASS_H
#define CUSTOMMODELCLASS_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_wrap_7;
  struct cell_wrap_15;
}

// Type Definitions
namespace RAT
{
  class customModelClass
  {
   public:
    static void processCustomLayers(const ::coder::array<real_T, 2U> &cBacks,
      const ::coder::array<real_T, 2U> &cShifts, const ::coder::array<real_T, 2U>
      &cScales, const ::coder::array<real_T, 2U> &cNbas, const ::coder::array<
      real_T, 2U> &cNbss, const ::coder::array<real_T, 2U> &cRes, const ::coder::
      array<real_T, 2U> &backs, const ::coder::array<real_T, 2U> &shifts, const ::
      coder::array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba, const
      ::coder::array<real_T, 2U> &nbs, const ::coder::array<real_T, 2U> &res,
      const ::coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts,
      const ::coder::array<cell_wrap_7, 2U> &customFiles, const ::coder::array<
      real_T, 2U> &params, boolean_T useImaginary, ::coder::array<cell_wrap_15,
      1U> &allLayers, ::coder::array<real_T, 1U> &allRoughs);
    static void processCustomXY(const ::coder::array<real_T, 2U> &cBacks, const ::
      coder::array<real_T, 2U> &cShifts, const ::coder::array<real_T, 2U>
      &cScales, const ::coder::array<real_T, 2U> &cNbas, const ::coder::array<
      real_T, 2U> &cNbss, const ::coder::array<real_T, 2U> &cRes, const ::coder::
      array<real_T, 2U> &backs, const ::coder::array<real_T, 2U> &shifts, const ::
      coder::array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba, const
      ::coder::array<real_T, 2U> &nbs, const ::coder::array<real_T, 2U> &res,
      const ::coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts,
      const ::coder::array<cell_wrap_7, 2U> &customFiles, const ::coder::array<
      real_T, 2U> &params, ::coder::array<cell_wrap_15, 1U> &allSLDs, ::coder::
      array<real_T, 1U> &allRoughs);
  };
}

#endif

// End of code generation (customModelClass.h)
