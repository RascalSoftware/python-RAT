"""Converts outputs from the compiled RAT code to python dataclasses"""

from dataclasses import dataclass
from typing import Any, Optional, Union

import numpy as np

import RATapi.rat_core
from RATapi.utils.enums import Procedures


def get_field_string(field: str, value: Any, array_limit: int):
    """Returns a string representation of class fields where large and multidimensional arrays are represented by their
    shape.

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
    def __str__(self):
        output = f"{self.__class__.__name__}(\n"
        for key, value in self.__dict__.items():
            output += "\t" + get_field_string(key, value, 100)
        output += ")"
        return output


@dataclass
class CalculationResults(RATResult):
    chiValues: np.ndarray
    sumChi: float


@dataclass
class ContrastParams(RATResult):
    backgroundParams: np.ndarray
    scalefactors: np.ndarray
    bulkIn: np.ndarray
    bulkOut: np.ndarray
    resolutionParams: np.ndarray
    subRoughs: np.ndarray
    resample: np.ndarray


@dataclass
class Results:
    reflectivity: list
    simulation: list
    shiftedData: list
    layerSlds: list
    sldProfiles: list
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
    reflectivity: list
    sld: list
    sampleChi: np.ndarray


@dataclass
class ConfidenceIntervals(RATResult):
    percentile95: np.ndarray
    percentile65: np.ndarray
    mean: np.ndarray


@dataclass
class DreamParams(RATResult):
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
    allChains: np.ndarray
    outlierChains: np.ndarray
    runtime: float
    iteration: float
    modelOutput: float
    AR: np.ndarray
    R_stat: np.ndarray
    CR: np.ndarray


@dataclass
class NestedSamplerOutput(RATResult):
    logZ: float
    nestSamples: np.ndarray
    postSamples: np.ndarray


@dataclass
class BayesResults(Results):
    predictionIntervals: PredictionIntervals
    confidenceIntervals: ConfidenceIntervals
    dreamParams: DreamParams
    dreamOutput: DreamOutput
    nestedSamplerOutput: NestedSamplerOutput
    chain: np.ndarray


def make_results(
    procedure: Procedures,
    output_results: RATapi.rat_core.OutputResult,
    bayes_results: Optional[RATapi.rat_core.BayesResults] = None,
) -> Union[Results, BayesResults]:
    """Initialise a python Results or BayesResults object using the outputs from a RAT calculation."""
    calculation_results = CalculationResults(
        chiValues=output_results.calculationResults.chiValues,
        sumChi=output_results.calculationResults.sumChi,
    )
    contrast_params = ContrastParams(
        backgroundParams=output_results.contrastParams.backgroundParams,
        scalefactors=output_results.contrastParams.scalefactors,
        bulkIn=output_results.contrastParams.bulkIn,
        bulkOut=output_results.contrastParams.bulkOut,
        resolutionParams=output_results.contrastParams.resolutionParams,
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
            modelOutput=bayes_results.dreamOutput.modelOutput,
            AR=bayes_results.dreamOutput.AR,
            R_stat=bayes_results.dreamOutput.R_stat,
            CR=bayes_results.dreamOutput.CR,
        )

        nested_sampler_output = NestedSamplerOutput(
            logZ=bayes_results.nestedSamplerOutput.logZ,
            nestSamples=bayes_results.nestedSamplerOutput.nestSamples,
            postSamples=bayes_results.nestedSamplerOutput.postSamples,
        )

        results = BayesResults(
            reflectivity=output_results.reflectivity,
            simulation=output_results.simulation,
            shiftedData=output_results.shiftedData,
            layerSlds=output_results.layerSlds,
            sldProfiles=output_results.sldProfiles,
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
            layerSlds=output_results.layerSlds,
            sldProfiles=output_results.sldProfiles,
            resampledLayers=output_results.resampledLayers,
            calculationResults=calculation_results,
            contrastParams=contrast_params,
            fitParams=output_results.fitParams,
            fitNames=output_results.fitNames,
        )

    return results
