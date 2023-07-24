//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation_terminate.cpp
//
// Code generation for function 'reflectivityCalculation_terminate'
//

// Include files
#include "reflectivityCalculation_terminate.h"
#include "reflectivityCalculation_data.h"
#include "rt_nonfinite.h"
#include "omp.h"

// Function Definitions
namespace RAT
{
  void reflectivityCalculation_terminate()
  {
    omp_destroy_nest_lock(&reflectivityCalculation_nestLockGlobal);
  }
}

// End of code generation (reflectivityCalculation_terminate.cpp)
