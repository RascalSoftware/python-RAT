//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// single5.cpp
//
// Code generation for function 'single5'
//

// Include files
#include "single5.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "applyBackgroundCorrection.h"
#include "averageReflectivity.h"
#include "backSort.h"
#include "callReflectivity.h"
#include "chiSquared.h"
#include "processCustomFunction3.h"
#include "resampleLayers.h"
#include "resampleLayersReIm.h"
#include "rt_nonfinite.h"
#include "shiftData.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace domainsTF
  {
    namespace customXY
    {
      void b_single(const struct5_T *problemDef, const cell_14 *problemDefCells,
                    const struct2_T *controls, ::coder::array<real_T, 1U>
                    &outSsubs, ::coder::array<real_T, 1U> &backgroundParams, ::
                    coder::array<real_T, 1U> &qzshifts, ::coder::array<real_T,
                    1U> &scalefactors, ::coder::array<real_T, 1U> &bulkIns, ::
                    coder::array<real_T, 1U> &bulkOuts, ::coder::array<real_T,
                    1U> &resolutionParams, ::coder::array<real_T, 1U> &chis, ::
                    coder::array<cell_wrap_11, 1U> &reflectivity, ::coder::array<
                    cell_wrap_11, 1U> &simulation, ::coder::array<cell_wrap_8,
                    1U> &shiftedData, ::coder::array<cell_wrap_8, 2U> &layerSlds,
                    ::coder::array<cell_wrap_8, 2U> &domainSldProfiles, ::coder::
                    array<cell_wrap_8, 2U> &allLayers, ::coder::array<real_T, 1U>
                    &allRoughs)
      {
        ::coder::array<cell_wrap_37, 1U> tempAllLayers;
        ::coder::array<cell_wrap_37, 1U> tempLayerSlds;
        ::coder::array<cell_wrap_37, 1U> tempSldProfiles;
        ::coder::array<real_T, 2U> b_domainSldProfiles;
        ::coder::array<real_T, 2U> b_problemDefCells;
        ::coder::array<real_T, 2U> c_domainSldProfiles;
        ::coder::array<real_T, 2U> r4;
        ::coder::array<real_T, 2U> reflect1;
        ::coder::array<real_T, 2U> reflect2;
        ::coder::array<real_T, 2U> shiftedDat;
        ::coder::array<real_T, 2U> simul1;
        ::coder::array<real_T, 2U> simul2;
        ::coder::array<real_T, 2U> totReflect;
        cell_wrap_8 r;
        cell_wrap_8 r1;
        cell_wrap_8 r2;
        cell_wrap_8 r3;
        int32_T b_loop_ub;
        int32_T i;
        int32_T loop_ub;
        boolean_T useImaginary;

        //  Extract individual cell arrays
        //  Splits up the master input list of all arrays into separate arrays
        //
        //  INPUTS:
        //      * problemDefCells: cell array where all the project data is grouped together.
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
        //  Extract individual parameters from problemDef struct
        // Extract individual parameters from problemDef
        // Pre-Allocation...
        i = static_cast<int32_T>(problemDef->numberOfContrasts);
        backgroundParams.set_size(i);

        //  Resampling parameters
        useImaginary = problemDef->useImaginary;

        //  Default for compile.
        processCustomFunction(problemDef->contrastBackgrounds,
                              problemDef->contrastQzshifts,
                              problemDef->contrastScalefactors,
                              problemDef->contrastBulkIns,
                              problemDef->contrastBulkOuts,
                              problemDef->contrastResolutions,
                              problemDef->backgroundParams, problemDef->qzshifts,
                              problemDef->scalefactors, problemDef->bulkIn,
                              problemDef->bulkOut, problemDef->resolutionParams,
                              problemDef->contrastCustomFiles,
                              problemDef->numberOfContrasts,
                              problemDefCells->f14, problemDef->params,
                              domainSldProfiles, allRoughs);
        outSsubs.set_size(i);
        qzshifts.set_size(i);
        scalefactors.set_size(i);
        bulkIns.set_size(i);
        bulkOuts.set_size(i);
        resolutionParams.set_size(i);
        tempLayerSlds.set_size(i);
        tempAllLayers.set_size(i);
        tempSldProfiles.set_size(i);
        shiftedData.set_size(i);
        reflectivity.set_size(i);
        simulation.set_size(i);
        chis.set_size(i);
        layerSlds.set_size(i, 2);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          real_T b_dv[2];
          real_T b_dv1[2];
          outSsubs[b_i] = allRoughs[b_i];
          backSort(problemDef->contrastBackgrounds[b_i],
                   problemDef->contrastQzshifts[b_i],
                   problemDef->contrastScalefactors[b_i],
                   problemDef->contrastBulkIns[b_i],
                   problemDef->contrastBulkOuts[b_i],
                   problemDef->contrastResolutions[b_i],
                   problemDef->backgroundParams, problemDef->qzshifts,
                   problemDef->scalefactors, problemDef->bulkIn,
                   problemDef->bulkOut, problemDef->resolutionParams,
                   &backgroundParams[b_i], &qzshifts[b_i], &scalefactors[b_i],
                   &bulkIns[b_i], &bulkOuts[b_i], &resolutionParams[b_i]);

          //  Get the domain ratio for this contrast
          //  Resample the sld profiles
          if (!useImaginary) {
            resampleLayers(domainSldProfiles[0].f1, controls->resamPars, r.f1);
            r1.f1.set_size(r.f1.size(0), 3);
            for (int32_T i1{0}; i1 < 3; i1++) {
              loop_ub = r.f1.size(0);
              for (int32_T i2{0}; i2 < loop_ub; i2++) {
                r1.f1[i2 + r1.f1.size(0) * i1] = r.f1[i2 + r.f1.size(0) * i1];
              }
            }

            resampleLayers(domainSldProfiles[1].f1, controls->resamPars, r.f1);
          } else {
            loop_ub = domainSldProfiles[0].f1.size(0);
            b_domainSldProfiles.set_size(domainSldProfiles[0].f1.size(0), 2);
            for (int32_T i1{0}; i1 < 2; i1++) {
              for (int32_T i2{0}; i2 < loop_ub; i2++) {
                b_domainSldProfiles[i2 + b_domainSldProfiles.size(0) * i1] =
                  domainSldProfiles[0].f1[i2 + domainSldProfiles[0].f1.size(0) *
                  i1];
              }
            }

            loop_ub = domainSldProfiles[0].f1.size(0);
            b_loop_ub = domainSldProfiles[0].f1.size(0);
            c_domainSldProfiles.set_size(domainSldProfiles[0].f1.size(0), 2);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              c_domainSldProfiles[i1] = domainSldProfiles[0].f1[i1];
            }

            for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
              c_domainSldProfiles[i1 + c_domainSldProfiles.size(0)] =
                domainSldProfiles[0].f1[i1 + domainSldProfiles[0].f1.size(0) * 2];
            }

            c_resampleLayersReIm(b_domainSldProfiles, c_domainSldProfiles,
                                 controls->resamPars, r.f1);
            r1.f1.set_size(r.f1.size(0), 4);
            for (int32_T i1{0}; i1 < 4; i1++) {
              loop_ub = r.f1.size(0);
              for (int32_T i2{0}; i2 < loop_ub; i2++) {
                r1.f1[i2 + r1.f1.size(0) * i1] = r.f1[i2 + r.f1.size(0) * i1];
              }
            }

            loop_ub = domainSldProfiles[1].f1.size(0);
            b_domainSldProfiles.set_size(domainSldProfiles[1].f1.size(0), 2);
            for (int32_T i1{0}; i1 < 2; i1++) {
              for (int32_T i2{0}; i2 < loop_ub; i2++) {
                b_domainSldProfiles[i2 + b_domainSldProfiles.size(0) * i1] =
                  domainSldProfiles[1].f1[i2 + domainSldProfiles[1].f1.size(0) *
                  i1];
              }
            }

            loop_ub = domainSldProfiles[1].f1.size(0);
            b_loop_ub = domainSldProfiles[1].f1.size(0);
            c_domainSldProfiles.set_size(domainSldProfiles[1].f1.size(0), 2);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              c_domainSldProfiles[i1] = domainSldProfiles[1].f1[i1];
            }

            for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
              c_domainSldProfiles[i1 + c_domainSldProfiles.size(0)] =
                domainSldProfiles[1].f1[i1 + domainSldProfiles[1].f1.size(0) * 2];
            }

            c_resampleLayersReIm(b_domainSldProfiles, c_domainSldProfiles,
                                 controls->resamPars, r.f1);
          }

          tempLayerSlds[b_i].f1[0] = r1;
          tempLayerSlds[b_i].f1[1] = r;
          tempAllLayers[b_i].f1[0] = r1;
          tempAllLayers[b_i].f1[1] = r;
          r2.f1.set_size(domainSldProfiles[0].f1.size(0), domainSldProfiles[0].
                         f1.size(1));
          loop_ub = domainSldProfiles[0].f1.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = domainSldProfiles[0].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              r2.f1[i2 + r2.f1.size(0) * i1] = domainSldProfiles[0].f1[i2 +
                domainSldProfiles[0].f1.size(0) * i1];
            }
          }

          r3.f1.set_size(domainSldProfiles[1].f1.size(0), domainSldProfiles[1].
                         f1.size(1));
          loop_ub = domainSldProfiles[1].f1.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = domainSldProfiles[1].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              r3.f1[i2 + r3.f1.size(0) * i1] = domainSldProfiles[1].f1[i2 +
                domainSldProfiles[1].f1.size(0) * i1];
            }
          }

          tempSldProfiles[b_i].f1[0] = r2;
          tempSldProfiles[b_i].f1[1] = r3;
          b_problemDefCells.set_size(problemDefCells->f2
            [problemDefCells->f2.size(0) * b_i].f1.size(0), problemDefCells->
            f2[problemDefCells->f2.size(0) * b_i].f1.size(1));
          loop_ub = problemDefCells->f2[b_i].f1.size(1) - 1;
          for (int32_T i1{0}; i1 <= loop_ub; i1++) {
            b_loop_ub = problemDefCells->f2[b_i].f1.size(0) - 1;
            for (int32_T i2{0}; i2 <= b_loop_ub; i2++) {
              b_problemDefCells[i2 + b_problemDefCells.size(0) * i1] =
                problemDefCells->f2[b_i].f1[i2 + problemDefCells->f2[b_i].
                f1.size(0) * i1];
            }
          }

          b_dv[0] = problemDefCells->f3[b_i].f1[0];
          b_dv[1] = problemDefCells->f3[b_i].f1[1];
          b_dv1[0] = problemDefCells->f4[b_i].f1[0];
          b_dv1[1] = problemDefCells->f4[b_i].f1[1];
          shiftData(scalefactors[b_i], qzshifts[b_i], problemDef->
                    dataPresent[b_i], b_problemDefCells, b_dv, b_dv1, shiftedDat);
          shiftedData[b_i].f1.set_size(shiftedDat.size(0), shiftedDat.size(1));
          loop_ub = shiftedDat.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = shiftedDat.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              shiftedData[b_i].f1[i2 + shiftedData[b_i].f1.size(0) * i1] =
                shiftedDat[i2 + shiftedDat.size(0) * i1];
            }
          }

          r4.set_size(r1.f1.size(0), r1.f1.size(1));
          loop_ub = r1.f1.size(1) - 1;
          for (int32_T i1{0}; i1 <= loop_ub; i1++) {
            b_loop_ub = r1.f1.size(0) - 1;
            for (int32_T i2{0}; i2 <= b_loop_ub; i2++) {
              r4[i2 + r4.size(0) * i1] = r1.f1[i2 + r1.f1.size(0) * i1];
            }
          }

          b_dv[0] = problemDefCells->f4[b_i].f1[0];
          b_dv[1] = problemDefCells->f4[b_i].f1[1];
          b_dv1[0] = problemDefCells->f1[b_i].f1[0];
          b_dv1[1] = problemDefCells->f1[b_i].f1[1];
          callReflectivity(bulkIns[b_i], bulkOuts[b_i], b_dv, b_dv1, shiftedDat,
                           r4, allRoughs[b_i], resolutionParams[b_i],
                           useImaginary, reflect1, simul1);
          r4.set_size(r.f1.size(0), r.f1.size(1));
          loop_ub = r.f1.size(1) - 1;
          for (int32_T i1{0}; i1 <= loop_ub; i1++) {
            b_loop_ub = r.f1.size(0) - 1;
            for (int32_T i2{0}; i2 <= b_loop_ub; i2++) {
              r4[i2 + r4.size(0) * i1] = r.f1[i2 + r.f1.size(0) * i1];
            }
          }

          b_dv[0] = problemDefCells->f4[b_i].f1[0];
          b_dv[1] = problemDefCells->f4[b_i].f1[1];
          b_dv1[0] = problemDefCells->f1[b_i].f1[0];
          b_dv1[1] = problemDefCells->f1[b_i].f1[1];
          callReflectivity(bulkIns[b_i], bulkOuts[b_i], b_dv, b_dv1, shiftedDat,
                           r4, allRoughs[b_i], resolutionParams[b_i],
                           useImaginary, reflect2, simul2);
          applyBackgroundCorrection(reflect1, simul1, shiftedDat,
            backgroundParams[b_i], problemDef->contrastBackgroundsType[b_i]);
          applyBackgroundCorrection(reflect2, simul2, shiftedDat,
            backgroundParams[b_i], problemDef->contrastBackgroundsType[b_i]);

          //  Calculate the average reflectivities....
          averageReflectivity(reflect1, reflect2, simul1, simul2,
                              problemDef->domainRatio[static_cast<int32_T>
                              (problemDef->contrastDomainRatios[b_i]) - 1],
                              totReflect, simulation[b_i].f1);
          loop_ub = totReflect.size(0);
          reflectivity[b_i].f1.set_size(totReflect.size(0), 2);
          for (int32_T i1{0}; i1 < 2; i1++) {
            for (int32_T i2{0}; i2 < loop_ub; i2++) {
              reflectivity[b_i].f1[i2 + reflectivity[b_i].f1.size(0) * i1] =
                totReflect[i2 + totReflect.size(0) * i1];
            }
          }

          if (problemDef->dataPresent[b_i] != 0.0) {
            chis[b_i] = chiSquared(shiftedDat, totReflect, static_cast<real_T>
              (problemDef->params.size(1)));
          } else {
            chis[b_i] = 0.0;
          }
        }

        allLayers.set_size(i, 2);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          loop_ub = tempSldProfiles[b_i].f1[0].f1.size(1);
          domainSldProfiles[b_i].f1.set_size(tempSldProfiles[b_i].f1[0].f1.size
            (0), tempSldProfiles[b_i].f1[0].f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempSldProfiles[b_i].f1[0].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              domainSldProfiles[b_i].f1[i2 + domainSldProfiles[b_i].f1.size(0) *
                i1] = tempSldProfiles[b_i].f1[0].f1[i2 + tempSldProfiles[b_i]
                .f1[0].f1.size(0) * i1];
            }
          }

          loop_ub = tempSldProfiles[b_i].f1[1].f1.size(1);
          domainSldProfiles[b_i + domainSldProfiles.size(0)].f1.set_size
            (tempSldProfiles[b_i].f1[1].f1.size(0), tempSldProfiles[b_i].f1[1].
             f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempSldProfiles[b_i].f1[1].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              domainSldProfiles[b_i + domainSldProfiles.size(0)].f1[i2 +
                domainSldProfiles[b_i + domainSldProfiles.size(0)].f1.size(0) *
                i1] = tempSldProfiles[b_i].f1[1].f1[i2 + tempSldProfiles[b_i]
                .f1[1].f1.size(0) * i1];
            }
          }

          loop_ub = tempAllLayers[b_i].f1[0].f1.size(1);
          allLayers[b_i].f1.set_size(tempAllLayers[b_i].f1[0].f1.size(0),
            tempAllLayers[b_i].f1[0].f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempAllLayers[b_i].f1[0].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1] =
                tempAllLayers[b_i].f1[0].f1[i2 + tempAllLayers[b_i].f1[0].
                f1.size(0) * i1];
            }
          }

          loop_ub = tempAllLayers[b_i].f1[1].f1.size(1);
          allLayers[b_i + allLayers.size(0)].f1.set_size(tempAllLayers[b_i].f1[1]
            .f1.size(0), tempAllLayers[b_i].f1[1].f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempAllLayers[b_i].f1[1].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              allLayers[b_i + allLayers.size(0)].f1[i2 + allLayers[b_i +
                allLayers.size(0)].f1.size(0) * i1] = tempAllLayers[b_i].f1[1].
                f1[i2 + tempAllLayers[b_i].f1[1].f1.size(0) * i1];
            }
          }

          loop_ub = tempLayerSlds[b_i].f1[0].f1.size(1);
          layerSlds[b_i].f1.set_size(tempLayerSlds[b_i].f1[0].f1.size(0),
            tempLayerSlds[b_i].f1[0].f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempLayerSlds[b_i].f1[0].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              layerSlds[b_i].f1[i2 + layerSlds[b_i].f1.size(0) * i1] =
                tempLayerSlds[b_i].f1[0].f1[i2 + tempLayerSlds[b_i].f1[0].
                f1.size(0) * i1];
            }
          }

          loop_ub = tempLayerSlds[b_i].f1[1].f1.size(1);
          layerSlds[b_i + layerSlds.size(0)].f1.set_size(tempLayerSlds[b_i].f1[1]
            .f1.size(0), tempLayerSlds[b_i].f1[1].f1.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = tempLayerSlds[b_i].f1[1].f1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              layerSlds[b_i + layerSlds.size(0)].f1[i2 + layerSlds[b_i +
                layerSlds.size(0)].f1.size(0) * i1] = tempLayerSlds[b_i].f1[1].
                f1[i2 + tempLayerSlds[b_i].f1[1].f1.size(0) * i1];
            }
          }
        }
      }
    }
  }
}

// End of code generation (single5.cpp)
