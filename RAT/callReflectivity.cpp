//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// callReflectivity.cpp
//
// Code generation for function 'callReflectivity'
//

// Include files
#include "callReflectivity.h"
#include "abelesParallelPoints.h"
#include "abelesSingle.h"
#include "colon.h"
#include "dataResolutionPolly.h"
#include "resolutionPolly.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cmath>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Function Definitions
namespace RAT
{
  void b_callReflectivity(real_T nbairs, real_T nbsubs, const real_T
    simLimits_data[], const real_T repeatLayers[2], const ::coder::array<real_T,
    2U> &this_data, ::coder::array<real_T, 2U> &layers, real_T ssubs, real_T res,
    boolean_T useImaginary, ::coder::array<real_T, 2U> &reflectivity, ::coder::
    array<real_T, 2U> &Simulation)
  {
    ::coder::array<creal_T, 1U> slds;
    ::coder::array<real_T, 2U> firstSection;
    ::coder::array<real_T, 2U> lastSection;
    ::coder::array<real_T, 1U> r;
    ::coder::array<real_T, 1U> roughs;
    ::coder::array<real_T, 1U> simRef;
    ::coder::array<real_T, 1U> simResolData;
    ::coder::array<real_T, 1U> simXdata;
    ::coder::array<real_T, 1U> thicks;
    real_T b;
    real_T nLayersTot;
    real_T nRepeats;
    real_T step;
    int32_T b_nLayersTot;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    uint32_T layerCount;
    uint32_T splits_idx_1;
    if (repeatLayers[0] != 0.0) {
      nRepeats = repeatLayers[1];
    } else {
      nRepeats = 1.0;
    }

    //  Build the input arrays for thick, sld and rough.....
    if (layers.size(0) == 0) {
      //  No layers defined. Make a zeros dummy zero layer
      layers.set_size(1, 3);
      layers[0] = 0.0;
      layers[layers.size(0)] = nbairs;
      layers[layers.size(0) * 2] = 0.0;
    }

    //  Number of layers (including repeats)
    nLayersTot = static_cast<real_T>(layers.size(0)) * nRepeats + 2.0;

    //  Make arrays for thick, sld, rough
    b_nLayersTot = static_cast<int32_T>(nLayersTot);
    thicks.set_size(b_nLayersTot);
    roughs.set_size(b_nLayersTot);
    for (i = 0; i < b_nLayersTot; i++) {
      thicks[i] = 0.0;
      roughs[i] = 0.0;
    }

    if (useImaginary) {
      slds.set_size(b_nLayersTot);
      for (i = 0; i < b_nLayersTot; i++) {
        slds[i].re = 0.0;
        slds[i].im = 0.0;
      }
    } else {
      slds.set_size(b_nLayersTot);
      for (i = 0; i < b_nLayersTot; i++) {
        slds[i].re = 0.0;
        slds[i].im = 0.0;
      }
    }

    //  Populate the d,rho,sig arrays...
    layerCount = 2U;
    i = static_cast<int32_T>(nRepeats);
    for (int32_T m{0}; m < i; m++) {
      i1 = layers.size(0);
      for (int32_T n{0}; n < i1; n++) {
        if (!useImaginary) {
          b_nLayersTot = static_cast<int32_T>(layerCount + static_cast<uint32_T>
            (n)) - 1;
          thicks[b_nLayersTot] = layers[n];
          slds[b_nLayersTot].re = layers[n + layers.size(0)];
          slds[b_nLayersTot].im = 0.0;
          roughs[b_nLayersTot] = layers[n + layers.size(0) * 2];
        } else {
          b_nLayersTot = static_cast<int32_T>(layerCount + static_cast<uint32_T>
            (n)) - 1;
          thicks[b_nLayersTot] = layers[n];
          slds[b_nLayersTot].re = layers[n + layers.size(0)];
          slds[b_nLayersTot].im = layers[n + layers.size(0) * 2];
          roughs[b_nLayersTot] = layers[n + layers.size(0) * 3];
        }
      }

      layerCount += static_cast<uint32_T>(layers.size(0));
    }

    //  Add the air and substrate parameters
    slds[0].re = nbairs;
    slds[0].im = 0.0;
    slds[slds.size(0) - 1].re = nbsubs;
    slds[slds.size(0) - 1].im = 0.0;
    roughs[roughs.size(0) - 1] = ssubs;
    if (simLimits_data[0] < this_data[0]) {
      step = this_data[1] - this_data[0];
      b = this_data[0] - step;
      if (std::isnan(step) || std::isnan(b)) {
        firstSection.set_size(1, 1);
        firstSection[0] = rtNaN;
      } else if ((step == 0.0) || ((simLimits_data[0] < b) && (step < 0.0)) ||
                 ((b < simLimits_data[0]) && (step > 0.0))) {
        firstSection.set_size(1, 0);
      } else if ((std::isinf(simLimits_data[0]) || std::isinf(b)) && (std::isinf
                  (step) || (simLimits_data[0] == b))) {
        firstSection.set_size(1, 1);
        firstSection[0] = rtNaN;
      } else if (std::isinf(step)) {
        firstSection.set_size(1, 1);
        firstSection[0] = simLimits_data[0];
      } else if ((std::floor(simLimits_data[0]) == simLimits_data[0]) && (std::
                  floor(step) == step)) {
        loop_ub = static_cast<int32_T>((b - simLimits_data[0]) / step);
        firstSection.set_size(1, loop_ub + 1);
        for (i = 0; i <= loop_ub; i++) {
          firstSection[i] = simLimits_data[0] + step * static_cast<real_T>(i);
        }
      } else {
        coder::eml_float_colon(simLimits_data[0], step, b, firstSection);
      }
    } else {
      firstSection.set_size(1, 0);
    }

    if (simLimits_data[1] > this_data[this_data.size(0) - 1]) {
      step = this_data[this_data.size(0) - 1] - this_data[this_data.size(0) - 2];
      b = this_data[this_data.size(0) - 1] + step;
      if (std::isnan(b) || std::isnan(step)) {
        lastSection.set_size(1, 1);
        lastSection[0] = rtNaN;
      } else if ((step == 0.0) || ((b < simLimits_data[1]) && (step < 0.0)) ||
                 ((simLimits_data[1] < b) && (step > 0.0))) {
        lastSection.set_size(1, 0);
      } else if ((std::isinf(b) || std::isinf(simLimits_data[1])) && (std::isinf
                  (step) || (b == simLimits_data[1]))) {
        lastSection.set_size(1, 1);
        lastSection[0] = rtNaN;
      } else if (std::isinf(step)) {
        lastSection.set_size(1, 1);
        lastSection[0] = b;
      } else if ((std::floor(b) == b) && (std::floor(step) == step)) {
        loop_ub = static_cast<int32_T>((simLimits_data[1] - b) / step);
        lastSection.set_size(1, loop_ub + 1);
        for (i = 0; i <= loop_ub; i++) {
          lastSection[i] = b + step * static_cast<real_T>(i);
        }
      } else {
        coder::eml_float_colon(b, step, simLimits_data[1], lastSection);
      }
    } else {
      lastSection.set_size(1, 0);
    }

    b_nLayersTot = firstSection.size(1);
    simXdata.set_size((this_data.size(0) + firstSection.size(1)) +
                      lastSection.size(1));
    loop_ub = firstSection.size(1);
    for (i = 0; i < loop_ub; i++) {
      simXdata[i] = firstSection[i];
    }

    loop_ub = this_data.size(0);
    for (i = 0; i < loop_ub; i++) {
      simXdata[i + b_nLayersTot] = this_data[i];
    }

    loop_ub = lastSection.size(1);
    for (i = 0; i < loop_ub; i++) {
      simXdata[(i + b_nLayersTot) + this_data.size(0)] = lastSection[i];
    }

    splits_idx_1 = static_cast<uint32_T>(firstSection.size(1)) +
      static_cast<uint32_T>(this_data.size(0));
    Simulation.set_size(simXdata.size(0), 2);
    loop_ub = simXdata.size(0);
    for (i = 0; i < 2; i++) {
      for (i1 = 0; i1 < loop_ub; i1++) {
        Simulation[i1 + Simulation.size(0) * i] = 0.0;
      }
    }

    loop_ub = simXdata.size(0);
    for (i = 0; i < loop_ub; i++) {
      Simulation[i] = simXdata[i];
    }

    //  If we are using data resolutions, then we also need to adjust the length
    //  of the reolution column. We do thit by just extending with the rosolution
    //  values at the ends of the curve.
    simResolData.set_size(1);
    simResolData[0] = 0.0;
    if (res == -1.0) {
      b_nLayersTot = firstSection.size(1);
      simResolData.set_size((this_data.size(0) + firstSection.size(1)) +
                            lastSection.size(1));
      loop_ub = firstSection.size(1);
      for (i = 0; i < loop_ub; i++) {
        simResolData[i] = this_data[this_data.size(0) * 3];
      }

      loop_ub = this_data.size(0);
      for (i = 0; i < loop_ub; i++) {
        simResolData[i + b_nLayersTot] = this_data[i + this_data.size(0) * 3];
      }

      loop_ub = lastSection.size(1);
      for (i = 0; i < loop_ub; i++) {
        simResolData[(i + b_nLayersTot) + this_data.size(0)] = this_data
          [(this_data.size(0) + this_data.size(0) * 3) - 1];
      }
    }

    //  Parallelise over points
    //  Calculate reflectivity....
    abelesParallelPoints(simXdata, nLayersTot, thicks, slds, roughs, simRef);

    //  Apply resolution
    //  Note: paraPoints gives an error during valifation, so use
    //  single cored resolution as a workaround for now.
    if (res == -1.0) {
      // simRef = dataResolutionPollyParallelPoints(simXdata,simRef,simResolData,length(simXdata));
      dataResolutionPolly(simXdata, simRef, simResolData, static_cast<real_T>
                          (simXdata.size(0)), r);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        Simulation[i + Simulation.size(0)] = r[i];
      }
    } else {
      // simRef = resolutionPollyParallelPoints(simXdata,simRef,res,length(simXdata));
      resolutionPolly(simXdata, simRef, res, static_cast<real_T>(simXdata.size(0)),
                      r);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        Simulation[i + Simulation.size(0)] = r[i];
      }
    }

    if (static_cast<uint32_T>(firstSection.size(1)) + 1U > splits_idx_1) {
      i = 0;
      i1 = 0;
    } else {
      i = firstSection.size(1);
      i1 = static_cast<int32_T>(splits_idx_1);
    }

    loop_ub = i1 - i;
    reflectivity.set_size(loop_ub, 2);
    for (i1 = 0; i1 < 2; i1++) {
      for (b_nLayersTot = 0; b_nLayersTot < loop_ub; b_nLayersTot++) {
        reflectivity[b_nLayersTot + reflectivity.size(0) * i1] = Simulation[(i +
          b_nLayersTot) + Simulation.size(0) * i1];
      }
    }
  }

  void callReflectivity(real_T nbairs, real_T nbsubs, const real_T
                        simLimits_data[], const real_T repeatLayers[2], const ::
                        coder::array<real_T, 2U> &this_data, ::coder::array<
                        real_T, 2U> &layers, real_T ssubs, real_T res, boolean_T
                        useImaginary, ::coder::array<real_T, 2U> &reflectivity, ::
                        coder::array<real_T, 2U> &Simulation)
  {
    ::coder::array<creal_T, 1U> slds;
    ::coder::array<real_T, 2U> firstSection;
    ::coder::array<real_T, 2U> lastSection;
    ::coder::array<real_T, 1U> r;
    ::coder::array<real_T, 1U> roughs;
    ::coder::array<real_T, 1U> simRef;
    ::coder::array<real_T, 1U> simResolData;
    ::coder::array<real_T, 1U> simXdata;
    ::coder::array<real_T, 1U> thicks;
    real_T b;
    real_T nLayersTot;
    real_T nRepeats;
    real_T step;
    int32_T b_nLayersTot;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    uint32_T layerCount;
    uint32_T splits_idx_1;
    if (repeatLayers[0] != 0.0) {
      nRepeats = repeatLayers[1];
    } else {
      nRepeats = 1.0;
    }

    //  Build the input arrays for thick, sld and rough.....
    if (layers.size(0) == 0) {
      //  No layers defined. Make a zeros dummy zero layer
      layers.set_size(1, 3);
      layers[0] = 0.0;
      layers[layers.size(0)] = nbairs;
      layers[layers.size(0) * 2] = 0.0;
    }

    //  Number of layers (including repeats)
    nLayersTot = static_cast<real_T>(layers.size(0)) * nRepeats + 2.0;

    //  Make arrays for thick, sld, rough
    b_nLayersTot = static_cast<int32_T>(nLayersTot);
    thicks.set_size(b_nLayersTot);
    roughs.set_size(b_nLayersTot);
    for (i = 0; i < b_nLayersTot; i++) {
      thicks[i] = 0.0;
      roughs[i] = 0.0;
    }

    if (useImaginary) {
      slds.set_size(b_nLayersTot);
      for (i = 0; i < b_nLayersTot; i++) {
        slds[i].re = 0.0;
        slds[i].im = 0.0;
      }
    } else {
      slds.set_size(b_nLayersTot);
      for (i = 0; i < b_nLayersTot; i++) {
        slds[i].re = 0.0;
        slds[i].im = 0.0;
      }
    }

    //  Populate the d,rho,sig arrays...
    layerCount = 2U;
    i = static_cast<int32_T>(nRepeats);
    for (int32_T m{0}; m < i; m++) {
      i1 = layers.size(0);
      for (int32_T n{0}; n < i1; n++) {
        if (!useImaginary) {
          b_nLayersTot = static_cast<int32_T>(layerCount + static_cast<uint32_T>
            (n)) - 1;
          thicks[b_nLayersTot] = layers[n];
          slds[b_nLayersTot].re = layers[n + layers.size(0)];
          slds[b_nLayersTot].im = 0.0;
          roughs[b_nLayersTot] = layers[n + layers.size(0) * 2];
        } else {
          b_nLayersTot = static_cast<int32_T>(layerCount + static_cast<uint32_T>
            (n)) - 1;
          thicks[b_nLayersTot] = layers[n];
          slds[b_nLayersTot].re = layers[n + layers.size(0)];
          slds[b_nLayersTot].im = layers[n + layers.size(0) * 2];
          roughs[b_nLayersTot] = layers[n + layers.size(0) * 3];
        }
      }

      layerCount += static_cast<uint32_T>(layers.size(0));
    }

    //  Add the air and substrate parameters
    slds[0].re = nbairs;
    slds[0].im = 0.0;
    slds[slds.size(0) - 1].re = nbsubs;
    slds[slds.size(0) - 1].im = 0.0;
    roughs[roughs.size(0) - 1] = ssubs;
    if (simLimits_data[0] < this_data[0]) {
      step = this_data[1] - this_data[0];
      b = this_data[0] - step;
      if (std::isnan(step) || std::isnan(b)) {
        firstSection.set_size(1, 1);
        firstSection[0] = rtNaN;
      } else if ((step == 0.0) || ((simLimits_data[0] < b) && (step < 0.0)) ||
                 ((b < simLimits_data[0]) && (step > 0.0))) {
        firstSection.set_size(1, 0);
      } else if ((std::isinf(simLimits_data[0]) || std::isinf(b)) && (std::isinf
                  (step) || (simLimits_data[0] == b))) {
        firstSection.set_size(1, 1);
        firstSection[0] = rtNaN;
      } else if (std::isinf(step)) {
        firstSection.set_size(1, 1);
        firstSection[0] = simLimits_data[0];
      } else if ((std::floor(simLimits_data[0]) == simLimits_data[0]) && (std::
                  floor(step) == step)) {
        loop_ub = static_cast<int32_T>((b - simLimits_data[0]) / step);
        firstSection.set_size(1, loop_ub + 1);
        for (i = 0; i <= loop_ub; i++) {
          firstSection[i] = simLimits_data[0] + step * static_cast<real_T>(i);
        }
      } else {
        coder::eml_float_colon(simLimits_data[0], step, b, firstSection);
      }
    } else {
      firstSection.set_size(1, 0);
    }

    if (simLimits_data[1] > this_data[this_data.size(0) - 1]) {
      step = this_data[this_data.size(0) - 1] - this_data[this_data.size(0) - 2];
      b = this_data[this_data.size(0) - 1] + step;
      if (std::isnan(b) || std::isnan(step)) {
        lastSection.set_size(1, 1);
        lastSection[0] = rtNaN;
      } else if ((step == 0.0) || ((b < simLimits_data[1]) && (step < 0.0)) ||
                 ((simLimits_data[1] < b) && (step > 0.0))) {
        lastSection.set_size(1, 0);
      } else if ((std::isinf(b) || std::isinf(simLimits_data[1])) && (std::isinf
                  (step) || (b == simLimits_data[1]))) {
        lastSection.set_size(1, 1);
        lastSection[0] = rtNaN;
      } else if (std::isinf(step)) {
        lastSection.set_size(1, 1);
        lastSection[0] = b;
      } else if ((std::floor(b) == b) && (std::floor(step) == step)) {
        loop_ub = static_cast<int32_T>((simLimits_data[1] - b) / step);
        lastSection.set_size(1, loop_ub + 1);
        for (i = 0; i <= loop_ub; i++) {
          lastSection[i] = b + step * static_cast<real_T>(i);
        }
      } else {
        coder::eml_float_colon(b, step, simLimits_data[1], lastSection);
      }
    } else {
      lastSection.set_size(1, 0);
    }

    b_nLayersTot = firstSection.size(1);
    simXdata.set_size((this_data.size(0) + firstSection.size(1)) +
                      lastSection.size(1));
    loop_ub = firstSection.size(1);
    for (i = 0; i < loop_ub; i++) {
      simXdata[i] = firstSection[i];
    }

    loop_ub = this_data.size(0);
    for (i = 0; i < loop_ub; i++) {
      simXdata[i + b_nLayersTot] = this_data[i];
    }

    loop_ub = lastSection.size(1);
    for (i = 0; i < loop_ub; i++) {
      simXdata[(i + b_nLayersTot) + this_data.size(0)] = lastSection[i];
    }

    splits_idx_1 = static_cast<uint32_T>(firstSection.size(1)) +
      static_cast<uint32_T>(this_data.size(0));
    Simulation.set_size(simXdata.size(0), 2);
    loop_ub = simXdata.size(0);
    for (i = 0; i < 2; i++) {
      for (i1 = 0; i1 < loop_ub; i1++) {
        Simulation[i1 + Simulation.size(0) * i] = 0.0;
      }
    }

    loop_ub = simXdata.size(0);
    for (i = 0; i < loop_ub; i++) {
      Simulation[i] = simXdata[i];
    }

    //  If we are using data resolutions, then we also need to adjust the length
    //  of the reolution column. We do thit by just extending with the rosolution
    //  values at the ends of the curve.
    simResolData.set_size(1);
    simResolData[0] = 0.0;
    if (res == -1.0) {
      b_nLayersTot = firstSection.size(1);
      simResolData.set_size((this_data.size(0) + firstSection.size(1)) +
                            lastSection.size(1));
      loop_ub = firstSection.size(1);
      for (i = 0; i < loop_ub; i++) {
        simResolData[i] = this_data[this_data.size(0) * 3];
      }

      loop_ub = this_data.size(0);
      for (i = 0; i < loop_ub; i++) {
        simResolData[i + b_nLayersTot] = this_data[i + this_data.size(0) * 3];
      }

      loop_ub = lastSection.size(1);
      for (i = 0; i < loop_ub; i++) {
        simResolData[(i + b_nLayersTot) + this_data.size(0)] = this_data
          [(this_data.size(0) + this_data.size(0) * 3) - 1];
      }
    }

    //  Single cored over points
    //  Calculate reflectivity.....
    abelesSingle(simXdata, nLayersTot, thicks, slds, roughs, simRef);

    //  Apply resolution correction...
    if (res == -1.0) {
      dataResolutionPolly(simXdata, simRef, simResolData, static_cast<real_T>
                          (simXdata.size(0)), r);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        Simulation[i + Simulation.size(0)] = r[i];
      }
    } else {
      resolutionPolly(simXdata, simRef, res, static_cast<real_T>(simXdata.size(0)),
                      r);
      loop_ub = r.size(0);
      for (i = 0; i < loop_ub; i++) {
        Simulation[i + Simulation.size(0)] = r[i];
      }
    }

    if (static_cast<uint32_T>(firstSection.size(1)) + 1U > splits_idx_1) {
      i = 0;
      i1 = 0;
    } else {
      i = firstSection.size(1);
      i1 = static_cast<int32_T>(splits_idx_1);
    }

    loop_ub = i1 - i;
    reflectivity.set_size(loop_ub, 2);
    for (i1 = 0; i1 < 2; i1++) {
      for (b_nLayersTot = 0; b_nLayersTot < loop_ub; b_nLayersTot++) {
        reflectivity[b_nLayersTot + reflectivity.size(0) * i1] = Simulation[(i +
          b_nLayersTot) + Simulation.size(0) * i1];
      }
    }
  }
}

// End of code generation (callReflectivity.cpp)
