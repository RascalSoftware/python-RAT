//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// validate_print_arguments.cpp
//
// Code generation for function 'validate_print_arguments'
//

// Include files
#include "validate_print_arguments.h"
#include "rt_nonfinite.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      void validate_print_arguments(real_T varargin_1, real_T varargin_2, real_T
        varargin_3, real_T varargin_4, real_T varargin_5, real_T
        validatedArguments[5])
      {
        validatedArguments[0] = varargin_1;
        validatedArguments[1] = varargin_2;
        validatedArguments[2] = varargin_3;
        validatedArguments[3] = varargin_4;
        validatedArguments[4] = varargin_5;
      }

      void validate_print_arguments(int32_T varargin_1, int32_T varargin_2,
        int32_T varargin_3, int32_T validatedArguments[3])
      {
        validatedArguments[0] = varargin_1;
        validatedArguments[1] = varargin_2;
        validatedArguments[2] = varargin_3;
      }
    }
  }
}

// End of code generation (validate_print_arguments.cpp)
