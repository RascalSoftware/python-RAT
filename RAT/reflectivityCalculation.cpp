//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation.cpp
//
// Code generation for function 'reflectivityCalculation'
//

// Include files
#include "reflectivityCalculation.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "standardTFReflectivityCalculation.h"
#include "strcmp.h"
#include "triggerEvent.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Type Definitions
namespace RAT
{
  struct cell_wrap_12
  {
    ::coder::array<real_T, 2U> f1;
  };
}

// Function Declarations
namespace RAT
{
  static void cast(const cell_8 *r, cell_19 *r1);
  static void cast(const ::coder::array<cell_wrap_2, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1);
  static void cast(const ::coder::array<cell_wrap_4, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1);
  static void cast(const ::coder::array<cell_wrap_1, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1);
  static void cast(const ::coder::array<cell_wrap_3, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1);
  static void cast(const ::coder::array<cell_wrap_9, 1U> &r, ::coder::array<
                   cell_wrap_12, 1U> &r1);
  static void cast(const ::coder::array<cell_wrap_15, 1U> &r, ::coder::array<
                   cell_wrap_12, 1U> &r1);
  static void cast(const ::coder::array<cell_wrap_15, 1U> &r, ::coder::array<
                   cell_wrap_13, 1U> &r1);
}

// Function Definitions
namespace RAT
{
  static void cast(const cell_8 *r, cell_19 *r1)
  {
    int32_T loop_ub;
    r1->f1.set_size(1, r->f1.size(1));
    loop_ub = r->f1.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f1[i] = r->f1[i];
    }

    cast(r->f2, r1->f2);
    r1->f3.set_size(1, r->f3.size(1));
    loop_ub = r->f3.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f3[i] = r->f3[i];
    }

    cast(r->f4, r1->f4);
    cast(r->f5, r1->f5);
    cast(r->f6, r1->f6);
    r1->f7.set_size(1, r->f7.size(1));
    loop_ub = r->f7.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f7[i] = r->f7[i];
    }

    r1->f8.set_size(1, r->f8.size(1));
    loop_ub = r->f8.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f8[i] = r->f8[i];
    }

    r1->f9.set_size(1, r->f9.size(1));
    loop_ub = r->f9.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f9[i] = r->f9[i];
    }

    r1->f10.set_size(1, r->f10.size(1));
    loop_ub = r->f10.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f10[i] = r->f10[i];
    }

    r1->f11.set_size(1, r->f11.size(1));
    loop_ub = r->f11.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f11[i] = r->f11[i];
    }

    r1->f12.set_size(1, r->f12.size(1));
    loop_ub = r->f12.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f12[i] = r->f12[i];
    }

    r1->f13.set_size(1, r->f13.size(1));
    loop_ub = r->f13.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f13[i] = r->f13[i];
    }

    r1->f14.set_size(1, r->f14.size(1));
    loop_ub = r->f14.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f14[i] = r->f14[i];
    }

    r1->f15.set_size(1, r->f15.size(1));
    loop_ub = r->f15.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f15[i] = r->f15[i];
    }

    r1->f16.set_size(1, r->f16.size(1));
    loop_ub = r->f16.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f16[i] = r->f16[i];
    }

    cast(r->f17, r1->f17);
    r1->f18.set_size(1, r->f18.size(1));
    loop_ub = r->f18.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f18[i] = r->f18[i];
    }

    cast(r->f19, r1->f19);
    r1->f20.set_size(1, r->f20.size(1));
    loop_ub = r->f20.size(1);
    for (int32_T i{0}; i < loop_ub; i++) {
      r1->f20[i] = r->f20[i];
    }
  }

  static void cast(const ::coder::array<cell_wrap_2, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size[1];
      r1[r1.size(0) * i1].f1.set_size(r[r.size(0) * i1].f1.size[0], r[r.size(0) *
        i1].f1.size[1]);
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size[0];
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[r1[i1].f1.size(0) * i2] = r[i1].f1.data[r[i1].f1.size[0] *
            i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_4, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size[1];
      r1[i1].f1.set_size(r[i1].f1.size[0], r[i1].f1.size[1]);
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size[0];
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[r1[i1].f1.size(0) * i2] = r[i1].f1.data[r[i1].f1.size[0] *
            i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_1, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[r1.size(0) * i1].f1.set_size(r[r.size(0) * i1].f1.size(0), r[r.size(0) *
        i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_3, 2U> &r, ::coder::array<
                   cell_wrap_15, 2U> &r1)
  {
    int32_T i;
    r1.set_size(1, r.size(1));
    i = r.size(1) - 1;
    for (int32_T i1{0}; i1 <= i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[r1.size(0) * i1].f1.set_size(r[r.size(0) * i1].f1.size(0), r[r.size(0) *
        i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[r1[i1].f1.size(0) * i2] = r[i1].f1[r[i1].f1.size(0) * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_9, 1U> &r, ::coder::array<
                   cell_wrap_12, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(0);
      r1[i1].f1.set_size(r[i1].f1.size(0), 2);
      for (int32_T i2{0}; i2 < 2; i2++) {
        for (int32_T i3{0}; i3 < loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_15, 1U> &r, ::coder::array<
                   cell_wrap_12, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[i1].f1.set_size(r[i1].f1.size(0), r[i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  static void cast(const ::coder::array<cell_wrap_15, 1U> &r, ::coder::array<
                   cell_wrap_13, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[i1].f1.set_size(r[i1].f1.size(0), r[i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  void reflectivityCalculation(const struct0_T *problemDef, const cell_8
    *problemDefCells, const struct1_T *, const struct2_T *controls, struct4_T
    *problem, cell_11 *result)
  {
    ::coder::array<cell_wrap_12, 1U> Simulation;
    ::coder::array<cell_wrap_12, 1U> reflectivity;
    ::coder::array<cell_wrap_12, 1U> sldProfiles;
    ::coder::array<cell_wrap_13, 1U> allLayers;
    ::coder::array<cell_wrap_13, 1U> layerSlds;
    ::coder::array<cell_wrap_13, 1U> shifted_data;
    ::coder::array<cell_wrap_15, 1U> b_allLayers;
    ::coder::array<cell_wrap_15, 1U> b_layerSlds;
    ::coder::array<cell_wrap_15, 1U> b_shifted_data;
    ::coder::array<cell_wrap_15, 1U> b_sldProfiles;
    ::coder::array<cell_wrap_9, 1U> b_Simulation;
    ::coder::array<cell_wrap_9, 1U> b_reflectivity;
    cell_19 r;
    int32_T i;
    int32_T i1;

    //  Main entry point into the reflectivity calculation for the toolbox.
    //  This is the main function that is called by any of the minimisers or
    //  analysis tools from the rest of the toolbox.
    //
    //  *The main job of this function is to decide which type of calculation
    //  (i.e. 'Target function' is required, and call the relevant routines.
    //  The types of available target functions are:*
    //
    //  * non polarised  - The main basic target function type, for non polarised neutrons (or x-rays) with non-absorbing samples. Different model types are specified in sub functions from here.
    //
    //  * oil water      - Target function for oil-water samples
    //
    //  * domains        - Target function for samples consisting of domains which are larger than the beam lateral coherence length.
    //
    //  * magnetic       - Target function for cases for polarised neutrons with polarisation analysis.
    //
    //  triggerEvent('message', 'Running reflectivity calculation...');
    //  for compilation, we have to preallocate memory for the output arrays
    //  Setting these parameters in the struct defines them as doubles
    problem->ssubs.set_size(1);
    problem->ssubs[0] = 0.0;
    problem->backgrounds.set_size(1);
    problem->backgrounds[0] = 0.0;
    problem->qshifts.set_size(1);
    problem->qshifts[0] = 0.0;
    problem->scalefactors.set_size(1);
    problem->scalefactors[0] = 0.0;
    problem->nbairs.set_size(1);
    problem->nbairs[0] = 0.0;
    problem->nbsubs.set_size(1);
    problem->nbsubs[0] = 0.0;
    problem->resolutions.set_size(1);
    problem->resolutions[0] = 0.0;
    problem->calculations.all_chis.set_size(1);
    problem->calculations.all_chis[0] = 0.0;
    problem->calculations.sum_chi = 0.0;
    problem->allSubRough.set_size(1);
    problem->allSubRough[0] = 0.0;
    problem->resample.set_size(1, 1);
    problem->resample[0] = 0.0;

    //  We also foll the results arrays to define their
    //  type and size. (NOTE: at the moment we have a 'coder.varsize'
    //  pre-processor directives for the compiler here and at the
    //  end for the results block. We are unlikely to need both
    //  TODO: Find out which is necessary and tidy this up.
    i = static_cast<int32_T>(problemDef->numberOfContrasts);
    reflectivity.set_size(i);
    Simulation.set_size(i);
    shifted_data.set_size(i);
    layerSlds.set_size(i);
    sldProfiles.set_size(i);
    allLayers.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      reflectivity[b_i].f1.set_size(2, 2);
      reflectivity[b_i].f1[0] = 1.0;
      reflectivity[b_i].f1[1] = 1.0;
      reflectivity[b_i].f1[reflectivity[b_i].f1.size(0)] = 1.0;
      reflectivity[b_i].f1[reflectivity[b_i].f1.size(0) + 1] = 1.0;
      Simulation[b_i].f1.set_size(2, 2);
      Simulation[b_i].f1[0] = 1.0;
      Simulation[b_i].f1[1] = 1.0;
      Simulation[b_i].f1[Simulation[b_i].f1.size(0)] = 1.0;
      Simulation[b_i].f1[Simulation[b_i].f1.size(0) + 1] = 1.0;
      shifted_data[b_i].f1.set_size(2, 3);
      layerSlds[b_i].f1.set_size(2, 3);
      sldProfiles[b_i].f1.set_size(2, 2);
      sldProfiles[b_i].f1[0] = 1.0;
      sldProfiles[b_i].f1[1] = 1.0;
      sldProfiles[b_i].f1[sldProfiles[b_i].f1.size(0)] = 1.0;
      sldProfiles[b_i].f1[sldProfiles[b_i].f1.size(0) + 1] = 1.0;
      allLayers[b_i].f1.set_size(2, 3);
      for (i1 = 0; i1 < 3; i1++) {
        shifted_data[b_i].f1[shifted_data[b_i].f1.size(0) * i1] = 1.0;
        shifted_data[b_i].f1[shifted_data[b_i].f1.size(0) * i1 + 1] = 1.0;
        layerSlds[b_i].f1[layerSlds[b_i].f1.size(0) * i1] = 1.0;
        layerSlds[b_i].f1[layerSlds[b_i].f1.size(0) * i1 + 1] = 1.0;
        allLayers[b_i].f1[allLayers[b_i].f1.size(0) * i1] = 1.0;
        allLayers[b_i].f1[allLayers[b_i].f1.size(0) * i1 + 1] = 1.0;
      }
    }

    //  Decide which target function we are calling and call the relevant routines
    if (coder::internal::b_strcmp(problemDef->TF.data, problemDef->TF.size)) {
      i1 = 0;
    } else {
      i1 = -1;
    }

    if (i1 == 0) {
      cast(problemDefCells, &r);
      standardTFReflectivityCalculation(problemDef, &r, controls, problem,
        b_reflectivity, b_Simulation, b_shifted_data, b_layerSlds, b_sldProfiles,
        b_allLayers);
      cast(b_reflectivity, reflectivity);
      cast(b_Simulation, Simulation);
      cast(b_shifted_data, shifted_data);
      cast(b_layerSlds, layerSlds);
      cast(b_sldProfiles, sldProfiles);
      cast(b_allLayers, allLayers);

      // case 'oil water'
      // problem = oilWaterTF_reflectivityCalculation(problemDef,problemDefCells,controls);
      // case 'magnetic'
      // problem = polarisedTF_reflectivityCalculation(problemDef,problemDefCells,controls);
      // case 'domains'
      // [problem,reflectivity,Simulation,shifted_data,layerSlds,sldProfiles,allLayers] = domainsTF_reflectivityCalculation(problemDef,problemDefCells,problemDefLimits,controls);
      // otherwise
      // error('The calculation type "%s" is not supported', whichTF);
    }

    result->f1.set_size(i);
    result->f2.set_size(i);
    result->f3.set_size(i);
    result->f4.set_size(i);
    result->f5.set_size(i);
    result->f6.set_size(i);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      int32_T b_loop_ub;
      int32_T loop_ub;
      loop_ub = reflectivity[b_i].f1.size(0);
      result->f1[b_i].f1.set_size(reflectivity[b_i].f1.size(0), 2);
      b_loop_ub = Simulation[b_i].f1.size(0);
      result->f2[b_i].f1.set_size(Simulation[b_i].f1.size(0), 2);
      for (i1 = 0; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result->f1[b_i].f1[i2 + result->f1[b_i].f1.size(0) * i1] =
            reflectivity[b_i].f1[i2 + reflectivity[b_i].f1.size(0) * i1];
        }

        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          result->f2[b_i].f1[i2 + result->f2[b_i].f1.size(0) * i1] =
            Simulation[b_i].f1[i2 + Simulation[b_i].f1.size(0) * i1];
        }
      }

      loop_ub = shifted_data[b_i].f1.size(0);
      result->f3[b_i].f1.set_size(shifted_data[b_i].f1.size(0), 3);
      b_loop_ub = layerSlds[b_i].f1.size(0);
      result->f4[b_i].f1.set_size(layerSlds[b_i].f1.size(0), 3);
      for (i1 = 0; i1 < 3; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result->f3[b_i].f1[i2 + result->f3[b_i].f1.size(0) * i1] =
            shifted_data[b_i].f1[i2 + shifted_data[b_i].f1.size(0) * i1];
        }

        for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
          result->f4[b_i].f1[i2 + result->f4[b_i].f1.size(0) * i1] =
            layerSlds[b_i].f1[i2 + layerSlds[b_i].f1.size(0) * i1];
        }
      }

      loop_ub = sldProfiles[b_i].f1.size(0);
      result->f5[b_i].f1.set_size(sldProfiles[b_i].f1.size(0), 2);
      for (i1 = 0; i1 < 2; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result->f5[b_i].f1[i2 + result->f5[b_i].f1.size(0) * i1] =
            sldProfiles[b_i].f1[i2 + sldProfiles[b_i].f1.size(0) * i1];
        }
      }

      loop_ub = allLayers[b_i].f1.size(0);
      result->f6[b_i].f1.set_size(allLayers[b_i].f1.size(0), 3);
      for (i1 = 0; i1 < 3; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          result->f6[b_i].f1[i2 + result->f6[b_i].f1.size(0) * i1] =
            allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1];
        }
      }
    }

    //  Send plot event
    triggerEvent(result->f1, result->f3, result->f5, result->f6, problem->ssubs,
                 problemDef->resample, problemDef->dataPresent,
                 problemDef->modelType.data, problemDef->modelType.size);

    //  Pre-processor directives for Matlab Coder
    //  to define the size of the output array
    // Result coder definitions....
    // Reflectivity
    // Simulatin
    // Shifted data
    // Layers slds
    // Sld profiles
    // All layers (resampled)
  }
}

// End of code generation (reflectivityCalculation.cpp)
