//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processCustomFunction3.cpp
//
// Code generation for function 'processCustomFunction3'
//

// Include files
#include "processCustomFunction3.h"
#include "RATMain_internal_types.h"
#include "RATMain_types.h"
#include "backSort.h"
#include "callCppFunction.h"
#include "rt_nonfinite.h"
#include "str2double.h"
#include "coder_array.h"
#include <cmath>

// Function Declarations
namespace RAT
{
  static void cast(const ::coder::array<cell_wrap_36, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1);
}

// Function Definitions
namespace RAT
{
  static void cast(const ::coder::array<cell_wrap_36, 2U> &r, ::coder::array<
                   cell_wrap_8, 2U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0), 2);
    i = r.size(0) << 1;
    for (int32_T i1{0}; i1 < i; i1++) {
      int32_T loop_ub;
      loop_ub = r[i1].f1.size(1);
      r1[i1].f1.set_size(r[i1].f1.size(0), r[i1].f1.size(1));
      for (int32_T i2{0}; i2 < loop_ub; i2++) {
        int32_T b_loop_ub;
        b_loop_ub = r[i1].f1.size(0);
        for (int32_T i3{0}; i3 < b_loop_ub; i3++) {
          r1[i1].f1[i3 + r1[i1].f1.size(0) * i2] = r[i1].f1[i3 + r[i1].f1.size(0)
            * i2];
        }
      }
    }
  }

  namespace domainsTF
  {
    namespace customXY
    {
      void processCustomFunction(const ::coder::array<real_T, 2U>
        &contrastBackgrounds, const ::coder::array<real_T, 2U> &contrastQzshifts,
        const ::coder::array<real_T, 2U> &contrastScalefactors, const ::coder::
        array<real_T, 2U> &contrastBulkIns, const ::coder::array<real_T, 2U>
        &contrastBulkOuts, const ::coder::array<real_T, 2U> &contrastResolutions,
        const ::coder::array<real_T, 2U> &backgroundParams, const ::coder::array<
        real_T, 2U> &qzshifts, const ::coder::array<real_T, 2U> &scalefactor,
        const ::coder::array<real_T, 2U> &bulkIn, const ::coder::array<real_T,
        2U> &bulkOut, const ::coder::array<real_T, 2U> &res, const ::coder::
        array<real_T, 2U> &cCustFiles, real_T numberOfContrasts, const ::coder::
        array<cell_wrap_1, 2U> &customFiles, const ::coder::array<real_T, 2U>
        &params, ::coder::array<cell_wrap_8, 2U> &allSLDs, ::coder::array<real_T,
        1U> &allRoughs)
      {
        ::coder::array<cell_wrap_36, 2U> tempAllSLDs;
        ::coder::array<real_T, 2U> b_params;
        ::coder::array<real_T, 2U> r;
        real_T a__1;
        real_T a__2;
        real_T a__3;
        real_T a__4;
        real_T a__6;
        real_T thisBulkIn;
        real_T thisBulkOut;
        int32_T i;

        //  Top-level function for processing custom XY profiles for all the
        //  contrasts.
        //  Do some pre-definitions to keep the compiler happy...
        i = static_cast<int32_T>(numberOfContrasts);
        allRoughs.set_size(i);

        //  3 columns to allow for potential imaginary curve
        tempAllSLDs.set_size(i, 2);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          creal_T x;
          real_T d;
          int32_T iv[2];

          //  TODO - the ambition is for parfor here, but would fail for Matlab and Python CM's..
          //  Choose which custom file is associated with this contrast
          //  Find values of 'bulkIn' and 'bulkOut' for this contrast...
          backSort(contrastBackgrounds[b_i], contrastQzshifts[b_i],
                   contrastScalefactors[b_i], contrastBulkIns[b_i],
                   contrastBulkOuts[b_i], contrastResolutions[b_i],
                   backgroundParams, qzshifts, scalefactor, bulkIn, bulkOut, res,
                   &a__1, &a__2, &a__3, &thisBulkIn, &thisBulkOut, &a__4);
          d = cCustFiles[b_i];
          iv[0] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)&customFiles[
                    static_cast<int32_T>(d) - 1].f1)->size())[0];
          iv[1] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)&customFiles[
                    static_cast<int32_T>(d) - 1].f1)->size())[1];
          x = coder::str2double((const char_T *)((::coder::array<char_T, 2U> *)
            &customFiles[static_cast<int32_T>(d) - 1].f1)->data(), iv);
          if ((!std::isnan(x.re)) && (!std::isnan(x.im))) {
            int32_T b_loop_ub;
            int32_T loop_ub;
            b_params.set_size(1, params.size(1));
            loop_ub = params.size(1) - 1;
            for (int32_T i1{0}; i1 <= loop_ub; i1++) {
              b_params[i1] = params[i1];
            }

            iv[0] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)
                      &customFiles[static_cast<int32_T>(d) - 1].f1)->size())[0];
            iv[1] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)
                      &customFiles[static_cast<int32_T>(d) - 1].f1)->size())[1];
            b_callCppFunction(b_params, thisBulkIn, thisBulkOut,
                              static_cast<real_T>(b_i) + 1.0, (const char_T *)((::
              coder::array<char_T, 2U> *)&customFiles[static_cast<int32_T>(d) -
              1].f1)->data(), iv, r, &allRoughs[b_i]);
            loop_ub = r.size(1);
            tempAllSLDs[b_i].f1.set_size(r.size(0), r.size(1));
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = r.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                tempAllSLDs[b_i].f1[i2 + tempAllSLDs[b_i].f1.size(0) * i1] =
                  r[i2 + r.size(0) * i1];
              }
            }

            b_params.set_size(1, params.size(1));
            loop_ub = params.size(1) - 1;
            for (int32_T i1{0}; i1 <= loop_ub; i1++) {
              b_params[i1] = params[i1];
            }

            iv[0] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)
                      &customFiles[static_cast<int32_T>(cCustFiles[b_i]) - 1].f1)
                     ->size())[0];
            iv[1] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)
                      &customFiles[static_cast<int32_T>(cCustFiles[b_i]) - 1].f1)
                     ->size())[1];
            c_callCppFunction(b_params, thisBulkIn, thisBulkOut,
                              static_cast<real_T>(b_i) + 1.0, (const char_T *)((::
              coder::array<char_T, 2U> *)&customFiles[static_cast<int32_T>
              (cCustFiles[b_i]) - 1].f1)->data(), iv, r, &a__6);
            loop_ub = r.size(1);
            tempAllSLDs[b_i + tempAllSLDs.size(0)].f1.set_size(r.size(0), r.size
              (1));
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = r.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                tempAllSLDs[b_i + tempAllSLDs.size(0)].f1[i2 + tempAllSLDs[b_i +
                  tempAllSLDs.size(0)].f1.size(0) * i1] = r[i2 + r.size(0) * i1];
              }
            }
          }
        }

        cast(tempAllSLDs, allSLDs);
      }
    }
  }
}

// End of code generation (processCustomFunction3.cpp)
