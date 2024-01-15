//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nsIntraFun.cpp
//
// Code generation for function 'nsIntraFun'
//

// Include files
#include "nsIntraFun.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "unpackParams.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  real_T nsIntraFun(const struct5_T *data_f1, const struct2_T *data_f2, const
                    cell_14 *data_f4, const ::coder::array<real_T, 2U> &p)
  {
    cell_13 a__1;
    struct5_T problemDef;
    struct_T b_problemDef;
    int32_T loop_ub;
    problemDef = *data_f1;

    //  Removed use of cells....
    problemDef.fitParams.set_size(1, p.size(1));
    loop_ub = p.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      problemDef.fitParams[problemDef.fitParams.size(0) * i] = p[i];
    }

    unpackParams(&problemDef, data_f2->checks.fitParam,
                 data_f2->checks.fitBackgroundParam, data_f2->checks.fitQzshift,
                 data_f2->checks.fitScalefactor, data_f2->checks.fitBulkIn,
                 data_f2->checks.fitBulkOut, data_f2->checks.fitResolutionParam,
                 data_f2->checks.fitDomainRatio);
    reflectivityCalculation(&problemDef, data_f4, data_f2, &b_problemDef, &a__1);
    return -b_problemDef.calculations.sumChi / 2.0;
  }

  real_T nsIntraFun(const struct5_T *data_f1, const struct2_T *data_f2, const
                    cell_14 *data_f4, const ::coder::array<real_T, 1U> &p)
  {
    cell_13 a__1;
    struct5_T problemDef;
    struct_T b_problemDef;
    int32_T p_idx_0;
    problemDef = *data_f1;

    //  Removed use of cells....
    p_idx_0 = p.size(0);
    problemDef.fitParams.set_size(p.size(0), 1);
    for (int32_T i{0}; i < p_idx_0; i++) {
      problemDef.fitParams[i] = p[i];
    }

    unpackParams(&problemDef, data_f2->checks.fitParam,
                 data_f2->checks.fitBackgroundParam, data_f2->checks.fitQzshift,
                 data_f2->checks.fitScalefactor, data_f2->checks.fitBulkIn,
                 data_f2->checks.fitBulkOut, data_f2->checks.fitResolutionParam,
                 data_f2->checks.fitDomainRatio);
    reflectivityCalculation(&problemDef, data_f4, data_f2, &b_problemDef, &a__1);
    return -b_problemDef.calculations.sumChi / 2.0;
  }
}

// End of code generation (nsIntraFun.cpp)
