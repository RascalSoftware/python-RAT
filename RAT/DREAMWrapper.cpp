//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// DREAMWrapper.cpp
//
// Code generation for function 'DREAMWrapper'
//

// Include files
#include "DREAMWrapper.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "unpackParams.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  real_T DREAMWrapper(const ::coder::array<real_T, 2U> &pars, const struct5_T
                      *ratInputs_problemDef, const cell_14
                      *ratInputs_problemDefCells, const struct2_T
                      *ratInputs_controls)
  {
    cell_13 a__1;
    struct5_T problem;
    struct_T outProblem;
    int32_T loop_ub;

    //  Get the inputs for Reflectivity Calculation
    problem = *ratInputs_problemDef;

    //  Put the current parameters into problem
    problem.fitParams.set_size(1, pars.size(1));
    loop_ub = pars.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      problem.fitParams[problem.fitParams.size(0) * i] = pars[i];
    }

    //  Distribute them to the right parts
    unpackParams(&problem, ratInputs_controls->checks.fitParam,
                 ratInputs_controls->checks.fitBackgroundParam,
                 ratInputs_controls->checks.fitQzshift,
                 ratInputs_controls->checks.fitScalefactor,
                 ratInputs_controls->checks.fitBulkIn,
                 ratInputs_controls->checks.fitBulkOut,
                 ratInputs_controls->checks.fitResolutionParam,
                 ratInputs_controls->checks.fitDomainRatio);

    //  Calculate....
    reflectivityCalculation(&problem, ratInputs_problemDefCells,
      ratInputs_controls, &outProblem, &a__1);

    //  Function value is chi-squared....
    return -outProblem.calculations.sumChi / 2.0;
  }
}

// End of code generation (DREAMWrapper.cpp)
