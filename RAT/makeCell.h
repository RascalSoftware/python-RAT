//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// makeCell.h
//
// Code generation for function 'makeCell'
//
#ifndef MAKECELL_H
#define MAKECELL_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void b_makeCell(real_T m, const real_T vals_data[], ::coder::array<cell_wrap_8,
                  2U> &x);
  void makeCell(cell_wrap_8 x[50]);
  void makeCell(real_T m, const real_T vals_data[], ::coder::array<cell_wrap_24,
                1U> &x);
  void makeCell(real_T m, const real_T vals_data[], ::coder::array<cell_wrap_8,
                2U> &x);
}

#endif

// End of code generation (makeCell.h)
