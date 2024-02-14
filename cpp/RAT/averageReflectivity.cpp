//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// averageReflectivity.cpp
//
// Code generation for function 'averageReflectivity'
//

// Include files
#include "averageReflectivity.h"
#include "cat.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  namespace domainsTF
  {
    void averageReflectivity(const ::coder::array<real_T, 2U> &reflect1, const ::
      coder::array<real_T, 2U> &reflect2, const ::coder::array<real_T, 2U>
      &simul1, const ::coder::array<real_T, 2U> &simul2, real_T domainRatio, ::
      coder::array<real_T, 2U> &totReflect, ::coder::array<real_T, 2U> &totSimul)
    {
      int32_T loop_ub;

      //  Calculates the avereaged reflectivity for domains samples (incoherent
      //  sum)
      if (reflect1.size(0) == reflect2.size(0)) {
        totReflect.set_size(reflect1.size(0), 2);
        loop_ub = reflect1.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          totReflect[i] = reflect1[i];
        }

        loop_ub = reflect1.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          totReflect[i + totReflect.size(0)] = domainRatio * reflect1[i +
            reflect1.size(0)] + (1.0 - domainRatio) * reflect2[i + reflect2.size
            (0)];
        }
      } else {
        binary_expand_op(totReflect, reflect1, domainRatio, reflect2);
      }

      if (simul1.size(0) == simul2.size(0)) {
        totSimul.set_size(simul1.size(0), 2);
        loop_ub = simul1.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          totSimul[i] = simul1[i];
        }

        loop_ub = simul1.size(0);
        for (int32_T i{0}; i < loop_ub; i++) {
          totSimul[i + totSimul.size(0)] = domainRatio * simul1[i + simul1.size
            (0)] + (1.0 - domainRatio) * simul2[i + simul2.size(0)];
        }
      } else {
        binary_expand_op(totSimul, simul1, domainRatio, simul2);
      }
    }
  }
}

// End of code generation (averageReflectivity.cpp)
