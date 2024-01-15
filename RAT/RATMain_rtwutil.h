//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// RATMain_rtwutil.h
//
// Code generation for function 'RATMain_rtwutil'
//
#ifndef RATMAIN_RTWUTIL_H
#define RATMAIN_RTWUTIL_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  extern void cast(const ::coder::array<cell_wrap_50, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1);
  extern void cast(const ::coder::array<cell_wrap_24, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1);
  extern void cast(const ::coder::array<cell_wrap_11, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1);
  extern void cast(const ::coder::array<cell_wrap_20, 1U> &r, ::coder::array<
                   cell_wrap_8, 1U> &r1);
  extern void cast(const ::coder::array<cell_wrap_21, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1);
  extern void emlrtFreeThreadStackData();
  extern RATMainTLS *emlrtGetThreadStackData();
  extern void emlrtInitThreadStackData();
  extern real_T rt_hypotd_snf(real_T u0, real_T u1);
  extern real_T rt_powd_snf(real_T u0, real_T u1);
  extern real_T rt_remd_snf(real_T u0, real_T u1);
}

#endif

// End of code generation (RATMain_rtwutil.h)
