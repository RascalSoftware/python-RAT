//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// find.cpp
//
// Code generation for function 'find'
//

// Include files
#include "find.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
                        real_T, 1U> &in2, const ::coder::array<real_T, 1U> &in3)
  {
    ::coder::array<boolean_T, 1U> b_in2;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    b_in2.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      b_in2[i] = (in2[i * stride_0_0] > in3[i * stride_1_0]);
    }

    coder::eml_find(b_in2, in1);
  }

  void binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
                        real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    ::coder::array<boolean_T, 2U> b_in2;
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    if (in3.size(1) == 1) {
      i1 = in2.size(1);
    } else {
      i1 = in3.size(1);
    }

    b_in2.set_size(i, i1);
    stride_0_0 = (in2.size(0) != 1);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_1_1 = (in3.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    if (in3.size(1) == 1) {
      loop_ub = in2.size(1);
    } else {
      loop_ub = in3.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      i1 = in3.size(0);
      if (i1 == 1) {
        b_loop_ub = in2.size(0);
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = (in2[i1 * stride_0_0 + in2.size(0) *
          aux_0_1] > in3[i1 * stride_1_0 + in3.size(0) * aux_1_1]);
      }

      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    coder::c_eml_find(b_in2, in1);
  }

  void c_binary_expand_op(::coder::array<int32_T, 1U> &in1, const ::coder::array<
    real_T, 2U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    ::coder::array<boolean_T, 2U> b_in2;
    int32_T aux_0_1;
    int32_T aux_1_1;
    int32_T i;
    int32_T i1;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_0_1;
    int32_T stride_1_0;
    int32_T stride_1_1;
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    if (in3.size(1) == 1) {
      i1 = in2.size(1);
    } else {
      i1 = in3.size(1);
    }

    b_in2.set_size(i, i1);
    stride_0_0 = (in2.size(0) != 1);
    stride_0_1 = (in2.size(1) != 1);
    stride_1_0 = (in3.size(0) != 1);
    stride_1_1 = (in3.size(1) != 1);
    aux_0_1 = 0;
    aux_1_1 = 0;
    if (in3.size(1) == 1) {
      loop_ub = in2.size(1);
    } else {
      loop_ub = in3.size(1);
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      i1 = in3.size(0);
      if (i1 == 1) {
        b_loop_ub = in2.size(0);
      } else {
        b_loop_ub = i1;
      }

      for (i1 = 0; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = (in2[i1 * stride_0_0 + in2.size(0) *
          aux_0_1] < in3[i1 * stride_1_0 + in3.size(0) * aux_1_1]);
      }

      aux_1_1 += stride_1_1;
      aux_0_1 += stride_0_1;
    }

    coder::c_eml_find(b_in2, in1);
  }

  namespace coder
  {
    void b_eml_find(const boolean_T x[2], int32_T i_data[], int32_T i_size[2])
    {
      int32_T idx;
      int32_T ii;
      boolean_T exitg1;
      idx = 0;
      i_size[0] = 1;
      ii = 0;
      exitg1 = false;
      while ((!exitg1) && (ii < 2)) {
        if (x[ii]) {
          idx++;
          i_data[idx - 1] = ii + 1;
          if (idx >= 2) {
            exitg1 = true;
          } else {
            ii++;
          }
        } else {
          ii++;
        }
      }

      if (idx < 1) {
        i_size[1] = 0;
      } else {
        i_size[1] = idx;
      }
    }

    void c_eml_find(const ::coder::array<boolean_T, 2U> &x, ::coder::array<
                    int32_T, 1U> &i)
    {
      int32_T idx;
      int32_T ii;
      int32_T nx;
      boolean_T exitg1;
      nx = x.size(0) * x.size(1);
      idx = 0;
      i.set_size(nx);
      ii = 0;
      exitg1 = false;
      while ((!exitg1) && (ii <= nx - 1)) {
        if (x[ii]) {
          idx++;
          i[idx - 1] = ii + 1;
          if (idx >= nx) {
            exitg1 = true;
          } else {
            ii++;
          }
        } else {
          ii++;
        }
      }

      if (nx == 1) {
        if (idx == 0) {
          i.set_size(0);
        }
      } else {
        if (idx < 1) {
          idx = 0;
        }

        i.set_size(idx);
      }
    }

    void d_eml_find(const ::coder::array<boolean_T, 2U> &x, ::coder::array<
                    int32_T, 2U> &i)
    {
      int32_T idx;
      int32_T ii;
      int32_T nx;
      boolean_T exitg1;
      nx = x.size(1);
      idx = 0;
      i.set_size(1, x.size(1));
      ii = 0;
      exitg1 = false;
      while ((!exitg1) && (ii <= nx - 1)) {
        if (x[ii]) {
          idx++;
          i[idx - 1] = ii + 1;
          if (idx >= nx) {
            exitg1 = true;
          } else {
            ii++;
          }
        } else {
          ii++;
        }
      }

      if (x.size(1) == 1) {
        if (idx == 0) {
          i.set_size(1, 0);
        }
      } else {
        if (idx < 1) {
          idx = 0;
        }

        i.set_size(i.size(0), idx);
      }
    }

    void eml_find(const ::coder::array<boolean_T, 1U> &x, ::coder::array<int32_T,
                  1U> &i)
    {
      int32_T idx;
      int32_T ii;
      int32_T nx;
      boolean_T exitg1;
      nx = x.size(0);
      idx = 0;
      i.set_size(x.size(0));
      ii = 0;
      exitg1 = false;
      while ((!exitg1) && (ii <= nx - 1)) {
        if (x[ii]) {
          idx++;
          i[idx - 1] = ii + 1;
          if (idx >= nx) {
            exitg1 = true;
          } else {
            ii++;
          }
        } else {
          ii++;
        }
      }

      if (x.size(0) == 1) {
        if (idx == 0) {
          i.set_size(0);
        }
      } else {
        if (idx < 1) {
          idx = 0;
        }

        i.set_size(idx);
      }
    }
  }
}

// End of code generation (find.cpp)
