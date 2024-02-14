//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// shiftData.cpp
//
// Code generation for function 'shiftData'
//

// Include files
#include "shiftData.h"
#include "find.h"
#include "linspace.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void shiftData(real_T scalefactor, real_T qzshift, real_T dataPresent, ::coder::
                 array<real_T, 2U> &data, const real_T dataLimits[2], const
                 real_T simLimits[2], ::coder::array<real_T, 2U> &shiftedData)
  {
    ::coder::array<real_T, 1U> b_data;
    ::coder::array<int32_T, 1U> b_i;
    ::coder::array<boolean_T, 1U> b_data_data;
    ::coder::array<boolean_T, 1U> c_data_data;
    boolean_T data_data[10000];

    //  Shifts the data according to scale factor. If there is no data, makes
    //  x-data over the simulation range.
    //
    //  INPUTS:
    //      * scalefactor: problemStruct.scalefactors
    //      * qzshift: problemStruct.qzhifts
    //      * dataPresent: problemStruct.dataPresent
    //      * data: problemStruct.data
    //      * dataLimits: problemStruct.dataLimits
    //      * simLimits: problemStruct.simLimits
    //
    //  OUTPUTS:
    //      * shiftedData: Data shifted using given scale factor
    if (static_cast<int32_T>(dataPresent) == 1) {
      int32_T data_size;
      int32_T hiIndex;
      int32_T i;
      int32_T i1;
      int32_T loop_ub;
      int32_T lowIndex;
      if (scalefactor == 0.0) {
        scalefactor = 1.0E-30;
      }

      b_data.set_size(data.size(0));
      loop_ub = data.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_data[i] = data[i] + qzshift;
      }

      loop_ub = b_data.size(0);
      for (i = 0; i < loop_ub; i++) {
        data[i] = b_data[i];
      }

      b_data.set_size(data.size(0));
      loop_ub = data.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_data[i] = data[i + data.size(0)] / scalefactor;
      }

      loop_ub = b_data.size(0);
      for (i = 0; i < loop_ub; i++) {
        data[i + data.size(0)] = b_data[i];
      }

      b_data.set_size(data.size(0));
      loop_ub = data.size(0);
      for (i = 0; i < loop_ub; i++) {
        b_data[i] = data[i + data.size(0) * 2] / scalefactor;
      }

      loop_ub = b_data.size(0);
      for (i = 0; i < loop_ub; i++) {
        data[i + data.size(0) * 2] = b_data[i];
      }

      data_size = data.size(0);
      loop_ub = data.size(0);
      for (i = 0; i < loop_ub; i++) {
        data_data[i] = (data[i] < dataLimits[0]);
      }

      b_data_data.set(&data_data[0], data_size);
      coder::eml_find(b_data_data, b_i);
      if (b_i.size(0) != 0) {
        lowIndex = b_i[b_i.size(0) - 1];
      } else {
        lowIndex = 1;
      }

      data_size = data.size(0);
      loop_ub = data.size(0);
      for (i = 0; i < loop_ub; i++) {
        data_data[i] = (data[i] > dataLimits[1]);
      }

      c_data_data.set(&data_data[0], data_size);
      coder::eml_find(c_data_data, b_i);
      if (b_i.size(0) != 0) {
        hiIndex = b_i[0];
      } else {
        hiIndex = data.size(0);
      }

      if (lowIndex > hiIndex) {
        i = 0;
        i1 = 0;
      } else {
        i = lowIndex - 1;
        i1 = hiIndex;
      }

      loop_ub = i1 - i;
      shiftedData.set_size(loop_ub, data.size(1));
      data_size = data.size(1);
      for (i1 = 0; i1 < data_size; i1++) {
        for (int32_T i2{0}; i2 < loop_ub; i2++) {
          shiftedData[i2 + shiftedData.size(0) * i1] = data[(i + i2) + data.size
            (0) * i1];
        }
      }
    } else {
      real_T b_dv1[3][500];
      real_T b_dv[500];
      coder::linspace(simLimits[0], simLimits[1], b_dv);
      for (int32_T i{0}; i < 500; i++) {
        b_dv1[0][i] = b_dv[i];
        b_dv1[1][i] = 0.0;
        b_dv1[2][i] = 0.0;
      }

      shiftedData.set_size(500, 3);
      for (int32_T i{0}; i < 3; i++) {
        for (int32_T i1{0}; i1 < 500; i1++) {
          shiftedData[i1 + shiftedData.size(0) * i] = b_dv1[i][i1];
        }
      }
    }
  }
}

// End of code generation (shiftData.cpp)
