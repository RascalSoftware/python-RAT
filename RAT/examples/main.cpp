//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// main.cpp
//
// Code generation for function 'main'
//

/*************************************************************************/
/* This automatically generated example C++ main file shows how to call  */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/

// Include files
#include "main.h"
#include "reflectivityCalculation.h"
#include "reflectivityCalculation_initialize.h"
#include "reflectivityCalculation_terminate.h"
#include "reflectivityCalculation_types.h"
#include "rt_nonfinite.h"
#include "coder_array.h"
#include "coder_bounded_array.h"

// Function Declarations
static void argInit_1x2_real_T(real_T result[2]);
static void argInit_1x3_cell_wrap_6(RAT::cell_wrap_6 result[3]);
static coder::array<RAT::cell_wrap_0, 2U> argInit_1xd10000_cell_wrap_0();
static coder::array<RAT::cell_wrap_1, 2U> argInit_1xd10000_cell_wrap_1();
static coder::array<RAT::cell_wrap_2, 2U> argInit_1xd10000_cell_wrap_2();
static coder::array<RAT::cell_wrap_3, 2U> argInit_1xd10000_cell_wrap_3();
static coder::array<RAT::cell_wrap_5, 2U> argInit_1xd10000_cell_wrap_5();
static coder::array<RAT::cell_wrap_6, 2U> argInit_1xd10000_cell_wrap_6();
static coder::array<RAT::cell_wrap_7, 2U> argInit_1xd10000_cell_wrap_7();
static void argInit_1xd10000_char_T(char_T result_data[], int32_T result_size[2]);
static coder::array<real_T, 2U> argInit_1xd10000_real_T();
static boolean_T argInit_boolean_T();
static void argInit_cell_8(RAT::cell_8 *result);
static RAT::cell_wrap_0 argInit_cell_wrap_0();
static RAT::cell_wrap_1 argInit_cell_wrap_1();
static RAT::cell_wrap_2 argInit_cell_wrap_2();
static RAT::cell_wrap_3 argInit_cell_wrap_3();
static void argInit_cell_wrap_4(RAT::cell_wrap_4 *result);
static void argInit_cell_wrap_5(RAT::cell_wrap_5 *result);
static void argInit_cell_wrap_6(RAT::cell_wrap_6 *result);
static void argInit_cell_wrap_7(RAT::cell_wrap_7 *result);
static char_T argInit_char_T();
static coder::array<RAT::cell_wrap_4, 1U> argInit_d10000x1_cell_wrap_4();
static coder::array<real_T, 2U> argInit_d10000x2_real_T();
static coder::array<real_T, 2U> argInit_d10000xd10000_real_T();
static coder::array<real_T, 2U> argInit_d1xd10000_real_T();
static void argInit_d1xd2_real_T(real_T result_data[], int32_T result_size[2]);
static real_T argInit_real_T();
static void argInit_struct0_T(RAT::struct0_T *result);
static void argInit_struct1_T(RAT::struct1_T *result);
static void argInit_struct2_T(RAT::struct2_T *result);
static void argInit_struct3_T(RAT::struct3_T *result);

// Function Definitions
static void argInit_1x2_real_T(real_T result[2])
{
  // Loop over the array to initialize each element.
  for (int32_T idx1{0}; idx1 < 2; idx1++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result[idx1] = argInit_real_T();
  }
}

static void argInit_1x3_cell_wrap_6(RAT::cell_wrap_6 result[3])
{
  // Loop over the array to initialize each element.
  for (int32_T idx1{0}; idx1 < 3; idx1++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    argInit_cell_wrap_6(&result[idx1]);
  }
}

static coder::array<RAT::cell_wrap_0, 2U> argInit_1xd10000_cell_wrap_0()
{
  coder::array<RAT::cell_wrap_0, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_cell_wrap_0();
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_1, 2U> argInit_1xd10000_cell_wrap_1()
{
  coder::array<RAT::cell_wrap_1, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_cell_wrap_1();
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_2, 2U> argInit_1xd10000_cell_wrap_2()
{
  coder::array<RAT::cell_wrap_2, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_cell_wrap_2();
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_3, 2U> argInit_1xd10000_cell_wrap_3()
{
  coder::array<RAT::cell_wrap_3, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_cell_wrap_3();
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_5, 2U> argInit_1xd10000_cell_wrap_5()
{
  coder::array<RAT::cell_wrap_5, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      argInit_cell_wrap_5(&result[idx1]);
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_6, 2U> argInit_1xd10000_cell_wrap_6()
{
  coder::array<RAT::cell_wrap_6, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      argInit_cell_wrap_6(&result[idx1]);
    }
  }

  return result;
}

static coder::array<RAT::cell_wrap_7, 2U> argInit_1xd10000_cell_wrap_7()
{
  coder::array<RAT::cell_wrap_7, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      argInit_cell_wrap_7(&result[idx1]);
    }
  }

  return result;
}

static void argInit_1xd10000_char_T(char_T result_data[], int32_T result_size[2])
{
  // Set the size of the array.
  // Change this size to the value that the application requires.
  result_size[0] = 1;
  result_size[1] = 2;

  // Loop over the array to initialize each element.
  for (int32_T idx1{0}; idx1 < 2; idx1++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result_data[idx1] = argInit_char_T();
  }
}

static coder::array<real_T, 2U> argInit_1xd10000_real_T()
{
  coder::array<real_T, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < 1; idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx1] = argInit_real_T();
    }
  }

  return result;
}

static boolean_T argInit_boolean_T()
{
  return false;
}

static void argInit_cell_8(RAT::cell_8 *result)
{
  coder::array<RAT::cell_wrap_0, 2U> result_tmp;
  coder::array<RAT::cell_wrap_1, 2U> b_result_tmp;
  coder::array<RAT::cell_wrap_3, 2U> c_result_tmp;
  coder::array<RAT::cell_wrap_5, 2U> d_result_tmp;
  coder::array<RAT::cell_wrap_6, 2U> e_result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result_tmp = argInit_1xd10000_cell_wrap_0();
  result->f1 = result_tmp;
  b_result_tmp = argInit_1xd10000_cell_wrap_1();
  result->f2 = b_result_tmp;
  result->f3 = result_tmp;
  result->f4 = argInit_1xd10000_cell_wrap_2();
  c_result_tmp = argInit_1xd10000_cell_wrap_3();
  result->f5 = c_result_tmp;
  result->f6 = argInit_d10000x1_cell_wrap_4();
  d_result_tmp = argInit_1xd10000_cell_wrap_5();
  result->f7 = d_result_tmp;
  e_result_tmp = argInit_1xd10000_cell_wrap_6();
  result->f8 = e_result_tmp;
  result->f9 = e_result_tmp;
  result->f10 = e_result_tmp;
  result->f11 = e_result_tmp;
  result->f12 = e_result_tmp;
  result->f13 = e_result_tmp;
  result->f14 = argInit_1xd10000_cell_wrap_7();
  result->f15 = d_result_tmp;
  result->f16 = d_result_tmp;
  result->f17 = b_result_tmp;
  result->f18 = result_tmp;
  result->f19 = c_result_tmp;
  result->f20 = e_result_tmp;
}

static RAT::cell_wrap_0 argInit_cell_wrap_0()
{
  RAT::cell_wrap_0 result;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_1x2_real_T(result.f1);
  return result;
}

static RAT::cell_wrap_1 argInit_cell_wrap_1()
{
  RAT::cell_wrap_1 result;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result.f1 = argInit_d10000xd10000_real_T();
  return result;
}

static RAT::cell_wrap_2 argInit_cell_wrap_2()
{
  RAT::cell_wrap_2 result;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_d1xd2_real_T(result.f1.data, result.f1.size);
  return result;
}

static RAT::cell_wrap_3 argInit_cell_wrap_3()
{
  RAT::cell_wrap_3 result;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result.f1 = argInit_d1xd10000_real_T();
  return result;
}

static void argInit_cell_wrap_4(RAT::cell_wrap_4 *result)
{
  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_d1xd2_real_T(result->f1.data, result->f1.size);
}

static void argInit_cell_wrap_5(RAT::cell_wrap_5 *result)
{
  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_1xd10000_char_T(result->f1.data, result->f1.size);
}

static void argInit_cell_wrap_6(RAT::cell_wrap_6 *result)
{
  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_1xd10000_char_T(result->f1.data, result->f1.size);
}

static void argInit_cell_wrap_7(RAT::cell_wrap_7 *result)
{
  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  argInit_1x3_cell_wrap_6(result->f1);
}

static char_T argInit_char_T()
{
  return '?';
}

static coder::array<RAT::cell_wrap_4, 1U> argInit_d10000x1_cell_wrap_4()
{
  coder::array<RAT::cell_wrap_4, 1U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < result.size(0); idx0++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    argInit_cell_wrap_4(&result[idx0]);
  }

  return result;
}

static coder::array<real_T, 2U> argInit_d10000x2_real_T()
{
  coder::array<real_T, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(2, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < result.size(0); idx0++) {
    for (int32_T idx1{0}; idx1 < 2; idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx0 + result.size(0) * idx1] = argInit_real_T();
    }
  }

  return result;
}

static coder::array<real_T, 2U> argInit_d10000xd10000_real_T()
{
  coder::array<real_T, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(2, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < result.size(0); idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[idx0 + result.size(0) * idx1] = argInit_real_T();
    }
  }

  return result;
}

static coder::array<real_T, 2U> argInit_d1xd10000_real_T()
{
  coder::array<real_T, 2U> result;

  // Set the size of the array.
  // Change this size to the value that the application requires.
  result.set_size(1, 2);

  // Loop over the array to initialize each element.
  for (int32_T idx0{0}; idx0 < result.size(0); idx0++) {
    for (int32_T idx1{0}; idx1 < result.size(1); idx1++) {
      // Set the value of the array element.
      // Change this value to the value that the application requires.
      result[result.size(0) * idx1] = argInit_real_T();
    }
  }

  return result;
}

static void argInit_d1xd2_real_T(real_T result_data[], int32_T result_size[2])
{
  // Set the size of the array.
  // Change this size to the value that the application requires.
  result_size[0] = 1;
  result_size[1] = 2;

  // Loop over the array to initialize each element.
  for (int32_T idx1{0}; idx1 < 2; idx1++) {
    // Set the value of the array element.
    // Change this value to the value that the application requires.
    result_data[idx1] = argInit_real_T();
  }
}

static real_T argInit_real_T()
{
  return 0.0;
}

static void argInit_struct0_T(RAT::struct0_T *result)
{
  coder::array<real_T, 2U> b_result_tmp;
  real_T result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result_tmp = argInit_real_T();
  result->numberOfLayers = result_tmp;
  result->numberOfDomainContrasts = result_tmp;
  b_result_tmp = argInit_1xd10000_real_T();
  result->contrastBacks = b_result_tmp;
  result->contrastBacksType = b_result_tmp;
  argInit_1xd10000_char_T(result->TF.data, result->TF.size);
  result->resample = b_result_tmp;
  result->dataPresent = b_result_tmp;
  result->oilChiDataPresent = b_result_tmp;
  result->numberOfContrasts = result_tmp;
  argInit_1xd10000_char_T(result->geometry.data, result->geometry.size);
  result->useImaginary = argInit_boolean_T();
  result->contrastShifts = b_result_tmp;
  result->contrastScales = b_result_tmp;
  result->contrastNbas = b_result_tmp;
  result->contrastNbss = b_result_tmp;
  result->contrastRes = b_result_tmp;
  result->backs = b_result_tmp;
  result->shifts = b_result_tmp;
  result->sf = b_result_tmp;
  result->nba = b_result_tmp;
  result->nbs = b_result_tmp;
  result->res = b_result_tmp;
  result->params = b_result_tmp;
  argInit_1xd10000_char_T(result->modelType.data, result->modelType.size);
  result->contrastCustomFiles = b_result_tmp;
  result->contrastDomainRatios = b_result_tmp;
  result->domainRatio = b_result_tmp;
  result->domainContrastCustomFiles = b_result_tmp;
  b_result_tmp = argInit_d10000xd10000_real_T();
  result->fitpars = b_result_tmp;
  result->otherpars = b_result_tmp;
  result->fitconstr = b_result_tmp;
  result->otherconstr = b_result_tmp;
}

static void argInit_struct1_T(RAT::struct1_T *result)
{
  coder::array<real_T, 2U> result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result_tmp = argInit_d10000x2_real_T();
  result->params = result_tmp;
  result->backs = result_tmp;
  result->scales = result_tmp;
  result->shifts = result_tmp;
  result->nba = result_tmp;
  result->nbs = result_tmp;
  result->res = result_tmp;
  result->domainRatio = result_tmp;
}

static void argInit_struct2_T(RAT::struct2_T *result)
{
  real_T result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result_tmp = argInit_real_T();
  result->tolFun = result_tmp;
  result->maxFunEvals = result_tmp;
  result->maxIter = result_tmp;
  result->populationSize = result_tmp;
  result->fWeight = result_tmp;
  result->F_CR = result_tmp;
  result->VTR = result_tmp;
  result->numGen = result_tmp;
  result->strategy = result_tmp;
  result->Nlive = result_tmp;
  result->nmcmc = result_tmp;
  result->propScale = result_tmp;
  result->nsTolerance = result_tmp;
  result->calcSld = result_tmp;
  result->updateFreq = result_tmp;
  result->updatePlotFreq = result_tmp;
  result->nSamples = result_tmp;
  result->nChains = result_tmp;
  result->lambda = result_tmp;
  result->pUnitGamma = result_tmp;
  argInit_1xd10000_char_T(result->para.data, result->para.size);
  argInit_1xd10000_char_T(result->proc.data, result->proc.size);
  argInit_1xd10000_char_T(result->display.data, result->display.size);
  result->tolX = result_tmp;
  argInit_1x2_real_T(result->resamPars);
  argInit_1xd10000_char_T(result->boundHandling.data, result->boundHandling.size);
  argInit_struct3_T(&result->checks);
}

static void argInit_struct3_T(RAT::struct3_T *result)
{
  coder::array<real_T, 2U> result_tmp;

  // Set the value of each structure field.
  // Change this value to the value that the application requires.
  result_tmp = argInit_1xd10000_real_T();
  result->params_fitYesNo = result_tmp;
  result->backs_fitYesNo = result_tmp;
  result->shifts_fitYesNo = result_tmp;
  result->scales_fitYesNo = result_tmp;
  result->nbairs_fitYesNo = result_tmp;
  result->nbsubs_fitYesNo = result_tmp;
  result->resol_fitYesNo = result_tmp;
  result->domainRatio_fitYesNo = result_tmp;
}

int32_T main(int32_T, char **)
{
  // Initialize the application.
  // You do not need to do this more than one time.
  RAT::reflectivityCalculation_initialize();

  // Invoke the entry-point functions.
  // You can call entry-point functions multiple times.
  main_reflectivityCalculation();

  // Terminate the application.
  // You do not need to do this more than one time.
  RAT::reflectivityCalculation_terminate();
  return 0;
}

void main_reflectivityCalculation()
{
  RAT::cell_11 result;
  RAT::cell_8 problemDefCells;
  RAT::struct0_T problemDef;
  RAT::struct1_T problemDefLimits;
  RAT::struct2_T controls;
  RAT::struct4_T problem;

  // Initialize function 'reflectivityCalculation' input arguments.
  // Initialize function input argument 'problemDef'.
  argInit_struct0_T(&problemDef);

  // Initialize function input argument 'problemDefCells'.
  argInit_cell_8(&problemDefCells);

  // Initialize function input argument 'problemDefLimits'.
  argInit_struct1_T(&problemDefLimits);

  // Initialize function input argument 'controls'.
  argInit_struct2_T(&controls);

  // Call the entry-point 'reflectivityCalculation'.
  RAT::reflectivityCalculation(&problemDef, &problemDefCells, &problemDefLimits,
    &controls, &problem, &result);
}

// End of code generation (main.cpp)
