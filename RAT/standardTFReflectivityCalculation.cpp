//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFReflectivityCalculation.cpp
//
// Code generation for function 'standardTFReflectivityCalculation'
//

// Include files
#include "standardTFReflectivityCalculation.h"
#include "lower.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFCustomLayersReflectivityCalculation.h"
#include "standardTFCustomXYReflectivityCalculation.h"
#include "standardTFStandardLayersReflectivityCalculation.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  void standardTFReflectivityCalculation(const struct0_T *problemDef, const
    cell_19 *problemDefCells, const struct2_T *controls, struct4_T *problem, ::
    coder::array<cell_wrap_9, 1U> &reflectivity, ::coder::array<cell_wrap_9, 1U>
    &Simulation, ::coder::array<cell_wrap_15, 1U> &shifted_data, ::coder::array<
    cell_wrap_15, 1U> &layerSlds, ::coder::array<cell_wrap_15, 1U> &sldProfiles,
    ::coder::array<cell_wrap_15, 1U> &allLayers)
  {
    int32_T switch_expression_size[2];
    int32_T loop_ub_tmp;
    char_T switch_expression_data[10000];

    //  Main function for the standardTF reflectivity calculation
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
    //  some execution paths' error
    loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
    problem->ssubs.set_size(loop_ub_tmp);
    problem->backgrounds.set_size(loop_ub_tmp);
    problem->qshifts.set_size(loop_ub_tmp);
    problem->scalefactors.set_size(loop_ub_tmp);
    problem->nbairs.set_size(loop_ub_tmp);
    problem->nbsubs.set_size(loop_ub_tmp);
    problem->resolutions.set_size(loop_ub_tmp);
    problem->calculations.all_chis.set_size(loop_ub_tmp);
    problem->calculations.sum_chi = 0.0;
    problem->allSubRough.set_size(loop_ub_tmp);
    problem->resample.set_size(1, loop_ub_tmp);
    reflectivity.set_size(loop_ub_tmp);
    Simulation.set_size(loop_ub_tmp);
    shifted_data.set_size(loop_ub_tmp);
    layerSlds.set_size(loop_ub_tmp);
    sldProfiles.set_size(loop_ub_tmp);
    allLayers.set_size(loop_ub_tmp);
    
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      problem->ssubs[i] = 0.0;
      problem->backgrounds[i] = 0.0;
      problem->qshifts[i] = 0.0;
      problem->scalefactors[i] = 0.0;
      problem->nbairs[i] = 0.0;
      problem->nbsubs[i] = 0.0;
      problem->resolutions[i] = 0.0;
      problem->calculations.all_chis[i] = 0.0;
      problem->allSubRough[i] = 0.0;
      problem->resample[i] = 0.0;
      reflectivity[i].f1.set_size(2, 2);
      reflectivity[i].f1[0] = 1.0;
      reflectivity[i].f1[1] = 1.0;
      reflectivity[i].f1[reflectivity[i].f1.size(0)] = 1.0;
      reflectivity[i].f1[reflectivity[i].f1.size(0) + 1] = 1.0;
      Simulation[i].f1.set_size(2, 2);
      Simulation[i].f1[0] = 1.0;
      Simulation[i].f1[1] = 1.0;
      Simulation[i].f1[Simulation[i].f1.size(0)] = 1.0;
      Simulation[i].f1[Simulation[i].f1.size(0) + 1] = 1.0;
      shifted_data[i].f1.set_size(2, 3);
      layerSlds[i].f1.set_size(2, 3);
      sldProfiles[i].f1.set_size(2, 2);
      sldProfiles[i].f1[0] = 1.0;
      sldProfiles[i].f1[1] = 1.0;
      sldProfiles[i].f1[sldProfiles[i].f1.size(0)] = 1.0;
      sldProfiles[i].f1[sldProfiles[i].f1.size(0) + 1] = 1.0;
      allLayers[i].f1.set_size(2, 3);
      for (int32_T b_i{0}; b_i < 3; b_i++) {
        shifted_data[i].f1[shifted_data[i].f1.size(0) * b_i] = 1.0;
        shifted_data[i].f1[shifted_data[i].f1.size(0) * b_i + 1] = 1.0;
        layerSlds[i].f1[layerSlds[i].f1.size(0) * b_i] = 1.0;
        layerSlds[i].f1[layerSlds[i].f1.size(0) * b_i + 1] = 1.0;
        allLayers[i].f1[allLayers[i].f1.size(0) * b_i] = 1.0;
        allLayers[i].f1[allLayers[i].f1.size(0) * b_i + 1] = 1.0;
      }
    }

    coder::lower(problemDef->modelType.data, problemDef->modelType.size,
                 switch_expression_data, switch_expression_size);
    if (coder::internal::c_strcmp(switch_expression_data, switch_expression_size))
    {
      loop_ub_tmp = 0;
    } else if (coder::internal::d_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 1;
    } else if (coder::internal::e_strcmp(switch_expression_data,
                switch_expression_size)) {
      loop_ub_tmp = 2;
    } else {
      loop_ub_tmp = -1;
    }

    switch (loop_ub_tmp) {
     case 0:
      //  Standard layers calculation
      standardTFStandardLayersReflectivityCalculation(problemDef,
        problemDefCells, controls, problem, reflectivity, Simulation,
        shifted_data, layerSlds, sldProfiles, allLayers);
      break;

     case 1:
      //  Custom layers with user supplied custom model file
      standardTFCustomLayersReflectivityCalculation(problemDef, problemDefCells,
        controls, problem, reflectivity, Simulation, shifted_data, layerSlds,
        sldProfiles, allLayers);
      break;

     case 2:
      //  Custom SLD profile with user defined model file
      standardTFCustomXYReflectivityCalculation(problemDef, problemDefCells,
        controls, problem, reflectivity, Simulation, shifted_data, layerSlds,
        sldProfiles, allLayers);
      break;
    }
  }
}

// End of code generation (standardTFReflectivityCalculation.cpp)
