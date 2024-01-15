//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// single4.cpp
//
// Code generation for function 'single4'
//

// Include files
#include "single4.h"
#include "RATMain_internal_types.h"
#include "RATMain_rtwutil.h"
#include "RATMain_types.h"
#include "averageReflectivity.h"
#include "backSort.h"
#include "chiSquared.h"
#include "coreLayersCalculation.h"
#include "processCustomFunction2.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  namespace domainsTF
  {
    namespace customLayers
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
        ::coder::array<cell_wrap_21, 2U> r;
        ::coder::array<cell_wrap_37, 1U> tempAllLayers;
        ::coder::array<cell_wrap_37, 1U> tempLayerSlds;
        ::coder::array<cell_wrap_37, 1U> tempSldProfiles;
        ::coder::array<cell_wrap_8, 2U> calcAllLayers;
        ::coder::array<real_T, 2U> a__5;
        ::coder::array<real_T, 2U> reflect1;
        ::coder::array<real_T, 2U> reflect2;
        ::coder::array<real_T, 2U> shiftedDat;
        ::coder::array<real_T, 2U> simul1;
        ::coder::array<real_T, 2U> simul2;
        ::coder::array<real_T, 2U> totReflect;
        cell_wrap_8 r1;
        cell_wrap_8 r2;
        cell_wrap_8 r3;
        cell_wrap_8 r4;
        cell_wrap_8 r5;
        cell_wrap_8 r6;
        real_T a__4;
        real_T a__6;
        real_T a__7;
        real_T thisBackground;
        real_T thisBulkIn;
        real_T thisBulkOut;
        real_T thisQzshift;
        real_T thisResol;
        real_T thisScalefactor;
        int32_T b_loop_ub;
        int32_T i;
        int32_T loop_ub;
        int32_T nParams;
        boolean_T calcSld;
        boolean_T useImaginary;

        //  Single threaded version of the custom layers, domainsTF reflectivity
        //  calculation. The function extracts the relevant parameters from the input
        //  arrays, allocates these on a pre-contrast basis, then calls the 'core'
        //  calculation (the core layers domainsTF calc is shared between multiple
        //  calculation types).
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
        //  Extract individual parameters from problemDef struct
        // Extract individual parameters from problemDef
        nParams = problemDef->params.size(1);
        calcSld = controls->calcSldDuringFit;
        useImaginary = problemDef->useImaginary;

        //  Default for compile.
        //  Pre-Allocation of output arrays...
        i = static_cast<int32_T>(problemDef->numberOfContrasts);
        backgroundParams.set_size(i);

        //  Resampling parameters
        //  Process the custom models....
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
                              problemDef->useImaginary, r, allRoughs);
        cast(r, calcAllLayers);

        //  Parallel over all contrasts
        // layersCounter = 1;
        outSsubs.set_size(i);
        tempSldProfiles.set_size(i);
        reflectivity.set_size(i);
        simulation.set_size(i);
        shiftedData.set_size(i);
        tempLayerSlds.set_size(i);
        tempAllLayers.set_size(i);
        chis.set_size(i);
        qzshifts.set_size(i);
        scalefactors.set_size(i);
        bulkIns.set_size(i);
        bulkOuts.set_size(i);
        resolutionParams.set_size(i);
        layerSlds.set_size(i, 2);
        domainSldProfiles.set_size(i, 2);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          real_T b_dv[2];
          real_T b_dv1[2];
          real_T dv2[2];

          //  Get the domain ratio for this contrast
          //  Extract the relevant parameter values for this contrast
          //  from the input arrays.
          //  First need to decide which values of the backgrounds, scalefactors
          //  data shifts and bulk contrasts are associated with this contrast
          backSort(problemDef->contrastBackgrounds[b_i],
                   problemDef->contrastQzshifts[b_i],
                   problemDef->contrastScalefactors[b_i],
                   problemDef->contrastBulkIns[b_i],
                   problemDef->contrastBulkOuts[b_i],
                   problemDef->contrastResolutions[b_i],
                   problemDef->backgroundParams, problemDef->qzshifts,
                   problemDef->scalefactors, problemDef->bulkIn,
                   problemDef->bulkOut, problemDef->resolutionParams,
                   &thisBackground, &thisQzshift, &thisScalefactor, &thisBulkIn,
                   &thisBulkOut, &thisResol);

          //  Get the custom layers output for this contrast
          //  We have two for each contrast - one for each domain
          //  For the other parameters, we extract the correct ones from the input
          //  arrays
          //  Now call the core layers reflectivity calculation
          //  In this case we are single cored, so we do not parallelise over
          //  points
          //  Call the reflectivity calculation for each domain
          //  Domain 1
          b_dv[0] = problemDefCells->f3[b_i].f1[0];
          b_dv[1] = problemDefCells->f3[b_i].f1[1];
          b_dv1[0] = problemDefCells->f4[b_i].f1[0];
          b_dv1[1] = problemDefCells->f4[b_i].f1[1];
          dv2[0] = problemDefCells->f1[b_i].f1[0];
          dv2[1] = problemDefCells->f1[b_i].f1[1];
          nonPolarisedTF::coreLayersCalculation(calcAllLayers[b_i].f1,
            allRoughs[b_i], problemDef->geometry.data, problemDef->geometry.size,
            thisBulkIn, thisBulkOut, problemDef->resample[b_i], calcSld,
            thisScalefactor, thisQzshift, problemDef->dataPresent[b_i],
            problemDefCells->f2[b_i].f1, b_dv, b_dv1, dv2, thisBackground,
            thisResol, problemDef->contrastBackgroundsType[b_i],
            static_cast<real_T>(nParams), controls->resamPars, useImaginary,
            r1.f1, reflect1, simul1, shiftedDat, r2.f1, r3.f1, &a__4,
            &outSsubs[b_i]);

          //  Domain 2
          b_dv[0] = problemDefCells->f3[b_i].f1[0];
          b_dv[1] = problemDefCells->f3[b_i].f1[1];
          b_dv1[0] = problemDefCells->f4[b_i].f1[0];
          b_dv1[1] = problemDefCells->f4[b_i].f1[1];
          dv2[0] = problemDefCells->f1[b_i].f1[0];
          dv2[1] = problemDefCells->f1[b_i].f1[1];
          nonPolarisedTF::coreLayersCalculation(calcAllLayers[b_i +
            calcAllLayers.size(0)].f1, allRoughs[b_i], problemDef->geometry.data,
            problemDef->geometry.size, thisBulkIn, thisBulkOut,
            problemDef->resample[b_i], calcSld, thisScalefactor, thisQzshift,
            problemDef->dataPresent[b_i], problemDefCells->f2[b_i].f1, b_dv,
            b_dv1, dv2, thisBackground, thisResol,
            problemDef->contrastBackgroundsType[b_i], static_cast<real_T>
            (nParams), controls->resamPars, useImaginary, r4.f1, reflect2,
            simul2, a__5, r5.f1, r6.f1, &a__6, &a__7);

          //  Calculate the average reflectivities....
          averageReflectivity(reflect1, reflect2, simul1, simul2,
                              problemDef->domainRatio[static_cast<int32_T>
                              (problemDef->contrastDomainRatios[b_i]) - 1],
                              totReflect, simulation[b_i].f1);

          //  Get an overall chi-squared for the new averaged curve..
          chis[b_i] = chiSquared(shiftedDat, totReflect, static_cast<real_T>
            (problemDef->params.size(1)));

          //  Store returned values for this contrast in the output arrays.
          //  As well as the calculated profiles, we also store a record of
          //  the other values (background, scalefactors etc) for each contrast
          //  for future use.
          tempSldProfiles[b_i].f1[0] = r1;
          tempSldProfiles[b_i].f1[1] = r4;
          reflectivity[b_i].f1.set_size(totReflect.size(0), 2);
          loop_ub = totReflect.size(0);
          for (int32_T i1{0}; i1 < 2; i1++) {
            for (int32_T i2{0}; i2 < loop_ub; i2++) {
              reflectivity[b_i].f1[i2 + reflectivity[b_i].f1.size(0) * i1] =
                totReflect[i2 + totReflect.size(0) * i1];
            }
          }

          shiftedData[b_i].f1.set_size(shiftedDat.size(0), shiftedDat.size(1));
          loop_ub = shiftedDat.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = shiftedDat.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              shiftedData[b_i].f1[i2 + shiftedData[b_i].f1.size(0) * i1] =
                shiftedDat[i2 + shiftedDat.size(0) * i1];
            }
          }

          tempLayerSlds[b_i].f1[0] = r2;
          tempLayerSlds[b_i].f1[1] = r5;
          tempAllLayers[b_i].f1[0] = r3;
          tempAllLayers[b_i].f1[1] = r6;
          backgroundParams[b_i] = thisBackground;
          qzshifts[b_i] = thisQzshift;
          scalefactors[b_i] = thisScalefactor;
          bulkIns[b_i] = thisBulkIn;
          bulkOuts[b_i] = thisBulkOut;
          resolutionParams[b_i] = thisResol;
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

// End of code generation (single4.cpp)
