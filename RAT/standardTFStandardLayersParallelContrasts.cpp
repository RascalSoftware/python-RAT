//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFStandardLayersParallelContrasts.cpp
//
// Code generation for function 'standardTFStandardLayersParallelContrasts'
//

// Include files
#include "standardTFStandardLayersParallelContrasts.h"
#include "allocateLayersForContrast.h"
#include "allocateParamsToLayers.h"
#include "backSort.h"
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
  void standardTFStandardLayersParallelContrasts(const struct0_T *problemDef,
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
    ::coder::array<cell_wrap_20, 2U> outParameterisedLayers;
    ::coder::array<real_T, 2U> Simul;
    ::coder::array<real_T, 2U> b_thisContrastLayers_data;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> reflect;
    ::coder::array<real_T, 2U> resampledLayers;
    ::coder::array<real_T, 2U> shifted_dat;
    ::coder::array<real_T, 2U> sldProfile;
    real_T thisContrastLayers_data[6000];
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
    int32_T thisContrastLayers_size[2];
    int32_T b_i;
    int32_T b_loop_ub;
    int32_T i1;
    int32_T loop_ub;
    int32_T nParams;
    int32_T ub_loop;
    boolean_T useImaginary;

    //  Standard Layers calculation paralelised over the outer loop
    //  This is the main reflectivity calculation of the standard layers
    //  calculation type. It extracts the required paramters for the contrasts
    //  from the input arrays, then passes the main calculation to
    //  'standardLayersCore', which carries out the calculation iteslf.
    //  The core calculation is common for both standard and custom layers.
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

    //  Allocate the memory for the output arrays before the main loop
    backgs.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));

    //  end memory allocation.
    //  First we need to allocate the absolute values of the input
    //  parameters to all the layers in the layers list. This only needs
    //  to be done once, and so is done outside the contrasts loop
    allocateParamsToLayers(problemDef->params, problemDefCells->f6,
      outParameterisedLayers);

    //  Resample parameters if required
    //  Loop over all the contrasts
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
    allRoughs.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    allLayers.set_size(static_cast<int32_T>(problemDef->numberOfContrasts));
    ub_loop = static_cast<int32_T>(problemDef->numberOfContrasts) - 1;

#pragma omp parallel for \
 num_threads(omp_get_max_threads()) \
 private(sldProfile,reflect,Simul,shifted_dat,layerSld,resampledLayers,thisSsubs,thisChiSquared,thisContrastLayers_data,thisContrastLayers_size,thisResol,thisNbs,thisNba,thisSf,thisQshift,thisBackground,dv,dv1,loop_ub,b_i,b_loop_ub,i1) \
 firstprivate(b_thisContrastLayers_data)

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

      //  Also need to determine which layers from the overall layers list
      //  are required for this contrast, and put them in the correct order
      //  according to geometry
      allocateLayersForContrast(problemDefCells->f5[i].f1,
        outParameterisedLayers, useImaginary, thisContrastLayers_data,
        thisContrastLayers_size);

      //  For the other parameters, we extract the correct ones from the input
      //  arrays
      //  Substrate roughness is always first parameter for standard layers
      //  Now call the core standardTF_stanlay reflectivity calculation
      //  In this case we are single cored, so we do not parallelise over
      //  points
      //  Call the core layers calculation
      b_thisContrastLayers_data.set(&thisContrastLayers_data[0],
        thisContrastLayers_size[0], thisContrastLayers_size[1]);
      dv[0] = problemDefCells->f3[i].f1[0];
      dv[1] = problemDefCells->f3[i].f1[1];
      dv1[0] = problemDefCells->f1[i].f1[0];
      dv1[1] = problemDefCells->f1[i].f1[1];
      standardTFLayersCore(b_thisContrastLayers_data, problemDef->params[0],
                           problemDef->geometry.data, problemDef->geometry.size,
                           thisNba, thisNbs, problemDef->resample[i], calcSld,
                           thisSf, thisQshift, problemDef->dataPresent[i],
                           problemDefCells->f2[i].f1, dv, (const real_T *)((::
        coder::array<real_T, 2U> *)&problemDefCells->f4[i].f1)->data(), dv1,
                           thisBackground, thisResol,
                           problemDef->contrastBacksType[i], static_cast<real_T>
                           (nParams), controls->resamPars, useImaginary,
                           sldProfile, reflect, Simul, shifted_dat, layerSld,
                           resampledLayers, &thisChiSquared, &thisSsubs);

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

      chis[i] = thisChiSquared;
      backgs[i] = thisBackground;
      qshifts[i] = thisQshift;
      sfs[i] = thisSf;
      nbas[i] = thisNba;
      nbss[i] = thisNbs;
      resols[i] = thisResol;
      allRoughs[i] = problemDef->params[0];
      loop_ub = resampledLayers.size(1);
      allLayers[i].f1.set_size(resampledLayers.size(0), resampledLayers.size(1));
      for (b_i = 0; b_i < loop_ub; b_i++) {
        b_loop_ub = resampledLayers.size(0);
        for (i1 = 0; i1 < b_loop_ub; i1++) {
          allLayers[i].f1[i1 + allLayers[i].f1.size(0) * b_i] =
            resampledLayers[i1 + resampledLayers.size(0) * b_i];
        }
      }
    }
  }
}

// End of code generation (standardTFStandardLayersParallelContrasts.cpp)
