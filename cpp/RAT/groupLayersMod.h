//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// groupLayersMod.h
//
// Code generation for function 'groupLayersMod'
//
#ifndef GROUPLAYERSMOD_H
#define GROUPLAYERSMOD_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void groupLayersMod(const ::coder::array<real_T, 2U> &allLayers, real_T
                      allRoughs, const char_T geometry_data[], const int32_T
                      geometry_size[2], real_T bulkIns, real_T bulkOuts, ::coder::
                      array<real_T, 2U> &outLayers, real_T *outSsubs);
}

#endif

// End of code generation (groupLayersMod.h)
