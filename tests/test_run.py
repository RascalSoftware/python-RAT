"""Test the run module using the example calculation from "DSPC_standard_layers.py".

We use the example for both a reflectivity calculation, and Bayesian analysis using the Dream algorithm.
"""

import io
from contextlib import redirect_stdout
from unittest import mock

import numpy as np
import pytest

import RATapi
import RATapi.outputs
import RATapi.rat_core
from RATapi.events import EventTypes, ProgressEventData, notify
from RATapi.run import ProgressBar, TextOutput
from RATapi.utils.enums import Calculations, Geometries, LayerModels, Procedures
from tests.utils import check_results_equal


@pytest.fixture
def reflectivity_calculation_problem():
    """The output C++ ProblemDefinition object for a reflectivity calculation of the project set out in
    "DSPC_standard_layers.py".
    """
    problem = RATapi.rat_core.ProblemDefinition()
    problem.TF = Calculations.Normal
    problem.modelType = LayerModels.StandardLayers
    problem.geometry = Geometries.SubstrateLiquid
    problem.useImaginary = False
    problem.params = np.array(
        [
            3.000e00,
            1.954e01,
            3.390e-06,
            2.266e01,
            -4.010e-07,
            5.252e00,
            5.640e00,
            1.712e01,
            0.000e00,
            8.560e00,
            1.750e-06,
            4.545e01,
            1.070e01,
            1.470e-06,
            6.014e00,
            1.782e01,
            -4.610e-07,
            1.764e01,
            3.615e01,
            1.000e02,
            2.361e01,
        ],
    )
    problem.bulkIns = np.array([2.073e-06])
    problem.bulkOuts = np.array([5.98e-06, 2.21e-06])
    problem.scalefactors = np.array([0.1, 0.15])
    problem.domainRatios = np.array([])
    problem.backgroundParams = np.array([2.23e-06, 3.38e-06])
    problem.resolutionParams = np.array([0.03])
    problem.contrastBulkIns = np.array([1.0, 1.0])
    problem.contrastBulkOuts = np.array([1.0, 2.0])
    problem.contrastScalefactors = np.array([1.0, 2.0])
    problem.contrastBackgroundParams = [[1.0], [2.0]]
    problem.contrastBackgroundTypes = ["constant", "constant"]
    problem.contrastBackgroundActions = [1.0, 1.0]
    problem.contrastResolutionParams = [[1.0], [1.0]]
    problem.contrastResolutionTypes = ["constant", "constant"]
    problem.contrastCustomFiles = np.array([np.nan, np.nan])
    problem.contrastDomainRatios = np.array([0.0, 0.0])
    problem.resample = np.array([0.0, 0.0])
    problem.dataPresent = np.array([1.0, 1.0])
    problem.dataLimits = [[0.011403, 0.59342], [0.011403, 0.59342]]
    problem.simulationLimits = [[0.011403, 0.59342], [0.011403, 0.59342]]
    problem.numberOfContrasts = 2.0
    problem.numberOfLayers = 6.0
    problem.repeatLayers = [1.0, 1.0]
    problem.layersDetails = [
        np.array([2.0]),
        np.array([4.0]),
        np.array([10.0]),
        np.array([8.0]),
        np.array([13.0]),
        np.array([16.0]),
    ]
    problem.contrastLayers = [
        np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 5.0]]),
        np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 5.0]]),
    ]
    problem.numberOfDomainContrasts = 0.0
    problem.domainContrastLayers = []
    problem.fitParams = np.array(
        [
            3.000e00,
            1.954e01,
            2.266e01,
            5.252e00,
            5.640e00,
            1.712e01,
            8.560e00,
            4.545e01,
            1.070e01,
            6.014e00,
            1.782e01,
            1.764e01,
            3.615e01,
            2.361e01,
            2.230e-06,
            3.380e-06,
            5.980e-06,
            2.210e-06,
        ],
    )
    problem.fitLimits = np.array(
        [
            [1.00e00, 1.00e01],
            [5.00e00, 6.00e01],
            [1.50e01, 3.50e01],
            [1.00e00, 5.00e01],
            [1.00e00, 1.50e01],
            [1.00e01, 2.80e01],
            [5.00e00, 1.70e01],
            [1.00e01, 5.00e01],
            [7.00e00, 1.70e01],
            [2.00e00, 1.50e01],
            [1.40e01, 2.20e01],
            [1.00e01, 5.00e01],
            [1.00e01, 5.00e01],
            [0.00e00, 6.00e01],
            [5.00e-10, 7.00e-06],
            [1.00e-10, 4.99e-06],
            [5.50e-06, 6.40e-06],
            [1.00e-06, 4.99e-06],
        ],
    )
    problem.names.params = [
        "Substrate Roughness",
        "Oxide Thickness",
        "Oxide SLD",
        "SAM Tails Thickness",
        "SAM Tails SLD",
        "SAM Tails Hydration",
        "SAM Roughness",
        "CW Thickness",
        "CW SLD",
        "SAM Heads Thickness",
        "SAM Heads SLD",
        "SAM Heads Hydration",
        "Bilayer Heads Thickness",
        "Bilayer Heads SLD",
        "Bilayer Roughness",
        "Bilayer Tails Thickness",
        "Bilayer Tails SLD",
        "Bilayer Tails Hydration",
        "Bilayer Heads Hydration",
        "CW Hydration",
        "Oxide Hydration",
    ]
    problem.names.backgroundParams = ["Background parameter D2O", "Background parameter SMW"]
    problem.names.scalefactors = ["Scalefactor 1", "Scalefactor 2"]
    problem.names.bulkIns = ["Silicon"]
    problem.names.bulkOuts = ["D2O", "SMW"]
    problem.names.resolutionParams = ["Resolution Param 1"]
    problem.names.domainRatios = []
    problem.names.contrasts = ["D2O", "SMW"]
    problem.checks.params = np.array(
        [1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0]
    )
    problem.checks.backgroundParams = np.array([1.0, 1.0])
    problem.checks.scalefactors = np.array([0.0, 0.0])
    problem.checks.bulkIns = np.array([0.0])
    problem.checks.bulkOuts = np.array([1.0, 1.0])
    problem.checks.resolutionParams = np.array([0.0])
    problem.checks.domainRatios = np.array([])

    return problem


@pytest.fixture
def dream_problem():
    """The output C++ ProblemDefinition object for a Dream optimisation of the project set out in
    "DSPC_standard_layers.py".

    This optimisation used the parameters: nSamples=50000, nChains=10.
    """
    problem = RATapi.rat_core.ProblemDefinition()
    problem.TF = Calculations.Normal
    problem.modelType = LayerModels.StandardLayers
    problem.geometry = Geometries.SubstrateLiquid
    problem.useImaginary = False
    problem.params = np.array(
        [
            6.19503045e00,
            1.84420960e01,
            3.39000000e-06,
            2.11039621e01,
            -4.01000000e-07,
            8.75538121e00,
            3.72292994e00,
            1.84624551e01,
            0.00000000e00,
            1.02316734e01,
            1.75000000e-06,
            2.31156093e01,
            1.09906265e01,
            1.47000000e-06,
            5.71005361e00,
            1.67933822e01,
            -4.61000000e-07,
            1.72009856e01,
            3.00260126e01,
            1.00000000e02,
            2.94448999e01,
        ],
    )
    problem.bulkIns = np.array([2.073e-06])
    problem.bulkOuts = np.array([6.01489149e-06, 1.59371685e-06])
    problem.scalefactors = np.array([0.1, 0.15])
    problem.domainRatios = np.array([])
    problem.backgroundParams = np.array([2.37113128e-06, 1.99006694e-06])
    problem.resolutionParams = np.array([0.03])
    problem.contrastBulkIns = np.array([1.0, 1.0])
    problem.contrastBulkOuts = np.array([1.0, 2.0])
    problem.contrastScalefactors = np.array([1.0, 2.0])
    problem.contrastBackgroundParams = [[1.0], [2.0]]
    problem.contrastBackgroundTypes = ["constant", "constant"]
    problem.contrastBackgroundActions = [1.0, 1.0]
    problem.contrastResolutionParams = [[1.0], [1.0]]
    problem.contrastResolutionTypes = ["constant", "constant"]
    problem.contrastCustomFiles = np.array([np.nan, np.nan])
    problem.contrastDomainRatios = np.array([0.0, 0.0])
    problem.resample = np.array([0.0, 0.0])
    problem.dataPresent = np.array([1.0, 1.0])
    problem.dataLimits = [[0.011403, 0.59342], [0.011403, 0.59342]]
    problem.simulationLimits = [[0.011403, 0.59342], [0.011403, 0.59342]]
    problem.numberOfContrasts = 2.0
    problem.numberOfLayers = 6.0
    problem.repeatLayers = [1.0, 1.0]
    problem.layersDetails = [
        np.array([2.0]),
        np.array([4.0]),
        np.array([10.0]),
        np.array([8.0]),
        np.array([13.0]),
        np.array([16.0]),
    ]
    problem.contrastLayers = [
        np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 5.0]]),
        np.array([[1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 6.0, 5.0]]),
    ]
    problem.numberOfDomainContrasts = 0.0
    problem.domainContrastLayers = []
    problem.fitParams = np.array(
        [
            6.19503045e00,
            1.84420960e01,
            2.11039621e01,
            8.75538121e00,
            3.72292994e00,
            1.84624551e01,
            1.02316734e01,
            2.31156093e01,
            1.09906265e01,
            5.71005361e00,
            1.67933822e01,
            1.72009856e01,
            3.00260126e01,
            2.94448999e01,
            2.37113128e-06,
            1.99006694e-06,
            6.01489149e-06,
            1.59371685e-06,
        ],
    )
    problem.fitLimits = np.array(
        [
            [1.00e00, 1.00e01],
            [5.00e00, 6.00e01],
            [1.50e01, 3.50e01],
            [1.00e00, 5.00e01],
            [1.00e00, 1.50e01],
            [1.00e01, 2.80e01],
            [5.00e00, 1.70e01],
            [1.00e01, 5.00e01],
            [7.00e00, 1.70e01],
            [2.00e00, 1.50e01],
            [1.40e01, 2.20e01],
            [1.00e01, 5.00e01],
            [1.00e01, 5.00e01],
            [0.00e00, 6.00e01],
            [5.00e-10, 7.00e-06],
            [1.00e-10, 4.99e-06],
            [5.50e-06, 6.40e-06],
            [1.00e-06, 4.99e-06],
        ],
    )
    problem.names.params = [
        "Substrate Roughness",
        "Oxide Thickness",
        "Oxide SLD",
        "SAM Tails Thickness",
        "SAM Tails SLD",
        "SAM Tails Hydration",
        "SAM Roughness",
        "CW Thickness",
        "CW SLD",
        "SAM Heads Thickness",
        "SAM Heads SLD",
        "SAM Heads Hydration",
        "Bilayer Heads Thickness",
        "Bilayer Heads SLD",
        "Bilayer Roughness",
        "Bilayer Tails Thickness",
        "Bilayer Tails SLD",
        "Bilayer Tails Hydration",
        "Bilayer Heads Hydration",
        "CW Hydration",
        "Oxide Hydration",
    ]
    problem.names.backgroundParams = ["Background parameter D2O", "Background parameter SMW"]
    problem.names.scalefactors = ["Scalefactor 1", "Scalefactor 2"]
    problem.names.bulkIns = ["Silicon"]
    problem.names.bulkOuts = ["D2O", "SMW"]
    problem.names.resolutionParams = ["Resolution Param 1"]
    problem.names.domainRatios = []
    problem.names.contrasts = ["D2O", "SMW"]
    problem.checks.params = np.array(
        [1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0]
    )
    problem.checks.backgroundParams = np.array([1.0, 1.0])
    problem.checks.scalefactors = np.array([0.0, 0.0])
    problem.checks.bulkIns = np.array([0.0])
    problem.checks.bulkOuts = np.array([1.0, 1.0])
    problem.checks.resolutionParams = np.array([0.0])
    problem.checks.domainRatios = np.array([])

    return problem


@pytest.mark.parametrize(
    ["test_procedure", "test_output_problem", "test_output_results", "test_bayes", "test_results"],
    [
        (
            Procedures.Calculate,
            "reflectivity_calculation_problem",
            "reflectivity_calculation_output_results",
            None,
            "reflectivity_calculation_results",
        ),
        (Procedures.DREAM, "dream_problem", "dream_output_results", "dream_bayes", "dream_results"),
    ],
)
def test_run(test_procedure, test_output_problem, test_output_results, test_bayes, test_results, request) -> None:
    input_project = request.getfixturevalue("input_project")
    test_output_problem = request.getfixturevalue(test_output_problem)
    test_output_results = request.getfixturevalue(test_output_results)
    if test_bayes:
        test_bayes = request.getfixturevalue(test_bayes)

    test_results = request.getfixturevalue(test_results)

    with mock.patch.object(
        RATapi.rat_core,
        "RATMain",
        mock.MagicMock(return_value=(test_output_problem, test_output_results, test_bayes)),
    ):
        # Use default project as we patch RATMain to give the desired outputs
        project, results = RATapi.run(input_project, RATapi.Controls(procedure=test_procedure))

    check_results_equal(test_results, results)


def test_progress_bar() -> None:
    event = ProgressEventData()
    event.message = "TESTING"
    event.percent = 0.2
    with ProgressBar() as bar:
        assert bar.pbar is None
        notify(EventTypes.Progress, event)
        assert bar.pbar.desc == "TESTING"
        assert bar.pbar.n == 20

        event.message = "AGAIN"
        event.percent = 0.6
        notify(EventTypes.Progress, event)
        assert bar.pbar.desc == "AGAIN"
        assert bar.pbar.n == 60


def test_text_output() -> None:
    with io.StringIO() as buf, redirect_stdout(buf), TextOutput():
        assert buf.getvalue() == ""
        notify(EventTypes.Message, "Hello World")
        assert buf.getvalue() == "Hello World"

    with io.StringIO() as buf, redirect_stdout(buf), TextOutput(display=False):
        assert buf.getvalue() == ""
        notify(EventTypes.Message, "Hello World")
        assert buf.getvalue() == ""
