//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// timeKeeper.h
//
// Code generation for function 'timeKeeper'
//
#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

// Include files
#include "rtwtypes.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      namespace time
      {
        namespace impl
        {
          void timeKeeper(real_T newTime_tv_sec, real_T newTime_tv_nsec);
          void timeKeeper(real_T *outTime_tv_sec, real_T *outTime_tv_nsec);
        }
      }
    }
  }

  void savedTime_not_empty_init();
}

#endif

// End of code generation (timeKeeper.h)
