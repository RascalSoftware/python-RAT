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
#include "parseResultToStruct.h"
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
    c_struct_T *problemStruct, const cell_11 *problemCells, const struct2_T
    *controlsStruct, f_struct_T *allPredInts)
  {
    ::coder::array<cell_wrap_22, 2U> r;
    ::coder::array<cell_wrap_8, 2U> b_expl_temp;
    ::coder::array<cell_wrap_8, 2U> c_expl_temp;
    ::coder::array<cell_wrap_8, 2U> calcResult_reflectivity;
    ::coder::array<cell_wrap_8, 2U> calcResult_sldProfiles;
    ::coder::array<cell_wrap_8, 2U> d_expl_temp;
    ::coder::array<cell_wrap_8, 2U> expl_temp;
    ::coder::array<cell_wrap_8, 2U> r1;
    ::coder::array<cell_wrap_8, 2U> refYVals;
    ::coder::array<cell_wrap_8, 2U> sldYVals;
    ::coder::array<real_T, 2U> r2;
    ::coder::array<real_T, 2U> refArray;
    ::coder::array<real_T, 2U> rowVals;
    ::coder::array<real_T, 2U> sldArray;
    ::coder::array<real_T, 2U> sldArray1;
    ::coder::array<real_T, 2U> sldArray2;
    ::coder::array<real_T, 2U> vals;
    ::coder::array<real_T, 1U> b_calcResult_reflectivity;
    ::coder::array<real_T, 1U> c_calcResult_reflectivity;
    cell_wrap_9 calcResult[6];
    d_struct_T calcContrastParams;
    d_struct_T e_expl_temp;
    real_T a[1000];
    real_T isample[1000];
    real_T thisCol_data[1000];
    real_T ci65[2];
    real_T ci95[2];
    real_T calcResult_calculationResults_sumChi;
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
    domains = coder::internal::b_strcmp(problemStruct->TF.data,
      problemStruct->TF.size);

    //  Calc the ref and SLD for the first row of the chain. This 'sticks' the x
    //  values of each that we then interpolate the values from the rest of the
    //  cain onto....
    problemStruct->fitParams.set_size(1, bayesOutputs_chain.size(1));
    loop_ub = bayesOutputs_chain.size(1);
    for (i = 0; i < loop_ub; i++) {
      problemStruct->fitParams[problemStruct->fitParams.size(0) * i] =
        bayesOutputs_chain[bayesOutputs_chain.size(0) * i];
    }

    unpackParams(problemStruct, controlsStruct->checks.fitParam,
                 controlsStruct->checks.fitBackgroundParam,
                 controlsStruct->checks.fitQzshift,
                 controlsStruct->checks.fitScalefactor,
                 controlsStruct->checks.fitBulkIn,
                 controlsStruct->checks.fitBulkOut,
                 controlsStruct->checks.fitResolutionParam,
                 controlsStruct->checks.fitDomainRatio);

    //  Calc the reflectivities....
    reflectivityCalculation(problemStruct, problemCells, controlsStruct,
      &calcContrastParams, calcResult);

    //  'result' is currently a cell array. Convert this to a struct because it's
    //  easier to work with fieldnames...
    parseResultToStruct(&calcContrastParams, calcResult, calcResult_reflectivity,
                        expl_temp, b_expl_temp, c_expl_temp,
                        calcResult_sldProfiles, d_expl_temp,
                        &calcResult_calculationResults_sumChi, &e_expl_temp);

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
    makeCell(problemStruct->numberOfContrasts, (const real_T *)rowVals.data(),
             allPredInts->refXdata);

    // cell(numberOfContrasts,1);
    makeCell(problemStruct->numberOfContrasts, (const real_T *)vals.data(),
             refYVals);

    // cell(numberOfContrasts,1);
    if (!domains) {
      makeCell(problemStruct->numberOfContrasts, (const real_T *)rowVals.data(),
               r);
      allPredInts->sldXdata.set_size(r.size(0), 1);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        allPredInts->sldXdata[i] = r[i];
      }

      makeCell(problemStruct->numberOfContrasts, (const real_T *)vals.data(), r1);
      sldYVals.set_size(r1.size(0), 1);
      loop_ub = r1.size(0);
      for (i = 0; i < loop_ub; i++) {
        sldYVals[i] = r1[i];
      }
    } else {
      b_makeCell(problemStruct->numberOfContrasts, (const real_T *)rowVals.data(),
                 r);
      allPredInts->sldXdata.set_size(r.size(0), 2);
      b_makeCell(problemStruct->numberOfContrasts, (const real_T *)vals.data(),
                 r1);
      sldYVals.set_size(r1.size(0), 2);
      loop_ub = r.size(0);
      k = r1.size(0);
      for (i = 0; i < 2; i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          allPredInts->sldXdata[i1 + allPredInts->sldXdata.size(0) * i] = r[i1 +
            r.size(0) * i];
        }

        for (i1 = 0; i1 < k; i1++) {
          sldYVals[i1 + sldYVals.size(0) * i] = r1[i1 + r1.size(0) * i];
        }
      }
    }

    //  We need to have the yvals interpolated onto the same xvals when we
    //  calculate the sample. So, take the current reflectivity value from above
    //  to get the 'base' x for ref and SLD, then all following
    //  interpelations are onto these x values....
    i = static_cast<int32_T>(problemStruct->numberOfContrasts);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      loop_ub = calcResult_reflectivity[b_i].f1.size(0);
      allPredInts->refXdata[b_i].f1.set_size(1, calcResult_reflectivity[b_i].
        f1.size(0));
      for (i1 = 0; i1 < loop_ub; i1++) {
        allPredInts->refXdata[b_i].f1[i1] = calcResult_reflectivity[b_i].f1[i1];
      }

      //  Transpose these into rows for storage
      if (!domains) {
        loop_ub = calcResult_sldProfiles[b_i].f1.size(0);
        allPredInts->sldXdata[b_i].f1.set_size(1, calcResult_sldProfiles[b_i].
          f1.size(0));
        for (i1 = 0; i1 < loop_ub; i1++) {
          allPredInts->sldXdata[b_i].f1[i1] = calcResult_sldProfiles[b_i].f1[i1];
        }
      } else {
        for (int32_T m{0}; m < 2; m++) {
          loop_ub = calcResult_sldProfiles[b_i + calcResult_sldProfiles.size(0) *
            m].f1.size(0);
          allPredInts->sldXdata[b_i + allPredInts->sldXdata.size(0) * m].
            f1.set_size(1, calcResult_sldProfiles[b_i +
                        calcResult_sldProfiles.size(0) * m].f1.size(0));
          for (i1 = 0; i1 < loop_ub; i1++) {
            allPredInts->sldXdata[b_i + allPredInts->sldXdata.size(0) * m].f1[i1]
              = calcResult_sldProfiles[b_i + calcResult_sldProfiles.size(0) * m]
              .f1[i1];
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
      refYVals[b_i].f1.set_size(1000, calcResult_reflectivity[b_i].f1.size(0));
      loop_ub = calcResult_reflectivity[b_i].f1.size(0);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (int32_T i2{0}; i2 < 1000; i2++) {
          refYVals[b_i].f1[i2 + refYVals[b_i].f1.size(0) * i1] = 0.0;
        }
      }

      if (!domains) {
        sldYVals[b_i].f1.set_size(1000, calcResult_sldProfiles[b_i].f1.size(0));
        loop_ub = calcResult_sldProfiles[b_i].f1.size(0);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 1000; i2++) {
            sldYVals[b_i].f1[i2 + sldYVals[b_i].f1.size(0) * i1] = 0.0;
          }
        }
      } else {
        sldYVals[b_i].f1.set_size(1000, calcResult_sldProfiles[b_i].f1.size(0));
        loop_ub = calcResult_sldProfiles[b_i].f1.size(0);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 1000; i2++) {
            sldYVals[b_i].f1[i2 + sldYVals[b_i].f1.size(0) * i1] = 0.0;
          }
        }

        sldYVals[b_i + sldYVals.size(0)].f1.set_size(1000,
          calcResult_sldProfiles[b_i + calcResult_sldProfiles.size(0)].f1.size(0));
        loop_ub = calcResult_sldProfiles[b_i + calcResult_sldProfiles.size(0)].
          f1.size(0);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 1000; i2++) {
            sldYVals[b_i + sldYVals.size(0)].f1[i2 + sldYVals[b_i +
              sldYVals.size(0)].f1.size(0) * i1] = 0.0;
          }
        }
      }
    }

    //  Calculate all the samples....
    loop_ub = bayesOutputs_chain.size(1);
    for (int32_T b_i{0}; b_i < 1000; b_i++) {
      problemStruct->fitParams.set_size(1, bayesOutputs_chain.size(1));
      for (i1 = 0; i1 < loop_ub; i1++) {
        problemStruct->fitParams[problemStruct->fitParams.size(0) * i1] =
          bayesOutputs_chain[(static_cast<int32_T>(isample[b_i]) +
                              bayesOutputs_chain.size(0) * i1) - 1];
      }

      unpackParams(problemStruct, controlsStruct->checks.fitParam,
                   controlsStruct->checks.fitBackgroundParam,
                   controlsStruct->checks.fitQzshift,
                   controlsStruct->checks.fitScalefactor,
                   controlsStruct->checks.fitBulkIn,
                   controlsStruct->checks.fitBulkOut,
                   controlsStruct->checks.fitResolutionParam,
                   controlsStruct->checks.fitDomainRatio);

      //  Calc the reflectivities....
      reflectivityCalculation(problemStruct, problemCells, controlsStruct,
        &calcContrastParams, calcResult);

      //  'result' is currently a cell array. Convert this to a struct because it's
      //  easier to work with fieldnames...
      parseResultToStruct(&calcContrastParams, calcResult,
                          calcResult_reflectivity, expl_temp, b_expl_temp,
                          c_expl_temp, calcResult_sldProfiles, d_expl_temp,
                          &calcResult_calculationResults_sumChi, &e_expl_temp);
      allPredInts->sampleChi[b_i] = calcResult_calculationResults_sumChi;
      for (int32_T n{0}; n < i; n++) {
        k = calcResult_reflectivity[n].f1.size(0);
        b_calcResult_reflectivity.set_size(calcResult_reflectivity[n].f1.size(0));
        for (i1 = 0; i1 < k; i1++) {
          b_calcResult_reflectivity[i1] = calcResult_reflectivity[n].f1[i1];
        }

        k = calcResult_reflectivity[n].f1.size(0);
        c_calcResult_reflectivity.set_size(calcResult_reflectivity[n].f1.size(0));
        for (i1 = 0; i1 < k; i1++) {
          c_calcResult_reflectivity[i1] = calcResult_reflectivity[n].f1[i1 +
            calcResult_reflectivity[n].f1.size(0)];
        }

        coder::interp1(b_calcResult_reflectivity, c_calcResult_reflectivity,
                       allPredInts->refXdata[n].f1, r2);
        k = r2.size(1);
        for (i1 = 0; i1 < k; i1++) {
          refYVals[n].f1[b_i + refYVals[n].f1.size(0) * i1] = r2[i1];
        }

        //  Automatically comes back as a row from inpterp1
        if (!domains) {
          k = calcResult_sldProfiles[n].f1.size(0);
          b_calcResult_reflectivity.set_size(calcResult_sldProfiles[n].f1.size(0));
          for (i1 = 0; i1 < k; i1++) {
            b_calcResult_reflectivity[i1] = calcResult_sldProfiles[n].f1[i1];
          }

          k = calcResult_sldProfiles[n].f1.size(0);
          c_calcResult_reflectivity.set_size(calcResult_sldProfiles[n].f1.size(0));
          for (i1 = 0; i1 < k; i1++) {
            c_calcResult_reflectivity[i1] = calcResult_sldProfiles[n].f1[i1 +
              calcResult_sldProfiles[n].f1.size(0)];
          }

          coder::b_interp1(b_calcResult_reflectivity, c_calcResult_reflectivity,
                           allPredInts->sldXdata[n].f1, r2);
          k = r2.size(1);
          for (i1 = 0; i1 < k; i1++) {
            sldYVals[n].f1[b_i + sldYVals[n].f1.size(0) * i1] = r2[i1];
          }
        } else {
          for (int32_T m{0}; m < 2; m++) {
            k = calcResult_sldProfiles[n + calcResult_sldProfiles.size(0) * m].
              f1.size(0);
            b_calcResult_reflectivity.set_size(calcResult_sldProfiles[n +
              calcResult_sldProfiles.size(0) * m].f1.size(0));
            for (i1 = 0; i1 < k; i1++) {
              b_calcResult_reflectivity[i1] = calcResult_sldProfiles[n +
                calcResult_sldProfiles.size(0) * m].f1[i1];
            }

            k = calcResult_sldProfiles[n + calcResult_sldProfiles.size(0) * m].
              f1.size(0);
            c_calcResult_reflectivity.set_size(calcResult_sldProfiles[n +
              calcResult_sldProfiles.size(0) * m].f1.size(0));
            for (i1 = 0; i1 < k; i1++) {
              c_calcResult_reflectivity[i1] = calcResult_sldProfiles[n +
                calcResult_sldProfiles.size(0) * m].f1[i1 +
                calcResult_sldProfiles[n + calcResult_sldProfiles.size(0) * m].
                f1.size(0)];
            }

            coder::b_interp1(b_calcResult_reflectivity,
                             c_calcResult_reflectivity, allPredInts->sldXdata[n
                             + allPredInts->sldXdata.size(0) * m].f1, r2);
            k = r2.size(1);
            for (i1 = 0; i1 < k; i1++) {
              sldYVals[n + sldYVals.size(0) * m].f1[b_i + sldYVals[n +
                sldYVals.size(0) * m].f1.size(0) * i1] = r2[i1];
            }
          }
        }
      }
    }

    //  Calculate the percentiles across all the calculated samples for each
    //  point in x... We calculate 95% and 65% CI's for each set of curves
    //  Reflectivity..
    allPredInts->refPredInts.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      refArray.set_size(5, refYVals[b_i].f1.size(1));
      loop_ub = refYVals[b_i].f1.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (int32_T i2{0}; i2 < 5; i2++) {
          refArray[i2 + 5 * i1] = 0.0;
        }
      }

      //  We could possibly use CIFn in one shot here (rather than loop over
      //  points....)
      i1 = refYVals[b_i].f1.size(1);
      if (refYVals[b_i].f1.size(1) - 1 >= 0) {
        thisCol_size = refYVals[b_i].f1.size(0);
        b_loop_ub = refYVals[b_i].f1.size(0);
      }

      for (int32_T points{0}; points < i1; points++) {
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          thisCol_data[i2] = refYVals[b_i].f1[i2 + refYVals[b_i].f1.size(0) *
            points];
        }

        coder::prctile(thisCol_data, thisCol_size, ci95);
        coder::b_prctile(thisCol_data, thisCol_size, ci65);
        refArray[5 * points] = ci95[0];
        refArray[5 * points + 1] = ci65[0];
        refArray[5 * points + 2] = coder::mean(thisCol_data, thisCol_size);
        refArray[5 * points + 3] = ci65[1];
        refArray[5 * points + 4] = ci95[1];
      }

      allPredInts->refPredInts[b_i].f1.set_size(5, refArray.size(1));
      loop_ub = refArray.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (int32_T i2{0}; i2 < 5; i2++) {
          allPredInts->refPredInts[b_i].f1[i2 + 5 * i1] = refArray[i2 + 5 * i1];
        }
      }
    }

    //  Also the SLD's
    if (!domains) {
      allPredInts->sldPredInts.set_size(i, 1);
      for (int32_T b_i{0}; b_i < i; b_i++) {
        sldArray.set_size(5, sldYVals[b_i].f1.size(1));
        loop_ub = sldYVals[b_i].f1.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            sldArray[i2 + 5 * i1] = 0.0;
          }
        }

        i1 = sldYVals[b_i].f1.size(1);
        if (sldYVals[b_i].f1.size(1) - 1 >= 0) {
          b_thisCol_size = sldYVals[b_i].f1.size(0);
          c_loop_ub = sldYVals[b_i].f1.size(0);
        }

        for (int32_T points{0}; points < i1; points++) {
          for (int32_T i2{0}; i2 < c_loop_ub; i2++) {
            thisCol_data[i2] = sldYVals[b_i].f1[i2 + sldYVals[b_i].f1.size(0) *
              points];
          }

          coder::prctile(thisCol_data, b_thisCol_size, ci95);
          coder::b_prctile(thisCol_data, b_thisCol_size, ci65);
          sldArray[5 * points] = ci95[0];
          sldArray[5 * points + 1] = ci65[0];
          sldArray[5 * points + 2] = coder::mean(thisCol_data, b_thisCol_size);
          sldArray[5 * points + 3] = ci65[1];
          sldArray[5 * points + 4] = ci95[1];
        }

        allPredInts->sldPredInts[b_i].f1.set_size(5, sldArray.size(1));
        loop_ub = sldArray.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            allPredInts->sldPredInts[b_i].f1[i2 + 5 * i1] = sldArray[i2 + 5 * i1];
          }
        }
      }
    } else {
      allPredInts->sldPredInts.set_size(i, 2);
      for (int32_T b_i{0}; b_i < i; b_i++) {
        sldArray1.set_size(5, sldYVals[b_i].f1.size(1));
        loop_ub = sldYVals[b_i].f1.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            sldArray1[i2 + 5 * i1] = 0.0;
          }
        }

        sldArray2.set_size(5, sldYVals[b_i + sldYVals.size(0)].f1.size(1));
        loop_ub = sldYVals[b_i + sldYVals.size(0)].f1.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            sldArray2[i2 + 5 * i1] = 0.0;
          }
        }

        i1 = sldYVals[b_i].f1.size(1);
        for (int32_T points{0}; points < i1; points++) {
          real_T thisCol1_data[1000];
          loop_ub = sldYVals[b_i].f1.size(0);
          k = sldYVals[b_i].f1.size(0);
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            thisCol1_data[i2] = sldYVals[b_i].f1[i2 + sldYVals[b_i].f1.size(0) *
              points];
          }

          real_T ci651[2];
          real_T ci951[2];
          coder::prctile(thisCol1_data, k, ci951);
          coder::b_prctile(thisCol1_data, k, ci651);
          sldArray1[5 * points] = ci951[0];
          sldArray1[5 * points + 1] = ci651[0];
          sldArray1[5 * points + 2] = coder::mean(thisCol1_data, k);
          sldArray1[5 * points + 3] = ci651[1];
          sldArray1[5 * points + 4] = ci951[1];
        }

        i1 = sldYVals[b_i + sldYVals.size(0)].f1.size(1);
        for (int32_T points{0}; points < i1; points++) {
          real_T thisCol2_data[1000];
          loop_ub = sldYVals[b_i + sldYVals.size(0)].f1.size(0);
          k = sldYVals[b_i + sldYVals.size(0)].f1.size(0);
          for (int32_T i2{0}; i2 < loop_ub; i2++) {
            thisCol2_data[i2] = sldYVals[b_i + sldYVals.size(0)].f1[i2 +
              sldYVals[b_i + sldYVals.size(0)].f1.size(0) * points];
          }

          real_T ci652[2];
          real_T ci952[2];
          coder::prctile(thisCol2_data, k, ci952);
          coder::b_prctile(thisCol2_data, k, ci652);
          sldArray2[5 * points] = ci952[0];
          sldArray2[5 * points + 1] = ci652[0];
          sldArray2[5 * points + 2] = coder::mean(thisCol2_data, k);
          sldArray2[5 * points + 3] = ci652[1];
          sldArray2[5 * points + 4] = ci952[1];
        }

        allPredInts->sldPredInts[b_i].f1.set_size(5, sldArray1.size(1));
        loop_ub = sldArray1.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            allPredInts->sldPredInts[b_i].f1[i2 + 5 * i1] = sldArray1[i2 + 5 *
              i1];
          }
        }

        allPredInts->sldPredInts[b_i + allPredInts->sldPredInts.size(0)].
          f1.set_size(5, sldArray2.size(1));
        loop_ub = sldArray2.size(1);
        for (i1 = 0; i1 < loop_ub; i1++) {
          for (int32_T i2{0}; i2 < 5; i2++) {
            allPredInts->sldPredInts[b_i + allPredInts->sldPredInts.size(0)]
              .f1[i2 + 5 * i1] = sldArray2[i2 + 5 * i1];
          }
        }
      }
    }
  }
}

// End of code generation (refPrctileConfInts.cpp)
