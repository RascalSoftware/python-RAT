//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// customModelClass.cpp
//
// Code generation for function 'customModelClass'
//

// Include files
#include "customModelClass.h"
#include "backSort.h"
#include "callCppFunc.h"
#include "reflectivityCalculation_internal_types.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "strcmp.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Type Definitions
namespace RAT
{
  struct cell_wrap_34
  {
    ::coder::array<real_T, 2U> f1;
  };

  struct cell_wrap_35
  {
    ::coder::array<real_T, 2U> f1;
  };
}

// Function Declarations
namespace RAT
{
  static void cast(const ::coder::array<cell_wrap_34, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1);
  static void cast(const ::coder::array<cell_wrap_35, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1);
}

// Function Definitions
namespace RAT
{
  static void cast(const ::coder::array<cell_wrap_34, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
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

  static void cast(const ::coder::array<cell_wrap_35, 1U> &r, ::coder::array<
                   cell_wrap_15, 1U> &r1)
  {
    int32_T i;
    r1.set_size(r.size(0));
    i = r.size(0);
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

  void customModelClass::processCustomLayers(const ::coder::array<real_T, 2U>
    &cBacks, const ::coder::array<real_T, 2U> &cShifts, const ::coder::array<
    real_T, 2U> &cScales, const ::coder::array<real_T, 2U> &cNbas, const ::coder::
    array<real_T, 2U> &cNbss, const ::coder::array<real_T, 2U> &cRes, const ::
    coder::array<real_T, 2U> &backs, const ::coder::array<real_T, 2U> &shifts,
    const ::coder::array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba,
    const ::coder::array<real_T, 2U> &nbs, const ::coder::array<real_T, 2U> &res,
    const ::coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts,
    const ::coder::array<cell_wrap_7, 2U> &customFiles, const ::coder::array<
    real_T, 2U> &params, boolean_T useImaginary, ::coder::array<cell_wrap_15, 1U>
    &allLayers, ::coder::array<real_T, 1U> &allRoughs)
  {
    ::coder::array<cell_wrap_34, 1U> tempAllLayers;
    ::coder::array<real_T, 2U> b_params;
    ::coder::array<real_T, 2U> thisContrastLayers;
    real_T a__1;
    real_T a__2;
    real_T a__3;
    real_T a__4;
    real_T bulkIn;
    real_T bulkOut;
    real_T d1;
    int32_T loop_ub_tmp;

    //  Top-level function for processing custom layers for all the
    //  contrasts.
    //  Do some pre-definitions to keep the compiler happy...
    loop_ub_tmp = static_cast<int32_T>(numberOfContrasts);
    allRoughs.set_size(loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      allRoughs[i] = 0.0;
    }

    tempAllLayers.set_size(loop_ub_tmp);
    for (int32_T b_i{0}; b_i < loop_ub_tmp; b_i++) {
      real_T d;
      int32_T b_index;

      //  TODO - the ambition is for parfor here, but would fail for Matlab and Python CM's..
      //  Choose which custom file is associated with this contrast
      //  Check what language it is....
      //  ... and path
      //  ....also file.
      //  Find values of 'bulkIn' and 'bulkOut' for this
      //  contrast...
      backSort(cBacks[b_i], cShifts[b_i], cScales[b_i], cNbas[b_i], cNbss[b_i],
               cRes[b_i], backs, shifts, sf, nba, nbs, res, &a__1, &a__2, &a__3,
               &bulkIn, &bulkOut, &a__4);
      thisContrastLayers.set_size(1, 3);
      thisContrastLayers[0] = 1.0;
      thisContrastLayers[thisContrastLayers.size(0)] = 1.0;
      thisContrastLayers[thisContrastLayers.size(0) * 2] = 1.0;

      //  typeDef
      d = cCustFiles[b_i];
      if (coder::internal::j_strcmp(customFiles[static_cast<int32_T>(d) - 1].f1
           [1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1[1].f1.size))
      {
        b_index = 0;
      } else if (coder::internal::k_strcmp(customFiles[static_cast<int32_T>(d) -
                  1].f1[1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1
                  [1].f1.size)) {
        b_index = 1;
      } else if (coder::internal::l_strcmp(customFiles[static_cast<int32_T>(d) -
                  1].f1[1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1
                  [1].f1.size)) {
        b_index = 2;
      } else {
        b_index = -1;
      }

      switch (b_index) {
       case 0:
        //  Excecute a custom model function in the base Matlab workspace.
        //  This function uses feval to cause Matlab coder to pass excecution
        //  of the function named in 'fname' back to the base Matlab workspace,
        //  'feval' generates an automatic coder.extrinsic call.
        //  Use a coder.extrinsic call here, so Coder will
        //  pass the excecution of the function back to the calling Matlab
        //  session. There is no need to explicitly declare the extrinsic
        //  as Coder automatically does this when it encounters 'feval'
        //  https://uk.mathworks.com/help/simulink/ug/use-matlab-engine-to-execute-a-function-call-in-generated-code.html
        //  Pre-define the outputs to keep the compiler happy
        //  Need to define the size of the outputs with coder preprocessor
        //  directives
        //  This maybe should be a 'coder.typeof' instead?
        //
        //      % Because it is feval, coder sends this call to
        //      % the base Matlab workspace. The outputs of feval are mxArrays,
        //      % so we need to do some work afterwards casting these to doubles (below)
        //
        //      % For backwards compatability with Rascal1...
        //      bulkOuts = zeros(nContrasts,1);
        //      bulkOuts(contrast) = bulkOut;
        //
        //      % Feval is automatic call to base Matlab workspace....
        //      [tempOut,tempRough] = feval(funcName,params,bulkIn,bulkOuts,contrast);
        //
        //      % Tell coder n is doubles by defining it (otherwise 'size(n)' seems to return
        //      % [mxArray mxArray] not [double double]!), fill n with the size of the mxArray
        //      % 'tempOut', and use n to allocate the correct number of elements to 'output'
        //      % by filling output with zeros (i.e. output is an array of doubles)
        //      n = size(tempOut);
        //
        //      % Copy the mxArrays to our doubles arrays, which then makes coder cast
        //      % the mxArrays to double.
        //      sRough = tempRough;
        //      output = tempOut;
        thisContrastLayers.set_size(0, 0);
        allRoughs[b_i] = 3.0;
        break;

       case 1:
        {
          b_params.set_size(1, params.size(1));
          b_index = params.size(1) - 1;
          for (int32_T i{0}; i <= b_index; i++) {
            b_params[i] = params[i];
          }

          int32_T iv[2];
          int32_T iv1[2];
          int32_T iv2[2];
          thisContrastLayers.reserve(3000);
          iv[0] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[0];
          iv[1] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[1];
          iv1[0] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[0];
          iv1[1] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[1];
          callCppFunc(b_params, bulkIn, bulkOut, static_cast<real_T>(b_i) + 1.0,
                      customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.data, iv,
                      customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.data,
                      iv1, (real_T *)thisContrastLayers.data(), iv2, &d1);
          allRoughs[b_i] = d1;
          (*(int32_T (*)[2])thisContrastLayers.size())[0] = iv2[0];
          (*(int32_T (*)[2])thisContrastLayers.size())[1] = iv2[1];
        }
        break;

       case 2:
        //  Excecute a python custom model function in the base Matlab workspace.
        //  This function uses feval to cause Matlab coder to execute
        //  'callPythonFunction' in the base Matlab workspace.
        //  'feval' generates an automatic coder.extrinsic call...
        //  Use a coder.extrinsic call here, so Coder will
        //  pass the excecution of the function back to the calling Matlab
        //  session. There is no need to explicitly declare the extrinsic
        //  as Coder automatically does this when it encounters 'feval'
        //  https://uk.mathworks.com/help/simulink/ug/use-matlab-engine-to-execute-a-function-call-in-generated-code.html
        //  Pre-define the outputs to keep the compiler happy
        //  Need to define the size of the outputs with coder preprocessor
        //  directives
        //  This maybe should be a 'coder.typeof' instead?
        //  Because it is feval, coder sends this call to
        //  the base Matlab workspace. The outputs of feval are mxArrays,
        //  so we need to do some work afterwards casting these to doubles (below)
        //      [tempOut,tempRough] = feval('callPythonFunction',funcName,params,bulkIn,bulkOut,contrast);
        //  Tell coder n is doubles by defining it (otherwise 'size(n)' seems to return
        //  [mxArray mxArray] not [double double]!), fill n with the size of the mxArray
        //  'tempOut', and use n to allocate the correct number of elements to 'output'
        //  by filling output with zeros (i.e. output is an array of doubles)
        //      n = size(tempOut);
        //  Copy the mxArrays to our doubles arrays, which then makes coder cast
        //  the mxArrays to double.
        //      sRough = tempRough;
        //      output = tempOut;
        thisContrastLayers.set_size(0, 0);
        allRoughs[b_i] = 3.0;
        break;
      }

      //  If the output layers has 5 columns, then we need to do
      //  the hydration correction (the user has not done it in the
      //  custom function). Do that here....
      if (!useImaginary) {
        //  Applies the hydration correction to real value of layers
        //  if it is necessary.. (This is for when im(SLD) is not used)
        //  The only guidance we have to whether the user is using hydration
        //  in their custom model is the numbre of columns of the output
        //  [nlayers x nCols]
        tempAllLayers[b_i].f1.set_size(thisContrastLayers.size(0),
          thisContrastLayers.size(1));
        b_index = thisContrastLayers.size(1);
        for (int32_T i{0}; i < b_index; i++) {
          int32_T loop_ub;
          loop_ub = thisContrastLayers.size(0);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            tempAllLayers[b_i].f1[i1 + tempAllLayers[b_i].f1.size(0) * i] =
              thisContrastLayers[i1 + thisContrastLayers.size(0) * i];
          }
        }
      } else {
        //  Applies the hydration correction to real value of layers
        //  if it is necessary.. (This is for when im(SLD) is used)
        //  The only guidance we have to whether the user is using hydration
        //  in their custom model is the number of columns of the output
        //  [nlayers x nCols]
        tempAllLayers[b_i].f1.set_size(thisContrastLayers.size(0),
          thisContrastLayers.size(1));
        b_index = thisContrastLayers.size(1);
        for (int32_T i{0}; i < b_index; i++) {
          int32_T loop_ub;
          loop_ub = thisContrastLayers.size(0);
          for (int32_T i1{0}; i1 < loop_ub; i1++) {
            tempAllLayers[b_i].f1[i1 + tempAllLayers[b_i].f1.size(0) * i] =
              thisContrastLayers[i1 + thisContrastLayers.size(0) * i];
          }
        }
      }
    }

    cast(tempAllLayers, allLayers);
  }

  void customModelClass::processCustomXY(const ::coder::array<real_T, 2U>
    &cBacks, const ::coder::array<real_T, 2U> &cShifts, const ::coder::array<
    real_T, 2U> &cScales, const ::coder::array<real_T, 2U> &cNbas, const ::coder::
    array<real_T, 2U> &cNbss, const ::coder::array<real_T, 2U> &cRes, const ::
    coder::array<real_T, 2U> &backs, const ::coder::array<real_T, 2U> &shifts,
    const ::coder::array<real_T, 2U> &sf, const ::coder::array<real_T, 2U> &nba,
    const ::coder::array<real_T, 2U> &nbs, const ::coder::array<real_T, 2U> &res,
    const ::coder::array<real_T, 2U> &cCustFiles, real_T numberOfContrasts,
    const ::coder::array<cell_wrap_7, 2U> &customFiles, const ::coder::array<
    real_T, 2U> &params, ::coder::array<cell_wrap_15, 1U> &allSLDs, ::coder::
    array<real_T, 1U> &allRoughs)
  {
    ::coder::array<cell_wrap_35, 1U> tempAllSLDs;
    ::coder::array<real_T, 2U> b_params;
    real_T tmp_data[3000];
    real_T a__5;
    real_T a__6;
    real_T a__7;
    real_T a__8;
    real_T bulkIn;
    real_T bulkOut;
    real_T d1;
    int32_T loop_ub_tmp;

    //  Top-level function for processing custom XY profiles for all the
    //  contrasts.
    //  Do some pre-definitions to keep the compiler happy...
    loop_ub_tmp = static_cast<int32_T>(numberOfContrasts);
    allRoughs.set_size(loop_ub_tmp);
    tempAllSLDs.set_size(loop_ub_tmp);
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      allRoughs[i] = 0.0;

      //  Type def as double (size not important)
      tempAllSLDs[i].f1.set_size(1, 2);
      tempAllSLDs[i].f1[0] = 0.0;
      tempAllSLDs[i].f1[tempAllSLDs[i].f1.size(0)] = 0.0;
    }

    //  3 columns to allow for potential imaginary curve
    for (int32_T i{0}; i < loop_ub_tmp; i++) {
      real_T d;
      int32_T b_index;

      //  TODO - the ambition is for parfor here, but would fail for Matlab and Python CM's..
      //  Choose which custom file is associated with this contrast
      //  Check what language it is....
      //  ... and path
      //  ....also file.
      //  Find values of 'bulkIn' and 'bulkOut' for thid
      //  contrast...
      backSort(cBacks[i], cShifts[i], cScales[i], cNbas[i], cNbss[i], cRes[i],
               backs, shifts, sf, nba, nbs, res, &a__5, &a__6, &a__7, &bulkIn,
               &bulkOut, &a__8);
      d = cCustFiles[i];
      if (coder::internal::j_strcmp(customFiles[static_cast<int32_T>(d) - 1].f1
           [1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1[1].f1.size))
      {
        b_index = 0;
      } else if (coder::internal::k_strcmp(customFiles[static_cast<int32_T>(d) -
                  1].f1[1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1
                  [1].f1.size)) {
        b_index = 1;
      } else if (coder::internal::l_strcmp(customFiles[static_cast<int32_T>(d) -
                  1].f1[1].f1.data, customFiles[static_cast<int32_T>(d) - 1].f1
                  [1].f1.size)) {
        b_index = 2;
      } else {
        b_index = -1;
      }

      switch (b_index) {
       case 0:
        //  Excecute a custom model function in the base Matlab workspace.
        //  This function uses feval to cause Matlab coder to pass excecution
        //  of the function named in 'fname' back to the base Matlab workspace,
        //  'feval' generates an automatic coder.extrinsic call.
        //  Use a coder.extrinsic call here, so Coder will
        //  pass the excecution of the function back to the calling Matlab
        //  session. There is no need to explicitly declare the extrinsic
        //  as Coder automatically does this when it encounters 'feval'
        //  https://uk.mathworks.com/help/simulink/ug/use-matlab-engine-to-execute-a-function-call-in-generated-code.html
        //  Pre-define the outputs to keep the compiler happy
        //  Need to define the size of the outputs with coder preprocessor
        //  directives
        //  This maybe should be a 'coder.typeof' instead?
        //
        //      % Because it is feval, coder sends this call to
        //      % the base Matlab workspace. The outputs of feval are mxArrays,
        //      % so we need to do some work afterwards casting these to doubles (below)
        //
        //      % For backwards compatability with Rascal1...
        //      bulkOuts = zeros(nContrasts,1);
        //      bulkOuts(contrast) = bulkOut;
        //
        //      % Feval is automatic call to base Matlab workspace....
        //      [tempOut,tempRough] = feval(funcName,params,bulkIn,bulkOuts,contrast);
        //
        //      % Tell coder n is doubles by defining it (otherwise 'size(n)' seems to return
        //      % [mxArray mxArray] not [double double]!), fill n with the size of the mxArray
        //      % 'tempOut', and use n to allocate the correct number of elements to 'output'
        //      % by filling output with zeros (i.e. output is an array of doubles)
        //      n = size(tempOut);
        //
        //      % Copy the mxArrays to our doubles arrays, which then makes coder cast
        //      % the mxArrays to double.
        //      sRough = tempRough;
        //      output = tempOut;
        tempAllSLDs[i].f1.set_size(0, 0);
        allRoughs[i] = 3.0;
        break;

       case 1:
        {
          int32_T tmp_size[2];
          b_params.set_size(1, params.size(1));
          b_index = params.size(1) - 1;
          for (int32_T b_i{0}; b_i <= b_index; b_i++) {
            b_params[b_i] = params[b_i];
          }

          int32_T iv[2];
          int32_T iv1[2];
          iv[0] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[0];
          iv[1] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[1];
          iv1[0] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[0];
          iv1[1] = customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.size[1];
          callCppFunc(b_params, bulkIn, bulkOut, static_cast<real_T>(i) + 1.0,
                      customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.data, iv,
                      customFiles[static_cast<int32_T>(d) - 1].f1[0].f1.data,
                      iv1, tmp_data, tmp_size, &d1);
          allRoughs[i] = d1;
          b_index = tmp_size[0];
          tempAllSLDs[i].f1.set_size(tmp_size[0], 3);
          for (int32_T b_i{0}; b_i < 3; b_i++) {
            for (int32_T i1{0}; i1 < b_index; i1++) {
              tempAllSLDs[i].f1[i1 + tempAllSLDs[i].f1.size(0) * b_i] =
                tmp_data[i1 + tmp_size[0] * b_i];
            }
          }
        }
        break;

       case 2:
        //  Excecute a python custom model function in the base Matlab workspace.
        //  This function uses feval to cause Matlab coder to execute
        //  'callPythonFunction' in the base Matlab workspace.
        //  'feval' generates an automatic coder.extrinsic call...
        //  Use a coder.extrinsic call here, so Coder will
        //  pass the excecution of the function back to the calling Matlab
        //  session. There is no need to explicitly declare the extrinsic
        //  as Coder automatically does this when it encounters 'feval'
        //  https://uk.mathworks.com/help/simulink/ug/use-matlab-engine-to-execute-a-function-call-in-generated-code.html
        //  Pre-define the outputs to keep the compiler happy
        //  Need to define the size of the outputs with coder preprocessor
        //  directives
        //  This maybe should be a 'coder.typeof' instead?
        //  Because it is feval, coder sends this call to
        //  the base Matlab workspace. The outputs of feval are mxArrays,
        //  so we need to do some work afterwards casting these to doubles (below)
        //      [tempOut,tempRough] = feval('callPythonFunction',funcName,params,bulkIn,bulkOut,contrast);
        //  Tell coder n is doubles by defining it (otherwise 'size(n)' seems to return
        //  [mxArray mxArray] not [double double]!), fill n with the size of the mxArray
        //  'tempOut', and use n to allocate the correct number of elements to 'output'
        //  by filling output with zeros (i.e. output is an array of doubles)
        //      n = size(tempOut);
        //  Copy the mxArrays to our doubles arrays, which then makes coder cast
        //  the mxArrays to double.
        //      sRough = tempRough;
        //      output = tempOut;
        tempAllSLDs[i].f1.set_size(0, 0);
        allRoughs[i] = 3.0;
        break;
      }
    }

    cast(tempAllSLDs, allSLDs);
  }
}

// End of code generation (customModelClass.cpp)
