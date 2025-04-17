"""Converts results from the compiled RAT code to python dataclasses."""

from dataclasses import dataclass
from typing import Any, Optional, Union

import numpy as np

import RATapi.rat_core
from RATapi.utils.enums import Procedures


def get_field_string(field: str, value: Any, array_limit: int):
    """Return a string representation of class fields where large arrays are represented by their shape.

    An array will be displayed as just its shape if it is multidimensional or 1D and longer than ``array_limit``.

    Parameters
    ----------
    field : str
        The name of the field in the RAT output class.
    value : Any
        The value of the given field in the RAT output class.
    array_limit : int
        The maximum length of 1D arrays which will be fully displayed.

    Returns
    -------
    field_string : str
        The string representation of the field in the RAT output class.

    Examples
    --------
    >>> get_field_string("data", 130, 5)
    "data = 130"

    >>> get_field_string("data", array([1, 2, 3, 4, 5]), 10)
    "data = [1 2 3 4 5]"

    >>> get_field_string("data", array([1, 2, 3, 4, 5]), 3)
    "data = Data array: [5],"

    >>> get_field_string("data", array([[1, 2, 3], [4, 5, 6]]), 10)
    "data = Data array: [2 x 3],"

    """
    array_text = "Data array: "
    if isinstance(value, list) and len(value) > 0:
        if isinstance(value[0], np.ndarray):
            array_strings = [f"{array_text}[{' x '.join(str(i) for i in array.shape)}]" for array in value]
            field_string = f"{field} = [{', '.join(str(string) for string in array_strings)}],\n"
        elif isinstance(value[0], list) and len(value[0]) > 0 and isinstance(value[0][0], np.ndarray):
            array_strings = [
                [f"{array_text}[{' x '.join(str(i) for i in array.shape)}]" for array in sub_list] for sub_list in value
            ]
            list_strings = [f"[{', '.join(string for string in list_string)}]" for list_string in array_strings]
            field_string = f"{field} = [{', '.join(list_strings)}],\n"
        else:
            field_string = f"{field} = {str(value)},\n"
    elif isinstance(value, np.ndarray):
        if value.ndim == 1 and value.size < array_limit:
            field_string = f"{field} = {str(value) if value.size > 0 else '[]'},\n"
        else:
            field_string = f"{field} = {array_text}[{' x '.join(str(i) for i in value.shape)}],\n"
    else:
        field_string = f"{field} = {str(value)},\n"

    return field_string


class RATResult:
    """A mixin class which truncates arrays when the class is displayed."""

    def __str__(self):
        output = f"{self.__class__.__name__}(\n"
        for key, value in self.__dict__.items():
            output += "\t" + get_field_string(key, value, 100)
        output += ")"
        return output


@dataclass
class CalculationResults(RATResult):
    """The goodness of fit from the Abeles calculation.

    Parameters
    ----------
    chiValues : np.ndarray
        The chi-squared value for each contrast.
    sumChi : float
        The sum of the chiValues array.

    """

    chiValues: np.ndarray
    sumChi: float


@dataclass
class ContrastParams(RATResult):
    """The experimental parameters for each contrast.

    Parameters
    ----------
    scalefactors : np.ndarray
        The scalefactor values for each contrast.
    bulkIn : np.ndarray
        The bulk in values for each contrast.
    bulkOut : np.ndarray
        The bulk out values for each contrast.
    subRoughs : np.ndarray
        The substrate roughness values for each contrast.
    resample : np.ndarray
        An array containing whether each contrast was resampled.

    """

    scalefactors: np.ndarray
    bulkIn: np.ndarray
    bulkOut: np.ndarray
    subRoughs: np.ndarray
    resample: np.ndarray


@dataclass
class Results:
    """The results of a RAT calculation.

    Parameters
    ----------
    reflectivity : list
        The reflectivity curves for each contrast,
        with the same range as the data
        (``data_range`` in the contrast's ``Data`` object)
    simulation : list
        The reflectivity curves for each contrast,
        which can be a wider range to allow extrapolation
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
        If resampling is used, the array of layer parameter values for each contrast after resampling has been
        performed.
    calculationResults : CalculationResults
        The chi-squared fit results from the final calculation and fit.
    contrastParams : ContrastParams
        The experimental parameters for the contrasts.
    fitParams : np.ndarray
        The best fit value of the parameter with name ``fitNames[i]``.
    fitNames : list[str]
        The names of the fit parameters, where ``fitNames[i]`` is the name
        of the parameter with value given in ``fitParams[i]``.

    """

    reflectivity: list
    simulation: list
    shiftedData: list
    backgrounds: list
    resolutions: list
    sldProfiles: list
    layers: list
    resampledLayers: list
    calculationResults: CalculationResults
    contrastParams: ContrastParams
    fitParams: np.ndarray
    fitNames: list[str]

    def __str__(self):
        output = ""
        for key, value in self.__dict__.items():
            output += get_field_string(key, value, 100)
        return output


@dataclass
class PredictionIntervals(RATResult):
    """The Bayesian prediction intervals for 95% and 65% confidence.

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
    SLD : list
        The prediction interval data for SLD of each contrast.
    sampleChi : np.ndarray
        The value of sumChi at each point of the Markov chain.

    """

    reflectivity: list
    sld: list
    sampleChi: np.ndarray


@dataclass
class ConfidenceIntervals(RATResult):
    """The 65% and 95% confidence intervals for the best fit results.

    Parameters
    ----------
    percentile95 : np.ndarray
        The 95% confidence intervals for each fit parameter.
    percentile65 : np.ndarray
        The 65% confidence intervals for each fit parameter.
    mean : np.ndarray
        The mean values for each fit parameter.

    """

    percentile95: np.ndarray
    percentile65: np.ndarray
    mean: np.ndarray


@dataclass
class DreamParams(RATResult):
    """The parameters used by the inner DREAM algorithm.

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
    R : np.ndarray
        An array where row ``i`` is the list of chains
        with which chain ``i`` can mutate.

    """

    nParams: float
    nChains: float
    nGenerations: float
    parallel: bool
    CPU: float
    jumpProbability: float
    pUnitGamma: float
    nCR: float
    delta: float
    steps: float
    zeta: float
    outlier: str
    adaptPCR: bool
    thinning: float
    epsilon: float
    ABC: bool
    IO: bool
    storeOutput: bool
    R: np.ndarray


@dataclass
class DreamOutput(RATResult):
    """The diagnostic output information from DREAM.

    Parameters
    ----------
    allChains : np.ndarray
        An ``nGenerations`` x ``nParams + 2`` x ``nChains`` size array,
        where ``chain_k = DreamOutput.allChains[:, :, k]``
        is the data of chain ``k`` in the final iteration;
        for generation i of the final iteration, ``chain_k[i, j]`` represents:

        - the sampled value of parameter ``j`` for ``j in 0:nParams``;
        - the associated log-prior for those sampled values for ``j = nParams + 1``;
        - the associated log-likelihood for those sampled values for ``j = nParams + 2``.

    outlierChains : np.ndarray
        A two-column array where ``DreamOutput.AR[i, 1]`` is the index of a chain
        and ``DreamOutput.AR[i, 0]`` is the length of that chain when it was removed
        for being an outlier.
    runtime : float
        The runtime of the DREAM algorithm in seconds.
    iteration : float
        The number of iterations performed.
    AR : np.ndarray
        A two-column array where ``DreamOutput.AR[i, 0]`` is an iteration number
        and ``DreamOutput.AR[i, 1]`` is the average acceptance rate of chain step
        proposals for that iteration.
    R_stat : np.ndarray
        An array where ``DreamOutput.R_stat[i, 0]`` is an iteration number and
        ``DreamOutput.R_stat[i, j]`` is the convergence statistic for parameter ``j``
        at that iteration (where chains are indexed 1 to ``nParams`` inclusive).
    CR : np.ndarray
        A four-column array where ``DreamOutput.CR[i, 0]`` is an iteration number,
        ``and DreamOutput.CR[i, j]`` is the selection probability of the ``j``'th crossover
        value for that iteration.

    """

    allChains: np.ndarray
    outlierChains: np.ndarray
    runtime: float
    iteration: float
    AR: np.ndarray
    R_stat: np.ndarray
    CR: np.ndarray


@dataclass
class NestedSamplerOutput(RATResult):
    """The output information from the Nested Sampler (ns).

    Parameters
    ----------
    logZ : float
        The natural logarithm of the evidence Z for the parameter values.
    logZErr : float
        The estimated uncertainty in the final value of logZ.
    nestSamples : np.ndarray
        ``NestedSamplerOutput.nestSamples[i, j]`` represents the values
        sampled at iteration ``i``, where this value is:

        - the value sampled for parameter ``j``, for ``j`` in ``0:nParams``,
        - the minimum log-likelihood for ``j = nParams + 1``.

    postSamples : np.ndarray
        The posterior values at the points sampled in ``NestedSamplerOutput.nestSamples``.

    """

    logZ: float
    logZErr: float
    nestSamples: np.ndarray
    postSamples: np.ndarray


@dataclass
class BayesResults(Results):
    """The results of a Bayesian RAT calculation.

    Parameters
    ----------
    predictionIntervals : PredictionIntervals
        The prediction intervals.
    confidenceIntervals : ConfidenceIntervals
        The 65% and 95% confidence intervals for the best fit results.
    dreamParams : DreamParams
        The parameters used by DREAM, if relevant.
    dreamOutput : DreamOutput
        The output from DREAM if DREAM was used.
    nestedSamplerOutput : NestedSamplerOutput
        The output from nested sampling if ns was used.
    chain : np.ndarray
        The MCMC chains for each parameter.
        The ``i``'th column of the array contains the chain for parameter ``fitNames[i]``.

    """

    predictionIntervals: PredictionIntervals
    confidenceIntervals: ConfidenceIntervals
    dreamParams: DreamParams
    dreamOutput: DreamOutput
    nestedSamplerOutput: NestedSamplerOutput
    chain: np.ndarray


def make_results(
    procedure: Procedures,
    output_results: RATapi.rat_core.OutputResult,
    bayes_results: Optional[RATapi.rat_core.OutputBayesResult] = None,
) -> Union[Results, BayesResults]:
    """Initialise a python Results or BayesResults object using the outputs from a RAT calculation.

    Parameters
    ----------
    procedure : Procedures
        The procedure used by the calculation.
    output_results : RATapi.rat_core.OutputResult
        The C++ output results from the calculation.
    bayes_results : Optional[RATapi.rat_core.OutputBayesResult]
        The optional extra C++ Bayesian output results from a Bayesian calculation.

    Returns
    -------
    Results or BayesResults
        A result object containing the results of the calculation, of type
        Results for non-Bayesian procedures and BayesResults for Bayesian procedures.

    """
    calculation_results = CalculationResults(
        chiValues=output_results.calculationResults.chiValues,
        sumChi=output_results.calculationResults.sumChi,
    )
    contrast_params = ContrastParams(
        scalefactors=output_results.contrastParams.scalefactors,
        bulkIn=output_results.contrastParams.bulkIn,
        bulkOut=output_results.contrastParams.bulkOut,
        subRoughs=output_results.contrastParams.subRoughs,
        resample=output_results.contrastParams.resample,
    )

    if procedure in [Procedures.NS, Procedures.DREAM]:
        prediction_intervals = PredictionIntervals(
            reflectivity=bayes_results.predictionIntervals.reflectivity,
            sld=bayes_results.predictionIntervals.sld,
            sampleChi=bayes_results.predictionIntervals.sampleChi,
        )

        confidence_intervals = ConfidenceIntervals(
            percentile95=bayes_results.confidenceIntervals.percentile95,
            percentile65=bayes_results.confidenceIntervals.percentile65,
            mean=bayes_results.confidenceIntervals.mean,
        )

        dream_params = DreamParams(
            nParams=bayes_results.dreamParams.nParams,
            nChains=bayes_results.dreamParams.nChains,
            nGenerations=bayes_results.dreamParams.nGenerations,
            parallel=bool(bayes_results.dreamParams.parallel),
            CPU=bayes_results.dreamParams.CPU,
            jumpProbability=bayes_results.dreamParams.jumpProbability,
            pUnitGamma=bayes_results.dreamParams.pUnitGamma,
            nCR=bayes_results.dreamParams.nCR,
            delta=bayes_results.dreamParams.delta,
            steps=bayes_results.dreamParams.steps,
            zeta=bayes_results.dreamParams.zeta,
            outlier=bayes_results.dreamParams.outlier,
            adaptPCR=bool(bayes_results.dreamParams.adaptPCR),
            thinning=bayes_results.dreamParams.thinning,
            epsilon=bayes_results.dreamParams.epsilon,
            ABC=bool(bayes_results.dreamParams.ABC),
            IO=bool(bayes_results.dreamParams.IO),
            storeOutput=bool(bayes_results.dreamParams.storeOutput),
            R=bayes_results.dreamParams.R,
        )

        dream_output = DreamOutput(
            allChains=bayes_results.dreamOutput.allChains,
            outlierChains=bayes_results.dreamOutput.outlierChains,
            runtime=bayes_results.dreamOutput.runtime,
            iteration=bayes_results.dreamOutput.iteration,
            AR=bayes_results.dreamOutput.AR,
            R_stat=bayes_results.dreamOutput.R_stat,
            CR=bayes_results.dreamOutput.CR,
        )

        nested_sampler_output = NestedSamplerOutput(
            logZ=bayes_results.nestedSamplerOutput.logZ,
            logZErr=bayes_results.nestedSamplerOutput.logZErr,
            nestSamples=bayes_results.nestedSamplerOutput.nestSamples,
            postSamples=bayes_results.nestedSamplerOutput.postSamples,
        )

        results = BayesResults(
            reflectivity=output_results.reflectivity,
            simulation=output_results.simulation,
            shiftedData=output_results.shiftedData,
            backgrounds=output_results.backgrounds,
            resolutions=output_results.resolutions,
            sldProfiles=output_results.sldProfiles,
            layers=output_results.layers,
            resampledLayers=output_results.resampledLayers,
            calculationResults=calculation_results,
            contrastParams=contrast_params,
            fitParams=output_results.fitParams,
            fitNames=output_results.fitNames,
            predictionIntervals=prediction_intervals,
            confidenceIntervals=confidence_intervals,
            dreamParams=dream_params,
            dreamOutput=dream_output,
            nestedSamplerOutput=nested_sampler_output,
            chain=bayes_results.chain,
        )

    else:
        results = Results(
            reflectivity=output_results.reflectivity,
            simulation=output_results.simulation,
            shiftedData=output_results.shiftedData,
            backgrounds=output_results.backgrounds,
            resolutions=output_results.resolutions,
            sldProfiles=output_results.sldProfiles,
            layers=output_results.layers,
            resampledLayers=output_results.resampledLayers,
            calculationResults=calculation_results,
            contrastParams=contrast_params,
            fitParams=output_results.fitParams,
            fitNames=output_results.fitNames,
        )

    return results
