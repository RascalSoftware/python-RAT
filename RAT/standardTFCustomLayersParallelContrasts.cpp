//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFCustomLayersParallelContrasts.cpp
//
// Code generation for function 'standardTFCustomLayersParallelContrasts'
//

// Include files
#include "standardTFCustomLayersParallelContrasts.h"
#include "backSort.h"
#include "customModelClass.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFLayersCore.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include "omp.h"

// Function Definitions
namespace RAT
{
  void standardTFCustomLayersParallelContrasts(const struct0_T *problemDef,
    const cell_19 *problemDefCells, const struct2_T *controls, ::coder::array<
    real_T, 1U> &outSsubs, ::coder::array<real_T, 1U> &backgs, ::coder::array<
    real_T, 1U> &qshifts, ::coder::array<real_T, 1U> &sfs, ::coder::array<real_T,
    1U> &nbas, ::coder::array<real_T, 1U> &nbss, ::coder::array<real_T, 1U>
    &resols, ::coder::array<real_T, 1U> &chis, ::coder::array<cell_wrap_9, 1U>
    &reflectivity, ::coder::array<cell_wrap_9, 1U> &Simulation, ::coder::array<
    cell_wrap_15, 1U> &shifted_data, ::coder::array<cell_wrap_15, 1U> &layerSlds,
    ::coder::array<cell_wrap_15, 1U> &sldProfiles, ::coder::array<cell_wrap_15,
    1U> &allLayers, ::coder::array<real_T, 1U> &allRoughs)
  {
    ::coder::array<real_T, 2U> Simul;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> reflect;
    ::coder::array<real_T, 2U> resamLayers;
    ::coder::array<real_T, 2U> shifted_dat;
    ::coder::array<real_T, 2U> sldProfile;
    real_T dv[2];
    real_T dv1[2];
    real_T calcSld;
    real_T thisBackground;
    real_T thisChiSquared;
    real_T thisNba;
    real_T thisNbs;
    real_T thisQshift;
    real_T thisResol;
    real_T thisSf;
    real_T thisSsubs;
    int32_T b_i;
    int32_T b_loop_ub;
    int32_T i1;
    int32_T loop_ub;
    int32_T nParams;
    int32_T ub_loop;
    boolean_T useImaginary;

    //  Multi threaded version of the custom layers, standardTF reflectivity
    //  calculation. The function extracts the relevant parameters from the input
    //  arrays, allocates these on a pre-contrast basis, then calls the 'core'
    //  calculation (the core layers standardTf calc is shared between multiple
    //  calculation types).
    //  Extract individual cell arrays
    //  Splits up the master input list of all arrays into separate arrays
    //  The min input array 'problemDefCells' is a master array where
    //  all the cell arrays are grouped together. There are
    //  repeatLayers      - controls repeating of the layers stack
    //  allData           - Array of all the data arrays
    //  dataLimits        - Min max limits in q for the data arrays
    //  simLimits         - Limits in Q for the reflkectivity simulations
    //  Layers details    - Master array of all available layers
    //  contrastLayers    - Which specific combination of arrays are needed for
    //                      each contrast.
    //  Custom files      - Filenames and path for any custom files used
    //  Extract individual parameters from problemDef struct
    // Extract individual parameters from problemDef
    nParams = problemDef->params.size(1);
    calcSld = controls->calcSld;
    useImaginary = problemDef->useImaginary;

    //  Pre-Allocation of output arrays...
    backgs.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));

    //    --- End Memory Allocation ---
    //  Resampling parameters
    //  Process the custom models....
    customModelClass::processCustomLayers(problemDef->contrastBacks,
      problemDef->contrastShifts, problemDef->contrastScales,
      problemDef->contrastNbas, problemDef->contrastNbss,
      problemDef->contrastRes, problemDef->backs, problemDef->shifts,
      problemDef->sf, problemDef->nba, problemDef->nbs, problemDef->res,
      problemDef->contrastCustomFiles, problemDef->numberOfContrasts,
      problemDefCells->f14, problemDef->params, problemDef->useImaginary,
      allLayers, allRoughs);

    //  Multi cored over all contrasts
    outSsubs.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    sldProfiles.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    reflectivity.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    Simulation.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    shifted_data.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    layerSlds.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    chis.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    qshifts.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    sfs.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    nbas.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    nbss.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    resols.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    ub_loop = static_cast<int32_T>(problemDef->numberOfContrasts) - 1;

#pragma omp parallel for \
 num_threads(omp_get_max_threads()) \
 private(sldProfile,reflect,Simul,shifted_dat,layerSld,resamLayers,thisSsubs,thisChiSquared,thisResol,thisNbs,thisNba,thisSf,thisQshift,thisBackground,dv,dv1,loop_ub,b_i,b_loop_ub,i1)

    for (int32_T i = 0; i <= ub_loop; i++) {
      //  Extract the relevant parameter values for this contrast
      //  from the input arrays.
      //  First need to decide which values of the backrounds, scalefactors
      //  data shifts and bulk contrasts are associated with this contrast
      backSort(problemDef->contrastBacks[i], problemDef->contrastShifts[i],
               problemDef->contrastScales[i], problemDef->contrastNbas[i],
               problemDef->contrastNbss[i], problemDef->contrastRes[i],
               problemDef->backs, problemDef->shifts, problemDef->sf,
               problemDef->nba, problemDef->nbs, problemDef->res,
               &thisBackground, &thisQshift, &thisSf, &thisNba, &thisNbs,
               &thisResol);

      //  Get the custom layers output for this contrast
      //  For the other parameters, we extract the correct ones from the input
      //  arrays
      //  Now call the core standardTF_stanlay reflectivity calculation
      //  In this case we are single cored, so we do not parallelise over
      //  points
      //  Call the reflectivity calculation
      dv[0] = problemDefCells->f3[i].f1[0];
      dv[1] = problemDefCells->f3[i].f1[1];
      dv1[0] = problemDefCells->f1[i].f1[0];
      dv1[1] = problemDefCells->f1[i].f1[1];
      standardTFLayersCore(allLayers[i].f1, allRoughs[i],
                           problemDef->geometry.data, problemDef->geometry.size,
                           thisNba, thisNbs, problemDef->resample[i], calcSld,
                           thisSf, thisQshift, problemDef->dataPresent[i],
                           problemDefCells->f2[i].f1, dv, (const real_T *)((::
        coder::array<real_T, 2U> *)&problemDefCells->f4[i].f1)->data(), dv1,
                           thisBackground, thisResol,
                           problemDef->contrastBacksType[i], static_cast<real_T>
                           (nParams), controls->resamPars, useImaginary,
                           sldProfile, reflect, Simul, shifted_dat, layerSld,
                           resamLayers, &thisChiSquared, &thisSsubs);

      //  Store returned values for this contrast in the output arrays.
      //  As well as the calculated profiles, we also store a record of
      //  the other values (background, scalefactors etc) for each contrast
      //  for future use.
      outSsubs[i] = thisSsubs;
      loop_ub = sldProfile.size(1);
      sldProfiles[i].f1.set_size(sldProfile.size(0), sldProfile.size(1));
      for (b_i = 0; b_i < loop_ub; b_i++) {
        b_loop_ub = sldProfile.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          sldProfiles[i].f1[i1 + sldProfiles[i].f1.size(0) * b_i] =
            sldProfile[i1 + sldProfile.size(0) * b_i];
        }
      }

      loop_ub = reflect.size(0);
      reflectivity[i].f1.set_size(reflect.size(0), 2);
      b_loop_ub = Simul.size(0);
      Simulation[i].f1.set_size(Simul.size(0), 2);
      for (b_i = 0; b_i < 2; b_i++) {
        for (i1 = 0; i1 < loop_ub; i1++) {
          reflectivity[i].f1[i1 + reflectivity[i].f1.size(0) * b_i] = reflect[i1
            + reflect.size(0) * b_i];
        }

        for (i1 = 0; i1 < b_loop_ub; i1++) {
          Simulation[i].f1[i1 + Simulation[i].f1.size(0) * b_i] = Simul[i1 +
            Simul.size(0) * b_i];
        }
      }

      loop_ub = shifted_dat.size(1);
      shifted_data[i].f1.set_size(shifted_dat.size(0), shifted_dat.size(1));
      for (b_i = 0; b_i < loop_ub; b_i++) {
        b_loop_ub = shifted_dat.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          shifted_data[i].f1[i1 + shifted_data[i].f1.size(0) * b_i] =
            shifted_dat[i1 + shifted_dat.size(0) * b_i];
        }
      }

      loop_ub = layerSld.size(1);
      layerSlds[i].f1.set_size(layerSld.size(0), layerSld.size(1));
      for (b_i = 0; b_i < loop_ub; b_i++) {
        b_loop_ub = layerSld.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          layerSlds[i].f1[i1 + layerSlds[i].f1.size(0) * b_i] = layerSld[i1 +
            layerSld.size(0) * b_i];
        }
      }

      loop_ub = resamLayers.size(1);
      allLayers[i].f1.set_size(resamLayers.size(0), resamLayers.size(1));
      for (b_i = 0; b_i < loop_ub; b_i++) {
        b_loop_ub = resamLayers.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          allLayers[i].f1[i1 + allLayers[i].f1.size(0) * b_i] = resamLayers[i1 +
            resamLayers.size(0) * b_i];
        }
      }

      chis[i] = thisChiSquared;
      backgs[i] = thisBackground;
      qshifts[i] = thisQshift;
      sfs[i] = thisSf;
      nbas[i] = thisNba;
      nbss[i] = thisNbs;
      resols[i] = thisResol;
    }
  }
}

// End of code generation (standardTFCustomLayersParallelContrasts.cpp)
