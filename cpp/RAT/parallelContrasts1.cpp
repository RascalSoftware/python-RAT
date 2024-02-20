//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// parallelContrasts1.cpp
//
// Code generation for function 'parallelContrasts1'
//

// Include files
#include "parallelContrasts1.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "backSort.h"
#include "coreLayersCalculation.h"
#include "processCustomFunction.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include "omp.h"

// Function Definitions
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace customLayers
    {
      void c_parallelContrasts(const c_struct_T *problemStruct, const cell_11
        *problemCells, const struct2_T *controls, ::coder::array<real_T, 1U>
        &outSsubs, ::coder::array<real_T, 1U> &backgroundParams, ::coder::array<
        real_T, 1U> &qzshifts, ::coder::array<real_T, 1U> &scalefactors, ::coder::
        array<real_T, 1U> &bulkIns, ::coder::array<real_T, 1U> &bulkOuts, ::
        coder::array<real_T, 1U> &resolutionParams, ::coder::array<real_T, 1U>
        &chis, ::coder::array<cell_wrap_20, 1U> &reflectivity, ::coder::array<
        cell_wrap_20, 1U> &simulation, ::coder::array<cell_wrap_8, 1U>
        &shiftedData, ::coder::array<cell_wrap_8, 1U> &layerSlds, ::coder::array<
        cell_wrap_8, 1U> &sldProfiles, ::coder::array<cell_wrap_8, 1U>
        &allLayers, ::coder::array<real_T, 1U> &allRoughs)
      {
        ::coder::array<real_T, 2U> layerSld;
        ::coder::array<real_T, 2U> reflect;
        ::coder::array<real_T, 2U> resamLayers;
        ::coder::array<real_T, 2U> shiftedDat;
        ::coder::array<real_T, 2U> simul;
        ::coder::array<real_T, 2U> sldProfile;
        real_T b_dv[2];
        real_T b_dv1[2];
        real_T dv2[2];
        real_T thisBackground;
        real_T thisBulkIn;
        real_T thisBulkOut;
        real_T thisChiSquared;
        real_T thisQzshift;
        real_T thisResol;
        real_T thisScalefactor;
        real_T thisSsubs;
        int32_T b_i;
        int32_T b_loop_ub;
        int32_T i1;
        int32_T loop_ub;
        int32_T nParams;
        int32_T ub_loop;
        boolean_T calcSld;
        boolean_T useImaginary;

        //  Multi threaded version of the custom layers, nonPolarisedTF reflectivity
        //  calculation. The function extracts the relevant parameters from the input
        //  arrays, allocates these on a pre-contrast basis, then calls the 'core'
        //  calculation (the core layers nonPolarisedTF calc is shared between
        //  multiple calculation types).
        //  Extract individual cell arrays
        //  Splits up the master input list of all arrays into separate arrays
        //
        //  INPUTS:
        //      * problemCells: cell array where all the project data is grouped together.
        //
        //  OUTPUTS:
        //      * repeatLayers: controls repeating of the layers stack.
        //      * allData: Array of all the data arrays.
        //      * dataLimits: Min max limits in q for the data arrays.
        //      * simLimits: Limits in Q for the reflectivity simulations.
        //      * layersDetails: Master array of all available layers.
        //      * contrastLayers: Which specific combination of arrays are needed for each contrast.
        //      * customFiles:Filenames and path for any custom files used.
        //  Extract individual parameters from problemStruct
        // Extract individual parameters from problem
        nParams = problemStruct->params.size(1);
        calcSld = controls->calcSldDuringFit;
        useImaginary = problemStruct->useImaginary;

        //  Pre-Allocation of output arrays...
        backgroundParams.set_size(static_cast<int32_T>
          (problemStruct->numberOfContrasts));

        //    --- End Memory Allocation ---
        //  Resampling parameters
        //  Process the custom models....
        processCustomFunction(problemStruct->contrastBulkIns,
                              problemStruct->contrastBulkOuts,
                              problemStruct->bulkIn, problemStruct->bulkOut,
                              problemStruct->contrastCustomFiles,
                              problemStruct->numberOfContrasts,
                              problemCells->f14, problemStruct->params,
                              problemStruct->useImaginary, allLayers, allRoughs);

        //  Multi cored over all contrasts
        outSsubs.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        sldProfiles.set_size(static_cast<int32_T>
                             (problemStruct->numberOfContrasts));
        reflectivity.set_size(static_cast<int32_T>
                              (problemStruct->numberOfContrasts));
        simulation.set_size(static_cast<int32_T>
                            (problemStruct->numberOfContrasts));
        shiftedData.set_size(static_cast<int32_T>
                             (problemStruct->numberOfContrasts));
        layerSlds.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        chis.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        qzshifts.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        scalefactors.set_size(static_cast<int32_T>
                              (problemStruct->numberOfContrasts));
        bulkIns.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        bulkOuts.set_size(static_cast<int32_T>(problemStruct->numberOfContrasts));
        resolutionParams.set_size(static_cast<int32_T>
          (problemStruct->numberOfContrasts));
        ub_loop = static_cast<int32_T>(problemStruct->numberOfContrasts) - 1;

#pragma omp parallel for \
 num_threads(omp_get_max_threads()) \
 private(sldProfile,reflect,simul,shiftedDat,layerSld,resamLayers,thisSsubs,thisChiSquared,thisResol,thisBulkOut,thisBulkIn,thisScalefactor,thisQzshift,thisBackground,b_dv,b_dv1,dv2,loop_ub,b_i,b_loop_ub,i1)

        for (int32_T i = 0; i <= ub_loop; i++) {
          //  Extract the relevant parameter values for this contrast
          //  from the input arrays.
          //  First need to decide which values of the backgrounds, scalefactors
          //  data shifts and bulk contrasts are associated with this contrast
          backSort(problemStruct->contrastBackgrounds[i],
                   problemStruct->contrastQzshifts[i],
                   problemStruct->contrastScalefactors[i],
                   problemStruct->contrastBulkIns[i],
                   problemStruct->contrastBulkOuts[i],
                   problemStruct->contrastResolutions[i],
                   problemStruct->backgroundParams, problemStruct->qzshifts,
                   problemStruct->scalefactors, problemStruct->bulkIn,
                   problemStruct->bulkOut, problemStruct->resolutionParams,
                   &thisBackground, &thisQzshift, &thisScalefactor, &thisBulkIn,
                   &thisBulkOut, &thisResol);

          //  Get the custom layers output for this contrast
          //  For the other parameters, we extract the correct ones from the input
          //  arrays
          //  Now call the core layers reflectivity calculation
          //  In this case we are single cored, so we do not parallelise over
          //  points
          //  Call the reflectivity calculation
          b_dv[0] = problemCells->f3[i].f1[0];
          b_dv[1] = problemCells->f3[i].f1[1];
          b_dv1[0] = problemCells->f4[i].f1[0];
          b_dv1[1] = problemCells->f4[i].f1[1];
          dv2[0] = problemCells->f1[i].f1[0];
          dv2[1] = problemCells->f1[i].f1[1];
          coreLayersCalculation(allLayers[i].f1, allRoughs[i],
                                problemStruct->geometry.data,
                                problemStruct->geometry.size, thisBulkIn,
                                thisBulkOut, problemStruct->resample[i], calcSld,
                                thisScalefactor, thisQzshift,
                                problemStruct->dataPresent[i], problemCells->
                                f2[i].f1, b_dv, b_dv1, dv2, thisBackground,
                                thisResol,
                                problemStruct->contrastBackgroundsType[i],
                                static_cast<real_T>(nParams),
                                controls->resamPars, useImaginary, sldProfile,
                                reflect, simul, shiftedDat, layerSld,
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
          b_loop_ub = simul.size(0);
          simulation[i].f1.set_size(simul.size(0), 2);
          for (b_i = 0; b_i < 2; b_i++) {
            for (i1 = 0; i1 < loop_ub; i1++) {
              reflectivity[i].f1[i1 + reflectivity[i].f1.size(0) * b_i] =
                reflect[i1 + reflect.size(0) * b_i];
            }

            for (i1 = 0; i1 < b_loop_ub; i1++) {
              simulation[i].f1[i1 + simulation[i].f1.size(0) * b_i] = simul[i1 +
                simul.size(0) * b_i];
            }
          }

          loop_ub = shiftedDat.size(1);
          shiftedData[i].f1.set_size(shiftedDat.size(0), shiftedDat.size(1));
          for (b_i = 0; b_i < loop_ub; b_i++) {
            b_loop_ub = shiftedDat.size(0);
            for (i1 = 0; i1 < b_loop_ub; i1++) {
              shiftedData[i].f1[i1 + shiftedData[i].f1.size(0) * b_i] =
                shiftedDat[i1 + shiftedDat.size(0) * b_i];
            }
          }

          loop_ub = layerSld.size(1);
          layerSlds[i].f1.set_size(layerSld.size(0), layerSld.size(1));
          for (b_i = 0; b_i < loop_ub; b_i++) {
            b_loop_ub = layerSld.size(0);
            for (i1 = 0; i1 < b_loop_ub; i1++) {
              layerSlds[i].f1[i1 + layerSlds[i].f1.size(0) * b_i] = layerSld[i1
                + layerSld.size(0) * b_i];
            }
          }

          loop_ub = resamLayers.size(1);
          allLayers[i].f1.set_size(resamLayers.size(0), resamLayers.size(1));
          for (b_i = 0; b_i < loop_ub; b_i++) {
            b_loop_ub = resamLayers.size(0);
            for (i1 = 0; i1 < b_loop_ub; i1++) {
              allLayers[i].f1[i1 + allLayers[i].f1.size(0) * b_i] =
                resamLayers[i1 + resamLayers.size(0) * b_i];
            }
          }

          chis[i] = thisChiSquared;
          backgroundParams[i] = thisBackground;
          qzshifts[i] = thisQzshift;
          scalefactors[i] = thisScalefactor;
          bulkIns[i] = thisBulkIn;
          bulkOuts[i] = thisBulkOut;
          resolutionParams[i] = thisResol;
        }
      }
    }
  }
}

// End of code generation (parallelContrasts1.cpp)