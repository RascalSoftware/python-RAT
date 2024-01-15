//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processCustomFunction2.cpp
//
// Code generation for function 'processCustomFunction2'
//

// Include files
#include "processCustomFunction2.h"
#include "RATMain_types.h"
#include "applyHydrationImag.h"
#include "applyHydrationReal.h"
#include "backSort.h"
#include "callCppFunction.h"
#include "rt_nonfinite.h"
#include "str2double.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  namespace domainsTF
  {
    namespace customLayers
    {
      void processCustomFunction(const ::coder::array<real_T, 2U>
        &contrastBackgrounds, const ::coder::array<real_T, 2U> &contrastQzshifts,
        const ::coder::array<real_T, 2U> &contrastScalefactors, const ::coder::
        array<real_T, 2U> &contrastBulkIns, const ::coder::array<real_T, 2U>
        &contrastBulkOuts, const ::coder::array<real_T, 2U> &contrastResolutions,
        const ::coder::array<real_T, 2U> &backgroundParams, const ::coder::array<
        real_T, 2U> &qzshifts, const ::coder::array<real_T, 2U> &scalefactor,
        const ::coder::array<real_T, 2U> &bulkIn, const ::coder::array<real_T,
        2U> &bulkOut, const ::coder::array<real_T, 2U> &resolutionParams, const ::
        coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts, const ::
        coder::array<cell_wrap_1, 2U> &customFiles, const ::coder::array<real_T,
        2U> &params, boolean_T useImaginary, ::coder::array<cell_wrap_21, 2U>
        &allLayers, ::coder::array<real_T, 1U> &allRoughs)
      {
        ::coder::array<real_T, 2U> b_params;
        ::coder::array<real_T, 2U> b_thisContrastLayers1;
        ::coder::array<real_T, 2U> c_thisContrastLayers1;
        ::coder::array<real_T, 2U> thisContrastLayers1;
        ::coder::array<real_T, 2U> thisContrastLayers2;
        real_T a__1;
        real_T a__2;
        real_T a__3;
        real_T a__4;
        real_T a__6;
        real_T thisBulkIn;
        real_T thisBulkOut;
        int32_T i;

        //  Top-level function for processing custom layers for all the
        //  contrasts.
        //  Do some pre-definitions to keep the compiler happy...
        // totNumCalcs = numberOfContrasts * 2;
        i = static_cast<int32_T>(numberOfContrasts);
        allRoughs.set_size(i);
        allLayers.set_size(i, 2);
        for (int32_T b_i{0}; b_i < i; b_i++) {
          creal_T x;
          real_T d;
          int32_T iv[2];
          int32_T b_loop_ub;
          int32_T loop_ub;

          //  Choose which custom file is associated with this contrast
          //  Find values of 'bulkIn' and 'bulkOut' for this
          //  contrast...
          backSort(contrastBackgrounds[b_i], contrastQzshifts[b_i],
                   contrastScalefactors[b_i], contrastBulkIns[b_i],
                   contrastBulkOuts[b_i], contrastResolutions[b_i],
                   backgroundParams, qzshifts, scalefactor, bulkIn, bulkOut,
                   resolutionParams, &a__1, &a__2, &a__3, &thisBulkIn,
                   &thisBulkOut, &a__4);

          //  typeDef
          d = cCustFiles[b_i];
          iv[0] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)&customFiles[
                    static_cast<int32_T>(d) - 1].f1)->size())[0];
          iv[1] = (*(int32_T (*)[2])((::coder::array<char_T, 2U> *)&customFiles[
                    static_cast<int32_T>(d) - 1].f1)->size())[1];
          x = coder::str2double((const char_T *)((::coder::array<char_T, 2U> *)
            &customFiles[static_cast<int32_T>(d) - 1].f1)->data(), iv);
          if ((!std::isnan(x.re)) && (!std::isnan(x.im))) {
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
              1].f1)->data(), iv, c_thisContrastLayers1, &allRoughs[b_i]);
            loop_ub = c_thisContrastLayers1.size(1);
            b_thisContrastLayers1.set_size(c_thisContrastLayers1.size(0),
              c_thisContrastLayers1.size(1));
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = c_thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1] =
                  c_thisContrastLayers1[i2 + c_thisContrastLayers1.size(0) * i1];
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
              (cCustFiles[b_i]) - 1].f1)->data(), iv, c_thisContrastLayers1,
                              &a__6);
            loop_ub = c_thisContrastLayers1.size(1);
            thisContrastLayers2.set_size(c_thisContrastLayers1.size(0),
              c_thisContrastLayers1.size(1));
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = c_thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers2[i2 + thisContrastLayers2.size(0) * i1] =
                  c_thisContrastLayers1[i2 + c_thisContrastLayers1.size(0) * i1];
              }
            }
          }

          //  If the output layers has 5 columns, then we need to do
          //  the hydration correction (the user has not done it in the
          //  custom function). Do that here....
          if (!useImaginary) {
            thisContrastLayers1.set_size(b_thisContrastLayers1.size(0),
              b_thisContrastLayers1.size(1));
            loop_ub = b_thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = b_thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1] =
                  b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1];
              }
            }

            applyHydrationReal(thisContrastLayers1, thisBulkIn, thisBulkOut);
            b_thisContrastLayers1.set_size(thisContrastLayers1.size(0),
              thisContrastLayers1.size(1));
            loop_ub = thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1] =
                  thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1];
              }
            }

            thisContrastLayers1.set_size(thisContrastLayers2.size(0),
              thisContrastLayers2.size(1));
            loop_ub = thisContrastLayers2.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers2.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1] =
                  thisContrastLayers2[i2 + thisContrastLayers2.size(0) * i1];
              }
            }

            applyHydrationReal(thisContrastLayers1, thisBulkIn, thisBulkOut);
            thisContrastLayers2.set_size(thisContrastLayers1.size(0),
              thisContrastLayers1.size(1));
            loop_ub = thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers2[i2 + thisContrastLayers2.size(0) * i1] =
                  thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1];
              }
            }
          } else {
            thisContrastLayers1.set_size(b_thisContrastLayers1.size(0),
              b_thisContrastLayers1.size(1));
            loop_ub = b_thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = b_thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1] =
                  b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1];
              }
            }

            applyHydrationImag(thisContrastLayers1, thisBulkIn, thisBulkOut);
            b_thisContrastLayers1.set_size(thisContrastLayers1.size(0),
              thisContrastLayers1.size(1));
            loop_ub = thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1] =
                  thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1];
              }
            }

            thisContrastLayers1.set_size(thisContrastLayers2.size(0),
              thisContrastLayers2.size(1));
            loop_ub = thisContrastLayers2.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers2.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1] =
                  thisContrastLayers2[i2 + thisContrastLayers2.size(0) * i1];
              }
            }

            applyHydrationImag(thisContrastLayers1, thisBulkIn, thisBulkOut);
            thisContrastLayers2.set_size(thisContrastLayers1.size(0),
              thisContrastLayers1.size(1));
            loop_ub = thisContrastLayers1.size(1);
            for (int32_T i1{0}; i1 < loop_ub; i1++) {
              b_loop_ub = thisContrastLayers1.size(0);
              for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
                thisContrastLayers2[i2 + thisContrastLayers2.size(0) * i1] =
                  thisContrastLayers1[i2 + thisContrastLayers1.size(0) * i1];
              }
            }
          }

          allLayers[b_i].f1.set_size(b_thisContrastLayers1.size(0),
            b_thisContrastLayers1.size(1));
          loop_ub = b_thisContrastLayers1.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = b_thisContrastLayers1.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1] =
                b_thisContrastLayers1[i2 + b_thisContrastLayers1.size(0) * i1];
            }
          }

          allLayers[b_i + allLayers.size(0)].f1.set_size
            (thisContrastLayers2.size(0), thisContrastLayers2.size(1));
          loop_ub = thisContrastLayers2.size(1);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            b_loop_ub = thisContrastLayers2.size(0);
            for (int32_T i2{0}; i2 < b_loop_ub; i2++) {
              allLayers[b_i + allLayers.size(0)].f1[i2 + allLayers[b_i +
                allLayers.size(0)].f1.size(0) * i1] = thisContrastLayers2[i2 +
                thisContrastLayers2.size(0) * i1];
            }
          }
        }
      }
    }
  }
}

// End of code generation (processCustomFunction2.cpp)
