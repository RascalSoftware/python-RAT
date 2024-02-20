//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// applyHydrationImag.cpp
//
// Code generation for function 'applyHydrationImag'
//

// Include files
#include "applyHydrationImag.h"
#include "rt_nonfinite.h"
#include "coder_array.h"

// Function Definitions
namespace RAT
{
  void applyHydrationImag(::coder::array<real_T, 2U> &thisContrastLayers, real_T
    bulkIn, real_T bulkOut)
  {
    ::coder::array<real_T, 2U> newOutLayers;
    int16_T outSize_idx_0;

    //  Applies the hydration correction to real value of layers
    //  if it is necessary.. (This is for when im(SLD) is used)
    //  The only guidance we have to whether the user is using hydration
    //  in their custom model is the number of columns of the output
    outSize_idx_0 = static_cast<int16_T>(thisContrastLayers.size(0));

    //  [nlayers x nCols]
    if (static_cast<int16_T>(thisContrastLayers.size(1)) == 6) {
      int32_T i;
      int32_T loop_ub;

      //  we need to calculate the hydrated SLD
      newOutLayers.set_size(static_cast<int32_T>(outSize_idx_0), 4);
      loop_ub = outSize_idx_0;
      for (i = 0; i < 4; i++) {
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          newOutLayers[i1 + newOutLayers.size(0) * i] = 0.0;
        }
      }

      loop_ub = thisContrastLayers.size(0);
      for (i = 0; i < loop_ub; i++) {
        newOutLayers[i] = thisContrastLayers[i];
      }

      //  Thickness'
      loop_ub = thisContrastLayers.size(0);
      for (i = 0; i < loop_ub; i++) {
        newOutLayers[i + newOutLayers.size(0) * 2] = thisContrastLayers[i +
          thisContrastLayers.size(0) * 2];
      }

      //  We never hydrate im(SLD)
      loop_ub = thisContrastLayers.size(0);
      for (i = 0; i < loop_ub; i++) {
        newOutLayers[i + newOutLayers.size(0) * 3] = thisContrastLayers[i +
          thisContrastLayers.size(0) * 3];
      }

      //  Roughness
      i = outSize_idx_0;
      for (int32_T n{0}; n < i; n++) {
        real_T d;
        real_T thisHydration;
        thisHydration = thisContrastLayers[n + thisContrastLayers.size(0) * 4] /
          100.0;

        //  Assume percent for backwards compatability
        if (thisContrastLayers[n + thisContrastLayers.size(0) * 5] == 0.0) {
          d = bulkIn;
        } else {
          d = bulkOut;
        }

        newOutLayers[n + newOutLayers.size(0)] = thisHydration * d + (1.0 -
          thisHydration) * thisContrastLayers[n + thisContrastLayers.size(0)];

        //  Reassignment to keep codegen happy
      }

      thisContrastLayers.set_size(newOutLayers.size(0), 4);
      loop_ub = newOutLayers.size(0);
      for (i = 0; i < 4; i++) {
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          thisContrastLayers[i1 + thisContrastLayers.size(0) * i] =
            newOutLayers[i1 + newOutLayers.size(0) * i];
        }
      }
    }
  }
}

// End of code generation (applyHydrationImag.cpp)