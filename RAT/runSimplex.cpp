//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// runSimplex.cpp
//
// Code generation for function 'runSimplex'
//

// Include files
#include "runSimplex.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "fMinSearch.h"
#include "packParams.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "simplexXTransform.h"
#include "strcmp.h"
#include "unpackParams.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void runSimplex(struct5_T *problemDef, const cell_14 *problemDefCells, const
                  struct1_T *problemDefLimits, const struct2_T *controls,
                  struct_T *problem, cell_wrap_9 result[6])
  {
    static const char_T b_cv1[6]{ 'n', 'o', 't', 'i', 'f', 'y' };

    static const char_T b_cv[5]{ 'f', 'i', 'n', 'a', 'l' };

    ::coder::array<cell_wrap_1, 1U> b_problemDef;
    ::coder::array<real_T, 1U> x;
    ::coder::array<real_T, 1U> x0u;
    cell_13 b_result;
    h_struct_T a__4;
    j_struct_T expl_temp;
    real_T a__2;
    real_T a__3;
    int32_T dis_size[2];
    int32_T i;
    int32_T loop_ub;
    int32_T outsize_idx_0;
    char_T dis_data[6];
    packParams(problemDef, problemDefCells->f7, problemDefCells->f8,
               problemDefCells->f9, problemDefCells->f10, problemDefCells->f11,
               problemDefCells->f12, problemDefCells->f13, problemDefCells->f20,
               problemDefLimits, &controls->checks, b_problemDef);

    // Check the bounds on all the selected
    //  out = checkBounds(problemDef,controls);
    //  if strcmp(out{1},'fail')
    //      return
    //  end
    if (coder::internal::r_strcmp(controls->display.data, controls->display.size))
    {
      outsize_idx_0 = 0;
    } else if (coder::internal::s_strcmp(controls->display.data,
                controls->display.size)) {
      outsize_idx_0 = 1;
    } else if (coder::internal::t_strcmp(controls->display.data,
                controls->display.size)) {
      outsize_idx_0 = 2;
    } else if (coder::internal::u_strcmp(controls->display.data,
                controls->display.size)) {
      outsize_idx_0 = 3;
    } else {
      outsize_idx_0 = -1;
    }

    switch (outsize_idx_0) {
     case 0:
      dis_size[0] = 1;
      dis_size[1] = 4;
      dis_data[0] = 'n';
      dis_data[1] = 'o';
      dis_data[2] = 'n';
      dis_data[3] = 'e';
      break;

     case 1:
      dis_size[0] = 1;
      dis_size[1] = 4;
      dis_data[0] = 'i';
      dis_data[1] = 't';
      dis_data[2] = 'e';
      dis_data[3] = 'r';
      break;

     case 2:
      dis_size[0] = 1;
      dis_size[1] = 6;
      for (i = 0; i < 6; i++) {
        dis_data[i] = b_cv1[i];
      }
      break;

     case 3:
      dis_size[0] = 1;
      dis_size[1] = 5;
      for (i = 0; i < 5; i++) {
        dis_data[i] = b_cv[i];
      }
      break;

     default:
      dis_size[0] = 1;
      dis_size[1] = 5;
      for (i = 0; i < 5; i++) {
        dis_data[i] = b_cv[i];
      }
      break;
    }

    expl_temp.LB.set_size(problemDef->fitLimits.size(0));
    loop_ub = problemDef->fitLimits.size(0);
    for (i = 0; i < loop_ub; i++) {
      expl_temp.LB[i] = problemDef->fitLimits[i];
    }

    expl_temp.UB.set_size(problemDef->fitLimits.size(0));
    loop_ub = problemDef->fitLimits.size(0);
    for (i = 0; i < loop_ub; i++) {
      expl_temp.UB[i] = problemDef->fitLimits[i + problemDef->fitLimits.size(0)];
    }

    //  size checks
    if (problemDef->fitLimits.size(0) == 0) {
      outsize_idx_0 = problemDef->fitParams.size(0) * problemDef->fitParams.size
        (1);
      expl_temp.LB.set_size(outsize_idx_0);
      for (i = 0; i < outsize_idx_0; i++) {
        expl_temp.LB[i] = rtMinusInf;
      }
    }

    if (problemDef->fitLimits.size(0) == 0) {
      outsize_idx_0 = problemDef->fitParams.size(0) * problemDef->fitParams.size
        (1);
      expl_temp.UB.set_size(outsize_idx_0);
      for (i = 0; i < outsize_idx_0; i++) {
        expl_temp.UB[i] = rtInf;
      }
    }

    //  stuff into a struct to pass around
    // varargin;
    // problemDef.modelFilename;%fun;
    //  0 --> unconstrained variable
    //  1 --> lower bound only
    //  2 --> upper bound only
    //  3 --> dual finite bounds
    outsize_idx_0 = problemDef->fitParams.size(0) * problemDef->fitParams.size(1);
    expl_temp.BoundClass.set_size(outsize_idx_0);
    for (i = 0; i < outsize_idx_0; i++) {
      expl_temp.BoundClass[i] = 0.0;
    }

    i = problemDef->fitParams.size(0) * problemDef->fitParams.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      expl_temp.BoundClass[b_i] = static_cast<real_T>((!std::isinf
        (expl_temp.LB[b_i])) && (!std::isnan(expl_temp.LB[b_i]))) + static_cast<
        real_T>(((!std::isinf(expl_temp.UB[b_i])) && (!std::isnan
                  (expl_temp.UB[b_i]))) << 1);
    }

    //  transform starting values into their unconstrained
    //  surrogates. Check for infeasible starting guesses.
    outsize_idx_0 = problemDef->fitParams.size(0) * problemDef->fitParams.size(1);
    x0u.set_size(outsize_idx_0);
    for (i = 0; i < outsize_idx_0; i++) {
      x0u[i] = problemDef->fitParams[i];
    }

    i = problemDef->fitParams.size(0) * problemDef->fitParams.size(1);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      switch (static_cast<int32_T>(expl_temp.BoundClass[b_i])) {
       case 1:
        //  lower bound only
        if (problemDef->fitParams[b_i] <= expl_temp.LB[b_i]) {
          //  infeasible starting value. Use bound.
          x0u[b_i] = 0.0;
        } else {
          x0u[b_i] = std::sqrt(problemDef->fitParams[b_i] - expl_temp.LB[b_i]);
        }
        break;

       case 2:
        //  upper bound only
        if (problemDef->fitParams[b_i] >= expl_temp.UB[b_i]) {
          //  infeasible starting value. use bound.
          x0u[b_i] = 0.0;
        } else {
          x0u[b_i] = std::sqrt(expl_temp.UB[b_i] - problemDef->fitParams[b_i]);
        }
        break;

       case 3:
        //  lower and upper bounds
        if (problemDef->fitParams[b_i] <= expl_temp.LB[b_i]) {
          //  infeasible starting value
          x0u[b_i] = -1.5707963267948966;
        } else if (problemDef->fitParams[b_i] >= expl_temp.UB[b_i]) {
          //  infeasible starting value
          x0u[b_i] = 1.5707963267948966;
        } else {
          x0u[b_i] = std::asin(std::fmax(-1.0, std::fmin(1.0, 2.0 *
            (problemDef->fitParams[b_i] - expl_temp.LB[b_i]) / (expl_temp.UB[b_i]
            - expl_temp.LB[b_i]) - 1.0)));
        }
        break;

       default:
        //  unconstrained variable. x0u(i) is set.
        break;
      }
    }

    //  now we can call fminsearch, but with our own
    //  intra-objective function.
    fMinSearch(x0u, controls->maxIter, controls->maxFunEvals, controls->tolX,
               controls->tolFun, dis_data, dis_size, problemDef,
               problemDefCells->f1, problemDefCells->f2, problemDefCells->f3,
               problemDefCells->f4, problemDefCells->f5, problemDefCells->f6,
               problemDefCells->f14, problemDefCells->f19, controls, &expl_temp,
               &a__2, &a__3, &a__4);

    // [xu,fval,exitflag,output] = simplex(@simplexIntrafun,x0u,problemDef,problemDefCells,problemDefLimits,controls,options,params,300);
    //  undo the variable transformations into the original space
    simplexXTransform(x0u, expl_temp.LB, expl_temp.UB, expl_temp.BoundClass, x);

    //  final reshape
    // x = reshape(x,xsize);
    outsize_idx_0 = x.size(0);
    problemDef->fitParams.set_size(x.size(0), 1);
    for (i = 0; i < outsize_idx_0; i++) {
      problemDef->fitParams[i] = x[i];
    }

    unpackParams(problemDef, controls->checks.fitParam,
                 controls->checks.fitBackgroundParam,
                 controls->checks.fitQzshift, controls->checks.fitScalefactor,
                 controls->checks.fitBulkIn, controls->checks.fitBulkOut,
                 controls->checks.fitResolutionParam,
                 controls->checks.fitDomainRatio);
    reflectivityCalculation(problemDef, problemDefCells, controls, problem,
      &b_result);
    result[0].f1.set_size(b_result.f1.size(0), 1);
    outsize_idx_0 = b_result.f1.size(0);
    for (i = 0; i < outsize_idx_0; i++) {
      loop_ub = b_result.f1[i].f1.size(0);
      result[0].f1[i].f1.set_size(b_result.f1[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[0].f1[i].f1[i2 + result[0].f1[i].f1.size(0) * i1] =
            b_result.f1[i].f1[i2 + b_result.f1[i].f1.size(0) * i1];
        }
      }
    }

    result[1].f1.set_size(b_result.f2.size(0), 1);
    outsize_idx_0 = b_result.f2.size(0);
    for (i = 0; i < outsize_idx_0; i++) {
      loop_ub = b_result.f2[i].f1.size(0);
      result[1].f1[i].f1.set_size(b_result.f2[i].f1.size(0), 2);
      for (int32_T i1{0}; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[1].f1[i].f1[i2 + result[1].f1[i].f1.size(0) * i1] =
            b_result.f2[i].f1[i2 + b_result.f2[i].f1.size(0) * i1];
        }
      }
    }

    result[2].f1.set_size(b_result.f3.size(0), 1);
    outsize_idx_0 = b_result.f3.size(0);
    for (i = 0; i < outsize_idx_0; i++) {
      loop_ub = b_result.f3[i].f1.size(0);
      result[2].f1[i].f1.set_size(b_result.f3[i].f1.size(0), 3);
      for (int32_T i1{0}; i1 < 3; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result[2].f1[i].f1[i2 + result[2].f1[i].f1.size(0) * i1] =
            b_result.f3[i].f1[i2 + b_result.f3[i].f1.size(0) * i1];
        }
      }
    }

    result[3].f1.set_size(b_result.f4.size(0), b_result.f4.size(1));
    loop_ub = b_result.f4.size(1);
    for (i = 0; i < loop_ub; i++) {
      outsize_idx_0 = b_result.f4.size(0);
      for (int32_T i1{0}; i1 < outsize_idx_0; i1++) {
        result[3].f1[i1 + result[3].f1.size(0) * i] = b_result.f4[i1 +
          b_result.f4.size(0) * i];
      }
    }

    result[4].f1.set_size(b_result.f5.size(0), b_result.f5.size(1));
    loop_ub = b_result.f5.size(1);
    for (i = 0; i < loop_ub; i++) {
      outsize_idx_0 = b_result.f5.size(0);
      for (int32_T i1{0}; i1 < outsize_idx_0; i1++) {
        result[4].f1[i1 + result[4].f1.size(0) * i] = b_result.f5[i1 +
          b_result.f5.size(0) * i];
      }
    }

    result[5].f1.set_size(b_result.f6.size(0), b_result.f6.size(1));
    loop_ub = b_result.f6.size(1);
    for (i = 0; i < loop_ub; i++) {
      outsize_idx_0 = b_result.f6.size(0);
      for (int32_T i1{0}; i1 < outsize_idx_0; i1++) {
        result[5].f1[i1 + result[5].f1.size(0) * i] = b_result.f6[i1 +
          b_result.f6.size(0) * i];
      }
    }
  }
}

// End of code generation (runSimplex.cpp)
