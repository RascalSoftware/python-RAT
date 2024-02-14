//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// applyBackgroundCorrection.cpp
//
// Code generation for function 'applyBackgroundCorrection'
//

// Include files
#include "applyBackgroundCorrection.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void applyBackgroundCorrection(::coder::array<real_T, 2U> &reflect, ::coder::
    array<real_T, 2U> &simul, ::coder::array<real_T, 2U> &shiftedData, real_T
    backgroundParams, real_T contrastBackgroundsType)
  {
    ::coder::array<real_T, 1U> b_reflect;
    switch (static_cast<int32_T>(contrastBackgroundsType)) {
     case 1:
      {
        int32_T loop_ub;

        // Add background to the simulation
        b_reflect.set_size(reflect.size(0));
        loop_ub = reflect.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          b_reflect[i] = reflect[i + reflect.size(0)] + backgroundParams;
        }

        loop_ub = b_reflect.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          reflect[i + reflect.size(0)] = b_reflect[i];
        }

        b_reflect.set_size(simul.size(0));
        loop_ub = simul.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          b_reflect[i] = simul[i + simul.size(0)] + backgroundParams;
        }

        loop_ub = b_reflect.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          simul[i + simul.size(0)] = b_reflect[i];
        }
      }
      break;

     case 2:
      {
        int32_T loop_ub;

        // Subtract the background from the data..
        b_reflect.set_size(shiftedData.size(0));
        loop_ub = shiftedData.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          b_reflect[i] = shiftedData[i + shiftedData.size(0)] - backgroundParams;
        }

        loop_ub = b_reflect.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          shiftedData[i + shiftedData.size(0)] = b_reflect[i];
        }

        // shiftedData(:,3) = shiftedData(:,3) - backgroundParams;
      }
      break;
    }
  }
}

// End of code generation (applyBackgroundCorrection.cpp)
