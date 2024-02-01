//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// single2.cpp
//
// Code generation for function 'single2'
//

// Include files
#include "single2.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "applyBackgroundCorrection.h"
#include "backSort.h"
#include "callReflectivity.h"
#include "chiSquared.h"
#include "processCustomFunction1.h"
#include "resampleLayers.h"
#include "resampleLayersReIm.h"
#include "rt_nonfinite.h"
#include "shiftData.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace customXY
    {
      void b_single(const c_struct_T *problemStruct, const cell_11 *problemCells,
                    const struct2_T *controls, ::coder::array<real_T, 1U>
                    &outSsubs, ::coder::array<real_T, 1U> &backgroundParams, ::
                    coder::array<real_T, 1U> &qzshifts, ::coder::array<real_T,
                    1U> &scalefactors, ::coder::array<real_T, 1U> &bulkIns, ::
                    coder::array<real_T, 1U> &bulkOuts, ::coder::array<real_T,
                    1U> &resolutionParams, ::coder::array<real_T, 1U> &chis, ::
                    coder::array<cell_wrap_20, 1U> &reflectivity, ::coder::array<
                    cell_wrap_20, 1U> &simulation, ::coder::array<cell_wrap_8,
                    1U> &shiftedData, ::coder::array<cell_wrap_8, 1U> &layerSlds,
                    ::coder::array<cell_wrap_8, 1U> &sldProfiles, ::coder::array<
                    cell_wrap_8, 1U> &allLayers, ::coder::array<real_T, 1U>
                    &allRoughs)
      {
        ::coder::array<real_T, 2U> b_problemCells;
        ::coder::array<real_T, 2U> b_sldProfiles;
        ::coder::array<real_T, 2U> c_sldProfiles;
        ::coder::array<real_T, 2U> layerSld;
        ::coder::array<real_T, 2U> reflect;
        ::coder::array<real_T, 2U> shiftedDat;
        int32_T loop_ub_tmp;
        boolean_T useImaginary;

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
        //         % Layers details N/A
        //  Extract individual parameters from problemStruct
        // Extract individual parameters from problem
        // Pre-Allocation...
        loop_ub_tmp = static_cast<int32_T>(problemStruct->numberOfContrasts);
        backgroundParams.set_size(loop_ub_tmp);
        outSsubs.set_size(loop_ub_tmp);
        for (int32_T i{0}; i < loop_ub_tmp; i++) {
          outSsubs[i] = 0.0;
        }

        //  Resampling parameters
        useImaginary = problemStruct->useImaginary;
        processCustomFunction(problemStruct->contrastBulkIns,
                              problemStruct->contrastBulkOuts,
                              problemStruct->bulkIn, problemStruct->bulkOut,
                              problemStruct->contrastCustomFiles,
                              problemStruct->numberOfContrasts,
                              problemCells->f14, problemStruct->params,
                              sldProfiles, allRoughs);
        qzshifts.set_size(loop_ub_tmp);
        scalefactors.set_size(loop_ub_tmp);
        bulkIns.set_size(loop_ub_tmp);
        bulkOuts.set_size(loop_ub_tmp);
        resolutionParams.set_size(loop_ub_tmp);
        layerSlds.set_size(loop_ub_tmp);
        allLayers.set_size(loop_ub_tmp);
        shiftedData.set_size(loop_ub_tmp);
        chis.set_size(loop_ub_tmp);
        simulation.set_size(loop_ub_tmp);
        reflectivity.set_size(loop_ub_tmp);
        for (int32_T b_i{0}; b_i < loop_ub_tmp; b_i++) {
          real_T b_dv[2];
          real_T b_dv1[2];
          int32_T b_loop_ub;
          int32_T loop_ub;
          backSort(problemStruct->contrastBackgrounds[b_i],
                   problemStruct->contrastQzshifts[b_i],
                   problemStruct->contrastScalefactors[b_i],
                   problemStruct->contrastBulkIns[b_i],
                   problemStruct->contrastBulkOuts[b_i],
                   problemStruct->contrastResolutions[b_i],
                   problemStruct->backgroundParams, problemStruct->qzshifts,
                   problemStruct->scalefactors, problemStruct->bulkIn,
                   problemStruct->bulkOut, problemStruct->resolutionParams,
                   &backgroundParams[b_i], &qzshifts[b_i], &scalefactors[b_i],
                   &bulkIns[b_i], &bulkOuts[b_i], &resolutionParams[b_i]);

          //  Resample the layers
          if (!useImaginary) {
            resampleLayers(sldProfiles[b_i].f1, controls->resamPars, layerSld);
          } else {
            loop_ub = sldProfiles[b_i].f1.size(0);
            b_sldProfiles.set_size(sldProfiles[b_i].f1.size(0), 2);
            for (int32_T i{0}; i < 2; i++) {
              for (int32_T i1{0}; i1 < loop_ub; i1++) {
                b_sldProfiles[i1 + b_sldProfiles.size(0) * i] = sldProfiles[b_i]
                  .f1[i1 + sldProfiles[b_i].f1.size(0) * i];
              }
            }

            loop_ub = sldProfiles[b_i].f1.size(0);
            c_sldProfiles.set_size(sldProfiles[b_i].f1.size(0), 2);
            for (int32_T i{0}; i < loop_ub; i++) {
              c_sldProfiles[i] = sldProfiles[b_i].f1[i];
              c_sldProfiles[i + c_sldProfiles.size(0)] = sldProfiles[b_i].f1[i +
                sldProfiles[b_i].f1.size(0) * 2];
            }

            b_resampleLayersReIm(b_sldProfiles, c_sldProfiles,
                                 controls->resamPars, layerSld);
          }

          layerSlds[b_i].f1.set_size(layerSld.size(0), layerSld.size(1));
          loop_ub = layerSld.size(1);
          allLayers[b_i].f1.set_size(layerSld.size(0), layerSld.size(1));
          b_loop_ub = layerSld.size(0);
          for (int32_T i{0}; i < loop_ub; i++) {
            for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
              layerSlds[b_i].f1[i1 + layerSlds[b_i].f1.size(0) * i] =
                layerSld[i1 + layerSld.size(0) * i];
              allLayers[b_i].f1[i1 + allLayers[b_i].f1.size(0) * i] =
                layerSld[i1 + layerSld.size(0) * i];
            }
          }

          b_problemCells.set_size(problemCells->f2[problemCells->f2.size(0) *
            b_i].f1.size(0), problemCells->f2[problemCells->f2.size(0) * b_i].
            f1.size(1));
          loop_ub = problemCells->f2[b_i].f1.size(1) - 1;
          for (int32_T i{0}; i <= loop_ub; i++) {
            b_loop_ub = problemCells->f2[b_i].f1.size(0) - 1;
            for (int32_T i1{0}; i1 <= b_loop_ub; i1++) {
              b_problemCells[i1 + b_problemCells.size(0) * i] = problemCells->
                f2[b_i].f1[i1 + problemCells->f2[b_i].f1.size(0) * i];
            }
          }

          b_dv[0] = problemCells->f3[b_i].f1[0];
          b_dv[1] = problemCells->f3[b_i].f1[1];
          b_dv1[0] = problemCells->f4[b_i].f1[0];
          b_dv1[1] = problemCells->f4[b_i].f1[1];
          shiftData(scalefactors[b_i], qzshifts[b_i], problemStruct->
                    dataPresent[b_i], b_problemCells, b_dv, b_dv1, shiftedDat);
          shiftedData[b_i].f1.set_size(shiftedDat.size(0), shiftedDat.size(1));
          loop_ub = shiftedDat.size(1);
          for (int32_T i{0}; i < loop_ub; i++) {
            b_loop_ub = shiftedDat.size(0);
            for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
              shiftedData[b_i].f1[i1 + shiftedData[b_i].f1.size(0) * i] =
                shiftedDat[i1 + shiftedDat.size(0) * i];
            }
          }

          b_dv[0] = problemCells->f4[b_i].f1[0];
          b_dv[1] = problemCells->f4[b_i].f1[1];
          b_dv1[0] = problemCells->f1[b_i].f1[0];
          b_dv1[1] = problemCells->f1[b_i].f1[1];
          callReflectivity(bulkIns[b_i], bulkOuts[b_i], b_dv, b_dv1, shiftedDat,
                           layerSld, 0.0, resolutionParams[b_i], useImaginary,
                           reflect, simulation[b_i].f1);
          applyBackgroundCorrection(reflect, simulation[b_i].f1, shiftedDat,
            backgroundParams[b_i], problemStruct->contrastBackgroundsType[b_i]);
          loop_ub = reflect.size(0);
          reflectivity[b_i].f1.set_size(reflect.size(0), 2);
          for (int32_T i{0}; i < 2; i++) {
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              reflectivity[b_i].f1[i1 + reflectivity[b_i].f1.size(0) * i] =
                reflect[i1 + reflect.size(0) * i];
            }
          }

          if (problemStruct->dataPresent[b_i] != 0.0) {
            chis[b_i] = chiSquared(shiftedDat, reflect, static_cast<real_T>
              (problemStruct->params.size(1)));
          } else {
            chis[b_i] = 0.0;
          }
        }
      }
    }
  }
}

// End of code generation (single2.cpp)
