//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// kmeans.h
//
// Code generation for function 'kmeans'
//
#ifndef KMEANS_H
#define KMEANS_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void kmeans(const ::coder::array<real_T, 2U> &X, ::coder::array<real_T, 2U>
              &means, real_T Nmeans[2]);
}

#endif

// End of code generation (kmeans.h)