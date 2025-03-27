"""Test the outputs module using the example calculation from "DSPC_standard_layers.py".

We use the example for both a reflectivity calculation, and Bayesian analysis using the Dream algorithm.
"""

import numpy as np
import pytest

import RATapi
import RATapi.outputs
import RATapi.rat_core
from RATapi.utils.enums import Procedures
from tests.utils import check_results_equal


@pytest.fixture
def reflectivity_calculation_str():
    """The string representation of the python results object for a reflectivity calculation of the project set out in
    "DSPC_standard_layers.py".
    """
    return (
        "reflectivity = [Data array: [21 x 2], Data array: [21 x 2]],\n"
        "simulation = [Data array: [21 x 2], Data array: [21 x 2]],\n"
        "shiftedData = [Data array: [21 x 3], Data array: [21 x 3]],\n"
        "backgrounds = [Data array: [82 x 3], Data array: [82 x 3]],\n"
        "resolutions = [Data array: [82 x 2], Data array: [82 x 2]],\n"
        "sldProfiles = [[Data array: [25 x 2], Data array: [25 x 2]]],\n"
        "layers = [[Data array: [8 x 3]], [Data array: [8 x 3]]],\n"
        "resampledLayers = [[Data array: [1 x 3]], [Data array: [1 x 3]]],\n"
        "calculationResults = CalculationResults(\n"
        "\tchiValues = [ 202.83057377 1641.4024969 ],\n"
        "\tsumChi = 1844.2330706690975,\n"
        "),\n"
        "contrastParams = ContrastParams(\n"
        "\tscalefactors = [0.1  0.15],\n"
        "\tbulkIn = [2.073e-06 2.073e-06],\n"
        "\tbulkOut = [5.98e-06 2.21e-06],\n"
        "\tsubRoughs = [3. 3.],\n"
        "\tresample = [0. 0.],\n"
        "),\n"
        "fitParams = [3.000e+00 1.954e+01 2.266e+01 5.252e+00 5.640e+00 1.712e+01 8.560e+00\n"
        " 4.545e+01 1.070e+01 6.014e+00 1.782e+01 1.764e+01 3.615e+01 2.361e+01\n"
        " 2.230e-06 3.380e-06 5.980e-06 2.210e-06],\n"
        "fitNames = ['Substrate Roughness', 'Oxide Thickness', 'SAM Tails Thickness', 'SAM Tails Hydration', "
        "'SAM Roughness', 'CW Thickness', 'SAM Heads Thickness', 'SAM Heads Hydration', 'Bilayer Heads Thickness', "
        "'Bilayer Roughness', 'Bilayer Tails Thickness', 'Bilayer Tails Hydration', 'Bilayer Heads Hydration', "
        "'Oxide Hydration', 'Background parameter D2O', 'Background parameter SMW', 'D2O', 'SMW'],\n"
    )


@pytest.fixture
def dream_str():
    """The string representation of the python BayesResults object for a Dream optimisation of the project set out in
    "DSPC_standard_layers.py".
    """
    return (
        "reflectivity = [Data array: [21 x 2], Data array: [21 x 2]],\n"
        "simulation = [Data array: [21 x 2], Data array: [21 x 2]],\n"
        "shiftedData = [Data array: [21 x 3], Data array: [21 x 3]],\n"
        "backgrounds = [Data array: [82 x 3], Data array: [82 x 3]],\n"
        "resolutions = [Data array: [82 x 2], Data array: [82 x 2]],\n"
        "sldProfiles = [[Data array: [29 x 2], Data array: [29 x 2]]],\n"
        "layers = [[Data array: [8 x 3]], [Data array: [8 x 3]]],\n"
        "resampledLayers = [[Data array: [1 x 3]], [Data array: [1 x 3]]],\n"
        "calculationResults = CalculationResults(\n"
        "\tchiValues = [4.6077885  7.00028098],\n"
        "\tsumChi = 11.608069475997699,\n"
        "),\n"
        "contrastParams = ContrastParams(\n"
        "\tscalefactors = [0.1  0.15],\n"
        "\tbulkIn = [2.073e-06 2.073e-06],\n"
        "\tbulkOut = [6.01489149e-06 1.59371685e-06],\n"
        "\tsubRoughs = [6.19503045 6.19503045],\n"
        "\tresample = [0. 0.],\n"
        "),\n"
        "fitParams = [6.19503045e+00 1.84420960e+01 2.11039621e+01 8.75538121e+00\n"
        " 3.72292994e+00 1.84624551e+01 1.02316734e+01 2.31156093e+01\n"
        " 1.09906265e+01 5.71005361e+00 1.67933822e+01 1.72009856e+01\n"
        " 3.00260126e+01 2.94448999e+01 2.37113128e-06 1.99006694e-06\n"
        " 6.01489149e-06 1.59371685e-06],\n"
        "fitNames = ['Substrate Roughness', 'Oxide Thickness', 'SAM Tails Thickness', 'SAM Tails Hydration', "
        "'SAM Roughness', 'CW Thickness', 'SAM Heads Thickness', 'SAM Heads Hydration', "
        "'Bilayer Heads Thickness', 'Bilayer Roughness', 'Bilayer Tails Thickness', "
        "'Bilayer Tails Hydration', 'Bilayer Heads Hydration', 'Oxide Hydration', "
        "'Background parameter D2O', 'Background parameter SMW', 'D2O', 'SMW'],\n"
        "predictionIntervals = PredictionIntervals(\n"
        "\treflectivity = [Data array: [5 x 21], Data array: [5 x 21]],\n"
        "\tsld = [[Data array: [5 x 29], Data array: [5 x 29]]],\n"
        "\tsampleChi = Data array: [1000],\n"
        "),\n"
        "confidenceIntervals = ConfidenceIntervals(\n"
        "\tpercentile95 = Data array: [2 x 18],\n"
        "\tpercentile65 = Data array: [2 x 18],\n"
        "\tmean = Data array: [1 x 18],\n"
        "),\n"
        "dreamParams = DreamParams(\n"
        "\tnParams = 18.0,\n"
        "\tnChains = 1.0,\n"
        "\tnGenerations = 1.0,\n"
        "\tparallel = False,\n"
        "\tCPU = 1.0,\n"
        "\tjumpProbability = 0.5,\n"
        "\tpUnitGamma = 0.2,\n"
        "\tnCR = 3.0,\n"
        "\tdelta = 3.0,\n"
        "\tsteps = 50.0,\n"
        "\tzeta = 1e-12,\n"
        "\toutlier = iqr,\n"
        "\tadaptPCR = False,\n"
        "\tthinning = 1.0,\n"
        "\tepsilon = 0.025,\n"
        "\tABC = False,\n"
        "\tIO = False,\n"
        "\tstoreOutput = False,\n"
        "\tR = Data array: [1 x 1],\n"
        "),\n"
        "dreamOutput = DreamOutput(\n"
        "\tallChains = Data array: [1 x 20 x 1],\n"
        "\toutlierChains = Data array: [1 x 2],\n"
        "\truntime = 2.6e-06,\n"
        "\titeration = 2.0,\n"
        "\tAR = Data array: [1 x 2],\n"
        "\tR_stat = Data array: [1 x 19],\n"
        "\tCR = Data array: [1 x 4],\n"
        "),\n"
        "nestedSamplerOutput = NestedSamplerOutput(\n"
        "\tlogZ = 0.0,\n"
        "\tlogZErr = 0.0,\n"
        "\tnestSamples = Data array: [1 x 2],\n"
        "\tpostSamples = Data array: [1 x 2],\n"
        "),\n"
        "chain = Data array: [2 x 18],\n"
    )


@pytest.mark.parametrize(
    ["test_value", "array_limit", "expected_field_string"],
    [
        (5, 100, "test_field = 5,\n"),
        ([], 100, "test_field = [],\n"),
        ([[], [], []], 100, "test_field = [[], [], []],\n"),
        ([1, 2, 3], 100, "test_field = [1, 2, 3],\n"),
        ([[1], [2], [3]], 100, "test_field = [[1], [2], [3]],\n"),
        (np.array([]), 100, "test_field = [],\n"),
        (np.array([[], []]), 100, "test_field = Data array: [2 x 0],\n"),
        (np.array([1, 2, 3]), 100, "test_field = [1 2 3],\n"),
        (np.array([1, 2, 3]), 3, "test_field = Data array: [3],\n"),
        (np.array([1, 2, 3]), 2, "test_field = Data array: [3],\n"),
        (np.array([[1, 2, 3], [4, 5, 6]]), 100, "test_field = Data array: [2 x 3],\n"),
        (np.array([[[1], [2], [3]], [[4], [5], [6]]]), 100, "test_field = Data array: [2 x 3 x 1],\n"),
        ([np.array([])], 100, "test_field = [Data array: [0]],\n"),
        ([np.array([1, 2, 3])], 100, "test_field = [Data array: [3]],\n"),
        ([np.array([[1], [2], [3]])], 100, "test_field = [Data array: [3 x 1]],\n"),
        ([[np.array([[1, 2, 3], [4, 5, 6]])]], 100, "test_field = [[Data array: [2 x 3]]],\n"),
        ([[np.array([1, 2, 3]), np.array([4, 5, 6])]], 100, "test_field = [[Data array: [3], Data array: [3]]],\n"),
        (
            [[np.array([[1, 2], [3, 4]]), np.array([[5, 6], [7, 8]])]],
            100,
            "test_field = [[Data array: [2 x 2], Data array: [2 x 2]]],\n",
        ),
    ],
)
def test_get_field_string(test_value, array_limit, expected_field_string) -> None:
    """For the string representation of output classes, we represent multidimensional and large arrays by their shape,
    with other variables printed normally.
    """
    field_string = RATapi.outputs.get_field_string("test_field", test_value, array_limit)

    assert field_string == expected_field_string


@pytest.mark.parametrize(
    ["test_procedure", "test_output_results", "test_bayes", "test_results"],
    [
        (Procedures.Calculate, "reflectivity_calculation_output_results", None, "reflectivity_calculation_results"),
        (Procedures.DREAM, "dream_output_results", "dream_bayes", "dream_results"),
    ],
)
def test_make_results(test_procedure, test_output_results, test_bayes, test_results, request) -> None:
    """The python results object should contain the relevant parameters defined in the C++ results and bayes objects."""
    test_output_results = request.getfixturevalue(test_output_results)
    if test_bayes:
        test_bayes = request.getfixturevalue(test_bayes)

    test_results = request.getfixturevalue(test_results)
    results = RATapi.outputs.make_results(test_procedure, test_output_results, test_bayes)

    check_results_equal(test_results, results)


@pytest.mark.parametrize(
    ["test_output_results", "test_str"],
    [
        ("reflectivity_calculation_results", "reflectivity_calculation_str"),
        ("dream_results", "dream_str"),
    ],
)
def test_results_str(test_output_results, test_str, request) -> None:
    """The string representation of the python results object should represent large and multidimensional arrays by
    their shape.
    """
    test_output_results = request.getfixturevalue(test_output_results)
    test_str = request.getfixturevalue(test_str)

    assert test_output_results.__str__() == test_str
