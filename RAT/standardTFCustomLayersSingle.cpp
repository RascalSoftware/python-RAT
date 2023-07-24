//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFCustomLayersSingle.cpp
//
// Code generation for function 'standardTFCustomLayersSingle'
//

// Include files
#include "standardTFCustomLayersSingle.h"
#include "backSort.h"
#include "customModelClass.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFLayersCore.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  void standardTFCustomLayersSingle(const struct0_T *problemDef, const cell_19
    *problemDefCells, const struct2_T *controls, ::coder::array<real_T, 1U>
    &outSsubs, ::coder::array<real_T, 1U> &backgs, ::coder::array<real_T, 1U>
    &qshifts, ::coder::array<real_T, 1U> &sfs, ::coder::array<real_T, 1U> &nbas,
    ::coder::array<real_T, 1U> &nbss, ::coder::array<real_T, 1U> &resols, ::
    coder::array<real_T, 1U> &chis, ::coder::array<cell_wrap_9, 1U>
    &reflectivity, ::coder::array<cell_wrap_9, 1U> &Simulation, ::coder::array<
    cell_wrap_15, 1U> &shifted_data, ::coder::array<cell_wrap_15, 1U> &layerSlds,
    ::coder::array<cell_wrap_15, 1U> &sldProfiles, ::coder::array<cell_wrap_15,
    1U> &allLayers, ::coder::array<real_T, 1U> &allRoughs)
  {
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> resamLayers;
    ::coder::array<real_T, 2U> shifted_dat;
    ::coder::array<real_T, 2U> sldProfile;
    real_T thisBackground;
    real_T thisNba;
    real_T thisNbs;
    real_T thisQshift;
    real_T thisResol;
    real_T thisSf;
    int32_T i;

    //  Single threaded version of the custom layers, standardTF reflectivity
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
    //  Pre-Allocation of output arrays...
    i = static_cast<int32_T>(problemDef->numberOfContrasts);
    backgs.set_size(i);

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

    //  Single cored over all contrasts
    outSsubs.set_size(i);
    sldProfiles.set_size(i);
    reflectivity.set_size(i);
    Simulation.set_size(i);
    shifted_data.set_size(i);
    layerSlds.set_size(i);
    chis.set_size(i);
    qshifts.set_size(i);
    sfs.set_size(i);
    nbas.set_size(i);
    nbss.set_size(i);
    resols.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T dv[2];
      real_T dv1[2];
      int32_T b_loop_ub;
      int32_T loop_ub;

      //  Extract the relevant parameter values for this contrast
      //  from the input arrays.
      //  First need to decide which values of the backrounds, scalefactors
      //  data shifts and bulk contrasts are associated with this contrast
      backSort(problemDef->contrastBacks[b_i], problemDef->contrastShifts[b_i],
               problemDef->contrastScales[b_i], problemDef->contrastNbas[b_i],
               problemDef->contrastNbss[b_i], problemDef->contrastRes[b_i],
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
      dv[0] = problemDefCells->f3[b_i].f1[0];
      dv[1] = problemDefCells->f3[b_i].f1[1];
      dv1[0] = problemDefCells->f1[b_i].f1[0];
      dv1[1] = problemDefCells->f1[b_i].f1[1];
      standardTFLayersCore(allLayers[b_i].f1, allRoughs[b_i],
                           problemDef->geometry.data, problemDef->geometry.size,
                           thisNba, thisNbs, problemDef->resample[b_i],
                           controls->calcSld, thisSf, thisQshift,
                           problemDef->dataPresent[b_i], problemDefCells->f2[b_i]
                           .f1, dv, (const real_T *)((::coder::array<real_T, 2U>
        *)&problemDefCells->f4[b_i].f1)->data(), dv1, thisBackground, thisResol,
                           problemDef->contrastBacksType[b_i],
                           static_cast<real_T>(problemDef->params.size(1)),
                           controls->resamPars, problemDef->useImaginary,
                           sldProfile, reflectivity[b_i].f1, Simulation[b_i].f1,
                           shifted_dat, layerSld, resamLayers, &chis[b_i],
                           &outSsubs[b_i]);

      //  Store returned values for this contrast in the output arrays.
      //  As well as the calculated profiles, we also store a record of
      //  the other values (background, scalefactors etc) for each contrast
      //  for future use.
      loop_ub = sldProfile.size(1);
      sldProfiles[b_i].f1.set_size(sldProfile.size(0), sldProfile.size(1));
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_loop_ub = sldProfile.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          sldProfiles[b_i].f1[i2 + sldProfiles[b_i].f1.size(0) * i1] =
            sldProfile[i2 + sldProfile.size(0) * i1];
        }
      }

      loop_ub = shifted_dat.size(1);
      shifted_data[b_i].f1.set_size(shifted_dat.size(0), shifted_dat.size(1));
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_loop_ub = shifted_dat.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          shifted_data[b_i].f1[i2 + shifted_data[b_i].f1.size(0) * i1] =
            shifted_dat[i2 + shifted_dat.size(0) * i1];
        }
      }

      loop_ub = layerSld.size(1);
      layerSlds[b_i].f1.set_size(layerSld.size(0), layerSld.size(1));
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_loop_ub = layerSld.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          layerSlds[b_i].f1[i2 + layerSlds[b_i].f1.size(0) * i1] = layerSld[i2 +
            layerSld.size(0) * i1];
        }
      }

      loop_ub = resamLayers.size(1);
      allLayers[b_i].f1.set_size(resamLayers.size(0), resamLayers.size(1));
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_loop_ub = resamLayers.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1] =
            resamLayers[i2 + resamLayers.size(0) * i1];
        }
      }

      backgs[b_i] = thisBackground;
      qshifts[b_i] = thisQshift;
      sfs[b_i] = thisSf;
      nbas[b_i] = thisNba;
      nbss[b_i] = thisNbs;
      resols[b_i] = thisResol;
    }
  }
}

// End of code generation (standardTFCustomLayersSingle.cpp)
