#ifndef RAT_DEFINES_H
#define RAT_DEFINES_H

#include <cstdlib>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

const std::string docsProgressEventData = R"(The Python binding for the C++ progressEventData struct. 
The progress event shows the percentage completion for the calculation. This can be emitted by 
the DREAM algorithm only.

Parameters
----------
message : str
    The title text for the event.
percent : float
    The percentage of the calculation completed (as a number between 0 and 1)
)";

struct ProgressEventData
{
    std::string message;
    double percent;
};

const std::string docsPlotEventData = R"(The Python binding for the C++ plotEventData struct. 
The plot event data contains intermediate results from the calculation. This can be emitted 
by the Simplex and DE algorithms only.

Parameters
----------
reflectivity : list
    The reflectivity curves for each contrast, with the same range as the data 
    (``data_range`` in the contrast's ``Data`` object)
shiftedData : list
    The data with scalefactors and background corrections applied.
sldProfiles : list
    The SLD profiles for each contrast.
resampledLayers : list
    If resampling is used, the SLD for each contrast after resampling has been performed.
subRoughs : np.ndarray[np.float]
    The substrate roughness values for each contrast.
resample : np.ndarray[np.float]
    An array containing whether each contrast was resampled.
dataPresent : np.ndarray[np.float]
    Non-zero values indicates if data is present for the contrast. 
modelType : str
    The model type for the project.
contrastNames : list
    The names of all contrasts in the project.
)";

struct PlotEventData
{
    py::list reflectivity;
    py::list shiftedData;
    py::list sldProfiles;
    py::list resampledLayers;
    py::array_t<double> subRoughs;
    py::array_t<double> resample;
    py::array_t<double> dataPresent;
    std::string modelType;
    py::list contrastNames;
};


const std::string docsPredictionIntervals = R"(The Python binding for the C++ predictionIntervals struct. 
The Bayesian prediction intervals for 95% and 65% confidence.

For ``reflectivity`` and ``sld``, each list item is an array
with five rows. The rows represent:

- 0: the 5th percentile;
- 1: the 35th percentile;
- 2: the mean value of the interval;
- 3: the 65th percentile;
- 4: the 95th percentile.

Parameters
----------
reflectivity : list
    The prediction interval data for reflectivity of each contrast.
sld : list
    The prediction interval data for SLD of each contrast.
sampleChi : np.ndarray[np.float]
    The value of sumChi at each point of the Markov chain.
)";

struct PredictionIntervals
{
    py::list reflectivity;
    py::list sld;
    py::array_t<real_T> sampleChi; 
};

const std::string docsConfidenceIntervals = R"(The Python binding for the C++ confidenceIntervals struct. 
The 65% and 95% confidence intervals for the best fit results.

Parameters
----------
percentile95 : np.ndarray[np.float]
    The 95% confidence intervals for each fit parameter.
percentile65 : np.ndarray[np.float]
    The 65% confidence intervals for each fit parameter.
mean : np.ndarray[np.float]
    The mean values for each fit parameter.
)";

struct ConfidenceIntervals
{
    py::array_t<real_T> percentile95;
    py::array_t<real_T> percentile65;
    py::array_t<real_T> mean;
};

const std::string docsNestedSamplerOutput = R"(The Python binding for the C++ nestedSamplerOutput struct. 
The output information from the Nested Sampler (ns).

Parameters
----------
logZ : float
    The natural logarithm of the evidence Z for the parameter values.
logZErr : float
    The estimated uncertainty in the final value of logZ.
nestSamples : np.ndarray[np.float]
    ``NestedSamplerOutput.nestSamples[i, j]`` represents the values
    sampled at iteration ``i``, where this value is:

    - the value sampled for parameter ``j``, for ``j`` in ``0:nParams``,
    - the minimum log-likelihood for ``j = nParams + 1``.

postSamples : np.ndarray[np.float]
    The posterior values at the points sampled in ``NestedSamplerOutput.nestSamples``.
)";

struct NestedSamplerOutput
{
    real_T logZ;
    real_T logZErr;
    py::array_t<real_T> nestSamples;
    py::array_t<real_T> postSamples;
};

const std::string docsDreamParams = R"(The Python binding for the C++ dreamParams struct. 
The parameters used by the inner DREAM algorithm.

Parameters
----------
nParams : float
    The number of parameters used by the algorithm.
nChains : float
    The number of MCMC chains used by the algorithm.
nGenerations : float
    The number of DE generations calculated per iteration.
parallel : bool
    Whether the algorithm should run chains in parallel.
CPU : float
    The number of processor cores used for parallel chains.
jumpProbability : float
    A probability range for the size of jumps when performing subspace sampling.
pUnitGamma : float
    The probability that the scaling-down factor of jumps will be ignored
    and a larger jump will be taken for one iteration.
nCR : float
    The number of crossovers performed each iteration.
delta : float
    The number of chain mutation pairs proposed each iteration.
steps : float
    The number of MCMC steps to perform between conversion checks.
zeta : float
    The ergodicity of the algorithm.
outlier : str
    What test should be used to detect outliers.
adaptPCR : bool
    Whether the crossover probability for differential evolution should be
    adapted by the algorithm as it runs.
thinning : float
    The thinning rate of each Markov chain (to reduce memory intensity)
epsilon : float
    The cutoff threshold for Approximate Bayesian Computation (if used)
ABC : bool
    Whether Approximate Bayesian Computation is used.
IO : bool
    Whether the algorithm should perform IO writes of the model in parallel.
storeOutput : bool
    Whether output model simulations are performed.
R : np.np.ndarray[np.float]
    An array where row ``i`` is the list of chains
    with which chain ``i`` can mutate.
)";

struct DreamParams
{
    real_T nParams;
    real_T nChains;
    real_T nGenerations;
    boolean_T parallel;
    real_T CPU;
    real_T jumpProbability;
    real_T pUnitGamma;
    real_T nCR;
    real_T delta;
    real_T steps;
    real_T zeta;
    std::string outlier;
    boolean_T adaptPCR;
    real_T thinning;
    real_T epsilon;
    boolean_T ABC;
    boolean_T IO;
    boolean_T storeOutput;
    py::array_t<real_T> R;
};

const std::string docsDreamOutput = R"(The Python binding for the C++ DreamOutput struct. 
The diagnostic output information from DREAM.

Parameters
----------
allChains : np.ndarray[np.float]
    An ``nGenerations`` x ``nParams + 2`` x ``nChains`` size array,
    where ``chain_k = DreamOutput.allChains[:, :, k]``
    is the data of chain ``k`` in the final iteration;
    for generation i of the final iteration, ``chain_k[i, j]`` represents:

    - the sampled value of parameter ``j`` for ``j in 0:nParams``;
    - the associated log-prior for those sampled values for ``j = nParams + 1``;
    - the associated log-likelihood for those sampled values for ``j = nParams + 2``.

outlierChains : np.ndarray[np.float]
    A two-column array where ``DreamOutput.AR[i, 1]`` is the index of a chain
    and ``DreamOutput.AR[i, 0]`` is the length of that chain when it was removed
    for being an outlier.
runtime : float
    The runtime of the DREAM algorithm in seconds.
iteration : float
    The number of iterations performed.
AR : np.ndarray[np.float]
    A two-column array where ``DreamOutput.AR[i, 0]`` is an iteration number
    and ``DreamOutput.AR[i, 1]`` is the average acceptance rate of chain step
    proposals for that iteration.
R_stat : np.ndarray[np.float]
    An array where ``DreamOutput.R_stat[i, 0]`` is an iteration number and
    ``DreamOutput.R_stat[i, j]`` is the convergence statistic for parameter ``j``
    at that iteration (where chains are indexed 1 to ``nParams`` inclusive).
CR : np.ndarray[np.float]
    A four-column array where ``DreamOutput.CR[i, 0]`` is an iteration number,
    ``and DreamOutput.CR[i, j]`` is the selection probability of the ``j``'th crossover
    value for that iteration.
)";

struct DreamOutput
{
    py::array_t<real_T> allChains;
    py::array_t<real_T> outlierChains;
    real_T runtime;
    real_T iteration;
    py::array_t<real_T> AR;
    py::array_t<real_T> R_stat;
    py::array_t<real_T> CR;
};

const std::string docsOutputBayesResult = R"(The Python binding for the C++ bayesResults struct. 
The results of a Bayesian RAT calculation.

Parameters
----------
predictionIntervals : RATapi.rat_core.PredictionIntervals
    The prediction intervals.
confidenceIntervals : RATapi.rat_core.ConfidenceIntervals
    The 65% and 95% confidence intervals for the best fit results.
dreamParams : RATapi.rat_core.DreamParams
    The parameters used by DREAM, if relevant.
dreamOutput : RATapi.rat_core.DreamOutput
    The output from DREAM if DREAM was used.
nestedSamplerOutput : RATapi.rat_core.NestedSamplerOutput
    The output from nested sampling if ns was used.
chain : np.ndarray
    The MCMC chains for each parameter.
    The ``i``'th column of the array contains the chain for parameter ``fitNames[i]``.
)";

struct OutputBayesResult
{
    PredictionIntervals predictionIntervals;
    ConfidenceIntervals confidenceIntervals;
    DreamParams dreamParams;
    DreamOutput dreamOutput;
    NestedSamplerOutput nestedSamplerOutput;
    py::array_t<real_T> chain;
};

const std::string docsCalculation = R"(The Python binding for the C++ calculationResult struct.
The goodness of fit from the Abeles calculation.

Parameters
----------
chiValues : np.ndarray[np.float]
    The chi-squared value for each contrast.
sumChi : float
    The sum of the chiValues array.
)";

struct Calculation
{
    py::array_t<real_T> chiValues;
    real_T sumChi;
};

const std::string docsContrastParams = R"(The Python binding for the C++ contrastParams struct.
The experimental parameters for each contrast.

Parameters
----------
scalefactors : np.ndarray[np.float]
    The scalefactor values for each contrast.
bulkIn : np.ndarray[np.float]
    The bulk in values for each contrast.
bulkOut : np.ndarray[np.float]
    The bulk out values for each contrast.
subRoughs : np.ndarray[np.float]
    The substrate roughness values for each contrast.
resample : np.ndarray[np.float]
    An array containing whether each contrast was resampled.
)";

struct ContrastParams
{
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIn;
    py::array_t<real_T> bulkOut;
    py::array_t<real_T> subRoughs;
    py::array_t<real_T> resample;
};

const std::string docsOutputResult = R"(The C++ result struct of a RAT calculation.

Parameters
----------
reflectivity : list
    The reflectivity curves for each contrast, with the same range as the data 
    (``data_range`` in the contrast's ``Data`` object)
simulation : list
    The reflectivity curves for each contrast, which can be a wider range to allow extrapolation
    (``simulation_range`` in the contrast's ``Data`` object).
shiftedData : list
    The data with scalefactors and background corrections applied.
backgrounds : list
    The background for each contrast defined over the simulation range.
resolutions : list
    The resolution for each contrast defined over the simulation range.
sldProfiles : list
    The SLD profiles for each contrast.
layers : list
    The array of layer parameter values for each contrast.
resampledLayers : list
    If resampling is used, the array of layer parameter values for each contrast after resampling has been performed.
calculationResults : RATapi.rat_core.Calculation
    The chi-squared fit results from the final calculation and fit.
contrastParams : RATapi.rat_core.ContrastParams
    The experimental parameters for the contrasts.
fitParams : np.ndarray[np.float]
    The best fit value of the parameter with name ``fitNames[i]``.
fitNames : list[str]
    The names of the fit parameters, where ``fitNames[i]`` is the name
    of the parameter with value given in ``fitParams[i]``.
)";

struct OutputResult {
    py::list reflectivity;
    py::list simulation;
    py::list shiftedData;
    py::list backgrounds;
    py::list resolutions;
    py::list sldProfiles;
    py::list layers;
    py::list resampledLayers;
    Calculation calculationResults {};
    ContrastParams contrastParams {};
    py::array_t<real_T> fitParams;
    py::list fitNames;
};

const std::string docsNameStore = R"(The Python binding for the C++ names struct which 
contains names of all parameters in the project.

Parameters
----------
params : list 
    Names of params in the problem definition.
backgroundParams : list 
    Names of backgroundParams in the problem definition.
scalefactors : list 
    Names of scalefactors in the problem definition.
bulkIns : list 
    Names of bulkIns in the problem definition.
bulkName: list 
    Names of bulkOuts in the problem definition.
resolutionParams : list 
    Names of resolutionParams in the problem definition.
domainRatios : list 
    Names of domainRatios in the problem definition.
)";

struct NameStore {
    py::list params;
    py::list backgroundParams;
    py::list scalefactors;
    py::list bulkIns;
    py::list bulkOuts;
    py::list resolutionParams;
    py::list domainRatios;
    py::list contrasts;
};


const std::string docsChecks = R"(The Python binding for the C++ checks struct which contains 
flags indicating which parameters should be fitted in the project.

For each attribute, if index ``i`` is non-zero, then parameter ``i`` in that attribute is fitted, e.g. if ``Checks.scalefactors = [0.0, 1.0, 1.0]``, then the second and third scalefactors are fitted and the first is not.

Parameters
----------
params : np.ndarray[np.float] 
    Non-zero values indicates which params is fitted.
backgroundParams : np.ndarray[np.float] 
    Non-zero values indicates which backgroundParams is fitted.
scalefactors : np.ndarray[np.float] 
    Non-zero values indicates which scalefactors is fitted.
bulkIns : np.ndarray[np.float] 
    Non-zero values indicates which bulkIns is fitted.
bulkOuts : np.ndarray[np.float] 
    Non-zero values indicates which bulkOuts is fitted.
resolutionParams : np.ndarray[np.float] 
    Non-zero values indicates which resolutionParams is fitted.
domainRatios : np.ndarray[np.float]     
    Non-zero values indicates which domainRatios is fitted.
)";

struct Checks {
    py::array_t<real_T> params;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIns;
    py::array_t<real_T> bulkOuts;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> domainRatios;
};

const std::string docsProblemDefinition = R"(The Python binding for the C++ problem struct.

Parameters
----------
TF : str
    The target function for the calculation which can be 'normal' or 'domains'.
resample : np.ndarray[np.float] 
    If ``resample[i]`` is non-zero, then contrast ``i`` will be resampled.
data : list
    Data for each contrast.
dataPresent : np.ndarray[np.float] 
    If ``dataPresent[i]`` is non-zero, then contrast ``i`` has experimental data.
dataLimits : list
    Data limits for each contrast.
simulationLimits : list;
    Simulation for each contrast.
numberOfContrasts : int
    Number of contrasts.
geometry : str
    The geometry to use which can be 'air/substrate' or 'substrate/liquid'
useImaginary : bool
    Indicates whether imaginary component is used for the SLD value in layers, i.e.
    absorption is set to True for the project.
repeatLayers : list
    Information about repeating layers for each contrast. This is currently not being used.
contrastBackgroundParams : list
    Indices of backgroundParams used for each contrast
contrastBackgroundTypes : list
    Background type for each contrast.
contrastBackgroundActions : list
    Background action for each contrast.
contrastScalefactors : np.ndarray[np.float] 
    Indices of scalefactors used for each contrast.
contrastBulkIns : np.ndarray[np.float] 
    Indices of BulkIns used for each contrast.
contrastBulkOuts : np.ndarray[np.float] 
    Indices of BulkIns used for each contrast.
contrastResolutionParams : list 
    Indices of resolutionParams used for each contrast
contrastResolutionTypes : list
    Resolution type for each contrast.
backgroundParams : np.ndarray[np.float] 
    Background parameter values.
scalefactors : np.ndarray[np.float] 
    Scalefactors values.
bulkIns : np.ndarray[np.float]
    BulkIn values.
bulkOuts : np.ndarray[np.float]
    BulkOut values.
resolutionParams : np.ndarray[np.float] 
    Resolution parameter values.
params : np.ndarray[np.float] 
    Parameter values.
numberOfLayers : int 
    Number of layers.
contrastLayers : list
    Indices of layers added to the model of each contrast. 
layersDetails : list
    Indices of parameters in each layer.
customFiles : object
    Iterable with custom file functions
modelType : str
   The layer model type which can be 'standard layers', 'custom layers', or 'custom xy'.
contrastCustomFiles : np.ndarray[np.float] 
    Indices of CustomFiles used for each  domain contrast 
contrastDomainRatios : np.ndarray[np.float] 
    Indices of DomainRatios used for each  domain contrast    
domainRatios : np.ndarray[np.float] 
    Domain ratio values
numberOfDomainContrasts : int
    Number of domain contrasts.
domainContrastLayers : list
    Indices of layers added to the model of each domain contrast.
fitParams : np.ndarray[np.float] 
    Values of fitted parameters.
fitLimits : np.ndarray[np.float] 
    Limits of fitted parameters.
priorNames : list 
    Parameter names for for all parameters in the problem definition.
priorValues : np.ndarray[np.float] 
    Prior type, mu, and sigma for all parameters in the problem definition.
names : RATapi.rat_core.NameStore
    Names of all parameters.
checks : RATapi.rat_core.Checks 
    Flags indicating which parameters should be fitted.
)";

struct ProblemDefinition {
    std::string  TF {};
    py::array_t<real_T> resample;
    py::list data;
    py::array_t<real_T> dataPresent;
    py::list dataLimits;
    py::list simulationLimits;
    real_T numberOfContrasts;
    std::string  geometry {};
    boolean_T useImaginary {};
    py::list repeatLayers;
    py::list contrastBackgroundParams;
    py::list contrastBackgroundTypes;
    py::list contrastBackgroundActions;
    py::array_t<real_T> contrastScalefactors;
    py::array_t<real_T> contrastBulkIns;
    py::array_t<real_T> contrastBulkOuts;
    py::list contrastResolutionParams;
    py::list contrastResolutionTypes;
    py::array_t<real_T> backgroundParams;
    py::array_t<real_T> scalefactors;
    py::array_t<real_T> bulkIns;
    py::array_t<real_T> bulkOuts;
    py::array_t<real_T> resolutionParams;
    py::array_t<real_T> params;
    real_T numberOfLayers {};
    py::list contrastLayers;
    py::list layersDetails;
    py::object customFiles;
    std::string  modelType {};
    py::array_t<real_T> contrastCustomFiles;
    py::array_t<real_T> contrastDomainRatios;
    py::array_t<real_T> domainRatios;
    real_T numberOfDomainContrasts {};
    py::list domainContrastLayers;
    py::array_t<real_T> fitParams;
    py::array_t<real_T> fitLimits;
    py::list priorNames;
    py::array_t<real_T> priorValues;
    NameStore names;
    Checks checks {};
};


const std::string docsControl = R"(The Python binding for the C++ controls struct.

Parameters
----------
parallel : str
    How the calculation should be parallelised (This uses the Parallel Computing Toolbox). Can be 'single', 'contrasts' or 'points'.
procedure : str
    Which procedure RAT should execute. Can be 'calculate', 'simplex', 'de', 'ns', or 'dream'.
numSimulationPoints : int
    The number of points used for a reflectivity simulation where no data is present.
resampleMinAngle : float
    The upper threshold on the angle between three sampled points for resampling, in units of radians over pi.
resampleNPoints : int
    The number of initial points to use for resampling.
display : str
    How much RAT should print to the terminal. Can be 'off', 'iter', 'notify', or 'final'.
IPCFilePath : str
    The path of the inter process communication file.
updateFreq : int
    [SIMPLEX, DE] Number of iterations between printing progress updates to the terminal.
updatePlotFreq : int
    [SIMPLEX, DE] Number of iterations between updates to live plots.
xTolerance : float
    [SIMPLEX] The termination tolerance for step size.
funcTolerance : float
    [SIMPLEX] The termination tolerance for change in chi-squared.
maxFuncEvals : int
    [SIMPLEX] The maximum number of function evaluations before the algorithm terminates.
maxIterations : int
    [SIMPLEX] The maximum number of iterations before the algorithm terminates.
populationSize : int
    [DE] The number of candidate solutions that exist at any time.
fWeight : float
    [DE] The step size for how different mutations are to their parents.
crossoverProbability : float
    [DE] The probability of exchange of parameters between individuals at any iteration.
strategy : int
    [DE] The algorithm used to generate new candidates.
targetValue : float
    [DE] The value of chi-squared at which the algorithm will terminate.
numGenerations : int
    [DE] The maximum number of iterations before the algorithm terminates.
nLive : int
    [NS] The number of points to sample.
nMCMC : int
    [NS] If non-zero, an MCMC process with ``nMCMC`` chains will be used instead of MultiNest.
propScale : float
    [NS] A scaling factor for the ellipsoid generated by MultiNest.
nsTolerance : float
    [NS] The tolerance threshold for when the algorithm should terminate.
nSamples : int
    [DREAM] The number of samples in the initial population for each chain.
nChains : int
    [DREAM] The number of Markov chains to use in the algorithm.
jumpProbability : float
    [DREAM] The probability range for the size of jumps in sampling. Larger values mean more variable jumps.
pUnitGamma : float
    [DREAM] The probability that the scaling-down factor of jumps will be ignored and a larger jump will be taken.
boundHandling : str
    [DREAM] How steps past the space boundaries should be handled. Can be 'off', 'reflect', 'bound', or 'fold'.
adaptPCR : bool
    [DREAM] Whether the crossover probability for differential evolution should be adapted during the run.
)";

struct Control {
    std::string parallel {};
    std::string procedure {};
    std::string display {};
    real_T xTolerance {};
    real_T funcTolerance {};
    real_T maxFuncEvals {};
    real_T maxIterations {};
    real_T populationSize {};
    real_T fWeight {};
    real_T crossoverProbability {};
    real_T targetValue {};
    real_T numGenerations {};
    real_T strategy {};
    real_T nLive {};
    real_T nMCMC {};
    real_T propScale {};
    real_T nsTolerance {};
    real_T numSimulationPoints {};
    real_T resampleMinAngle {};
    real_T resampleNPoints {};
    real_T updateFreq {};
    real_T updatePlotFreq {};
    real_T nSamples {};
    real_T nChains {};
    real_T jumpProbability {};
    real_T pUnitGamma {};
    std::string boundHandling {};
    boolean_T adaptPCR;
    std::string IPCFilePath {};
};

#endif  // RAT_DEFINES_H