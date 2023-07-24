//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFCustomLayersReflectivityCalculation.cpp
//
// Code generation for function 'standardTFCustomLayersReflectivityCalculation'
//

// Include files
#include "standardTFCustomLayersReflectivityCalculation.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFCustomLayersParallelContrasts.h"
#include "standardTFCustomLayersParallelPoints.h"
#include "standardTFCustomLayersSingle.h"
#include "strcmp.h"
#include "sum.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Definitions
namespace RAT
{
  void standardTFCustomLayersReflectivityCalculation(const struct0_T *problemDef,
    const cell_19 *problemDefCells, const struct2_T *controls, struct4_T
    *problem, ::coder::array<cell_wrap_9, 1U> &reflectivity, ::coder::array<
    cell_wrap_9, 1U> &Simulation, ::coder::array<cell_wrap_15, 1U> &shifted_data,
    ::coder::array<cell_wrap_15, 1U> &layerSlds, ::coder::array<cell_wrap_15, 1U>
    &sldProfiles, ::coder::array<cell_wrap_15, 1U> &allLayers)
  {
    int32_T loop_ub_tmp;

    //  Custom layers reflectivity calculation for standardTF
    //  This function decides on parallelisation options before calling the
    //  relevant version of the main custom layers calculation. It is more
    //  efficient to have multiple versions of the core calculation, each dealing
    //  with a different scheme for parallelisation. These are:
    //  single    - single threaded teflectivity calculation
    //  points    - parallelise over points in the reflectivity calculation
    //  contrasts - parallelise over contrasts.
    //  Pre-allocation - It's necessary to
    //  pre-allocate the memory for all the arrays
    //  for compilation, so do this in this block.
    loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
    problem->ssubs.set_size(loop_ub_tmp);
    problem->backgrounds.set_size(loop_ub_tmp);
    problem->qshifts.set_size(loop_ub_tmp);
    problem->scalefactors.set_size(loop_ub_tmp);
    problem->nbairs.set_size(loop_ub_tmp);
    problem->nbsubs.set_size(loop_ub_tmp);
    problem->calculations.all_chis.set_size(loop_ub_tmp);
    problem->resolutions.set_size(loop_ub_tmp);
    problem->allSubRough.set_size(loop_ub_tmp);
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
      problem->calculations.all_chis[i] = 0.0;
      problem->resolutions[i] = 0.0;
      problem->allSubRough[i] = 0.0;
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

    //  End pre-allocation
    if (coder::internal::f_strcmp(controls->para.data, controls->para.size)) {
      loop_ub_tmp = 0;
    } else if (coder::internal::g_strcmp(controls->para.data,
                controls->para.size)) {
      loop_ub_tmp = 1;
    } else if (coder::internal::h_strcmp(controls->para.data,
                controls->para.size)) {
      loop_ub_tmp = 2;
    } else {
      loop_ub_tmp = -1;
    }

    switch (loop_ub_tmp) {
     case 0:
      standardTFCustomLayersSingle(problemDef, problemDefCells, controls,
        problem->ssubs, problem->backgrounds, problem->qshifts,
        problem->scalefactors, problem->nbairs, problem->nbsubs,
        problem->resolutions, problem->calculations.all_chis, reflectivity,
        Simulation, shifted_data, layerSlds, sldProfiles, allLayers,
        problem->allSubRough);
      break;

     case 1:
      standardTFCustomLayersParallelPoints(problemDef, problemDefCells, controls,
        problem->ssubs, problem->backgrounds, problem->qshifts,
        problem->scalefactors, problem->nbairs, problem->nbsubs,
        problem->resolutions, problem->calculations.all_chis, reflectivity,
        Simulation, shifted_data, layerSlds, sldProfiles, allLayers,
        problem->allSubRough);
      break;

     case 2:
      standardTFCustomLayersParallelContrasts(problemDef, problemDefCells,
        controls, problem->ssubs, problem->backgrounds, problem->qshifts,
        problem->scalefactors, problem->nbairs, problem->nbsubs,
        problem->resolutions, problem->calculations.all_chis, reflectivity,
        Simulation, shifted_data, layerSlds, sldProfiles, allLayers,
        problem->allSubRough);
      break;
    }

    problem->calculations.sum_chi = coder::sum(problem->calculations.all_chis);
    problem->resample.set_size(1, problemDef->resample.size(1));
    loop_ub_tmp = problemDef->resample.size(1);
    for (int32_T b_i{0}; b_i < loop_ub_tmp; b_i++) {
      problem->resample[b_i] = problemDef->resample[b_i];
    }
  }
}

// End of code generation (standardTFCustomLayersReflectivityCalculation.cpp)