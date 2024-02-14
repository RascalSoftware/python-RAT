//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// eml_setop.cpp
//
// Code generation for function 'eml_setop'
//

// Include files
#include "eml_setop.h"
#include "relop.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Declarations
namespace RAT
{
  namespace coder
  {
    static real_T skip_to_last_equal_value(int32_T *k, const ::coder::array<
      real_T, 2U> &x);
  }
}

// Function Definitions
namespace RAT
{
  namespace coder
  {
    static real_T skip_to_last_equal_value(int32_T *k, const ::coder::array<
      real_T, 2U> &x)
    {
      real_T xk;
      xk = x[*k - 1];
      while ((*k < x.size(1)) && (x[*k] == xk)) {
        (*k)++;
      }

      return xk;
    }

    void do_vectors(const ::coder::array<real_T, 2U> &a, real_T b, ::coder::
                    array<real_T, 2U> &c, ::coder::array<int32_T, 1U> &ia,
                    int32_T *ib_size)
    {
      real_T ak;
      int32_T b_ialast;
      int32_T iafirst;
      int32_T ialast;
      int32_T iblast;
      int32_T na;
      int32_T nc;
      int32_T nia;
      na = a.size(1);
      c.set_size(1, a.size(1));
      ia.set_size(a.size(1));
      *ib_size = 0;
      nc = 0;
      nia = 0;
      iafirst = 0;
      ialast = 1;
      iblast = 1;
      while ((ialast <= na) && (iblast <= 1)) {
        b_ialast = ialast;
        ak = skip_to_last_equal_value(&b_ialast, a);
        ialast = b_ialast;
        if (ak == b) {
          ialast = b_ialast + 1;
          iafirst = b_ialast;
          iblast = 2;
        } else if (internal::c_relop(ak, b)) {
          nc++;
          nia++;
          c[nc - 1] = ak;
          ia[nia - 1] = iafirst + 1;
          ialast = b_ialast + 1;
          iafirst = b_ialast;
        } else {
          iblast = 2;
        }
      }

      while (ialast <= na) {
        iblast = ialast;
        ak = skip_to_last_equal_value(&iblast, a);
        nc++;
        nia++;
        c[nc - 1] = ak;
        ia[nia - 1] = iafirst + 1;
        ialast = iblast + 1;
        iafirst = iblast;
      }

      if (a.size(1) > 0) {
        if (nia < 1) {
          nia = 0;
        }

        ia.set_size(nia);
        if (nc < 1) {
          nc = 0;
        }

        c.set_size(c.size(0), nc);
      }
    }
  }
}

// End of code generation (eml_setop.cpp)
