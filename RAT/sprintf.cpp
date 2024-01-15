//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// sprintf.cpp
//
// Code generation for function 'sprintf'
//

// Include files
#include "sprintf.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include <cstddef>
#include <stdio.h>

// Function Definitions
namespace RAT
{
  namespace coder
  {
    void b_sprintf(real_T varargin_1, real_T varargin_2, ::coder::array<char_T,
                   2U> &str)
    {
      int32_T nbytes;
      nbytes = snprintf(nullptr, 0,
                        "Exiting - X satisfies termination criteria: TolX %e, TolF %e",
                        varargin_1, varargin_2);
      str.set_size(1, nbytes + 1);
      snprintf(&str[0], (size_t)(nbytes + 1),
               "Exiting - X satisfies termination criteria: TolX %e, TolF %e",
               varargin_1, varargin_2);
      if (nbytes < 1) {
        nbytes = 0;
      }

      str.set_size(str.size(0), nbytes);
    }

    void snPrint(real_T varargin_2, const ::coder::array<char_T, 2U> &varargin_3,
                 ::coder::array<char_T, 2U> &str)
    {
      ::coder::array<char_T, 2U> b_varargin_3;
      int32_T loop_ub;
      int32_T nbytes;
      b_varargin_3.set_size(1, varargin_3.size(1));
      loop_ub = varargin_3.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        b_varargin_3[i] = varargin_3[i];
      }

      nbytes = snprintf(nullptr, 0, "\n %s %5.1f%% %s", "DREAM: ", varargin_2,
                        &b_varargin_3[0]);
      str.set_size(1, nbytes + 1);
      b_varargin_3.set_size(1, varargin_3.size(1));
      loop_ub = varargin_3.size(1);
      for (int32_T i{0}; i < loop_ub; i++) {
        b_varargin_3[i] = varargin_3[i];
      }

      snprintf(&str[0], (size_t)(nbytes + 1), "\n %s %5.1f%% %s", "DREAM: ",
               varargin_2, &b_varargin_3[0]);
      if (nbytes < 1) {
        nbytes = 0;
      }

      str.set_size(str.size(0), nbytes);
    }
  }
}

// End of code generation (sprintf.cpp)
