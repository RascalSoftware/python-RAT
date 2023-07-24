//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// triggerEvent.cpp
//
// Code generation for function 'triggerEvent'
//

// Include files
#include "triggerEvent.h"
#include "getenv.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "eventHelper.hpp"
#include <algorithm>
#include <stdio.h>

// Variable Definitions
namespace RAT
{
  static boolean_T notified;
  static boolean_T helper_not_empty;
}

// Function Declarations
namespace RAT
{
  static void packCellArray(const ::coder::array<cell_wrap_9, 1U> &cellArray, ::
    coder::array<real_T, 1U> &packedArray, ::coder::array<real_T, 1U> &counts);
  static void packCellArray(const ::coder::array<cell_wrap_13, 1U> &cellArray, ::
    coder::array<real_T, 1U> &packedArray, ::coder::array<real_T, 1U> &counts);
}

// Function Definitions
namespace RAT
{
  static void packCellArray(const ::coder::array<cell_wrap_9, 1U> &cellArray, ::
    coder::array<real_T, 1U> &packedArray, ::coder::array<real_T, 1U> &counts)
  {
    real_T rowSize;
    real_T start;
    int32_T b_rowSize;
    int32_T i;

    //  Packs a cell array with different sized arrays into a single row
    //  array and an array of counts for each cell. For the example below
    //  reflect will be [1, 2, 3, 4, 5, 6, 7] and nReflect will be [3, 4]
    //
    //  [reflect, nReflect] = packCellArray({[1, 2, 3], [4, 5, 6, 7]});
    rowSize = 0.0;
    counts.set_size(cellArray.size(0));
    i = cellArray.size(0);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T d;
      d = static_cast<real_T>(cellArray[b_i].f1.size(0)) * 2.0;
      counts[b_i] = d;
      rowSize += d;
    }

    b_rowSize = static_cast<int32_T>(rowSize);
    packedArray.set_size(b_rowSize);
    for (i = 0; i < b_rowSize; i++) {
      packedArray[i] = 0.0;
    }

    start = 1.0;
    i = cellArray.size(0);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T stop;
      int32_T i1;
      stop = start + counts[b_i];
      if (start > stop - 1.0) {
        i1 = 1;
      } else {
        i1 = static_cast<int32_T>(start);
      }

      b_rowSize = cellArray[b_i].f1.size(0) << 1;
      for (int32_T i2{0}; i2 < b_rowSize; i2++) {
        packedArray[(i1 + i2) - 1] = cellArray[b_i].f1[i2];
      }

      start = stop;
    }
  }

  static void packCellArray(const ::coder::array<cell_wrap_13, 1U> &cellArray, ::
    coder::array<real_T, 1U> &packedArray, ::coder::array<real_T, 1U> &counts)
  {
    real_T rowSize;
    real_T start;
    int32_T b_rowSize;
    int32_T i;

    //  Packs a cell array with different sized arrays into a single row
    //  array and an array of counts for each cell. For the example below
    //  reflect will be [1, 2, 3, 4, 5, 6, 7] and nReflect will be [3, 4]
    //
    //  [reflect, nReflect] = packCellArray({[1, 2, 3], [4, 5, 6, 7]});
    rowSize = 0.0;
    counts.set_size(cellArray.size(0));
    i = cellArray.size(0);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T d;
      d = static_cast<real_T>(cellArray[b_i].f1.size(0)) * 3.0;
      counts[b_i] = d;
      rowSize += d;
    }

    b_rowSize = static_cast<int32_T>(rowSize);
    packedArray.set_size(b_rowSize);
    for (i = 0; i < b_rowSize; i++) {
      packedArray[i] = 0.0;
    }

    start = 1.0;
    i = cellArray.size(0);
    for (int32_T b_i{0}; b_i < i; b_i++) {
      real_T stop;
      int32_T i1;
      stop = start + counts[b_i];
      if (start > stop - 1.0) {
        i1 = 1;
      } else {
        i1 = static_cast<int32_T>(start);
      }

      b_rowSize = cellArray[b_i].f1.size(0) * 3;
      for (int32_T i2{0}; i2 < b_rowSize; i2++) {
        packedArray[(i1 + i2) - 1] = cellArray[b_i].f1[i2];
      }

      start = stop;
    }
  }

  void helper_not_empty_init()
  {
    helper_not_empty = false;
  }

  void triggerEvent(const ::coder::array<cell_wrap_9, 1U> &data_f1_f1, const ::
                    coder::array<cell_wrap_13, 1U> &data_f1_f3, const ::coder::
                    array<cell_wrap_9, 1U> &data_f1_f5, const ::coder::array<
                    cell_wrap_13, 1U> &data_f1_f6, const ::coder::array<real_T,
                    1U> &data_f2, const ::coder::array<real_T, 2U>
                    &data_f3_resample, const ::coder::array<real_T, 2U>
                    &data_f3_dataPresent, const char_T data_f3_modelType_data[],
                    const int32_T data_f3_modelType_size[2])
  {
    static eventHelper helper;
    ::coder::array<real_T, 2U> dataPresent;
    ::coder::array<real_T, 2U> resample;
    ::coder::array<real_T, 1U> layers;
    ::coder::array<real_T, 1U> nLayers;
    ::coder::array<real_T, 1U> nReflect;
    ::coder::array<real_T, 1U> nShiftedData;
    ::coder::array<real_T, 1U> nSldProfiles;
    ::coder::array<real_T, 1U> reflect;
    ::coder::array<real_T, 1U> shiftedData;
    ::coder::array<real_T, 1U> sldProfiles;
    ::coder::array<real_T, 1U> ssubs;
    ::coder::array<char_T, 2U> path;
    ::coder::array<char_T, 2U> r;
    int32_T loop_ub;
    char_T modelType_data[10001];
    boolean_T initialised;

    //  Triggers the event type with the given data. The supported event types are
    //  'message' and 'plot'. The data for message is a char array while for
    //  the plot it is a cell array containing the result cell, problem.ssubs
    //  and problemDef
    //
    //  triggerEvent('message', 'Hello world');
    if (!helper_not_empty) {
      //  Declaration for coder
      helper_not_empty = true;

      //  Make an instance
      helper = eventHelper();
      coder::b_getenv(r);
      path.set_size(1, r.size(1) + 1);
      loop_ub = r.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        path[i] = r[i];
      }

      path[r.size(1)] = '\x00';
      std::mem_fn(&eventHelper::init)(helper, &path[0]);
    }

    initialised = std::mem_fn(&eventHelper::isInitialised)(helper);
    if (initialised) {
      packCellArray(data_f1_f1, reflect, nReflect);

      //  reflectivity
      packCellArray(data_f1_f3, shiftedData, nShiftedData);
      packCellArray(data_f1_f5, sldProfiles, nSldProfiles);
      packCellArray(data_f1_f6, layers, nLayers);

      //  All Layers
      ssubs.set_size(data_f2.size(0));
      loop_ub = data_f2.size(0);
      for (int32_T i{0}; i < loop_ub; i++) {
        ssubs[i] = data_f2[i];
      }

      //  ssubs
      loop_ub = data_f3_modelType_size[1];
      if (loop_ub - 1 >= 0) {
        std::copy(&data_f3_modelType_data[0], &data_f3_modelType_data[loop_ub],
                  &modelType_data[0]);
      }

      modelType_data[data_f3_modelType_size[1]] = '\x00';
      resample.set_size(1, data_f3_resample.size(1));
      loop_ub = data_f3_resample.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        resample[i] = data_f3_resample[i];
      }

      dataPresent.set_size(1, data_f3_dataPresent.size(1));
      loop_ub = data_f3_dataPresent.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        dataPresent[i] = data_f3_dataPresent[i];
      }

      std::mem_fn(&eventHelper::updatePlot)(helper, static_cast<real_T>
        (data_f1_f1.size(0)), &(reflect.data())[0], &(nReflect.data())[0],
        &(shiftedData.data())[0], &(nShiftedData.data())[0], &(sldProfiles.data())
        [0], &(nSldProfiles.data())[0], &(layers.data())[0], &(nLayers.data())[0],
        &(ssubs.data())[0], &resample[0], &dataPresent[0], &modelType_data[0]);
      notified = false;

      //  This avoids printing the error message multiple times during the optimization.
    } else if (!notified) {
      fprintf(stderr,
              "\neventManager library coult be loaded. Check that the dynamic library is present in the compile/events folder.\n");
      fflush(stderr);
      notified = true;
    }
  }

  void triggerEvent_init()
  {
    notified = false;
  }
}

// End of code generation (triggerEvent.cpp)
