//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// structConstructorHelper.h
//
// Code generation for function 'structConstructorHelper'
//
#ifndef STRUCTCONSTRUCTORHELPER_H
#define STRUCTCONSTRUCTORHELPER_H

// Include files
#include "RATMain_types.h"
#include "rtwtypes.h"
#include "coder_array.h"
#include <cstddef>
#include <cstdlib>

// Type Declarations
namespace RAT
{
  struct cell_wrap_15;
  struct cell_wrap_17;
}

// Function Declarations
namespace RAT
{
  namespace coder
  {
    namespace internal
    {
      void structConstructorHelper(const cell_wrap_15 *varargin_2, const
        cell_wrap_9 *varargin_4, const cell_wrap_15 *varargin_8, ::coder::array<
        cell_wrap_8, 1U> &s_ref, ::coder::array<cell_wrap_8, 2U> &s_sld, real_T *
        s_chi, ::coder::array<cell_wrap_8, 1U> &s_data);
      void structConstructorHelper(const cell_wrap_15 *varargin_2, const
        cell_wrap_15 *varargin_4, const cell_wrap_15 *varargin_6, const
        cell_wrap_9 *varargin_8, const cell_wrap_17 *varargin_10, ::coder::array<
        cell_wrap_8, 1U> &s_refPredInts, ::coder::array<cell_wrap_8, 1U>
        &s_sldPredInts, ::coder::array<cell_wrap_8, 1U> &s_refXdata, ::coder::
        array<cell_wrap_8, 2U> &s_sldXdata, real_T s_sampleChi_data[], int32_T
        *s_sampleChi_size);
    }
  }
}

#endif

// End of code generation (structConstructorHelper.h)
