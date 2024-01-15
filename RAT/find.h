//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// find.h
//
// Code generation for function 'find'
//
#ifndef FIND_H
#define FIND_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Function Declarations
namespace RAT
{
  void binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
                        real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3);
  void binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
                        real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3);
  void c_binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
    real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3);
  namespace coder
  {
    void b_eml_find(const boolean_T x[2], int32_T i_data[], int32_T i_size[2]);
    void c_eml_find(const ::coder::array<boolean_T, 2U> &x, ::coder::array<
                    int32_T, 1U> &i);
    void d_eml_find(const ::coder::array<boolean_T, 2U> &x, ::coder::array<
                    int32_T, 2U> &i);
    void eml_find(const ::coder::array<boolean_T, 1U> &x, ::coder::array<int32_T,
                  1U> &i);
  }
}

#endif

// End of code generation (find.h)
