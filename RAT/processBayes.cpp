//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processBayes.cpp
//
// Code generation for function 'processBayes'
//

// Include files
#include "processBayes.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "prctileConfInts.h"
#include "refPrctileConfInts.h"
#include "reflectivityCalculation.h"
#include "rt_nonfinite.h"
#include "unpackParams.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void processBayes(const real_T bayesOutputs_bestPars_data[], const int32_T
                    bayesOutputs_bestPars_size[2], const ::coder::array<real_T,
                    2U> &bayesOutputs_chain, const struct5_T *allProblem_f1,
                    const struct2_T *allProblem_f2, const cell_14 *allProblem_f4,
                    struct5_T *problemDef, struct_T *outProblem, cell_13 *result,
                    e_struct_T *bayesResults_bestFitsMean, f_struct_T
                    *bayesResults_predlims, struct11_T *bayesResults_parConfInts)
  {
    static struct2_T controlsStruct;
    struct5_T b_problemDef;
    int32_T loop_ub;

    // problem = {problemDef ; controls ; problemDefLimits ; problemDefCells};
    *problemDef = *allProblem_f1;
    controlsStruct = *allProblem_f2;

    //  Need to impose that we calculate the SLD..
    controlsStruct.calcSldDuringFit = true;

    // ... and use the Bayes bestpars
    problemDef->fitParams.set_size(1, bayesOutputs_bestPars_size[1]);
    loop_ub = bayesOutputs_bestPars_size[1];
    for (int32_T i{0}; i < loop_ub; i++) {
      problemDef->fitParams[problemDef->fitParams.size(0) * i] =
        bayesOutputs_bestPars_data[i];
    }

    unpackParams(problemDef, allProblem_f2->checks.fitParam,
                 allProblem_f2->checks.fitBackgroundParam,
                 allProblem_f2->checks.fitQzshift,
                 allProblem_f2->checks.fitScalefactor,
                 allProblem_f2->checks.fitBulkIn,
                 allProblem_f2->checks.fitBulkOut,
                 allProblem_f2->checks.fitResolutionParam,
                 allProblem_f2->checks.fitDomainRatio);
    prctileConfInts(bayesOutputs_chain, bayesResults_parConfInts->par95,
                    bayesResults_parConfInts->par65,
                    bayesResults_parConfInts->mean);

    // iterShortest(output.chain,length(fitNames),[],0.95);
    //  % 2. Find maximum values of posteriors. Store the max and mean posterior
    //  %    values, and calculate the best fit and SLD's from these.
    //  [bestPars_max,posteriors] = findPosteriorsMax(output.chain);
    //  bestPars_mean = output.results.mean;
    //  % Calculate Max best fit curves
    //  controls.calcSldDuringFit = true;
    //  problemDef.fitParams = bestPars_max;
    //  problemDef = unpackParams(problemDef,controls);
    //  [outProblem,result] = reflectivityCalculation(problemDef,problemDefCells,controls);
    //  bestFitMax_Ref = result(1);
    //  bestFitMax_Sld = result(5);
    //  bestFitMax_chi = outProblem.calculations.sumChi;
    //  Calculate 'mean' best fit curves
    //  problemDef.fitParams = parConfInts.mean;
    //  problemDef = unpackParams(problemDef,controlsStruct);
    reflectivityCalculation(problemDef, allProblem_f4, &controlsStruct,
      outProblem, result);

    //  Reflectivity art points
    //  Reflectivity between sim limits
    //  Data corrected for sfs
    //  Layers if defined (i.e. not customXY)
    //  Calculated SLD profiles
    //  Resampled layers
    //  For compile, we can't remove a field, so just clear it for now...
    // problem = rmfield(problem,'calculations');
    bayesResults_bestFitsMean->ref.set_size(result->f1.size(0));
    loop_ub = result->f1.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      bayesResults_bestFitsMean->ref[i] = result->f1[i];
    }

    bayesResults_bestFitsMean->sld.set_size(result->f5.size(0), result->f5.size
      (1));
    loop_ub = result->f5.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      int32_T b_loop_ub;
      b_loop_ub = result->f5.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        bayesResults_bestFitsMean->sld[i1 + bayesResults_bestFitsMean->sld.size
          (0) * i] = result->f5[i1 + result->f5.size(0) * i];
      }
    }

    bayesResults_bestFitsMean->chi = outProblem->calculations.sumChi;
    bayesResults_bestFitsMean->data.set_size(result->f3.size(0));
    loop_ub = result->f3.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      bayesResults_bestFitsMean->data[i] = result->f3[i];
    }

    //  2. Reflectivity and SLD shading
    //  predIntRef = mcmcpred_compile(output.results,output.chain,[],output.data,problem,500);
    //  predIntRef = predIntRef.predlims;
    //
    //  predIntSld_calcs = mcmcpred_compile_sld(output.results,output.chain,bestFitMean_Sld,[],output.data,problem,500);
    //  predIntSld = predIntSld_calcs.predlims;
    //  predIntSld_xdata = predIntSld_calcs.data;
    b_problemDef = *problemDef;
    refPrctileConfInts(bayesOutputs_chain, &b_problemDef, allProblem_f4,
                       &controlsStruct, bayesResults_predlims);

    //  ---------------------------------
    //  bayesResults.chain = bayesOutputs.chain;
    // bayesResults.bestPars_Max = bestPars_max;
    // bayesResults.bayesData = bayesOutputs.data;
    //  bayesResults.bestFitsMax = {bestFitMax_Ref, bestFitMax_Sld, bestFitMax_chi};
    //  bayesResults.bestFitsMean = bestFitMean;
    //  bayesResults.predlims = allPredInts;
    //  bayesResults.parConfInts = parConfInts;
    //  bayesResults.bestPars = bayesOutputs.bestPars;
  }

  void processBayes(const ::coder::array<real_T, 2U> &bayesOutputs_bestPars,
                    const ::coder::array<real_T, 2U> &bayesOutputs_chain, const
                    struct5_T *allProblem_f1, const struct2_T *allProblem_f2,
                    const cell_14 *allProblem_f4, struct5_T *problemDef,
                    struct_T *outProblem, cell_13 *result, e_struct_T
                    *bayesResults_bestFitsMean, f_struct_T
                    *bayesResults_predlims, struct11_T *bayesResults_parConfInts)
  {
    struct2_T controlsStruct;
    struct5_T b_problemDef;
    int32_T loop_ub;

    // problem = {problemDef ; controls ; problemDefLimits ; problemDefCells};
    *problemDef = *allProblem_f1;
    controlsStruct = *allProblem_f2;

    //  Need to impose that we calculate the SLD..
    controlsStruct.calcSldDuringFit = true;

    // ... and use the Bayes bestpars
    problemDef->fitParams.set_size(1, bayesOutputs_bestPars.size(1));
    loop_ub = bayesOutputs_bestPars.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      problemDef->fitParams[problemDef->fitParams.size(0) * i] =
        bayesOutputs_bestPars[i];
    }

    unpackParams(problemDef, allProblem_f2->checks.fitParam,
                 allProblem_f2->checks.fitBackgroundParam,
                 allProblem_f2->checks.fitQzshift,
                 allProblem_f2->checks.fitScalefactor,
                 allProblem_f2->checks.fitBulkIn,
                 allProblem_f2->checks.fitBulkOut,
                 allProblem_f2->checks.fitResolutionParam,
                 allProblem_f2->checks.fitDomainRatio);
    prctileConfInts(bayesOutputs_chain, bayesResults_parConfInts->par95,
                    bayesResults_parConfInts->par65,
                    bayesResults_parConfInts->mean);

    // iterShortest(output.chain,length(fitNames),[],0.95);
    //  % 2. Find maximum values of posteriors. Store the max and mean posterior
    //  %    values, and calculate the best fit and SLD's from these.
    //  [bestPars_max,posteriors] = findPosteriorsMax(output.chain);
    //  bestPars_mean = output.results.mean;
    //  % Calculate Max best fit curves
    //  controls.calcSldDuringFit = true;
    //  problemDef.fitParams = bestPars_max;
    //  problemDef = unpackParams(problemDef,controls);
    //  [outProblem,result] = reflectivityCalculation(problemDef,problemDefCells,controls);
    //  bestFitMax_Ref = result(1);
    //  bestFitMax_Sld = result(5);
    //  bestFitMax_chi = outProblem.calculations.sumChi;
    //  Calculate 'mean' best fit curves
    //  problemDef.fitParams = parConfInts.mean;
    //  problemDef = unpackParams(problemDef,controlsStruct);
    reflectivityCalculation(problemDef, allProblem_f4, &controlsStruct,
      outProblem, result);

    //  Reflectivity art points
    //  Reflectivity between sim limits
    //  Data corrected for sfs
    //  Layers if defined (i.e. not customXY)
    //  Calculated SLD profiles
    //  Resampled layers
    //  For compile, we can't remove a field, so just clear it for now...
    // problem = rmfield(problem,'calculations');
    bayesResults_bestFitsMean->ref.set_size(result->f1.size(0));
    loop_ub = result->f1.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      bayesResults_bestFitsMean->ref[i] = result->f1[i];
    }

    bayesResults_bestFitsMean->sld.set_size(result->f5.size(0), result->f5.size
      (1));
    loop_ub = result->f5.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      int32_T b_loop_ub;
      b_loop_ub = result->f5.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        bayesResults_bestFitsMean->sld[i1 + bayesResults_bestFitsMean->sld.size
          (0) * i] = result->f5[i1 + result->f5.size(0) * i];
      }
    }

    bayesResults_bestFitsMean->chi = outProblem->calculations.sumChi;
    bayesResults_bestFitsMean->data.set_size(result->f3.size(0));
    loop_ub = result->f3.size(0);
    for (int32_T i{0}; i < loop_ub; i++) {
      bayesResults_bestFitsMean->data[i] = result->f3[i];
    }

    //  2. Reflectivity and SLD shading
    //  predIntRef = mcmcpred_compile(output.results,output.chain,[],output.data,problem,500);
    //  predIntRef = predIntRef.predlims;
    //
    //  predIntSld_calcs = mcmcpred_compile_sld(output.results,output.chain,bestFitMean_Sld,[],output.data,problem,500);
    //  predIntSld = predIntSld_calcs.predlims;
    //  predIntSld_xdata = predIntSld_calcs.data;
    b_problemDef = *problemDef;
    refPrctileConfInts(bayesOutputs_chain, &b_problemDef, allProblem_f4,
                       &controlsStruct, bayesResults_predlims);

    //  ---------------------------------
    //  bayesResults.chain = bayesOutputs.chain;
    // bayesResults.bestPars_Max = bestPars_max;
    // bayesResults.bayesData = bayesOutputs.data;
    //  bayesResults.bestFitsMax = {bestFitMax_Ref, bestFitMax_Sld, bestFitMax_chi};
    //  bayesResults.bestFitsMean = bestFitMean;
    //  bayesResults.predlims = allPredInts;
    //  bayesResults.parConfInts = parConfInts;
    //  bayesResults.bestPars = bayesOutputs.bestPars;
  }
}

// End of code generation (processBayes.cpp)
