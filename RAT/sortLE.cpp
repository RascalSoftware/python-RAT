//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// sortLE.cpp
//
// Code generation for function 'sortLE'
//

// Include files
#include "sortLE.h"
#include "rt_nonfinite.h"
#include "sortAscendLE.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      boolean_T sortLE(const ::coder::array<real_T, 2U> &v, const int32_T
                       dir_data[], int32_T idx1, int32_T idx2)
      {
        int32_T k;
        boolean_T exitg1;
        boolean_T p;
        p = true;
        k = 0;
        exitg1 = false;
        while ((!exitg1) && (k < 2)) {
          real_T v1;
          real_T v2;
          v1 = v[(idx1 + v.size(0) * (dir_data[k] - 1)) - 1];
          v2 = v[(idx2 + v.size(0) * (dir_data[k] - 1)) - 1];
          if ((v1 == v2) || (std::isnan(v1) && std::isnan(v2))) {
            k++;
          } else {
            p = sortAscendLE(v1, v2);
            exitg1 = true;
          }
        }

        return p;
      }
    }
  }
}

// End of code generation (sortLE.cpp)
