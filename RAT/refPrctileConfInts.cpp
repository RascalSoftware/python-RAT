//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// refPrctileConfInts.cpp
//
// Code generation for function 'refPrctileConfInts'
//

// Include files
#include "refPrctileConfInts.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "interp1.h"
#include "makeCell.h"
#include "mean.h"
#include "prctile.h"
#include "rand.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "unpackParams.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void refPrctileConfInts(const ::coder::array<real_T, 2U> &bayesOutputs_chain,
    struct5_T *problemDef, const cell_14 *problemDefCells, const struct2_T
    *controlsStruct, f_struct_T *allPredInts)
  {
    ::coder::array<cell_wrap_8, 2U> r;
    ::coder::array<cell_wrap_8, 2U> ref_yVals;
    ::coder::array<cell_wrap_8, 2U> sld_yVals;
    ::coder::array<real_T, 2U> r1;
    ::coder::array<real_T, 2U> refArray;
    ::coder::array<real_T, 2U> rowVals;
    ::coder::array<real_T, 2U> sldArray;
    ::coder::array<real_T, 2U> vals;
    ::coder::array<real_T, 1U> b_calcResult;
    ::coder::array<real_T, 1U> c_calcResult;
    cell_13 calcResult;
    struct_T calcProblem;
    real_T a[1000];
    real_T isample[1000];
    real_T thisCol_data[1000];
    real_T CI_65[2];
    real_T CI_95[2];
    int32_T b_loop_ub;
    int32_T b_thisCol_size;
    int32_T c_loop_ub;
    int32_T i;
    int32_T i1;
    int32_T k;
    int32_T loop_ub;
    int32_T thisCol_size;
    boolean_T domains;

    //  Need to deal slightly differently with SLDs if there are domains
    domains = coder::internal::b_strcmp(problemDef->TF.data, problemDef->TF.size);

    //  Calc the ref and SLD for the first row of the chain. This 'sticks' the x
    //  values of each that we then interpolate the values from the rest of the
    //  cain onto....
    problemDef->fitParams.set_size(1, bayesOutputs_chain.size(1));
    loop_ub = bayesOutputs_chain.size(1);
    for (i = 0; i < loop_ub; i++) {
      problemDef->fitParams[problemDef->fitParams.size(0) * i] =
        bayesOutputs_chain[bayesOutputs_chain.size(0) * i];
    }

    unpackParams(problemDef, controlsStruct->checks.fitParam,
                 controlsStruct->checks.fitBackgroundParam,
                 controlsStruct->checks.fitQzshift,
                 controlsStruct->checks.fitScalefactor,
                 controlsStruct->checks.fitBulkIn,
                 controlsStruct->checks.fitBulkOut,
                 controlsStruct->checks.fitResolutionParam,
                 controlsStruct->checks.fitDomainRatio);

    //  Calc the reflectivities....
    reflectivityCalculation(problemDef, problemDefCells, controlsStruct,
      &calcProblem, &calcResult);

    //  'result' is currently a cell array. Convert this to a struct because it's
    //  easier to work with fieldnames...
    //  Reflectivity art points
    //  Reflectivity between sim limits
    //  Data corrected for sfs
    //  Layers if defined (i.e. not customXY)
    //  Calculated SLD profiles
    //  Resampled layers
    //  For compile, we can't remove a field, so just clear it for now...
    // problem = rmfield(problem,'calculations');
    //  so each is a {n x 1} cell array, because of n contrasts.
    //  Prepare some arrays to hold the SLD's and Refs for all the chain, keeping only the Y vales.
    //  We'll save x values in a separate array
    vals.set_size(1, 3);
    rowVals.set_size(1, 3);
    vals[0] = 0.0;
    rowVals[0] = 0.0;
    vals[vals.size(0)] = 0.0;
    rowVals[1] = 0.0;
    vals[vals.size(0) * 2] = 0.0;
    rowVals[2] = 0.0;
    makeCell(problemDef->numberOfContrasts, (const real_T *)rowVals.data(),
             allPredInts->refXdata);

    // cell(numberOfContrasts,1);
    makeCell(problemDef->numberOfContrasts, (const real_T *)vals.data(),
             ref_yVals);

    // cell(numberOfContrasts,1);
    makeCell(problemDef->numberOfContrasts, (const real_T *)rowVals.data(),
             allPredInts->sldXdata);
    if (!domains) {
      makeCell(problemDef->numberOfContrasts, (const real_T *)vals.data(), r);
      sld_yVals.set_size(r.size(0), 1);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        sld_yVals[i] = r[i];
      }
    } else {
      b_makeCell(problemDef->numberOfContrasts, (const real_T *)vals.data(), r);
      sld_yVals.set_size(r.size(0), 2);
      loop_ub = r.size(0);
      for (i = 0; i < 2; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          sld_yVals[i1 + sld_yVals.size(0) * i] = r[i1 + r.size(0) * i];
        }
      }
    }

    //  We need to have the yvals interpolated onto the same xvals when we
    //  calculate the sample. So, take the current reflectivity value from above
    //  to get the 'base' x for ref and SLD, then all following
    //  interpelations are onto these x values....
    i = static_cast<int32_T>(problemDef->numberOfContrasts);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      loop_ub = calcResult.f1[b_i].f1.size(0);
      allPredInts->refXdata[b_i].f1.set_size(1, calcResult.f1[b_i].f1.size(0));
      for (i1 = 0; i1 < loop_ub; i1++) {
        allPredInts->refXdata[b_i].f1[i1] = calcResult.f1[b_i].f1[i1];
      }

      //  Transpose these into rows for storage
      if (!domains) {
        loop_ub = calcResult.f5[b_i].f1.size(0);
        allPredInts->sldXdata[b_i].f1.set_size(1, calcResult.f5[b_i].f1.size(0));
        for (i1 = 0; i1 < loop_ub; i1++) {
          allPredInts->sldXdata[b_i].f1[i1] = calcResult.f5[b_i].f1[i1];
        }
      } else {
        loop_ub = calcResult.f5[b_i].f1.size(0);
        for (int32_T m{0}; m < 2; m++) {
          allPredInts->sldXdata[b_i].f1.set_size(1, loop_ub);
          for (i1 = 0; i1 < loop_ub; i1++) {
            allPredInts->sldXdata[b_i].f1[i1] = calcResult.f5[b_i].f1[i1];
          }
        }
      }
    }

    //  Loop over the whole chain, collecting the Sld's and Refs together into
    //  one array for each contrast.
    // will be = nsimu
    //  To speed things up, we'll take a random sample of the chain, rather than
    //  calculating >20000 reflectivities...
    coder::c_rand(a);
    for (k = 0; k < 1000; k++) {
      isample[k] = std::ceil(a[k] * static_cast<real_T>(bayesOutputs_chain.size
        (0)));
    }

    //  First, we populate the yVals arrays with zero arrays of the correct size...
    for (int32_T b_i{0}; b_i < i; b_i++) {
      ref_yVals[b_i].f1.set_size(1000, calcResult.f1[b_i].f1.size(0));
      loop_ub = calcResult.f1[b_i].f1.size(0);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 1000; k++) {
          ref_yVals[b_i].f1[k + ref_yVals[b_i].f1.size(0) * i1] = 0.0;
        }
      }

      sld_yVals[b_i].f1.set_size(1000, calcResult.f5[b_i].f1.size(0));
      loop_ub = calcResult.f5[b_i].f1.size(0);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 1000; k++) {
          sld_yVals[b_i].f1[k + sld_yVals[b_i].f1.size(0) * i1] = 0.0;
        }
      }
    }

    //  Calculate all the samples....
    loop_ub = bayesOutputs_chain.size(1);
    for (int32_T b_i{0}; b_i < 1000; b_i++) {
      problemDef->fitParams.set_size(1, bayesOutputs_chain.size(1));
      for (i1 = 0; i1 < loop_ub; i1++) {
        problemDef->fitParams[problemDef->fitParams.size(0) * i1] =
          bayesOutputs_chain[(static_cast<int32_T>(isample[b_i]) +
                              bayesOutputs_chain.size(0) * i1) - 1];
      }

      unpackParams(problemDef, controlsStruct->checks.fitParam,
                   controlsStruct->checks.fitBackgroundParam,
                   controlsStruct->checks.fitQzshift,
                   controlsStruct->checks.fitScalefactor,
                   controlsStruct->checks.fitBulkIn,
                   controlsStruct->checks.fitBulkOut,
                   controlsStruct->checks.fitResolutionParam,
                   controlsStruct->checks.fitDomainRatio);

      //  Calc the reflectivities....
      reflectivityCalculation(problemDef, problemDefCells, controlsStruct,
        &calcProblem, &calcResult);

      //  'result' is currently a cell array. Convert this to a struct because it's
      //  easier to work with fieldnames...
      //  Reflectivity art points
      //  Reflectivity between sim limits
      //  Data corrected for sfs
      //  Layers if defined (i.e. not customXY)
      //  Calculated SLD profiles
      //  Resampled layers
      //  For compile, we can't remove a field, so just clear it for now...
      // problem = rmfield(problem,'calculations');
      allPredInts->sampleChi[b_i] = calcProblem.calculations.sumChi;
      for (int32_T n{0}; n < i; n++) {
        k = calcResult.f1[n].f1.size(0);
        b_calcResult.set_size(calcResult.f1[n].f1.size(0));
        for (i1 = 0; i1 < k; i1++) {
          b_calcResult[i1] = calcResult.f1[n].f1[i1];
        }

        k = calcResult.f1[n].f1.size(0);
        c_calcResult.set_size(calcResult.f1[n].f1.size(0));
        for (i1 = 0; i1 < k; i1++) {
          c_calcResult[i1] = calcResult.f1[n].f1[i1 + calcResult.f1[n].f1.size(0)];
        }

        coder::interp1(b_calcResult, c_calcResult, allPredInts->refXdata[n].f1,
                       r1);
        k = r1.size(1);
        for (i1 = 0; i1 < k; i1++) {
          ref_yVals[n].f1[b_i + ref_yVals[n].f1.size(0) * i1] = r1[i1];
        }

        //  Automatically comes back as a row from inpterp1
        if (!domains) {
          k = calcResult.f5[n].f1.size(0);
          b_calcResult.set_size(calcResult.f5[n].f1.size(0));
          for (i1 = 0; i1 < k; i1++) {
            b_calcResult[i1] = calcResult.f5[n].f1[i1];
          }

          k = calcResult.f5[n].f1.size(0);
          c_calcResult.set_size(calcResult.f5[n].f1.size(0));
          for (i1 = 0; i1 < k; i1++) {
            c_calcResult[i1] = calcResult.f5[n].f1[i1 + calcResult.f5[n].f1.size
              (0)];
          }

          coder::b_interp1(b_calcResult, c_calcResult, allPredInts->sldXdata[n].
                           f1, r1);
          k = r1.size(1);
          for (i1 = 0; i1 < k; i1++) {
            sld_yVals[n].f1[b_i + sld_yVals[n].f1.size(0) * i1] = r1[i1];
          }
        } else {
          int32_T d_loop_ub;
          k = calcResult.f5[n].f1.size(0);
          d_loop_ub = calcResult.f5[n].f1.size(0);
          for (int32_T m{0}; m < 2; m++) {
            int32_T e_loop_ub;
            b_calcResult.set_size(k);
            for (i1 = 0; i1 < k; i1++) {
              b_calcResult[i1] = calcResult.f5[n].f1[i1];
            }

            c_calcResult.set_size(d_loop_ub);
            for (i1 = 0; i1 < d_loop_ub; i1++) {
              c_calcResult[i1] = calcResult.f5[n].f1[i1 + calcResult.f5[n].
                f1.size(0)];
            }

            coder::b_interp1(b_calcResult, c_calcResult, allPredInts->sldXdata[n]
                             .f1, r1);
            e_loop_ub = r1.size(1);
            for (i1 = 0; i1 < e_loop_ub; i1++) {
              sld_yVals[n].f1[b_i + sld_yVals[n].f1.size(0) * i1] = r1[i1];
            }
          }
        }
      }
    }

    //  Calculate the percentiles across all the calculated samples for each
    //  point in x... We calculate 95% and 65% CI's for each set of curves
    allPredInts->refPredInts.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      refArray.set_size(5, ref_yVals[b_i].f1.size(1));
      loop_ub = ref_yVals[b_i].f1.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 5; k++) {
          refArray[k + 5 * i1] = 0.0;
        }
      }

      //  We could possibly use CIFn in one shot here (rather than loop over
      //  points....)
      i1 = ref_yVals[b_i].f1.size(1);
      if (ref_yVals[b_i].f1.size(1) - 1 >= 0) {
        thisCol_size = ref_yVals[b_i].f1.size(0);
        b_loop_ub = ref_yVals[b_i].f1.size(0);
      }

      for (int32_T points{0}; points < i1; points++) {
        for (k = 0; k < b_loop_ub; k++) {
          thisCol_data[k] = ref_yVals[b_i].f1[k + ref_yVals[b_i].f1.size(0) *
            points];
        }

        coder::prctile(thisCol_data, thisCol_size, CI_95);
        coder::b_prctile(thisCol_data, thisCol_size, CI_65);
        refArray[5 * points] = CI_95[0];
        refArray[5 * points + 1] = CI_65[0];
        refArray[5 * points + 2] = coder::mean(thisCol_data, thisCol_size);
        refArray[5 * points + 3] = CI_65[1];
        refArray[5 * points + 4] = CI_95[1];
      }

      allPredInts->refPredInts[b_i].f1.set_size(5, refArray.size(1));
      loop_ub = refArray.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 5; k++) {
          allPredInts->refPredInts[b_i].f1[k + 5 * i1] = refArray[k + 5 * i1];
        }
      }
    }

    //  TODO: need to add domains here....
    allPredInts->sldPredInts.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      sldArray.set_size(5, sld_yVals[b_i].f1.size(1));
      loop_ub = sld_yVals[b_i].f1.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 5; k++) {
          sldArray[k + 5 * i1] = 0.0;
        }
      }

      i1 = sld_yVals[b_i].f1.size(1);
      if (sld_yVals[b_i].f1.size(1) - 1 >= 0) {
        b_thisCol_size = sld_yVals[b_i].f1.size(0);
        c_loop_ub = sld_yVals[b_i].f1.size(0);
      }

      for (int32_T points{0}; points < i1; points++) {
        for (k = 0; k < c_loop_ub; k++) {
          thisCol_data[k] = sld_yVals[b_i].f1[k + sld_yVals[b_i].f1.size(0) *
            points];
        }

        coder::prctile(thisCol_data, b_thisCol_size, CI_95);
        coder::b_prctile(thisCol_data, b_thisCol_size, CI_65);
        sldArray[5 * points] = CI_95[0];
        sldArray[5 * points + 1] = CI_65[0];
        sldArray[5 * points + 2] = coder::mean(thisCol_data, b_thisCol_size);
        sldArray[5 * points + 3] = CI_65[1];
        sldArray[5 * points + 4] = CI_95[1];
      }

      allPredInts->sldPredInts[b_i].f1.set_size(5, sldArray.size(1));
      loop_ub = sldArray.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (k = 0; k < 5; k++) {
          allPredInts->sldPredInts[b_i].f1[k + 5 * i1] = sldArray[k + 5 * i1];
        }
      }
    }
  }
}

// End of code generation (refPrctileConfInts.cpp)
