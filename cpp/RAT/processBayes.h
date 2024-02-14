//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// processBayes.h
//
// Code generation for function 'processBayes'
//
#ifndef PROCESSBAYES_H
#define PROCESSBAYES_H

// Include files
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct c_struct_T;
  struct struct2_T;
  struct cell_11;
  struct d_struct_T;
  struct cell_wrap_9;
  struct e_struct_T;
  struct f_struct_T;
  struct struct10_T;
}

// Function Declarations
namespace RAT
{
  void processBayes(const real_T bayesOutputs_bestPars_data[], const int32_T
                    bayesOutputs_bestPars_size[2], const ::coder::array<real_T,
                    2U> &bayesOutputs_chain, const c_struct_T *allProblem_f1,
                    const struct2_T *allProblem_f2, const cell_11 *allProblem_f4,
                    c_struct_T *problemStruct, d_struct_T *contrastParams,
                    cell_wrap_9 result[6], e_struct_T *bayesResults_bestFitsMean,
                    f_struct_T *bayesResults_predlims, struct10_T
                    *bayesResults_parConfInts);
  void processBayes(const ::coder::array<real_T, 2U> &bayesOutputs_bestPars,
                    const ::coder::array<real_T, 2U> &bayesOutputs_chain, const
                    c_struct_T *allProblem_f1, const struct2_T *allProblem_f2,
                    const cell_11 *allProblem_f4, c_struct_T *problemStruct,
                    d_struct_T *contrastParams, cell_wrap_9 result[6],
                    e_struct_T *bayesResults_bestFitsMean, f_struct_T
                    *bayesResults_predlims, struct10_T *bayesResults_parConfInts);
}

#endif

// End of code generation (processBayes.h)
