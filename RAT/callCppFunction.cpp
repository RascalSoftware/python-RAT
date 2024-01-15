//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// callCppFunction.cpp
//
// Code generation for function 'callCppFunction'
//

// Include files
#include "callCppFunction.h"
#include "rt_nonfinite.h"
#include "classHandle.hpp"
#include "coder_array.h"
#include <algorithm>
#include <functional>

// Function Definitions
namespace RAT
{
  void b_callCppFunction(::coder::array<real_T, 2U> &params, real_T bulkIn,
    real_T bulkOut, real_T contrast, const char_T pointer_data[], const int32_T
    pointer_size[2], ::coder::array<real_T, 2U> &output, real_T *subRough)
  {
    static real_T tempOutput[6000];
    static real_T tempOutput_data[6000];
    CallbackInterface * callback;
    ClassHandle<CallbackInterface> * callbackHandle;
    real_T outputSize[2];
    real_T size;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    char_T b_pointer_data[10000];
    loop_ub = pointer_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&pointer_data[0], &pointer_data[loop_ub], &b_pointer_data[0]);
    }

    callbackHandle = convertString2HandlePtr<CallbackInterface>(&b_pointer_data
      [0]);
    callback = std::mem_fn(&ClassHandle<CallbackInterface>::ptr)(callbackHandle);

    //  This is not ideal, it needs to be fixed
    //  domain should either before 0 or 1. A value less than zero indicates no domains
    std::mem_fn<void(double*, double*, double*, int, int, double*, double*,
                     double*)>(&CallbackInterface::invoke)(callback, &params[0],
      &bulkIn, &bulkOut, contrast, 0.0, &tempOutput[0], &outputSize[0], subRough);
    size = outputSize[0] * outputSize[1];
    if (size < 1.0) {
      loop_ub = 0;
    } else {
      loop_ub = static_cast<int32_T>(size);
    }

    if (loop_ub - 1 >= 0) {
      std::copy(&tempOutput[0], &tempOutput[loop_ub], &tempOutput_data[0]);
    }

    loop_ub_tmp = static_cast<int32_T>(outputSize[1]);
    loop_ub = static_cast<int32_T>(outputSize[0]);
    output.set_size(loop_ub, loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        output[i1 + output.size(0) * i] = tempOutput_data[i + loop_ub_tmp * i1];
      }
    }
  }

  void c_callCppFunction(::coder::array<real_T, 2U> &params, real_T bulkIn,
    real_T bulkOut, real_T contrast, const char_T pointer_data[], const int32_T
    pointer_size[2], ::coder::array<real_T, 2U> &output, real_T *subRough)
  {
    static real_T tempOutput[6000];
    static real_T tempOutput_data[6000];
    CallbackInterface * callback;
    ClassHandle<CallbackInterface> * callbackHandle;
    real_T outputSize[2];
    real_T size;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    char_T b_pointer_data[10000];
    loop_ub = pointer_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&pointer_data[0], &pointer_data[loop_ub], &b_pointer_data[0]);
    }

    callbackHandle = convertString2HandlePtr<CallbackInterface>(&b_pointer_data
      [0]);
    callback = std::mem_fn(&ClassHandle<CallbackInterface>::ptr)(callbackHandle);

    //  This is not ideal, it needs to be fixed
    //  domain should either before 0 or 1. A value less than zero indicates no domains
    std::mem_fn<void(double*, double*, double*, int, int, double*, double*,
                     double*)>(&CallbackInterface::invoke)(callback, &params[0],
      &bulkIn, &bulkOut, contrast, 1.0, &tempOutput[0], &outputSize[0], subRough);
    size = outputSize[0] * outputSize[1];
    if (size < 1.0) {
      loop_ub = 0;
    } else {
      loop_ub = static_cast<int32_T>(size);
    }

    if (loop_ub - 1 >= 0) {
      std::copy(&tempOutput[0], &tempOutput[loop_ub], &tempOutput_data[0]);
    }

    loop_ub_tmp = static_cast<int32_T>(outputSize[1]);
    loop_ub = static_cast<int32_T>(outputSize[0]);
    output.set_size(loop_ub, loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        output[i1 + output.size(0) * i] = tempOutput_data[i + loop_ub_tmp * i1];
      }
    }
  }

  void callCppFunction(::coder::array<real_T, 2U> &params, real_T bulkIn, real_T
                       bulkOut, real_T contrast, const char_T pointer_data[],
                       const int32_T pointer_size[2], ::coder::array<real_T, 2U>
                       &output, real_T *subRough)
  {
    static real_T tempOutput[6000];
    static real_T tempOutput_data[6000];
    CallbackInterface * callback;
    ClassHandle<CallbackInterface> * callbackHandle;
    real_T outputSize[2];
    real_T size;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    char_T b_pointer_data[10000];
    loop_ub = pointer_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&pointer_data[0], &pointer_data[loop_ub], &b_pointer_data[0]);
    }

    callbackHandle = convertString2HandlePtr<CallbackInterface>(&b_pointer_data
      [0]);
    callback = std::mem_fn(&ClassHandle<CallbackInterface>::ptr)(callbackHandle);

    //  This is not ideal, it needs to be fixed
    //  domain should either before 0 or 1. A value less than zero indicates no domains
    std::mem_fn<void(double*, double*, double*, int, double*, double*, double*)>
      (&CallbackInterface::invoke)(callback, &params[0], &bulkIn, &bulkOut,
      contrast, &tempOutput[0], &outputSize[0], subRough);
    size = outputSize[0] * outputSize[1];
    if (size < 1.0) {
      loop_ub = 0;
    } else {
      loop_ub = static_cast<int32_T>(size);
    }

    if (loop_ub - 1 >= 0) {
      std::copy(&tempOutput[0], &tempOutput[loop_ub], &tempOutput_data[0]);
    }

    loop_ub_tmp = static_cast<int32_T>(outputSize[1]);
    loop_ub = static_cast<int32_T>(outputSize[0]);
    output.set_size(loop_ub, loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        output[i1 + output.size(0) * i] = tempOutput_data[i + loop_ub_tmp * i1];
      }
    }
  }
}

// End of code generation (callCppFunction.cpp)
