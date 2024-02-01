//
// Non-Degree Granting Education License -- for use at non-degree
// granting, nonprofit, education, and research organizations only. Not
// for commercial or industrial use.
//
// fMinSearch.cpp
//
// Code generation for function 'fMinSearch'
//

// Include files
#include "fMinSearch.h"
#include "RATMain_internal_types.h"
#include "RATMain_rtwutil.h"
#include "RATMain_types.h"
#include "abs.h"
#include "blockedSummation.h"
#include "eps.h"
#include "minOrMax.h"
#include "rt_nonfinite.h"
#include "simplexIntrafun.h"
#include "sort.h"
#include "sprintf.h"
#include "strcmp.h"
#include "triggerEvent.h"
#include "coder_array.h"
#include "coder_bounded_array.h"
#include <cmath>
#include <stdio.h>

// Function Declarations
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3);
  static void c_binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, int32_T in3, int32_T in4, int32_T in5);
  static void c_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3);
  static void f_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3);
  static void g_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3);
}

// Function Definitions
namespace RAT
{
  static void binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    int32_T b_in3;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    b_in3 = in3.size(1);
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = 3.0 * in2[i * stride_0_0] - 2.0 * in3[i * stride_1_0 + in3.size(0)
        * (b_in3 - 1)];
    }
  }

  static void c_binary_expand_op(::coder::array<real_T, 2U> &in1, const ::coder::
    array<real_T, 2U> &in2, int32_T in3, int32_T in4, int32_T in5)
  {
    ::coder::array<real_T, 2U> b_in2;
    int32_T aux_0_1;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_1;
    if (in5 == 1) {
      i = (in4 - in3) + 1;
    } else {
      i = in5;
    }

    b_in2.set_size(in2.size(0), i);
    stride_0_1 = ((in4 - in3) + 1 != 1);
    aux_0_1 = 0;
    if (in5 == 1) {
      loop_ub = (in4 - in3) + 1;
    } else {
      loop_ub = in5;
    }

    for (i = 0; i < loop_ub; i++) {
      int32_T b_loop_ub;
      b_loop_ub = in2.size(0);
      for (int32_T i1{0}; i1 < b_loop_ub; i1++) {
        b_in2[i1 + b_in2.size(0) * i] = in2[i1 + in2.size(0) * (in3 + aux_0_1)]
          - in2[i1];
      }

      aux_0_1 += stride_0_1;
    }

    coder::c_abs(b_in2, in1);
  }

  static void c_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    int32_T b_in3;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    b_in3 = in3.size(1);
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = 1.5 * in2[i * stride_0_0] - 0.5 * in3[i * stride_1_0 + in3.size(0)
        * (b_in3 - 1)];
    }
  }

  static void f_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    int32_T b_in3;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    b_in3 = in3.size(1);
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = 0.5 * in2[i * stride_0_0] + 0.5 * in3[i * stride_1_0 + in3.size(0)
        * (b_in3 - 1)];
    }
  }

  static void g_binary_expand_op(::coder::array<real_T, 1U> &in1, const ::coder::
    array<real_T, 1U> &in2, const ::coder::array<real_T, 2U> &in3)
  {
    int32_T b_in3;
    int32_T i;
    int32_T loop_ub;
    int32_T stride_0_0;
    int32_T stride_1_0;
    b_in3 = in3.size(1);
    if (in3.size(0) == 1) {
      i = in2.size(0);
    } else {
      i = in3.size(0);
    }

    in1.set_size(i);
    stride_0_0 = (in2.size(0) != 1);
    stride_1_0 = (in3.size(0) != 1);
    if (in3.size(0) == 1) {
      loop_ub = in2.size(0);
    } else {
      loop_ub = in3.size(0);
    }

    for (i = 0; i < loop_ub; i++) {
      in1[i] = 2.0 * in2[i * stride_0_0] - in3[i * stride_1_0 + in3.size(0) *
        (b_in3 - 1)];
    }
  }

  void fMinSearch(::coder::array<real_T, 1U> &x, real_T options_MaxIter, real_T
                  options_MaxFunEvals, real_T options_TolX, real_T
                  options_TolFun, const char_T dis_data[], const int32_T
                  dis_size[2], const c_struct_T *varargin_1, const ::coder::
                  array<cell_wrap_2, 2U> &varargin_2_f1, const ::coder::array<
                  cell_wrap_8, 2U> &varargin_2_f2, const ::coder::array<
                  cell_wrap_2, 2U> &varargin_2_f3, const ::coder::array<
                  cell_wrap_2, 2U> &varargin_2_f4, const ::coder::array<
                  cell_wrap_8, 2U> &varargin_2_f5, const ::coder::array<
                  cell_wrap_8, 1U> &varargin_2_f6, const ::coder::array<
                  cell_wrap_1, 2U> &varargin_2_f14, const ::coder::array<
                  cell_wrap_8, 2U> &varargin_2_f19, const struct2_T *varargin_3,
                  const k_struct_T *varargin_4, real_T *fval, real_T *exitflag,
                  i_struct_T *output)
  {
    static const char_T cv6[33]{ 'N', 'e', 'l', 'd', 'e', 'r', '-', 'M', 'e',
      'a', 'd', ' ', 's', 'i', 'm', 'p', 'l', 'e', 'x', ' ', 'd', 'i', 'r', 'e',
      'c', 't', ' ', 's', 'e', 'a', 'r', 'c', 'h' };

    static const char_T cv11[16]{ 'c', 'o', 'n', 't', 'r', 'a', 'c', 't', ' ',
      'o', 'u', 't', 's', 'i', 'd', 'e' };

    static const char_T cv10[15]{ 'c', 'o', 'n', 't', 'r', 'a', 'c', 't', ' ',
      'i', 'n', 's', 'i', 'd', 'e' };

    static const char_T cv5[15]{ 'i', 'n', 'i', 't', 'i', 'a', 'l', ' ', 's',
      'i', 'm', 'p', 'l', 'e', 'x' };

    static const char_T cv7[7]{ 'r', 'e', 'f', 'l', 'e', 'c', 't' };

    static const char_T b_cv[6]{ 'n', 'o', 't', 'i', 'f', 'y' };

    static const char_T cv8[6]{ 'e', 'x', 'p', 'a', 'n', 'd' };

    static const char_T cv9[6]{ 's', 'h', 'r', 'i', 'n', 'k' };

    static const char_T cv4[5]{ 'f', 'i', 'n', 'a', 'l' };

    static const char_T b_cv1[4]{ 'n', 'o', 'n', 'e' };

    static const char_T cv3[4]{ 'i', 't', 'e', 'r' };

    static const char_T cv2[3]{ 'o', 'f', 'f' };

    ::coder::array<real_T, 2U> b_v;
    ::coder::array<real_T, 2U> c_fv;
    ::coder::array<real_T, 2U> fv;
    ::coder::array<real_T, 2U> r;
    ::coder::array<real_T, 2U> r1;
    ::coder::array<real_T, 2U> r3;
    ::coder::array<real_T, 2U> v;
    ::coder::array<real_T, 1U> c_v;
    ::coder::array<real_T, 1U> r2;
    ::coder::array<real_T, 1U> xbar;
    ::coder::array<real_T, 1U> xc;
    ::coder::array<real_T, 1U> xcc;
    ::coder::array<real_T, 1U> xe;
    ::coder::array<real_T, 1U> xr;
    ::coder::array<real_T, 1U> y;
    ::coder::array<int32_T, 2U> iidx;
    ::coder::array<char_T, 2U> b_varargin_4;
    cell_wrap_9 result[6];
    d_struct_T problem;
    real_T func_evals;
    real_T fxc;
    real_T fxcc;
    real_T fxe;
    real_T fxr;
    real_T itercount;
    int32_T how_size[2];
    int32_T b_index;
    int32_T i;
    int32_T i1;
    int32_T n;
    int32_T prnt;
    int32_T x_idx_1;
    boolean_T exitg1;
    boolean_T printMsg;

    // FMINSEARCH Multidimensional unconstrained nonlinear minimization (Nelder-Mead).
    //    X = FMINSEARCH(FUN,X0) starts at X0 and attempts to find a local minimizer
    //    X of the function FUN.  FUN is a function handle.  FUN accepts input X and
    //    returns a scalar function value F evaluated at X. X0 can be a scalar, vector
    //    or matrix.
    //
    //    X = FMINSEARCH(FUN,X0,OPTIONS)  minimizes with the default optimization
    //    parameters replaced by values in the structure OPTIONS, created
    //    with the OPTIMSET function.  See OPTIMSET for details.  FMINSEARCH uses
    //    these options: Display, TolX, TolFun, MaxFunEvals, MaxIter, FunValCheck,
    //    PlotFcns, and OutputFcn.
    //
    //    X = FMINSEARCH(PROBLEM) finds the minimum for PROBLEM. PROBLEM is a
    //    structure with the function FUN in PROBLEM.objective, the start point
    //    in PROBLEM.x0, the options structure in PROBLEM.options, and solver
    //    name 'fminsearch' in PROBLEM.solver.
    //
    //    [X,FVAL]= FMINSEARCH(...) returns the value of the objective function,
    //    described in FUN, at X.
    //
    //    [X,FVAL,EXITFLAG] = FMINSEARCH(...) returns an EXITFLAG that describes
    //    the exit condition. Possible values of EXITFLAG and the corresponding
    //    exit conditions are
    //
    //     1  Maximum coordinate difference between current best point and other
    //        points in simplex is less than or equal to TolX, and corresponding
    //        difference in function values is less than or equal to TolFun.
    //     0  Maximum number of function evaluations or iterations reached.
    //    -1  Algorithm terminated by the output function.
    //
    //    [X,FVAL,EXITFLAG,OUTPUT] = FMINSEARCH(...) returns a structure
    //    OUTPUT with the number of iterations taken in OUTPUT.iterations, the
    //    number of function evaluations in OUTPUT.funcCount, the algorithm name
    //    in OUTPUT.algorithm, and the exit message in OUTPUT.message.
    //
    //    Examples
    //      FUN can be specified using @:
    //         X = fminsearch(@sin,3)
    //      finds a minimum of the SIN function near 3.
    //      In this case, SIN is a function that returns a scalar function value
    //      SIN evaluated at X.
    //
    //      FUN can be an anonymous function:
    //         X = fminsearch(@(x) norm(x),[1;2;3])
    //      returns a point near the minimizer [0;0;0].
    //
    //      FUN can be a parameterized function. Use an anonymous function to
    //      capture the problem-dependent parameters:
    //         f = @(x,c) x(1).^2+c.*x(2).^2;  % The parameterized function.
    //         c = 1.5;                        % The parameter.
    //         X = fminsearch(@(x) f(x,c),[0.3;1])
    //
    //    FMINSEARCH uses the Nelder-Mead simplex (direct search) method.
    //
    //    See also OPTIMSET, FMINBND, FUNCTION_HANDLE.
    //    Reference: Jeffrey C. Lagarias, James A. Reeds, Margaret H. Wright,
    //    Paul E. Wright, "Convergence Properties of the Nelder-Mead Simplex
    //    Method in Low Dimensions", SIAM Journal of Optimization, 9(1):
    //    p.112-147, 1998.
    //    Copyright 1984-2018 The MathWorks, Inc.
    output->message.set_size(1, 2);
    output->message[0] = 'o';
    output->message[1] = 'k';

    //  If just 'defaults' passed in, return the default options in X
    //  -------- Inputs are standardised for RAT, so no need to check ---------
    //  Detect problem structure input
    //  if nargin == 1
    //      if isa(funfcn,'struct')
    //          [funfcn,x,options] = separateOptimStruct(funfcn);
    //      else % Single input and non-structure
    //          error('MATLAB:fminsearch:InputArg',...
    //              sprintf('MATLAB:optimfun:fminsearch:InputArg'));
    //      end
    //  end
    //
    //  if nargin == 0
    //      error('MATLAB:fminsearch:NotEnoughInputs',...
    //          sprintf('MATLAB:optimfun:fminsearch:NotEnoughInputs'));
    //  end
    //
    //  % Check for non-double inputs
    //  if ~isa(x,'double')
    //    error('MATLAB:fminsearch:NonDoubleInput',...
    //      sprintf('MATLAB:optimfun:fminsearch:NonDoubleInput'));
    //  end
    //  -------------------------------------------------------------------
    //  n = numel(x);
    //  numberOfVariables = n;
    //  ------------- Check is done upstream ----------------
    //  Check that options is a struct
    //  if ~isempty(options) && ~isa(options,'struct')
    //      error('MATLAB:fminsearch:ArgNotStruct',...
    //          getString(message('MATLAB:optimfun:commonMessages:ArgNotStruct', 3)));
    //  end
    //  ------------------- AVH -----------------------------
    //  printtype = optimget(options,'Display',defaultopt,'fast');
    //  In case the defaults were gathered from calling: optimset('fminsearch'):
    //  if ischar(maxfun) || isstring(maxfun)
    //      if strcmpi(maxfun,'200*numberofvariables')
    //          maxfun = 200*numberOfVariables;
    //      else
    //          error('MATLAB:fminsearch:OptMaxFunEvalsNotInteger',...
    //              getString(message('MATLAB:optimfun:fminsearch:OptMaxFunEvalsNotInteger')));
    //      end
    //  end
    //  if ischar(maxiter) || isstring(maxiter)
    //      if strcmpi(maxiter,'200*numberofvariables')
    //          maxiter = 200*numberOfVariables;
    //      else
    //          error('MATLAB:fminsearch:OptMaxIterNotInteger',...
    //              getString(message('MATLAB:optimfun:fminsearch:OptMaxIterNotInteger')));
    //      end
    //  end
    if (coder::internal::b_strcmp(dis_data, dis_size, b_cv)) {
      b_index = 0;
    } else if (coder::internal::c_strcmp(dis_data, dis_size, b_cv1)) {
      b_index = 1;
    } else if (coder::internal::d_strcmp(dis_data, dis_size, cv2)) {
      b_index = 1;
    } else if (coder::internal::c_strcmp(dis_data, dis_size, cv3)) {
      b_index = 2;
    } else if (coder::internal::e_strcmp(dis_data, dis_size, cv4)) {
      b_index = 3;
    } else {
      b_index = -1;
    }

    switch (b_index) {
     case 0:
      //  Changed from TMW fminsearch
      prnt = 1;
      break;

     case 1:
      prnt = 0;
      break;

     case 2:
      prnt = 3;
      break;

     case 3:
      prnt = 2;

      //      case 'simplex'
      //          prnt = 4;
      break;

     default:
      prnt = 1;
      break;
    }

    //  ----------------- Not using output functions for RAT ---------
    //  % Handle the output
    //  outputfcn = optimget(options,'OutputFcn',defaultopt,'fast');
    //  if isempty(outputfcn)
    //      haveoutputfcn = false;
    //  else
    //      haveoutputfcn = true;
    //      xOutputfcn = x; % Last x passed to outputfcn; has the input x's shape
    //      % Parse OutputFcn which is needed to support cell array syntax for OutputFcn.
    //      outputfcn = createCellArrayOfFunctions(outputfcn,'OutputFcn');
    //  end
    //
    //  % Handle the plot
    //  plotfcns = optimget(options,'PlotFcns',defaultopt,'fast');
    //  if isempty(plotfcns)
    //      haveplotfcn = false;
    //  else
    //      haveplotfcn = true;
    //      xOutputfcn = x; % Last x passed to plotfcns; has the input x's shape
    //      % Parse PlotFcns which is needed to support cell array syntax for PlotFcns.
    //      plotfcns = createCellArrayOfFunctions(plotfcns,'PlotFcns');
    //  end
    //  ---------------------- AVH ------------------------------
    //  Convert to function handle as needed.
    //  funfcn = fcnchk(funfcn,length(varargin));
    //  Add a wrapper function to check for Inf/NaN/complex values
    //
    n = x.size(0);

    //  Initialize parameters
    //  two2np1 = 2:n+1;
    //  one2n = 1:n;
    //  Set up a simplex near the initial guess.
    //  Force xin to be a column vector
    b_index = x.size(0);
    x_idx_1 = x.size(0) + 1;
    v.set_size(b_index, x_idx_1);
    for (i = 0; i < x_idx_1; i++) {
      for (i1 = 0; i1 < b_index; i1++) {
        v[i1 + v.size(0) * i] = 0.0;
      }
    }

    x_idx_1 = x.size(0) + 1;
    fv.set_size(1, x_idx_1);
    for (i = 0; i < x_idx_1; i++) {
      fv[i] = 0.0;
    }

    x_idx_1 = x.size(0);
    for (i = 0; i < x_idx_1; i++) {
      v[i] = x[i];
    }

    //  Place input guess in the simplex! (credit L.Pfeffer at Stanford)
    //  Change x to the form expected by funfcn
    simplexIntrafun(x, varargin_1, varargin_2_f1, varargin_2_f2, varargin_2_f3,
                    varargin_2_f4, varargin_2_f5, varargin_2_f6, varargin_2_f14,
                    varargin_2_f19, varargin_3, varargin_4, &fv[0], &problem,
                    result);

    //  Initial simplex setup continues later
    //  Initialize the output and plot functions.
    //
    //  ----------------------------------------
    //  RAT doesn't use output or plot functions...
    //
    //  --------------------- AVH -----------
    //  if haveoutputfcn || haveplotfcn
    //      [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,v(:,1),xOutputfcn,'init',itercount, ...
    //          func_evals, how, fv(:,1),varargin{:});
    //      if stop
    //          [x,fval,exitflag,output] = cleanUpInterrupt(xOutputfcn,optimValues);
    //          if  prnt > 0
    //              fprintf('%s \n', output.message)
    //          end
    //          return;
    //      end
    //  end
    //  Print out initial f(x) as 0th iteration
    if (prnt == 3) {
      printf("\n%s\n", " Iteration   Func-count     min f(x)         Procedure");
      fflush(stdout);
      b_varargin_4.set_size(1, 1);
      b_varargin_4[0] = '\x00';
      printf(" %5.0f        %5.0f     %12.6g         %s\n", 0.0, 1.0, fv[0],
             &b_varargin_4[0]);
      fflush(stdout);

      //  elseif prnt == 4
      //  Option never used in RAT
      //      formatsave.format = get(0,'format');
      //      formatsave.formatspacing = get(0,'formatspacing');
      //      % reset format when done
      //      oc1 = onCleanup(@()set(0,'format',formatsave.format));
      //      oc2 = onCleanup(@()set(0,'formatspacing',formatsave.formatspacing));
      //      format compact
      //      format short e
      //      fprintf('%s \n', ' ')
      //      fprintf('%s \n', how)
      //      fprintf('%s \n', 'v = ')
      //      fprintf('%g \n', v)
      //      fprintf('%s \n', 'fv = ')
      //      fprintf('%g \n', fv)
      //      fprintf('%s \n', 'func_evals = ')
      //      fprintf('%g \n', func_evals)
    }

    triggerEvent(result, problem.ssubs, varargin_1->TF.data, varargin_1->TF.size,
                 varargin_1->resample, varargin_1->dataPresent,
                 varargin_1->modelType.data, varargin_1->modelType.size);

    //  OutputFcn and PlotFcns call
    //  if haveoutputfcn || haveplotfcn
    //      [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,v(:,1),xOutputfcn,'iter',itercount, ...
    //          func_evals, how, fv(:,1),varargin{:});
    //      if stop  % Stop per user request.
    //          [x,fval,exitflag,output] = cleanUpInterrupt(xOutputfcn,optimValues);
    //          if  prnt > 0
    //              fprintf('%s \n', output.message)
    //          end
    //          return;
    //      end
    //  end
    //  Continue setting up the initial simplex.
    //  Following improvement suggested by L.Pfeffer at Stanford
    //  5 percent deltas for non-zero terms
    //  Even smaller delta for zero elements of x
    i = x.size(0);
    for (int32_T j{0}; j < i; j++) {
      x_idx_1 = x.size(0);
      y.set_size(x_idx_1);
      for (i1 = 0; i1 < x_idx_1; i1++) {
        y[i1] = x[i1];
      }

      if (x[j] != 0.0) {
        y[j] = 1.05 * x[j];
      } else {
        y[j] = 0.00025;
      }

      x_idx_1 = y.size(0);
      for (i1 = 0; i1 < x_idx_1; i1++) {
        v[i1 + v.size(0) * (j + 1)] = y[i1];
      }

      simplexIntrafun(y, varargin_1, varargin_2_f1, varargin_2_f2, varargin_2_f3,
                      varargin_2_f4, varargin_2_f5, varargin_2_f6,
                      varargin_2_f14, varargin_2_f19, varargin_3, varargin_4,
                      &fv[j + 1], &problem, result);
    }

    //  sort so v(1,:) has the lowest function value
    coder::internal::sort(fv, iidx);
    b_index = v.size(0);
    b_v.set_size(v.size(0), iidx.size(1));
    x_idx_1 = iidx.size(1);
    for (i = 0; i < x_idx_1; i++) {
      for (i1 = 0; i1 < b_index; i1++) {
        b_v[i1 + b_v.size(0) * i] = v[i1 + v.size(0) * (iidx[i] - 1)];
      }
    }

    v.set_size(b_v.size(0), b_v.size(1));
    x_idx_1 = b_v.size(1);
    for (i = 0; i < x_idx_1; i++) {
      b_index = b_v.size(0);
      for (i1 = 0; i1 < b_index; i1++) {
        v[i1 + v.size(0) * i] = b_v[i1 + b_v.size(0) * i];
      }
    }

    itercount = 1.0;
    func_evals = static_cast<real_T>(x.size(0)) + 1.0;
    if (prnt == 3) {
      b_varargin_4.set_size(1, 16);
      for (i = 0; i < 15; i++) {
        b_varargin_4[i] = cv5[i];
      }

      b_varargin_4[15] = '\x00';
      printf(" %5.0f        %5.0f     %12.6g         %s\n", 1.0,
             static_cast<real_T>(x.size(0)) + 1.0, fv[0], &b_varargin_4[0]);
      fflush(stdout);

      //  elseif prnt == 4
      //      fprintf('%s \n', ' ')
      //      fprintf('%s \n', how)
      //      fprintf('%s \n', 'v = ')
      //      fprintf('%g \n', v)
      //      fprintf('%s \n', 'fv = ')
      //      fprintf('%g \n', fv)
      //      fprintf('%s \n', 'func_evals = ')
      //      fprintf('%g \n', func_evals)
    }

    if (rt_remd_snf(1.0, varargin_3->updatePlotFreq) == 0.0) {
      triggerEvent(result, problem.ssubs, varargin_1->TF.data,
                   varargin_1->TF.size, varargin_1->resample,
                   varargin_1->dataPresent, varargin_1->modelType.data,
                   varargin_1->modelType.size);
    }

    //  OutputFcn and PlotFcns call
    //  if haveoutputfcn || haveplotfcn
    //      [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,v(:,1),xOutputfcn,'iter',itercount, ...
    //          func_evals, how, fv(:,1),varargin{:});
    //      if stop  % Stop per user request.
    //          [x,fval,exitflag,output] = cleanUpInterrupt(xOutputfcn,optimValues);
    //          if  prnt > 0
    //              fprintf('%s \n', output.message)
    //          end
    //          return;
    //      end
    //  end
    //  exitflag = 1;
    //  Main algorithm: iterate until
    //  (a) the maximum coordinate difference between the current best point and the
    //  other points in the simplex is less than or equal to TolX. Specifically,
    //  until max(||v2-v1||,||v3-v1||,...,||v(n+1)-v1||) <= TolX,
    //  where ||.|| is the infinity-norm, and v1 holds the
    //  vertex with the current lowest value; AND
    //  (b) the corresponding difference in function values is less than or equal
    //  to TolFun. (Cannot use OR instead of AND.)
    //  The iteration stops if the maximum number of iterations or function evaluations
    //  are exceeded
    exitg1 = false;
    while ((!exitg1) && ((func_evals < options_MaxFunEvals) && (itercount <
             options_MaxIter))) {
      real_T b_fv;
      boolean_T guard1;
      if (n + 1 < 2) {
        i = 0;
        i1 = -1;
      } else {
        i = 1;
        i1 = n;
      }

      b_fv = fv[0];
      x_idx_1 = i1 - i;
      c_fv.set_size(1, x_idx_1 + 1);
      for (i1 = 0; i1 <= x_idx_1; i1++) {
        c_fv[i1] = b_fv - fv[i + i1];
      }

      coder::b_abs(c_fv, r);
      guard1 = false;
      if (coder::internal::maximum(r) <= std::fmax(options_TolFun, 10.0 * coder::
           eps(fv[0]))) {
        if (n + 1 < 2) {
          i = 0;
          i1 = -1;
        } else {
          i = 1;
          i1 = n;
        }

        x_idx_1 = i1 - i;
        if (x_idx_1 + 1 == n) {
          b_v.set_size(v.size(0), x_idx_1 + 1);
          for (i1 = 0; i1 <= x_idx_1; i1++) {
            b_index = v.size(0);
            for (int32_T i2{0}; i2 < b_index; i2++) {
              b_v[i2 + b_v.size(0) * i1] = v[i2 + v.size(0) * (i + i1)] - v[i2];
            }
          }

          coder::c_abs(b_v, r1);
        } else {
          c_binary_expand_op(r1, v, i, i1, n);
        }

        c_v.set_size(v.size(0));
        x_idx_1 = v.size(0);
        for (i = 0; i < x_idx_1; i++) {
          c_v[i] = v[i];
        }

        coder::internal::maximum(r1, r3);
        if (coder::internal::maximum(r3) <= std::fmax(options_TolX, 10.0 * coder::
             eps(coder::internal::maximum(c_v)))) {
          exitg1 = true;
        } else {
          guard1 = true;
        }
      } else {
        guard1 = true;
      }

      if (guard1) {
        char_T how_data[16];

        //  Compute the reflection point
        //  xbar = average of the n (NOT n+1) best points
        if (n < 1) {
          x_idx_1 = 0;
        } else {
          x_idx_1 = n;
        }

        b_v.set_size(v.size(0), x_idx_1);
        for (i = 0; i < x_idx_1; i++) {
          b_index = v.size(0);
          for (i1 = 0; i1 < b_index; i1++) {
            b_v[i1 + b_v.size(0) * i] = v[i1 + v.size(0) * i];
          }
        }

        coder::blockedSummation(b_v, x_idx_1, r2);
        xbar.set_size(r2.size(0));
        x_idx_1 = r2.size(0);
        for (i = 0; i < x_idx_1; i++) {
          xbar[i] = r2[i] / static_cast<real_T>(n);
        }

        if (xbar.size(0) == v.size(0)) {
          xr.set_size(xbar.size(0));
          x_idx_1 = xbar.size(0);
          for (i = 0; i < x_idx_1; i++) {
            xr[i] = 2.0 * xbar[i] - v[i + v.size(0) * (v.size(1) - 1)];
          }
        } else {
          g_binary_expand_op(xr, xbar, v);
        }

        simplexIntrafun(xr, varargin_1, varargin_2_f1, varargin_2_f2,
                        varargin_2_f3, varargin_2_f4, varargin_2_f5,
                        varargin_2_f6, varargin_2_f14, varargin_2_f19,
                        varargin_3, varargin_4, &fxr, &problem, result);
        func_evals++;
        if (fxr < fv[0]) {
          //  Calculate the expansion point
          if (xbar.size(0) == v.size(0)) {
            xe.set_size(xbar.size(0));
            x_idx_1 = xbar.size(0);
            for (i = 0; i < x_idx_1; i++) {
              xe[i] = 3.0 * xbar[i] - 2.0 * v[i + v.size(0) * (v.size(1) - 1)];
            }
          } else {
            binary_expand_op(xe, xbar, v);
          }

          simplexIntrafun(xe, varargin_1, varargin_2_f1, varargin_2_f2,
                          varargin_2_f3, varargin_2_f4, varargin_2_f5,
                          varargin_2_f6, varargin_2_f14, varargin_2_f19,
                          varargin_3, varargin_4, &fxe, &problem, result);
          func_evals++;
          if (fxe < fxr) {
            x_idx_1 = xe.size(0);
            for (i = 0; i < x_idx_1; i++) {
              v[i + v.size(0) * (v.size(1) - 1)] = xe[i];
            }

            fv[fv.size(1) - 1] = fxe;
            how_size[1] = 6;
            for (i = 0; i < 6; i++) {
              how_data[i] = cv8[i];
            }
          } else {
            x_idx_1 = xr.size(0);
            for (i = 0; i < x_idx_1; i++) {
              v[i + v.size(0) * (v.size(1) - 1)] = xr[i];
            }

            fv[fv.size(1) - 1] = fxr;
            how_size[1] = 7;
            for (i = 0; i < 7; i++) {
              how_data[i] = cv7[i];
            }
          }

          //  fv(:,1) <= fxr
        } else if (fxr < fv[n - 1]) {
          x_idx_1 = xr.size(0);
          for (i = 0; i < x_idx_1; i++) {
            v[i + v.size(0) * (v.size(1) - 1)] = xr[i];
          }

          fv[fv.size(1) - 1] = fxr;
          how_size[1] = 7;
          for (i = 0; i < 7; i++) {
            how_data[i] = cv7[i];
          }
        } else {
          //  fxr >= fv(:,n)
          //  Perform contraction
          if (fxr < fv[fv.size(1) - 1]) {
            //  Perform an outside contraction
            if (xbar.size(0) == v.size(0)) {
              xc.set_size(xbar.size(0));
              x_idx_1 = xbar.size(0);
              for (i = 0; i < x_idx_1; i++) {
                xc[i] = 1.5 * xbar[i] - 0.5 * v[i + v.size(0) * (v.size(1) - 1)];
              }
            } else {
              c_binary_expand_op(xc, xbar, v);
            }

            simplexIntrafun(xc, varargin_1, varargin_2_f1, varargin_2_f2,
                            varargin_2_f3, varargin_2_f4, varargin_2_f5,
                            varargin_2_f6, varargin_2_f14, varargin_2_f19,
                            varargin_3, varargin_4, &fxc, &problem, result);
            func_evals++;
            if (fxc <= fxr) {
              x_idx_1 = xc.size(0);
              for (i = 0; i < x_idx_1; i++) {
                v[i + v.size(0) * (v.size(1) - 1)] = xc[i];
              }

              fv[fv.size(1) - 1] = fxc;
              how_size[0] = 1;
              how_size[1] = 16;
              for (i = 0; i < 16; i++) {
                how_data[i] = cv11[i];
              }
            } else {
              //  perform a shrink
              how_size[0] = 1;
              how_size[1] = 6;
              for (i = 0; i < 6; i++) {
                how_data[i] = cv9[i];
              }
            }
          } else {
            //  Perform an inside contraction
            if (xbar.size(0) == v.size(0)) {
              xcc.set_size(xbar.size(0));
              x_idx_1 = xbar.size(0);
              for (i = 0; i < x_idx_1; i++) {
                xcc[i] = 0.5 * xbar[i] + 0.5 * v[i + v.size(0) * (v.size(1) - 1)];
              }
            } else {
              f_binary_expand_op(xcc, xbar, v);
            }

            simplexIntrafun(xcc, varargin_1, varargin_2_f1, varargin_2_f2,
                            varargin_2_f3, varargin_2_f4, varargin_2_f5,
                            varargin_2_f6, varargin_2_f14, varargin_2_f19,
                            varargin_3, varargin_4, &fxcc, &problem, result);
            func_evals++;
            if (fxcc < fv[fv.size(1) - 1]) {
              x_idx_1 = xcc.size(0);
              for (i = 0; i < x_idx_1; i++) {
                v[i + v.size(0) * (v.size(1) - 1)] = xcc[i];
              }

              fv[fv.size(1) - 1] = fxcc;
              how_size[0] = 1;
              how_size[1] = 15;
              for (i = 0; i < 15; i++) {
                how_data[i] = cv10[i];
              }
            } else {
              //  perform a shrink
              how_size[0] = 1;
              how_size[1] = 6;
              for (i = 0; i < 6; i++) {
                how_data[i] = cv9[i];
              }
            }
          }

          if (coder::internal::v_strcmp(how_data, how_size)) {
            for (int32_T j{0}; j < n; j++) {
              x_idx_1 = v.size(0);
              c_v.set_size(v.size(0));
              for (i = 0; i < x_idx_1; i++) {
                b_fv = v[i];
                c_v[i] = b_fv + 0.5 * (v[i + v.size(0) * (j + 1)] - b_fv);
              }

              x_idx_1 = c_v.size(0);
              for (i = 0; i < x_idx_1; i++) {
                v[i + v.size(0) * (j + 1)] = c_v[i];
              }

              x_idx_1 = v.size(0);
              c_v.set_size(v.size(0));
              for (i = 0; i < x_idx_1; i++) {
                c_v[i] = v[i + v.size(0) * (j + 1)];
              }

              simplexIntrafun(c_v, varargin_1, varargin_2_f1, varargin_2_f2,
                              varargin_2_f3, varargin_2_f4, varargin_2_f5,
                              varargin_2_f6, varargin_2_f14, varargin_2_f19,
                              varargin_3, varargin_4, &fv[j + 1], &problem,
                              result);
            }

            func_evals += static_cast<real_T>(n);
          }
        }

        coder::internal::sort(fv, iidx);
        b_index = v.size(0);
        b_v.set_size(v.size(0), iidx.size(1));
        x_idx_1 = iidx.size(1);
        for (i = 0; i < x_idx_1; i++) {
          for (i1 = 0; i1 < b_index; i1++) {
            b_v[i1 + b_v.size(0) * i] = v[i1 + v.size(0) * (iidx[i] - 1)];
          }
        }

        v.set_size(b_v.size(0), b_v.size(1));
        x_idx_1 = b_v.size(1);
        for (i = 0; i < x_idx_1; i++) {
          b_index = b_v.size(0);
          for (i1 = 0; i1 < b_index; i1++) {
            v[i1 + v.size(0) * i] = b_v[i1 + b_v.size(0) * i];
          }
        }

        itercount++;
        if (prnt == 3) {
          b_varargin_4.set_size(1, how_size[1] + 1);
          x_idx_1 = how_size[1];
          for (i = 0; i < x_idx_1; i++) {
            b_varargin_4[i] = how_data[i];
          }

          b_varargin_4[how_size[1]] = '\x00';
          printf(" %5.0f        %5.0f     %12.6g         %s\n", itercount,
                 func_evals, fv[0], &b_varargin_4[0]);
          fflush(stdout);

          //      elseif prnt == 4
          //          fprintf('%s \n', ' ')
          //          fprintf('%s \n', num2str(how))
          //          fprintf('%s \n', 'v = ')
          //          fprintf('%s \n', v)
          //          fprintf('%s \n', 'fv = ')
          //          fprintf('%s \n', fv)
          //          fprintf('%s \n', 'func_evals = ')
          //          fprintf('%s \n', num2str(func_evals))
        }

        if (rt_remd_snf(itercount, varargin_3->updatePlotFreq) == 0.0) {
          triggerEvent(result, problem.ssubs, varargin_1->TF.data,
                       varargin_1->TF.size, varargin_1->resample,
                       varargin_1->dataPresent, varargin_1->modelType.data,
                       varargin_1->modelType.size);
        }

        //  OutputFcn and PlotFcns call
        //      if haveoutputfcn || haveplotfcn
        //          [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,v(:,1),xOutputfcn,'iter',itercount, ...
        //              func_evals, how, fv(:,1),varargin{:});
        //          if stop  % Stop per user request.
        //              [x,fval,exitflag,output] = cleanUpInterrupt(xOutputfcn,optimValues);
        //              if  prnt > 0
        //                  fprintf('%s \n', output.message)
        //              end
        //              return;
        //          end
        //      end
      }
    }

    //  while
    x.set_size(v.size(0));
    x_idx_1 = v.size(0);
    for (i = 0; i < x_idx_1; i++) {
      x[i] = v[i];
    }

    *fval = fv[0];

    //  OutputFcn and PlotFcns call
    //  if haveoutputfcn || haveplotfcn
    //      callOutputAndPlotFcns(outputfcn,plotfcns,x,xOutputfcn,'done',itercount, func_evals, how, fval, varargin{:});
    //  end
    if (func_evals >= options_MaxFunEvals) {
      printMsg = (prnt > 0);

      // msg = getString(message('MATLAB:optimfun:fminsearch:ExitingMaxFunctionEvals', sprintf('%f',fval)));
      b_index = 0;
    } else if (itercount >= options_MaxIter) {
      printMsg = (prnt > 0);

      // msg = getString(message('MATLAB:optimfun:fminsearch:ExitingMaxIterations', sprintf('%f',fval)));
      b_index = 0;
    } else {
      printMsg = (prnt > 1);
      coder::b_sprintf(options_TolX, options_TolFun, output->message);
      b_index = 1;
    }

    output->iterations = itercount;
    output->funcCount = func_evals;
    for (i = 0; i < 33; i++) {
      output->algorithm[i] = cv6[i];
    }

    if (printMsg) {
      b_varargin_4.set_size(1, output->message.size(1) + 1);
      x_idx_1 = output->message.size(1);
      for (i = 0; i < x_idx_1; i++) {
        b_varargin_4[i] = output->message[i];
      }

      b_varargin_4[output->message.size(1)] = '\x00';
      printf("\n%s\n", &b_varargin_4[0]);
      fflush(stdout);
    }

    // --------------------------------------------------------------------------
    //  function [xOutputfcn, optimValues, stop] = callOutputAndPlotFcns(outputfcn,plotfcns,x,xOutputfcn,state,iter,...
    //      numf,how,f,varargin)
    //  CALLOUTPUTANDPLOTFCNS assigns values to the struct OptimValues and then calls the
    //  outputfcn/plotfcns.
    //
    //  state - can have the values 'init','iter', or 'done'.
    //  For the 'done' state we do not check the value of 'stop' because the
    //  optimization is already done.
    //  optimValues.iteration = iter;
    //  optimValues.funccount = numf;
    //  optimValues.fval = f;
    //  optimValues.procedure = how;
    //  xOutputfcn(:) = x;  % Set x to have user expected size
    //  stop = false;
    //  state = char(state);
    //  Call output functions
    //  ---- Remove these from function for compile - AVH
    //  if ~isempty(outputfcn)
    //      switch state
    //          case {'iter','init'}
    //              stop = callAllOptimOutputFcns(outputfcn,xOutputfcn,optimValues,state,varargin{:}) || stop;
    //          case 'done'
    //              callAllOptimOutputFcns(outputfcn,xOutputfcn,optimValues,state,varargin{:});
    //      end
    //  end
    //  % Call plot functions
    //  if ~isempty(plotfcns)
    //      switch state
    //          case {'iter','init'}
    //              stop = callAllOptimPlotFcns(plotfcns,xOutputfcn,optimValues,state,varargin{:}) || stop;
    //          case 'done'
    //              callAllOptimPlotFcns(plotfcns,xOutputfcn,optimValues,state,varargin{:});
    //      end
    //  end
    //  -----------------------------------
    // --------------------------------------------------------------------------
    //  function [x,FVAL,EXITFLAG,OUTPUT] = cleanUpInterrupt(xOutputfcn,optimValues)
    //  CLEANUPINTERRUPT updates or sets all the output arguments of FMINBND when the optimization
    //  is interrupted.
    //  Call plot function driver to finalize the plot function figure window. If
    //  no plot functions have been specified or the plot function figure no
    //  longer exists, this call just returns.
    //  callAllOptimPlotFcns('cleanuponstopsignal');
    //  x = xOutputfcn;
    //  FVAL = optimValues.fval;
    //  EXITFLAG = -1;
    //  OUTPUT.iterations = optimValues.iteration;
    //  OUTPUT.funcCount = optimValues.funccount;
    //  OUTPUT.algorithm = 'Nelder-Mead simplex direct search';
    //  OUTPUT.message = fprintf('Optimisation terminated by user'); %getString(message('MATLAB:optimfun:fminsearch:OptimizationTerminatedPrematurelyByUser'));
    // --------------------------------------------------------------------------
    //  function f = checkfun(x,userfcn,varargin)
    //  CHECKFUN checks for complex or NaN results from userfcn.
    //  f = userfcn(x,varargin{:});
    //  Note: we do not check for Inf as FMINSEARCH handles it naturally.
    //  if isnan(f)
    //      error('MATLAB:fminsearch:checkfun:NaNFval','Target function is NaN');
    //  elseif ~isreal(f)
    //      error('MATLAB:fminsearch:checkfun:ComplexFval',...
    //          getString(message('MATLAB:optimfun:fminsearch:checkfun:ComplexFval', localChar( userfcn ))));
    //          error(sprintf('Target function is complex'));
    //  end
    // --------------------------------------------------------------------------
    //  function strfcn = localChar(fcn)
    //  % Convert the fcn to a character array for printing
    //
    //  if ischar(fcn)
    //      strfcn = fcn;
    //  elseif isstring(fcn) || isa(fcn,'inline')
    //      strfcn = char(fcn);
    //  elseif isa(fcn,'function_handle')
    //      strfcn = func2str(fcn);
    //  else
    //      try
    //          strfcn = char(fcn);
    //      catch
    //          strfcn = getString(message('MATLAB:optimfun:fminsearch:NameNotPrintable'));
    //      end
    //  end
    *exitflag = b_index;
  }
}

// End of code generation (fMinSearch.cpp)
