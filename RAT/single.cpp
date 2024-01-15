//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// single.cpp
//
// Code generation for function 'single'
//

// Include files
#include "single.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "allocateLayersForContrast.h"
#include "allocateParamsToLayers.h"
#include "backSort.h"
#include "coreLayersCalculation.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace standardLayers
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
                    1U> &shiftedData, ::coder::array<cell_wrap_8, 1U> &layerSlds,
                    ::coder::array<cell_wrap_8, 1U> &sldProfiles, ::coder::array<
                    cell_wrap_8, 1U> &allLayers, ::coder::array<real_T, 1U>
                    &allRoughs)
      {
        static real_T thisContrastLayers_data[6000];
        ::coder::array<cell_wrap_24, 2U> outParameterisedLayers;
        ::coder::array<real_T, 2U> b_thisContrastLayers_data;
        ::coder::array<real_T, 2U> shiftedDat;
        ::coder::array<real_T, 2U> sldProfile;
        real_T thisBackground;
        real_T thisBulkIn;
        real_T thisBulkOut;
        real_T thisQzshift;
        real_T thisResol;
        real_T thisScalefactor;
        int32_T i;
        boolean_T useImaginary;

        //  Single threaded version of the Standard Layers calculation
        //  This is the main reflectivity calculation of the standard layers
        //  calculation type. It extracts the required parameters for the contrasts
        //  from the input arrays, then passes the main calculation to
        //  'standardLayersCore', which carries out the calculation itself.
        //  The core calculation is common for both standard and custom layers.
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
        useImaginary = problemDef->useImaginary;

        //  Allocate the memory for the output arrays before the main loop
        i = static_cast<int32_T>(problemDef->numberOfContrasts);
        backgroundParams.set_size(i);

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
        simulation.set_size(i);
        shiftedData.set_size(i);
        layerSlds.set_size(i);
        chis.set_size(i);
        qzshifts.set_size(i);
        scalefactors.set_size(i);
        bulkIns.set_size(i);
        bulkOuts.set_size(i);
        resolutionParams.set_size(i);
        allRoughs.set_size(i);
        allLayers.set_size(i);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          real_T b_dv[2];
          real_T b_dv1[2];
          real_T dv2[2];
          int32_T thisContrastLayers_size[2];
          int32_T b_loop_ub;
          int32_T loop_ub;

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

          //  Also need to determine which layers from the overall layers list
          //  are required for this contrast, and put them in the correct order
          //  according to geometry
          allocateLayersForContrast(problemDefCells->f5[b_i].f1,
            outParameterisedLayers, useImaginary, thisContrastLayers_data,
            thisContrastLayers_size);

          //  For the other parameters, we extract the correct ones from the input
          //  arrays
          //  Substrate roughness is always first parameter for standard layers
          //  Now call the core layers reflectivity calculation
          //  In this case we are single cored, so we do not parallelise over
          //  points
          //  Call the core layers calculation
          b_thisContrastLayers_data.set(&thisContrastLayers_data[0],
            thisContrastLayers_size[0], thisContrastLayers_size[1]);
          b_dv[0] = problemDefCells->f3[b_i].f1[0];
          b_dv[1] = problemDefCells->f3[b_i].f1[1];
          b_dv1[0] = problemDefCells->f4[b_i].f1[0];
          b_dv1[1] = problemDefCells->f4[b_i].f1[1];
          dv2[0] = problemDefCells->f1[b_i].f1[0];
          dv2[1] = problemDefCells->f1[b_i].f1[1];
          coreLayersCalculation(b_thisContrastLayers_data, problemDef->params[0],
                                problemDef->geometry.data,
                                problemDef->geometry.size, thisBulkIn,
                                thisBulkOut, problemDef->resample[b_i],
                                controls->calcSldDuringFit, thisScalefactor,
                                thisQzshift, problemDef->dataPresent[b_i],
                                problemDefCells->f2[b_i].f1, b_dv, b_dv1, dv2,
                                thisBackground, thisResol,
                                problemDef->contrastBackgroundsType[b_i],
                                static_cast<real_T>(problemDef->params.size(1)),
                                controls->resamPars, useImaginary, sldProfile,
                                reflectivity[b_i].f1, simulation[b_i].f1,
                                shiftedDat, layerSlds[b_i].f1, allLayers[b_i].f1,
                                &chis[b_i], &outSsubs[b_i]);

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

          loop_ub = shiftedDat.size(1);
          shiftedData[b_i].f1.set_size(shiftedDat.size(0), shiftedDat.size(1));
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = shiftedDat.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              shiftedData[b_i].f1[i2 + shiftedData[b_i].f1.size(0) * i1] =
                shiftedDat[i2 + shiftedDat.size(0) * i1];
            }
          }

          backgroundParams[b_i] = thisBackground;
          qzshifts[b_i] = thisQzshift;
          scalefactors[b_i] = thisScalefactor;
          bulkIns[b_i] = thisBulkIn;
          bulkOuts[b_i] = thisBulkOut;
          resolutionParams[b_i] = thisResol;
          allRoughs[b_i] = problemDef->params[0];
        }
      }
    }
  }
}

// End of code generation (single.cpp)
