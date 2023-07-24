//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// callCppFunc.cpp
//
// Code generation for function 'callCppFunc'
//

// Include files
#include "callCppFunc.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "libManager.h"
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <tuple>
#include <vector>

// Function Definitions
namespace RAT
{
  void callCppFunc(::coder::array<real_T, 2U> &params, real_T nba, real_T nbs,
                   real_T numberOfContrasts, const char_T libraryName_data[],
                   const int32_T libraryName_size[2], const char_T
                   functionName_data[], const int32_T functionName_size[2],
                   real_T output_data[], int32_T output_size[2], real_T
                   *subRough)
  {
    Library p;
    real_T tempOutput[1000][3];
    real_T nLayers;
    int32_T loop_ub;
    char_T b_functionName_data[10001];
    char_T b_libraryName_data[10001];

    //  Use 3rd party 'dylib.hpp' to implement a cross-platform Cpp runtime
    //  class loader.... https://github.com/martin-olivier/dylib
    //  coder.varsize('output',[1000 1],[1 0]);
    //  Make an instance
    p = Library();

    //  We need to add a null terminator to the library names in order for
    //  them to match C/C++ format. See....
    //  https://uk.mathworks.com/help/coder/ug/c-strings-for-null-terminated-matlab-strings.html
    loop_ub = libraryName_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&libraryName_data[0], &libraryName_data[loop_ub],
                &b_libraryName_data[0]);
    }

    b_libraryName_data[libraryName_size[1]] = '\x00';
    loop_ub = functionName_size[1];
    if (loop_ub - 1 >= 0) {
      std::copy(&functionName_data[0], &functionName_data[loop_ub],
                &b_functionName_data[0]);
    }

    b_functionName_data[functionName_size[1]] = '\x00';

    //  call the class method in libManager.h
    std::mem_fn(&Library::loadRunner)(p, &params[0], &nba, &nbs,
      numberOfContrasts, &tempOutput[0][0], subRough, &nLayers,
      &b_libraryName_data[0], &b_functionName_data[0]);
    if (nLayers < 1.0) {
      loop_ub = 0;
    } else {
      loop_ub = static_cast<int32_T>(nLayers);
    }

    output_size[0] = loop_ub;
    output_size[1] = 3;
    for (int32_T i{0}; i < 3; i++) {
      for (int32_T i1{0}; i1 < loop_ub; i1++) {
        output_data[i1 + loop_ub * i] = tempOutput[i1][i];
      }
    }
  }
}

// End of code generation (callCppFunc.cpp)
