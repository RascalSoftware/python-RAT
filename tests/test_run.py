"""Test the run module using the example calculation from "DSPC_standard_layers.py".

We use the example for both a reflectivity calculation, and Bayesian analysis using the Dream algorithm.
"""

from unittest import mock

import numpy as np
import pytest

import RATapi
import RATapi.outputs
import RATapi.rat_core
from RATapi.utils.enums import Calculations, Geometries, LayerModels, Procedures
from tests.utils import check_results_equal


@pytest.fixture
def input_project():
    """A cut-down version of the input Project object for a reflectivity calculation set out in
    "DSPC_standard_layers.py".
    """
    project = RATapi.Project(
        name="original_dspc_bilayer",
        calculation="non polarised",
        model="standard layers",
        geometry="substrate/liquid",
        absorption=False,
    )

    # Set up the relevant parameters
    project.parameters.append(
        name="Oxide Thickness",
        min=5.0,
        value=19.54,
        max=60.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Oxide SLD",
        min=3.39e-06,
        value=3.39e-06,
        max=3.41e-06,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Tails Thickness",
        min=15.0,
        value=22.66,
        max=40.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Tails SLD",
        min=-5e-07,
        value=-4.01e-07,
        max=-3e-07,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Tails Hydration",
        min=1.0,
        value=5.252,
        max=50.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Roughness",
        min=1.0,
        value=5.64,
        max=15.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="CW Thickness",
        min=10.0,
        value=17.12,
        max=28.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="CW SLD",
        min=0.0,
        value=0.0,
        max=1e-09,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Heads Thickness",
        min=5.0,
        value=8.56,
        max=17.0,
        fit=True,
        prior_type="gaussian",
        mu=10.0,
        sigma=2.0,
    )
    project.parameters.append(
        name="SAM Heads SLD",
        min=1.0e-07,
        value=1.75e-06,
        max=2.0e-06,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="SAM Heads Hydration",
        min=10.0,
        value=45.45,
        max=50.0,
        fit=True,
        prior_type="uniform",
        mu=30.0,
        sigma=3.0,
    )
    project.parameters.append(
        name="Bilayer Heads Thickness",
        min=7.0,
        value=10.7,
        max=17.0,
        fit=True,
        prior_type="gaussian",
        mu=10.0,
        sigma=2.0,
    )
    project.parameters.append(
        name="Bilayer Heads SLD",
        min=5.0e-07,
        value=1.47e-06,
        max=1.5e-06,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Bilayer Roughness",
        min=2.0,
        value=6.014,
        max=15.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Bilayer Tails Thickness",
        min=14.0,
        value=17.82,
        max=22.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Bilayer Tails SLD",
        min=-5.0e-07,
        value=-4.61e-07,
        max=0.0,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Bilayer Tails Hydration",
        min=10.0,
        value=17.64,
        max=50.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Bilayer Heads Hydration",
        min=10.0,
        value=36.15,
        max=50.0,
        fit=True,
        prior_type="gaussian",
        mu=30.0,
        sigma=3.0,
    )
    project.parameters.append(
        name="CW Hydration",
        min=99.9,
        value=100.0,
        max=100.0,
        fit=False,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )
    project.parameters.append(
        name="Oxide Hydration",
        min=0.0,
        value=23.61,
        max=60.0,
        fit=True,
        prior_type="uniform",
        mu=0.0,
        sigma=np.inf,
    )

    project.parameters.set_fields(0, max=10)

    del project.bulk_in[0]
    project.bulk_in.append(name="Silicon", min=2.0e-06, value=2.073e-06, max=2.1e-06, fit=False)

    del project.bulk_out[0]
    project.bulk_out.append(name="D2O", min=5.50e-06, value=5.98e-06, max=6.4e-06, fit=True)
    project.bulk_out.append(name="SMW", min=1.0e-06, value=2.21e-06, max=4.99e-06, fit=True)

    del project.scalefactors[0]
    project.scalefactors.append(name="Scalefactor 1", min=0.05, value=0.10, max=0.2, fit=False)
    project.scalefactors.append(name="Scalefactor 2", min=0.05, value=0.15, max=0.2, fit=False)

    del project.backgrounds[0]
    del project.background_parameters[0]
    project.background_parameters.append(
        name="Background parameter D2O",
        min=5.0e-10,
        value=2.23e-06,
        max=7.0e-06,
        fit=True,
    )
    project.background_parameters.append(
        name="Background parameter SMW",
        min=1.0e-10,
        value=3.38e-06,
        max=4.99e-06,
        fit=True,
    )

    return project


@pytest.fixture
def reflectivity_calculation_problem():
    """The output C++ ProblemDefinition object for a reflectivity calculation of the project set out in
    "DSPC_standard_layers.py".
    """
    problem = RATapi.rat_core.ProblemDefinition()
    problem.TF = Calculations.NonPolarised
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
    problem.bulkIn = np.array([2.073e-06])
    problem.bulkOut = np.array([5.98e-06, 2.21e-06])
    problem.qzshifts = np.array([0.0])
    problem.scalefactors = np.array([0.1, 0.15])
    problem.domainRatio = np.array([])
    problem.backgroundParams = np.array([2.23e-06, 3.38e-06])
    problem.resolutionParams = np.array([0.03])
    problem.contrastBulkIns = np.array([1.0, 1.0])
    problem.contrastBulkOuts = np.array([1.0, 2.0])
    problem.contrastQzshifts = np.array([0.0, 0.0])
    problem.contrastScalefactors = np.array([1.0, 2.0])
    problem.contrastBackgroundParams = np.array([1.0, 2.0])
    problem.contrastBackgroundActions = np.array([1.0, 1.0])
    problem.contrastResolutionParams = np.array([1.0, 1.0])
    problem.contrastCustomFiles = np.array([np.nan, np.nan])
    problem.contrastDomainRatios = np.array([0.0, 0.0])
    problem.resample = np.array([0.0, 0.0])
    problem.dataPresent = np.array([1.0, 1.0])
    problem.oilChiDataPresent = np.array([0.0, 0.0])
    problem.numberOfContrasts = 2.0
    problem.numberOfLayers = 6.0
    problem.numberOfDomainContrasts = 0.0
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
    problem.otherParams = np.array(
        [
            3.390e-06,
            -4.010e-07,
            0.000e00,
            1.750e-06,
            1.470e-06,
            -4.610e-07,
            1.000e02,
            1.000e-01,
            1.500e-01,
            2.073e-06,
            3.000e-02,
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
    problem.otherLimits = np.array(
        [
            [3.39e-06, 3.41e-06],
            [-5.00e-07, -3.00e-07],
            [0.00e00, 1.00e-09],
            [1.00e-07, 2.00e-06],
            [5.00e-07, 1.50e-06],
            [-5.00e-07, 0.00e00],
            [9.99e01, 1.00e02],
            [5.00e-02, 2.00e-01],
            [5.00e-02, 2.00e-01],
            [2.00e-06, 2.10e-06],
            [1.00e-02, 5.00e-02],
        ],
    )

    return problem


@pytest.fixture
def dream_problem():
    """The output C++ ProblemDefinition object for a Dream optimisation of the project set out in
    "DSPC_standard_layers.py".

    This optimisation used the parameters: nSamples=50000, nChains=10.
    """
    problem = RATapi.rat_core.ProblemDefinition()
    problem.TF = Calculations.NonPolarised
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
    problem.bulkIn = np.array([2.073e-06])
    problem.bulkOut = np.array([6.01489149e-06, 1.59371685e-06])
    problem.qzshifts = np.array([0.0])
    problem.scalefactors = np.array([0.1, 0.15])
    problem.domainRatio = np.array([])
    problem.backgroundParams = np.array([2.37113128e-06, 1.99006694e-06])
    problem.resolutionParams = np.array([0.03])
    problem.contrastBulkIns = np.array([1.0, 1.0])
    problem.contrastBulkOuts = np.array([1.0, 2.0])
    problem.contrastQzshifts = np.array([0.0, 0.0])
    problem.contrastScalefactors = np.array([1.0, 2.0])
    problem.contrastBackgroundParams = np.array([1.0, 2.0])
    problem.contrastBackgroundActions = np.array([1.0, 1.0])
    problem.contrastResolutionParams = np.array([1.0, 1.0])
    problem.contrastCustomFiles = np.array([np.nan, np.nan])
    problem.contrastDomainRatios = np.array([0.0, 0.0])
    problem.resample = np.array([0.0, 0.0])
    problem.dataPresent = np.array([1.0, 1.0])
    problem.oilChiDataPresent = np.array([0.0, 0.0])
    problem.numberOfContrasts = 2.0
    problem.numberOfLayers = 6.0
    problem.numberOfDomainContrasts = 0.0
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
    problem.otherParams = np.array(
        [
            3.390e-06,
            -4.010e-07,
            0.000e00,
            1.750e-06,
            1.470e-06,
            -4.610e-07,
            1.000e02,
            1.000e-01,
            1.500e-01,
            2.073e-06,
            3.000e-02,
            0.000e00,
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
    problem.otherLimits = np.array(
        [
            [3.39e-06, 3.41e-06],
            [-5.00e-07, -3.00e-07],
            [0.00e00, 1.00e-09],
            [1.00e-07, 2.00e-06],
            [5.00e-07, 1.50e-06],
            [-5.00e-07, 0.00e00],
            [9.99e01, 1.00e02],
            [5.00e-02, 2.00e-01],
            [5.00e-02, 2.00e-01],
            [2.00e-06, 2.10e-06],
            [1.00e-02, 5.00e-02],
            [0.00e00, 0.00e00],
        ],
    )

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
        (Procedures.Dream, "dream_problem", "dream_output_results", "dream_bayes", "dream_results"),
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
