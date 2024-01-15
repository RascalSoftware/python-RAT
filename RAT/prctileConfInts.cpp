//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// prctileConfInts.cpp
//
// Code generation for function 'prctileConfInts'
//

// Include files
#include "prctileConfInts.h"
#include "mean.h"
#include "prctile.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void prctileConfInts(const ::coder::array<real_T, 2U> &chain, ::coder::array<
                       real_T, 2U> &parConfInts_par95, ::coder::array<real_T, 2U>
                       &parConfInts_par65, ::coder::array<real_T, 2U>
                       &parConfInts_mean)
  {
    coder::prctile(chain, parConfInts_par95);
    coder::b_prctile(chain, parConfInts_par65);
    coder::mean(chain, parConfInts_mean);
  }
}

// End of code generation (prctileConfInts.cpp)
