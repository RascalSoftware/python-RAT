//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nullAssignment.cpp
//
// Code generation for function 'nullAssignment'
//

// Include files
#include "nullAssignment.h"
#include "rt_nonfinite.h"

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      void nullAssignment(real_T x_data[], int32_T x_size[2])
      {
        int32_T i;
        int32_T i1;
        int32_T j;
        int32_T nrows;
        i = x_size[1];
        nrows = x_size[0];
        for (j = 0; j < i; j++) {
          for (int32_T b_i{0}; b_i <= nrows - 2; b_i++) {
            i1 = b_i + x_size[0] * j;
            x_data[i1] = x_data[i1 + 1];
          }
        }

        if (x_size[0] - 1 < 1) {
          nrows = 0;
        } else {
          nrows = x_size[0] - 1;
        }

        j = x_size[1];
        for (i = 0; i < j; i++) {
          for (i1 = 0; i1 < nrows; i1++) {
            x_data[i1 + nrows * i] = x_data[i1 + x_size[0] * i];
          }
        }

        x_size[0] = nrows;
        x_size[1] = j;
      }
    }
  }
}

// End of code generation (nullAssignment.cpp)
