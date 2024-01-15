//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// scaledGaussPrior.cpp
//
// Code generation for function 'scaledGaussPrior'
//

// Include files
#include "scaledGaussPrior.h"
#include "find.h"
#include "rescale.h"
#include "rt_nonfinite.h"
#include "sum.h"
#include "unsafeSxfun.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  real_T scaledGaussPrior(const ::coder::array<real_T, 2U> &m, const ::coder::
    array<real_T, 2U> &extras_problemDef_fitLimits, const ::coder::array<real_T,
    2U> &extras_priors)
  {
    ::coder::array<real_T, 2U> usedConstr;
    ::coder::array<real_T, 2U> usedPriors;
    ::coder::array<real_T, 1U> b_m;
    ::coder::array<real_T, 1U> b_usedConstr;
    ::coder::array<real_T, 1U> c_usedConstr;
    ::coder::array<real_T, 1U> r;
    ::coder::array<real_T, 1U> r1;
    ::coder::array<real_T, 1U> r2;
    ::coder::array<int32_T, 1U> usedPriorInd;
    ::coder::array<boolean_T, 1U> b_extras_priors;
    real_T pVal2;
    int32_T i;
    int32_T loop_ub;

    //  All are in range, so check for Gaussian priors....
    //  We pick out any priors that are Gaussians and calculate the mvnpdf
    //  Find all the Gaussian priors....
    //  usedPriorInd = find(strcmpi(priorList(:,1),'gaussian'));
    b_extras_priors.set_size(extras_priors.size(0));
    loop_ub = extras_priors.size(0);
    for (i = 0; i < loop_ub; i++) {
      b_extras_priors[i] = (extras_priors[i] == 2.0);
    }

    coder::eml_find(b_extras_priors, usedPriorInd);
    usedPriors.set_size(usedPriorInd.size(0), 5);
    loop_ub = usedPriorInd.size(0);
    for (i = 0; i < 5; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        usedPriors[i1 + usedPriors.size(0) * i] = extras_priors[(usedPriorInd[i1]
          + extras_priors.size(0) * i) - 1];
      }
    }

    usedConstr.set_size(usedPriorInd.size(0), 2);
    loop_ub = usedPriorInd.size(0);
    for (i = 0; i < 2; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        usedConstr[i1 + usedConstr.size(0) * i] = extras_problemDef_fitLimits
          [(usedPriorInd[i1] + extras_problemDef_fitLimits.size(0) * i) - 1];
      }
    }

    if (usedPriorInd.size(0) != 0) {
      //  There may be no Gaussian priors defined!
      //  Scale (minVal+prior) will give scaled sigma since minVal goes to 0...
      b_m.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_m[i] = m[usedPriorInd[i] - 1];
      }

      b_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_usedConstr[i] = usedConstr[i];
      }

      c_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        c_usedConstr[i] = usedConstr[i + usedConstr.size(0)];
      }

      coder::rescale(b_m, b_usedConstr, c_usedConstr, r);
      b_m.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_m[i] = usedPriors[i + usedPriors.size(0)];
      }

      b_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_usedConstr[i] = usedConstr[i];
      }

      c_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        c_usedConstr[i] = usedConstr[i + usedConstr.size(0)];
      }

      coder::rescale(b_m, b_usedConstr, c_usedConstr, r1);
      b_m.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_m[i] = usedPriors[i + usedPriors.size(0) * 2] + usedConstr[i];
      }

      b_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_usedConstr[i] = usedConstr[i];
      }

      c_usedConstr.set_size(usedPriorInd.size(0));
      loop_ub = usedPriorInd.size(0);
      for (i = 0; i < loop_ub; i++) {
        c_usedConstr[i] = usedConstr[i + usedConstr.size(0)];
      }

      coder::rescale(b_m, b_usedConstr, c_usedConstr, r2);
      if (r.size(0) == 1) {
        i = r1.size(0);
      } else {
        i = r.size(0);
      }

      if ((r.size(0) == r1.size(0)) && (i == r2.size(0))) {
        loop_ub = r.size(0);
        for (i = 0; i < loop_ub; i++) {
          real_T varargin_1;
          varargin_1 = (r[i] - r1[i]) / r2[i];
          r[i] = varargin_1 * varargin_1;
        }
      } else {
        binary_expand_op(r, r1, r2);
      }

      pVal2 = -coder::sum(r);
    } else {
      pVal2 = 0.0;
    }

    return pVal2;
  }
}

// End of code generation (scaledGaussPrior.cpp)
