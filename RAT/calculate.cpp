//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// calculate.cpp
//
// Code generation for function 'calculate'
//

// Include files
#include "calculate.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "parallelContrasts.h"
#include "parallelPoints.h"
#include "rt_nonfinite.h"
#include "single.h"
#include "strcmp.h"
#include "sum.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  namespace nonPolarisedTF
  {
    namespace standardLayers
    {
      void calculate(const struct5_T *problemDef, const cell_14 *problemDefCells,
                     const struct2_T *controls, struct_T *problem, ::coder::
                     array<cell_wrap_11, 1U> &reflectivity, ::coder::array<
                     cell_wrap_11, 1U> &simulation, ::coder::array<cell_wrap_8,
                     1U> &shiftedData, ::coder::array<cell_wrap_8, 1U>
                     &layerSlds, ::coder::array<cell_wrap_8, 1U> &sldProfiles, ::
                     coder::array<cell_wrap_8, 1U> &allLayers)
      {
        int32_T loop_ub_tmp;

        //  Standard layers reflectivity calculation for nonPolarisedTF
        //  This function decides on parallelisation options before calling the
        //  relevant version ofthe main standard layers calculation. Parallelisation
        //  is either over the outer loop ('contrasts'), or the inner loop
        //  ('points'). The easiest way to do this is to have multiple versions of
        //  the same core calculation, rather than trying to make the parallel
        //  for loops conditional (although that would be much neater) There are:
        //  points    - parallelise over points in the reflectivity calculation
        //  contrasts - parallelise over contrasts (outer for loop)
        //  Pre-allocation - It's necessary to
        //  pre-define the types for all the arrays
        //  for compilation, so do this in this block.
        loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
        problem->ssubs.set_size(loop_ub_tmp);
        problem->backgroundParams.set_size(loop_ub_tmp);
        problem->qzshifts.set_size(loop_ub_tmp);
        problem->scalefactors.set_size(loop_ub_tmp);
        problem->bulkIn.set_size(loop_ub_tmp);
        problem->bulkOut.set_size(loop_ub_tmp);
        problem->calculations.allChis.set_size(loop_ub_tmp);
        problem->resolutionParams.set_size(loop_ub_tmp);
        problem->allSubRough.set_size(loop_ub_tmp);
        reflectivity.set_size(loop_ub_tmp);
        simulation.set_size(loop_ub_tmp);
        shiftedData.set_size(loop_ub_tmp);
        layerSlds.set_size(loop_ub_tmp);
        sldProfiles.set_size(loop_ub_tmp);
        allLayers.set_size(loop_ub_tmp);
        for (int32_T i{0}; i < loop_ub_tmp; i++) {
          problem->ssubs[i] = 0.0;
          problem->backgroundParams[i] = 0.0;
          problem->qzshifts[i] = 0.0;
          problem->scalefactors[i] = 0.0;
          problem->bulkIn[i] = 0.0;
          problem->bulkOut[i] = 0.0;
          problem->calculations.allChis[i] = 0.0;
          problem->resolutionParams[i] = 0.0;
          problem->allSubRough[i] = 0.0;
          reflectivity[i].f1.set_size(2, 2);
          reflectivity[i].f1[0] = 1.0;
          reflectivity[i].f1[1] = 1.0;
          reflectivity[i].f1[reflectivity[i].f1.size(0)] = 1.0;
          reflectivity[i].f1[reflectivity[i].f1.size(0) + 1] = 1.0;
          simulation[i].f1.set_size(2, 2);
          simulation[i].f1[0] = 1.0;
          simulation[i].f1[1] = 1.0;
          simulation[i].f1[simulation[i].f1.size(0)] = 1.0;
          simulation[i].f1[simulation[i].f1.size(0) + 1] = 1.0;
          shiftedData[i].f1.set_size(2, 3);
          layerSlds[i].f1.set_size(2, 3);
          sldProfiles[i].f1.set_size(2, 2);
          sldProfiles[i].f1[0] = 1.0;
          sldProfiles[i].f1[1] = 1.0;
          sldProfiles[i].f1[sldProfiles[i].f1.size(0)] = 1.0;
          sldProfiles[i].f1[sldProfiles[i].f1.size(0) + 1] = 1.0;
          allLayers[i].f1.set_size(2, 3);
          for (int32_T b_i{0}; b_i < 3; b_i++) {
            shiftedData[i].f1[shiftedData[i].f1.size(0) * b_i] = 1.0;
            shiftedData[i].f1[shiftedData[i].f1.size(0) * b_i + 1] = 1.0;
            layerSlds[i].f1[layerSlds[i].f1.size(0) * b_i] = 1.0;
            layerSlds[i].f1[layerSlds[i].f1.size(0) * b_i + 1] = 1.0;
            allLayers[i].f1[allLayers[i].f1.size(0) * b_i] = 1.0;
            allLayers[i].f1[allLayers[i].f1.size(0) * b_i + 1] = 1.0;
          }
        }

        //  ------- End type definitions -------------
        if (coder::internal::n_strcmp(controls->parallel.data,
             controls->parallel.size)) {
          loop_ub_tmp = 0;
        } else if (coder::internal::o_strcmp(controls->parallel.data,
                    controls->parallel.size)) {
          loop_ub_tmp = 1;
        } else if (coder::internal::p_strcmp(controls->parallel.data,
                    controls->parallel.size)) {
          loop_ub_tmp = 2;
        } else {
          loop_ub_tmp = -1;
        }

        switch (loop_ub_tmp) {
         case 0:
          b_single(problemDef, problemDefCells, controls, problem->ssubs,
                   problem->backgroundParams, problem->qzshifts,
                   problem->scalefactors, problem->bulkIn, problem->bulkOut,
                   problem->resolutionParams, problem->calculations.allChis,
                   reflectivity, simulation, shiftedData, layerSlds, sldProfiles,
                   allLayers, problem->allSubRough);
          break;

         case 1:
          parallelPoints(problemDef, problemDefCells, controls, problem->ssubs,
                         problem->backgroundParams, problem->qzshifts,
                         problem->scalefactors, problem->bulkIn,
                         problem->bulkOut, problem->resolutionParams,
                         problem->calculations.allChis, reflectivity, simulation,
                         shiftedData, layerSlds, sldProfiles, allLayers,
                         problem->allSubRough);
          break;

         case 2:
          c_parallelContrasts(problemDef, problemDefCells, controls,
                              problem->ssubs, problem->backgroundParams,
                              problem->qzshifts, problem->scalefactors,
                              problem->bulkIn, problem->bulkOut,
                              problem->resolutionParams,
                              problem->calculations.allChis, reflectivity,
                              simulation, shiftedData, layerSlds, sldProfiles,
                              allLayers, problem->allSubRough);
          break;
        }

        //  Package everything into one array for tidy output
        problem->calculations.sumChi = coder::sum(problem->calculations.allChis);
        problem->resample.set_size(1, problemDef->resample.size(1));
        loop_ub_tmp = problemDef->resample.size(1);
        for (int32_T b_i{0}; b_i < loop_ub_tmp; b_i++) {
          problem->resample[b_i] = problemDef->resample[b_i];
        }
      }
    }
  }
}

// End of code generation (calculate.cpp)
