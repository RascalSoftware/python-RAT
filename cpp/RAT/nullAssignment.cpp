//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// nullAssignment.cpp
//
// Code generation for function 'nullAssignment'
//

// Include files
#include "nullAssignment.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      static void make_bitarray(int32_T n, const ::coder::array<int32_T, 2U>
        &idx, ::coder::array<boolean_T, 2U> &b);
      static int32_T num_true(const ::coder::array<boolean_T, 2U> &b);
    }
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      static void make_bitarray(int32_T n, const ::coder::array<int32_T, 2U>
        &idx, ::coder::array<boolean_T, 2U> &b)
      {
        int32_T i;
        b.set_size(1, n);
        for (i = 0; i < n; i++) {
          b[i] = false;
        }

        i = idx.size(1);
        for (int32_T k{0}; k < i; k++) {
          b[idx[k] - 1] = true;
        }
      }

      static int32_T num_true(const ::coder::array<boolean_T, 2U> &b)
      {
        int32_T i;
        int32_T n;
        n = 0;
        i = b.size(1);
        for (int32_T k{0}; k < i; k++) {
          n += b[k];
        }

        return n;
      }

      void nullAssignment(::coder::array<real_T, 2U> &x, const ::coder::array<
                          int32_T, 2U> &idx)
      {
        ::coder::array<boolean_T, 2U> b;
        int32_T k0;
        int32_T nxin;
        int32_T nxout;
        nxin = x.size(1);
        make_bitarray(x.size(1), idx, b);
        nxout = x.size(1) - num_true(b);
        k0 = -1;
        for (int32_T k{0}; k < nxin; k++) {
          if ((k + 1 > b.size(1)) || (!b[k])) {
            k0++;
            x[k0] = x[k];
          }
        }

        if (nxout < 1) {
          nxout = 0;
        }

        x.set_size(x.size(0), nxout);
      }

      void nullAssignment(real_T x_data[], int32_T x_size[2])
      {
        int32_T i;
        int32_T i1;
        int32_T j;
        int32_T nrows;
        i = x_size[1];
        nrows = x_size[0];
        for (j = 0; j < i; j++) {
          for (int32_T b_i{0}; b_i <= nrows - 2; b_i++) {
            i1 = b_i + x_size[0] * j;
            x_data[i1] = x_data[i1 + 1];
          }
        }

        if (x_size[0] - 1 < 1) {
          nrows = 0;
        } else {
          nrows = x_size[0] - 1;
        }

        j = x_size[1];
        for (i = 0; i < j; i++) {
          for (i1 = 0; i1 < nrows; i1++) {
            x_data[i1 + nrows * i] = x_data[i1 + x_size[0] * i];
          }
        }

        x_size[0] = nrows;
        x_size[1] = j;
      }
    }
  }
}

// End of code generation (nullAssignment.cpp)