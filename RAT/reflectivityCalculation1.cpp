//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation1.cpp
//
// Code generation for function 'reflectivityCalculation1'
//

// Include files
#include "reflectivityCalculation1.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "calculate.h"
#include "calculate1.h"
#include "calculate2.h"
#include "lower.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  namespace nonPolarisedTF
  {
    void b_reflectivityCalculation(const struct5_T *problemDef, const cell_14
      *problemDefCells, const struct2_T *controls, struct_T *problem, ::coder::
      array<cell_wrap_11, 1U> &reflectivity, ::coder::array<cell_wrap_11, 1U>
      &simulation, ::coder::array<cell_wrap_8, 1U> &shiftedData, ::coder::array<
      cell_wrap_8, 1U> &layerSlds, ::coder::array<cell_wrap_8, 1U> &sldProfiles,
      ::coder::array<cell_wrap_8, 1U> &allLayers)
    {
      int32_T switch_expression_size[2];
      int32_T loop_ub_tmp;
      char_T switch_expression_data[10000];

      //  Main function for the nonPolarisedTF reflectivity calculation
      //  This function decides what type of model is being analysed and barnches
      //  to the correct one. The main options are:
      //  layers            - This is the equivalent of Standard Layers in RasCAL
      //  Custom Layers     - This is also a layers calculation, but the
      //                    specification of the layers is dne using a user defined
      //                    function.
      //  Custom XY         - This also has a model described by a user supplied
      //                    function, but in this case the function generates an
      //                    SLD profile (i.e. XY function) rather than a list of
      //                    layers.
      //  Find out the model type from the input structs
      //  Pre-allocate the output arrays.. this is necessary because otherwise
      //  the compiler complains with 'Output argument <....> is not assigned on
      //  some execution paths' error.
      loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
      problem->ssubs.set_size(loop_ub_tmp);
      problem->backgroundParams.set_size(loop_ub_tmp);
      problem->qzshifts.set_size(loop_ub_tmp);
      problem->scalefactors.set_size(loop_ub_tmp);
      problem->bulkIn.set_size(loop_ub_tmp);
      problem->bulkOut.set_size(loop_ub_tmp);
      problem->resolutionParams.set_size(loop_ub_tmp);
      problem->calculations.allChis.set_size(loop_ub_tmp);
      problem->calculations.sumChi = 0.0;
      problem->allSubRough.set_size(loop_ub_tmp);
      problem->resample.set_size(1, loop_ub_tmp);
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
        problem->resolutionParams[i] = 0.0;
        problem->calculations.allChis[i] = 0.0;
        problem->allSubRough[i] = 0.0;
        problem->resample[i] = 0.0;
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

      coder::lower(problemDef->modelType.data, problemDef->modelType.size,
                   switch_expression_data, switch_expression_size);
      if (coder::internal::k_strcmp(switch_expression_data,
           switch_expression_size)) {
        loop_ub_tmp = 0;
      } else if (coder::internal::l_strcmp(switch_expression_data,
                  switch_expression_size)) {
        loop_ub_tmp = 1;
      } else if (coder::internal::m_strcmp(switch_expression_data,
                  switch_expression_size)) {
        loop_ub_tmp = 2;
      } else {
        loop_ub_tmp = -1;
      }

      switch (loop_ub_tmp) {
       case 0:
        //  Standard layers calculation
        standardLayers::calculate(problemDef, problemDefCells, controls, problem,
          reflectivity, simulation, shiftedData, layerSlds, sldProfiles,
          allLayers);
        break;

       case 1:
        //  Custom layers with user supplied custom model file
        customLayers::calculate(problemDef, problemDefCells, controls, problem,
          reflectivity, simulation, shiftedData, layerSlds, sldProfiles,
          allLayers);
        break;

       case 2:
        //  Custom SLD profile with user defined model file
        customXY::calculate(problemDef, problemDefCells, controls, problem,
                            reflectivity, simulation, shiftedData, layerSlds,
                            sldProfiles, allLayers);
        break;
      }
    }
  }
}

// End of code generation (reflectivityCalculation1.cpp)
