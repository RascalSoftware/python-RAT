//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// backSort.cpp
//
// Code generation for function 'backSort'
//

// Include files
#include "backSort.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void backSort(real_T cBacks, real_T cShifts, real_T cScales, real_T cNbas,
                real_T cNbss, real_T cRes, const ::coder::array<real_T, 2U>
                &backs, const ::coder::array<real_T, 2U> &shifts, const ::coder::
                array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba,
                const ::coder::array<real_T, 2U> &nbs, const ::coder::array<
                real_T, 2U> &res, real_T *backg, real_T *qshift, real_T *b_sf,
                real_T *b_nba, real_T *b_nbs, real_T *resol)
  {
    //      Distributes the background and shift values among the different contrasts
    //
    //      USAGE::
    //
    //          [backg,qshift,sf,nba,nbs,resol] = backsort(cBacks,cShifts,cScales,cNbas,cNbss,cRes,backs,shifts,sf,nba,nbs,res)
    //
    //      INPUTS:
    //         * cBacks  :         Which backround value is associated with each contrast
    //         * cShifts  :        Which qz_shift value is associated with each contrast
    //         * cScales  :        Which scalefactor value is associated with each contrast
    //         * cNbas  :          Which NBa value is associated with each contrast
    //         * cNbss  :          Which Nbs value is associated with each contrast
    //         * cRes  :           Which resolution value is associated with each contrast
    //         * backs  :          List of all background values.
    //         * shifts  :         List of all qz-shift values
    //         * sf :              List of all scalefactor values
    //         * nba :             List of all nba values
    //         * nbs :             List of all nbs values
    //         * res :             List of all resolution values
    //
    //      OUTPUTS:
    //         * backgs : list of actual background values for each contrast
    //         * qshifts : list of actual shift values for each contrast
    //         * sfs : list of actual shift values for each contrast
    //         * nbas : list of actual shift values for each contrast
    //         * nbss : list of actual shift values for each contrast
    //         * nbss : list of actual shift values for each contrast
    //
    // for i = 1:nc
    // thisBack = cBacks(i);
    *backg = backs[static_cast<int32_T>(cBacks) - 1];

    // thisShift = cShifts(i);
    *qshift = shifts[static_cast<int32_T>(cShifts) - 1];

    // thisScale = cScales(i);
    *b_sf = sf[static_cast<int32_T>(cScales) - 1];

    // thisNbair = cNbas(i);
    *b_nba = nba[static_cast<int32_T>(cNbas) - 1];

    // thisNbsub = cNbss(i);
    *b_nbs = nbs[static_cast<int32_T>(cNbss) - 1];

    // thisResol = cRes(i);
    if (cRes != -1.0) {
      *resol = res[static_cast<int32_T>(cRes) - 1];
    } else {
      *resol = -1.0;

      //  Negative value means we have a data resolution..
    }

    // end
  }
}

// End of code generation (backSort.cpp)
