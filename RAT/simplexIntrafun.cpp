//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// simplexIntrafun.cpp
//
// Code generation for function 'simplexIntrafun'
//

// Include files
#include "simplexIntrafun.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "simplexXTransform.h"
#include "unpackParams.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void simplexIntrafun(const ::coder::array<real_T, 1U> &x, const struct5_T
                       *problemDef, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f1, const ::coder::array<cell_wrap_8, 2U>
                       &problemDefCells_f2, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f3, const ::coder::array<cell_wrap_2, 2U>
                       &problemDefCells_f4, const ::coder::array<cell_wrap_8, 2U>
                       &problemDefCells_f5, const ::coder::array<cell_wrap_8, 1U>
                       &problemDefCells_f6, const ::coder::array<cell_wrap_1, 2U>
                       &problemDefCells_f14, const ::coder::array<cell_wrap_8,
                       2U> &problemDefCells_f19, const struct2_T *controls,
                       const j_struct_T *params, real_T *fval, struct_T
                       *b_problemDef, cell_13 *result)
  {
    ::coder::array<real_T, 1U> xtrans;
    cell_14 expl_temp;
    struct5_T c_problemDef;
    int32_T xtrans_idx_0;
    c_problemDef = *problemDef;

    //  transform variables, then call original function
    simplexXTransform(x, params->LB, params->UB, params->BoundClass, xtrans);

    // Unpck the params..
    xtrans_idx_0 = xtrans.size(0);
    c_problemDef.fitParams.set_size(xtrans.size(0), 1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      c_problemDef.fitParams[i] = xtrans[i];
    }

    unpackParams(&c_problemDef, controls->checks.fitParam,
                 controls->checks.fitBackgroundParam,
                 controls->checks.fitQzshift, controls->checks.fitScalefactor,
                 controls->checks.fitBulkIn, controls->checks.fitBulkOut,
                 controls->checks.fitResolutionParam,
                 controls->checks.fitDomainRatio);
    expl_temp.f19.set_size(1, problemDefCells_f19.size(1));
    xtrans_idx_0 = problemDefCells_f19.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f19[i] = problemDefCells_f19[i];
    }

    expl_temp.f14.set_size(1, problemDefCells_f14.size(1));
    xtrans_idx_0 = problemDefCells_f14.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f14[i] = problemDefCells_f14[i];
    }

    expl_temp.f6.set_size(problemDefCells_f6.size(0));
    xtrans_idx_0 = problemDefCells_f6.size(0);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f6[i] = problemDefCells_f6[i];
    }

    expl_temp.f5.set_size(1, problemDefCells_f5.size(1));
    xtrans_idx_0 = problemDefCells_f5.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f5[i] = problemDefCells_f5[i];
    }

    expl_temp.f4.set_size(1, problemDefCells_f4.size(1));
    xtrans_idx_0 = problemDefCells_f4.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f4[i] = problemDefCells_f4[i];
    }

    expl_temp.f3.set_size(1, problemDefCells_f3.size(1));
    xtrans_idx_0 = problemDefCells_f3.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f3[i] = problemDefCells_f3[i];
    }

    expl_temp.f2.set_size(1, problemDefCells_f2.size(1));
    xtrans_idx_0 = problemDefCells_f2.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f2[i] = problemDefCells_f2[i];
    }

    expl_temp.f1.set_size(1, problemDefCells_f1.size(1));
    xtrans_idx_0 = problemDefCells_f1.size(1);
    for (int32_T i{0}; i < xtrans_idx_0; i++) {
      expl_temp.f1[i] = problemDefCells_f1[i];
    }

    reflectivityCalculation(&c_problemDef, &expl_temp, controls, b_problemDef,
      result);
    *fval = b_problemDef->calculations.sumChi;
  }
}

// End of code generation (simplexIntrafun.cpp)
