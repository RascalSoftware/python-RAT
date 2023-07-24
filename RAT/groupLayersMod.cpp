//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// groupLayersMod.cpp
//
// Code generation for function 'groupLayersMod'
//

// Include files
#include "groupLayersMod.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "coder_array.h"
#include <cmath>

// Function Definitions
namespace RAT
{
  void groupLayersMod(const ::coder::array<real_T, 2U> &allLayers, real_T
                      allRoughs, const char_T geometry_data[], const int32_T
                      geometry_size[2], real_T nbair, real_T nbsubs, ::coder::
                      array<real_T, 2U> &outLayers, real_T *outSsubs)
  {
    ::coder::array<real_T, 2U> layers;
    ::coder::array<real_T, 1U> roughs;
    int32_T b_loop_ub;
    int32_T i;
    int32_T loop_ub;
    uint32_T unnamed_idx_0;

    // Arrange layers according to geometry and apply any coverage correction.
    //
    //  USAGE::
    //
    //      [outLayers, outSsubs] = groupLayersMod(allLayers,allRoughs,numberOfContrasts,geometry,nbairs,nbsubs)
    //
    //  INPUTS:
    //
    //      * allLayers =         cell array, one for each contrast. Each cell is the list of layer values for each contrast.
    //      * allRoughs =         Double of substrate roughness for each contrast.
    //      * numberOfContrasts = double.
    //      * geometry =          'Air / Liquid (or solid)' or 'Solid / Liquid'
    //      * nbairs =            vector of nbair values.
    //      * nbsubs =            vector of nbsub values.
    //
    //      The paratt calculation procedds through the
    //      z,rho,rough stack, and the parameter 'ssub' in
    //      callParatt is the final roughness encountered.
    //
    //      * For air liquid 'ssub' is therefore the substrate roughness.
    //
    //      * For solid liquid, the substrate roughness is the first roughness encountered, and 'ssub' is then the roughness of the outermost layer
    //
    //  Outputs:
    //
    //      * outLayers = cell array of layers param values for each contrast.
    //
    //      * outSsubs =  vector of substrate roughness values.
    //
    // outLayers = cell(1,numberOfContrasts);
    // outSsubs = zeros(1,numberOfContrasts);
    // for i = 1:numberOfContrasts
    *outSsubs = allRoughs;
    unnamed_idx_0 = static_cast<uint32_T>(allLayers.size(0));
    layers.set_size(allLayers.size(0), allLayers.size(1));
    loop_ub = allLayers.size(1);
    for (i = 0; i < loop_ub; i++) {
      b_loop_ub = static_cast<int32_T>(unnamed_idx_0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        layers[i1 + layers.size(0) * i] = 0.0;
      }
    }

    if ((allLayers.size(0) != 0) && (allLayers.size(1) != 0)) {
      if (coder::internal::i_strcmp(geometry_data, geometry_size)) {
        layers.set_size(allLayers.size(0), allLayers.size(1));
        loop_ub = allLayers.size(1);
        b_loop_ub = allLayers.size(0);
        for (i = 0; i < loop_ub; i++) {
          for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
            layers[i1 + layers.size(0) * i] = allLayers[i1 + allLayers.size(0) *
              i];
          }
        }

        // s_sub = rsub;
      } else {
        *outSsubs = allLayers[(allLayers.size(0) + allLayers.size(0) * 2) - 1];
        if (allLayers.size(0) > 1) {
          roughs.set_size(allLayers.size(0));
          roughs[0] = allRoughs;
          loop_ub = allLayers.size(0);
          for (i = 0; i <= loop_ub - 2; i++) {
            roughs[i + 1] = allLayers[i + allLayers.size(0) * 2];
          }
        } else {
          roughs.set_size(1);
          roughs[0] = allRoughs;
        }

        if (allLayers.size(1) == 5) {
          layers.set_size(allLayers.size(0), 4);
          loop_ub = allLayers.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i] = allLayers[i];
          }

          loop_ub = allLayers.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i + layers.size(0)] = allLayers[i + allLayers.size(0)];
          }

          loop_ub = roughs.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i + layers.size(0) * 2] = roughs[i];
          }

          loop_ub = allLayers.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i + layers.size(0) * 3] = allLayers[i + allLayers.size(0) * 3];
          }
        } else {
          layers.set_size(allLayers.size(0), 3);
          loop_ub = allLayers.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i] = allLayers[i];
          }

          loop_ub = allLayers.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i + layers.size(0)] = allLayers[i + allLayers.size(0)];
          }

          loop_ub = roughs.size(0);
          for (i = 0; i < loop_ub; i++) {
            layers[i + layers.size(0) * 2] = roughs[i];
          }
        }
      }

      // Deal with the %coverage if present
      if (allLayers.size(1) == 5) {
        i = allLayers.size(0);
        for (int32_T j{0}; j < i; j++) {
          real_T this_pcw;
          this_pcw = allLayers[j + allLayers.size(0) * 3];
          if (!std::isnan(this_pcw)) {
            real_T d;
            if (allLayers[j + allLayers.size(0) * 4] == 1.0) {
              d = nbair;
            } else {
              d = nbsubs;
            }

            layers[j + layers.size(0)] = d * (this_pcw / 100.0) + (1.0 -
              this_pcw / 100.0) * layers[j + layers.size(0)];
          }
        }
      }
    }

    //      problem.layers{i} = layers;
    //      problem.ssubs(i) = s_sub;
    if ((layers.size(0) != 0) && (layers.size(1) != 0)) {
      outLayers.set_size(layers.size(0), 3);
      loop_ub = layers.size(0);
      for (i = 0; i < 3; i++) {
        for (int32_T i1{0}; i1 < loop_ub; i1++) {
          outLayers[i1 + outLayers.size(0) * i] = layers[i1 + layers.size(0) * i];
        }
      }
    } else {
      outLayers.set_size(1, 3);
      outLayers[0] = 0.0;
      outLayers[outLayers.size(0)] = 0.0;
      outLayers[outLayers.size(0) * 2] = 0.0;
    }
  }
}

// End of code generation (groupLayersMod.cpp)
