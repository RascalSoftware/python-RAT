//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFStandardLayersSingle.cpp
//
// Code generation for function 'standardTFStandardLayersSingle'
//

// Include files
#include "standardTFStandardLayersSingle.h"
#include "allocateLayersForContrast.h"
#include "allocateParamsToLayers.h"
#include "backSort.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFLayersCore.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  void standardTFStandardLayersSingle(const struct0_T *problemDef, const cell_19
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
    ::coder::array<cell_wrap_20, 2U> outParameterisedLayers;
    ::coder::array<real_T, 2U> b_thisContrastLayers_data;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> resampledLayers;
    ::coder::array<real_T, 2U> shifted_dat;
    ::coder::array<real_T, 2U> sldProfile;
    real_T thisContrastLayers_data[6000];
    real_T thisBackground;
    real_T thisNba;
    real_T thisNbs;
    real_T thisQshift;
    real_T thisResol;
    real_T thisSf;
    int32_T i;
    boolean_T useImaginary;

    //  Single theraded version of the Standard Layers calculation
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
    useImaginary = problemDef->useImaginary;

    //  Allocate the memory for the output arrays before the main loop
    i = static_cast<int32_T>(problemDef->numberOfContrasts);
    backgs.set_size(i);

    //  end memory allocation.
    //  First we need to allocate the absolute values of the input
    //  parameters to all the layers in the layers list. This only needs
    //  to be done once, and so is done outside the contrasts loop
    allocateParamsToLayers(problemDef->params, problemDefCells->f6,
      outParameterisedLayers);

    //  Resample params if requiired
    //  Loop over all the contrasts
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
    allRoughs.set_size(i);
    allLayers.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T dv[2];
      real_T dv1[2];
      int32_T thisContrastLayers_size[2];
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

      //  Also need to determine which layers from the overall layers list
      //  are required for this contrast, and put them in the correct order
      //  according to geometry
      allocateLayersForContrast(problemDefCells->f5[b_i].f1,
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
      dv[0] = problemDefCells->f3[b_i].f1[0];
      dv[1] = problemDefCells->f3[b_i].f1[1];
      dv1[0] = problemDefCells->f1[b_i].f1[0];
      dv1[1] = problemDefCells->f1[b_i].f1[1];
      standardTFLayersCore(b_thisContrastLayers_data, problemDef->params[0],
                           problemDef->geometry.data, problemDef->geometry.size,
                           thisNba, thisNbs, problemDef->resample[b_i],
                           controls->calcSld, thisSf, thisQshift,
                           problemDef->dataPresent[b_i], problemDefCells->f2[b_i]
                           .f1, dv, (const real_T *)((::coder::array<real_T, 2U>
        *)&problemDefCells->f4[b_i].f1)->data(), dv1, thisBackground, thisResol,
                           problemDef->contrastBacksType[b_i],
                           static_cast<real_T>(problemDef->params.size(1)),
                           controls->resamPars, useImaginary, sldProfile,
                           reflectivity[b_i].f1, Simulation[b_i].f1, shifted_dat,
                           layerSld, resampledLayers, &chis[b_i], &outSsubs[b_i]);

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

      backgs[b_i] = thisBackground;
      qshifts[b_i] = thisQshift;
      sfs[b_i] = thisSf;
      nbas[b_i] = thisNba;
      nbss[b_i] = thisNbs;
      resols[b_i] = thisResol;
      allRoughs[b_i] = problemDef->params[0];
      loop_ub = resampledLayers.size(1);
      allLayers[b_i].f1.set_size(resampledLayers.size(0), resampledLayers.size(1));
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        b_loop_ub = resampledLayers.size(0);
        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1] =
            resampledLayers[i2 + resampledLayers.size(0) * i1];
        }
      }
    }
  }
}

// End of code generation (standardTFStandardLayersSingle.cpp)
