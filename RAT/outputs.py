"""Converts outputs from the compiled RAT code to python dataclasses"""

from dataclasses import dataclass
import numpy as np
from typing import Optional, Union
from RAT.utils.enums import Procedures
import RAT.rat_core


@dataclass
class CalculationResults:
    chiValues: np.ndarray
    sumChi: float


@dataclass
class ContrastParams:
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


@dataclass
class BestFitMean:
    reflectivity: list
    sld: list
    chi: float
    data: list


@dataclass
class PredictionIntervals:
    reflectivity: list
    sld: list
    reflectivityXData: list
    sldXData: list
    sampleChi: np.ndarray


@dataclass
class ConfidenceIntervals:
    percentile95: np.ndarray
    percentile65: np.ndarray
    mean: np.ndarray


@dataclass
class DreamParams:
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
class DreamOutput:
    allChains: np.ndarray
    outlierChains: np.ndarray
    runtime: float
    iteration: float
    modelOutput: float
    AR: np.ndarray
    R_stat: np.ndarray
    CR: np.ndarray


@dataclass
class NestedSamplerOutput:
    logZ: float
    nestSamples: np.ndarray
    postSamples: np.ndarray


@dataclass
class BayesResults(Results):
    bestFitMean: BestFitMean
    predictionIntervals: PredictionIntervals
    confidenceIntervals: ConfidenceIntervals
    dreamParams: DreamParams
    dreamOutput: DreamOutput
    nestedSamplerOutput: NestedSamplerOutput
    chain: np.ndarray


def make_results(procedure: Procedures, output_results: RAT.rat_core.OutputResult,
                 bayes_results: Optional[RAT.rat_core.BayesResults] = None) -> Union[Results, BayesResults]:
    """Initialise a python Results or BayesResults object using the outputs from a RAT calculation."""

    calculation_results = CalculationResults(chiValues=output_results.calculationResults.chiValues,
                                             sumChi=output_results.calculationResults.sumChi
                                             )
    contrast_params = ContrastParams(
        backgroundParams=output_results.contrastParams.backgroundParams,
        scalefactors=output_results.contrastParams.scalefactors,
        bulkIn=output_results.contrastParams.bulkIn,
        bulkOut=output_results.contrastParams.bulkOut,
        resolutionParams=output_results.contrastParams.resolutionParams,
        subRoughs=output_results.contrastParams.subRoughs,
        resample=output_results.contrastParams.resample
    )

    if procedure in [Procedures.NS, Procedures.Dream]:

        best_fit_mean = BestFitMean(
            reflectivity=bayes_results.bestFitMean.reflectivity,
            sld=bayes_results.bestFitMean.sld,
            chi=bayes_results.bestFitMean.chi,
            data=bayes_results.bestFitMean.data
        )

        prediction_intervals = PredictionIntervals(
            reflectivity=bayes_results.predictionIntervals.reflectivity,
            sld=bayes_results.predictionIntervals.sld,
            reflectivityXData=bayes_results.predictionIntervals.reflectivityXData,
            sldXData=bayes_results.predictionIntervals.sldXData,
            sampleChi=bayes_results.predictionIntervals.sampleChi
        )

        confidence_intervals = ConfidenceIntervals(
            percentile95=bayes_results.confidenceIntervals.percentile95,
            percentile65=bayes_results.confidenceIntervals.percentile65,
            mean=bayes_results.confidenceIntervals.mean
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
            R=bayes_results.dreamParams.R
        )

        dream_output = DreamOutput(
            allChains=bayes_results.dreamOutput.allChains,
            outlierChains=bayes_results.dreamOutput.outlierChains,
            runtime=bayes_results.dreamOutput.runtime,
            iteration=bayes_results.dreamOutput.iteration,
            modelOutput=bayes_results.dreamOutput.modelOutput,
            AR=bayes_results.dreamOutput.AR,
            R_stat=bayes_results.dreamOutput.R_stat,
            CR=bayes_results.dreamOutput.CR
        )

        nested_sampler_output = NestedSamplerOutput(
            logZ=bayes_results.nestedSamplerOutput.logZ,
            nestSamples=bayes_results.nestedSamplerOutput.nestSamples,
            postSamples=bayes_results.nestedSamplerOutput.postSamples
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
            bestFitMean=best_fit_mean,
            predictionIntervals=prediction_intervals,
            confidenceIntervals=confidence_intervals,
            dreamParams=dream_params,
            dreamOutput=dream_output,
            nestedSamplerOutput=nested_sampler_output,
            chain=bayes_results.chain
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
            fitNames=output_results.fitNames
        )

    return results
