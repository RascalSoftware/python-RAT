//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// standardTFCustomXYParallelContrasts.cpp
//
// Code generation for function 'standardTFCustomXYParallelContrasts'
//

// Include files
#include "standardTFCustomXYParallelContrasts.h"
#include "applyBackgroundCorrection.h"
#include "backSort.h"
#include "callReflectivity.h"
#include "chiSquared.h"
#include "customModelClass.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "resampleLayers.h"
#include "resampleLayersReIm.h"
#include "rt_nonfinite.h"
#include "shiftData.h"
#include "coder_array.h"
#include "omp.h"

// Function Definitions
namespace RAT
{
  void standardTFCustomXYParallelContrasts(const struct0_T *problemDef, const
    cell_19 *problemDefCells, const struct2_T *controls, ::coder::array<real_T,
    1U> &outSsubs, ::coder::array<real_T, 1U> &backgs, ::coder::array<real_T, 1U>
    &qshifts, ::coder::array<real_T, 1U> &sfs, ::coder::array<real_T, 1U> &nbas,
    ::coder::array<real_T, 1U> &nbss, ::coder::array<real_T, 1U> &resols, ::
    coder::array<real_T, 1U> &chis, ::coder::array<cell_wrap_9, 1U>
    &reflectivity, ::coder::array<cell_wrap_9, 1U> &Simulation, ::coder::array<
    cell_wrap_15, 1U> &shifted_data, ::coder::array<cell_wrap_15, 1U> &layerSlds,
    ::coder::array<cell_wrap_15, 1U> &sldProfiles, ::coder::array<cell_wrap_15,
    1U> &allLayers, ::coder::array<real_T, 1U> &allRoughs)
  {
    ::coder::array<real_T, 2U> Simul;
    ::coder::array<real_T, 2U> b_problemDefCells;
    ::coder::array<real_T, 2U> b_sldProfiles;
    ::coder::array<real_T, 2U> layerSld;
    ::coder::array<real_T, 2U> reflect;
    ::coder::array<real_T, 2U> shifted_dat;
    real_T reSLD_data[2000];
    real_T dv[2];
    real_T d;
    real_T d1;
    real_T d2;
    real_T d3;
    real_T d4;
    real_T d5;
    int32_T iv[2];
    int32_T reSLD_size[2];
    int32_T b_loop_ub;
    int32_T i1;
    int32_T i2;
    int32_T loop_ub;
    int32_T loop_ub_tmp;
    int32_T nParams;
    boolean_T useImaginary;

    //  Extract individual cell arrays
    //  Splits up the master input list of all arrays into separate arrays
    //  The min input array 'problemDefCells' is a master array where
    //  all the cell arrays are grouped together. There are
    //  repeatLayers      - controls repeating of the layers stack
    //  allData           - Array of all the data arrays
    //  dataLimits        - Min max limits in q for the data arrays
    //  simLimits         - Limits in Q for the reflkectivity simulations
    //  Layers details    - Master array of all available layers
    //  contrastLayers    - Which specific combination of arrays are needed for
    //                      each contrast.
    //  Custom files      - Filenames and path for any custom files used
    //         % Layers details N/A
    //  Extract individual parameters from problemDef struct
    // Extract individual parameters from problemDef
    nParams = problemDef->params.size(1);

    // Pre-Allocation...
    loop_ub_tmp = static_cast<int32_T>(problemDef->numberOfContrasts);
    backgs.set_size(loop_ub_tmp);
    outSsubs.set_size(loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      outSsubs[i] = 0.0;
    }

    //  Resampling parameters
    useImaginary = problemDef->useImaginary;
    customModelClass::processCustomXY(problemDef->contrastBacks,
      problemDef->contrastShifts, problemDef->contrastScales,
      problemDef->contrastNbas, problemDef->contrastNbss,
      problemDef->contrastRes, problemDef->backs, problemDef->shifts,
      problemDef->sf, problemDef->nba, problemDef->nbs, problemDef->res,
      problemDef->contrastCustomFiles, problemDef->numberOfContrasts,
      problemDefCells->f14, problemDef->params, sldProfiles, allRoughs);
    qshifts.set_size(loop_ub_tmp);
    sfs.set_size(loop_ub_tmp);
    nbas.set_size(loop_ub_tmp);
    nbss.set_size(loop_ub_tmp);
    resols.set_size(loop_ub_tmp);
    layerSlds.set_size(loop_ub_tmp);
    allLayers.set_size(loop_ub_tmp);
    shifted_data.set_size(loop_ub_tmp);
    chis.set_size(loop_ub_tmp);
    Simulation.set_size(loop_ub_tmp);
    reflectivity.set_size(loop_ub_tmp);
    loop_ub_tmp--;

#pragma omp parallel for \
 num_threads(omp_get_max_threads()) \
 private(b_sldProfiles,b_problemDefCells,layerSld,reflect,Simul,shifted_dat,reSLD_data,reSLD_size,d,d1,d2,d3,d4,d5,loop_ub,b_loop_ub,i1,i2,dv,iv)

    for (int32_T b_i = 0; b_i <= loop_ub_tmp; b_i++) {
      backSort(problemDef->contrastBacks[b_i], problemDef->contrastShifts[b_i],
               problemDef->contrastScales[b_i], problemDef->contrastNbas[b_i],
               problemDef->contrastNbss[b_i], problemDef->contrastRes[b_i],
               problemDef->backs, problemDef->shifts, problemDef->sf,
               problemDef->nba, problemDef->nbs, problemDef->res, &d5, &d4, &d3,
               &d2, &d1, &d);
      backgs[b_i] = d5;
      qshifts[b_i] = d4;
      sfs[b_i] = d3;
      nbas[b_i] = d2;
      nbss[b_i] = d1;
      resols[b_i] = d;

      //  Resample the layers
      if (!useImaginary) {
        resampleLayers(sldProfiles[b_i].f1, controls->resamPars, layerSld);
      } else {
        loop_ub = sldProfiles[b_i].f1.size(0);
        reSLD_size[0] = sldProfiles[b_i].f1.size(0);
        reSLD_size[1] = 2;
        for (i1 = 0; i1 < 2; i1++) {
          for (i2 = 0; i2 < loop_ub; i2++) {
            reSLD_data[i2 + reSLD_size[0] * i1] = sldProfiles[b_i].f1[i2 +
              sldProfiles[b_i].f1.size(0) * i1];
          }
        }

        loop_ub = sldProfiles[b_i].f1.size(0);
        b_sldProfiles.set_size(sldProfiles[b_i].f1.size(0), 2);
        for (i1 = 0; i1 < loop_ub; i1++) {
          b_sldProfiles[i1] = sldProfiles[b_i].f1[i1];
          b_sldProfiles[i1 + b_sldProfiles.size(0)] = sldProfiles[b_i].f1[i1 +
            sldProfiles[b_i].f1.size(0) * 2];
        }

        iv[0] = (*(int32_T (*)[2])b_sldProfiles.size())[0];
        iv[1] = (*(int32_T (*)[2])b_sldProfiles.size())[1];
        resampleLayersReIm(reSLD_data, reSLD_size, (const real_T *)
                           b_sldProfiles.data(), iv, controls->resamPars,
                           layerSld);
      }

      layerSlds[b_i].f1.set_size(layerSld.size(0), layerSld.size(1));
      loop_ub = layerSld.size(1);
      allLayers[b_i].f1.set_size(layerSld.size(0), layerSld.size(1));
      b_loop_ub = layerSld.size(0);
      for (i1 = 0; i1 < loop_ub; i1++) {
        for (i2 = 0; i2 < b_loop_ub; i2++) {
          layerSlds[b_i].f1[i2 + layerSlds[b_i].f1.size(0) * i1] = layerSld[i2 +
            layerSld.size(0) * i1];
          allLayers[b_i].f1[i2 + allLayers[b_i].f1.size(0) * i1] = layerSld[i2 +
            layerSld.size(0) * i1];
        }
      }

      b_problemDefCells.set_size(problemDefCells->f2[problemDefCells->f2.size(0)
        * b_i].f1.size(0), problemDefCells->f2[problemDefCells->f2.size(0) * b_i]
        .f1.size(1));
      loop_ub = problemDefCells->f2[b_i].f1.size(1) - 1;
      for (i1 = 0; i1 <= loop_ub; i1++) {
        b_loop_ub = problemDefCells->f2[b_i].f1.size(0) - 1;
        for (i2 = 0; i2 <= b_loop_ub; i2++) {
          b_problemDefCells[i2 + b_problemDefCells.size(0) * i1] =
            problemDefCells->f2[b_i].f1[i2 + problemDefCells->f2[b_i].f1.size(0)
            * i1];
        }
      }

      dv[0] = problemDefCells->f3[b_i].f1[0];
      dv[1] = problemDefCells->f3[b_i].f1[1];
      shiftData(sfs[b_i], qshifts[b_i], problemDef->dataPresent[b_i],
                b_problemDefCells, dv, (const real_T *)((::coder::array<real_T,
                  2U> *)&problemDefCells->f4[b_i].f1)->data(), shifted_dat);
      shifted_data[b_i].f1.set_size(shifted_dat.size(0), shifted_dat.size(1));
      loop_ub = shifted_dat.size(1);
      for (i1 = 0; i1 < loop_ub; i1++) {
        b_loop_ub = shifted_dat.size(0);
        for (i2 = 0; i2 < b_loop_ub; i2++) {
          shifted_data[b_i].f1[i2 + shifted_data[b_i].f1.size(0) * i1] =
            shifted_dat[i2 + shifted_dat.size(0) * i1];
        }
      }

      dv[0] = problemDefCells->f1[b_i].f1[0];
      dv[1] = problemDefCells->f1[b_i].f1[1];
      callReflectivity(nbas[b_i], nbss[b_i], (const real_T *)((::coder::array<
        real_T, 2U> *)&problemDefCells->f4[b_i].f1)->data(), dv, shifted_dat,
                       layerSld, 0.0, resols[b_i], useImaginary, reflect, Simul);
      applyBackgroundCorrection(reflect, Simul, shifted_dat, backgs[b_i],
        problemDef->contrastBacksType[b_i]);
      loop_ub = reflect.size(0);
      reflectivity[b_i].f1.set_size(reflect.size(0), 2);
      b_loop_ub = Simul.size(0);
      Simulation[b_i].f1.set_size(Simul.size(0), 2);
      for (i1 = 0; i1 < 2; i1++) {
        for (i2 = 0; i2 < loop_ub; i2++) {
          reflectivity[b_i].f1[i2 + reflectivity[b_i].f1.size(0) * i1] =
            reflect[i2 + reflect.size(0) * i1];
        }

        for (i2 = 0; i2 < b_loop_ub; i2++) {
          Simulation[b_i].f1[i2 + Simulation[b_i].f1.size(0) * i1] = Simul[i2 +
            Simul.size(0) * i1];
        }
      }

      if (problemDef->dataPresent[b_i] != 0.0) {
        chis[b_i] = chiSquared(shifted_dat, reflect, static_cast<real_T>(nParams));
      } else {
        chis[b_i] = 0.0;
      }
    }
  }
}

// End of code generation (standardTFCustomXYParallelContrasts.cpp)
