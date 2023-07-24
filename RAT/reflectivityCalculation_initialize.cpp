//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// reflectivityCalculation_initialize.cpp
//
// Code generation for function 'reflectivityCalculation_initialize'
//

// Include files
#include "reflectivityCalculation_initialize.h"
#include "reflectivityCalculation_data.h"
#include "rt_nonfinite.h"
#include "triggerEvent.h"
#include "omp.h"

// Function Definitions
namespace RAT
{
  void reflectivityCalculation_initialize()
  {
    omp_init_nest_lock(&reflectivityCalculation_nestLockGlobal);
    helper_not_empty_init();
    triggerEvent_init();
  }
}

// End of code generation (reflectivityCalculation_initialize.cpp)
