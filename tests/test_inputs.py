"""Test the inputs module."""

import pathlib
import pickle

import numpy as np
import pytest

import RATapi
import RATapi.wrappers
from RATapi.inputs import check_indices, make_controls, make_input, make_problem
from RATapi.rat_core import Checks, Control, Limits, Priors, ProblemDefinition
from RATapi.utils.enums import (
    BackgroundActions,
    BoundHandling,
    Calculations,
    Display,
    Geometries,
    LayerModels,
    Parallel,
    Procedures,
)
from tests.utils import dummy_function


@pytest.fixture
def standard_layers_project():
    """Add parameters to the default project for a normal calculation."""
    test_project = RATapi.Project(
        data=RATapi.ClassList([RATapi.models.Data(name="Test Data", data=np.array([[1.0, 1.0, 1.0]]))]),
    )
    test_project.parameters.append(name="Test Thickness")
    test_project.parameters.append(name="Test SLD")
    test_project.parameters.append(name="Test Roughness")
    test_project.custom_files.append(
        name="Test Custom File",
        filename="python_test.py",
        function_name="dummy_function",
        language="python",
    )
    test_project.layers.append(
        name="Test Layer",
        thickness="Test Thickness",
        SLD="Test SLD",
        roughness="Test Roughness",
    )
    test_project.contrasts.append(
        name="Test Contrast",
        data="Test Data",
        background="Background 1",
        bulk_in="SLD Air",
        bulk_out="SLD D2O",
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        model=["Test Layer"],
    )
    return test_project


@pytest.fixture
def domains_project():
    """Add parameters to the default project for a domains calculation."""
    test_project = RATapi.Project(
        calculation=Calculations.Domains,
        data=RATapi.ClassList([RATapi.models.Data(name="Test Data", data=np.array([[1.0, 1.0, 1.0]]))]),
    )
    test_project.parameters.append(name="Test Thickness")
    test_project.parameters.append(name="Test SLD")
    test_project.parameters.append(name="Test Roughness")
    test_project.custom_files.append(name="Test Custom File", filename="matlab_test.m", language="matlab")
    test_project.layers.append(
        name="Test Layer",
        thickness="Test Thickness",
        SLD="Test SLD",
        roughness="Test Roughness",
    )
    test_project.domain_contrasts.append(name="up", model=["Test Layer"])
    test_project.domain_contrasts.append(name="down", model=["Test Layer"])
    test_project.contrasts.append(
        name="Test Contrast",
        data="Test Data",
        background="Background 1",
        bulk_in="SLD Air",
        bulk_out="SLD D2O",
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        domain_ratio="Domain Ratio 1",
        model=["down", "up"],
    )
    return test_project


@pytest.fixture
def custom_xy_project():
    """Add parameters to the default project for a normal calculation and use the custom xy model."""
    test_project = RATapi.Project(model=LayerModels.CustomXY)
    test_project.parameters.append(name="Test Thickness")
    test_project.parameters.append(name="Test SLD")
    test_project.parameters.append(name="Test Roughness")
    test_project.custom_files.append(name="Test Custom File", filename="cpp_test.dll", language="cpp")
    test_project.contrasts.append(
        name="Test Contrast",
        data="Simulation",
        background="Background 1",
        bulk_in="SLD Air",
        bulk_out="SLD D2O",
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        model=["Test Custom File"],
    )
    return test_project


@pytest.fixture
def standard_layers_problem():
    """The expected problem object from "standard_layers_project"."""
    problem = ProblemDefinition()
    problem.TF = Calculations.Normal
    problem.modelType = LayerModels.StandardLayers
    problem.geometry = Geometries.AirSubstrate
    problem.useImaginary = False
    problem.params = [3.0, 0.0, 0.0, 0.0]
    problem.bulkIns = [0.0]
    problem.bulkOuts = [6.35e-06]
    problem.qzshifts = []
    problem.scalefactors = [0.23]
    problem.domainRatios = []
    problem.backgroundParams = [1e-06]
    problem.resolutionParams = [0.03]
    problem.contrastBulkIns = [1]
    problem.contrastBulkOuts = [1]
    problem.contrastQzshifts = []
    problem.contrastScalefactors = [1]
    problem.contrastBackgroundParams = [[1]]
    problem.contrastBackgroundActions = [BackgroundActions.Add]
    problem.contrastResolutionParams = [1]
    problem.contrastCustomFiles = [float("NaN")]
    problem.contrastDomainRatios = [0]
    problem.resample = [False]
    problem.dataPresent = [1]
    problem.oilChiDataPresent = [0]
    problem.numberOfContrasts = 1
    problem.numberOfLayers = 1
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [3.0]
    problem.otherParams = [0.0, 0.0, 0.0, 1e-06, 0.23, 0.0, 6.35e-06, 0.03]
    problem.fitLimits = [[1.0, 5.0]]
    problem.otherLimits = [
        [0.0, 0.0],
        [0.0, 0.0],
        [0.0, 0.0],
        [1e-07, 1e-05],
        [0.02, 0.25],
        [0.0, 0.0],
        [6.2e-06, 6.35e-06],
        [0.01, 0.05],
    ]

    return problem


@pytest.fixture
def domains_problem():
    """The expected problem object from "standard_layers_project"."""
    problem = ProblemDefinition()
    problem.TF = Calculations.Domains
    problem.modelType = LayerModels.StandardLayers
    problem.geometry = Geometries.AirSubstrate
    problem.useImaginary = False
    problem.params = [3.0, 0.0, 0.0, 0.0]
    problem.bulkIns = [0.0]
    problem.bulkOuts = [6.35e-06]
    problem.qzshifts = []
    problem.scalefactors = [0.23]
    problem.domainRatios = [0.5]
    problem.backgroundParams = [1e-06]
    problem.resolutionParams = [0.03]
    problem.contrastBulkIns = [1]
    problem.contrastBulkOuts = [1]
    problem.contrastQzshifts = []
    problem.contrastScalefactors = [1]
    problem.contrastBackgroundParams = [[1]]
    problem.contrastBackgroundActions = [BackgroundActions.Add]
    problem.contrastResolutionParams = [1]
    problem.contrastCustomFiles = [float("NaN")]
    problem.contrastDomainRatios = [1]
    problem.resample = [False]
    problem.dataPresent = [1]
    problem.oilChiDataPresent = [0]
    problem.numberOfContrasts = 1
    problem.numberOfLayers = 1
    problem.numberOfDomainContrasts = 2
    problem.fitParams = [3.0]
    problem.otherParams = [0.0, 0.0, 0.0, 1e-06, 0.23, 0.0, 6.35e-06, 0.03, 0.5]
    problem.fitLimits = [[1.0, 5.0]]
    problem.otherLimits = [
        [0.0, 0.0],
        [0.0, 0.0],
        [0.0, 0.0],
        [1e-07, 1e-05],
        [0.02, 0.25],
        [0.0, 0.0],
        [6.2e-06, 6.35e-06],
        [0.01, 0.05],
        [0.4, 0.6],
    ]

    return problem


@pytest.fixture
def custom_xy_problem():
    """The expected problem object from "custom_xy_project"."""
    problem = ProblemDefinition()
    problem.TF = Calculations.Normal
    problem.modelType = LayerModels.CustomXY
    problem.geometry = Geometries.AirSubstrate
    problem.useImaginary = False
    problem.params = [3.0, 0.0, 0.0, 0.0]
    problem.bulkIns = [0.0]
    problem.bulkOuts = [6.35e-06]
    problem.qzshifts = []
    problem.scalefactors = [0.23]
    problem.domainRatios = []
    problem.backgroundParams = [1e-06]
    problem.resolutionParams = [0.03]
    problem.contrastBulkIns = [1]
    problem.contrastBulkOuts = [1]
    problem.contrastQzshifts = []
    problem.contrastScalefactors = [1]
    problem.contrastBackgroundParams = [[1]]
    problem.contrastBackgroundActions = [BackgroundActions.Add]
    problem.contrastResolutionParams = [1]
    problem.contrastCustomFiles = [1]
    problem.contrastDomainRatios = [0]
    problem.resample = [False]
    problem.dataPresent = [0]
    problem.oilChiDataPresent = [0]
    problem.numberOfContrasts = 1
    problem.numberOfLayers = 0
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [3.0]
    problem.otherParams = [0.0, 0.0, 0.0, 1e-06, 0.23, 0.0, 6.35e-06, 0.03]
    problem.fitLimits = [[1.0, 5.0]]
    problem.otherLimits = [
        [0.0, 0.0],
        [0.0, 0.0],
        [0.0, 0.0],
        [1e-07, 1e-05],
        [0.02, 0.25],
        [0.0, 0.0],
        [6.2e-06, 6.35e-06],
        [0.01, 0.05],
    ]

    return problem


@pytest.fixture
def normal_limits():
    """The expected limits object from "standard_layers_project" and "custom_xy_project"."""
    limits = Limits()
    limits.params = [[1.0, 5.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]
    limits.backgroundParams = [[1e-7, 1e-5]]
    limits.qzshifts = []
    limits.scalefactors = [[0.02, 0.25]]
    limits.bulkIns = [[0.0, 0.0]]
    limits.bulkOuts = [[6.2e-6, 6.35e-6]]
    limits.resolutionParams = [[0.01, 0.05]]
    limits.domainRatios = []

    return limits


@pytest.fixture
def domains_limits():
    """The expected limits object from "domains_project"."""
    limits = Limits()
    limits.params = [[1.0, 5.0], [0.0, 0.0], [0.0, 0.0], [0.0, 0.0]]
    limits.backgroundParams = [[1e-7, 1e-5]]
    limits.qzshifts = []
    limits.scalefactors = [[0.02, 0.25]]
    limits.bulkIns = [[0.0, 0.0]]
    limits.bulkOuts = [[6.2e-6, 6.35e-6]]
    limits.resolutionParams = [[0.01, 0.05]]
    limits.domainRatios = [[0.4, 0.6]]

    return limits


@pytest.fixture
def normal_priors():
    """The expected priors object from "standard_layers_project" and "custom_xy_project"."""
    priors = Priors()
    priors.params = [
        ["Substrate Roughness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test Thickness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test SLD", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test Roughness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
    ]
    priors.backgroundParams = [["Background Param 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.qzshifts = []
    priors.scalefactors = [["Scalefactor 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.bulkIns = [["SLD Air", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.bulkOuts = [["SLD D2O", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.resolutionParams = [["Resolution Param 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.domainRatios = []
    priors.priorNames = [
        "Substrate Roughness",
        "Test Thickness",
        "Test SLD",
        "Test Roughness",
        "Background Param 1",
        "Scalefactor 1",
        "SLD Air",
        "SLD D2O",
        "Resolution Param 1",
    ]
    priors.priorValues = [
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
    ]

    return priors


@pytest.fixture
def domains_priors():
    """The expected priors object from "domains_project"."""
    priors = Priors()
    priors.params = [
        ["Substrate Roughness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test Thickness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test SLD", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
        ["Test Roughness", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf],
    ]
    priors.backgroundParams = [["Background Param 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.qzshifts = []
    priors.scalefactors = [["Scalefactor 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.bulkIns = [["SLD Air", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.bulkOuts = [["SLD D2O", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.resolutionParams = [["Resolution Param 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.domainRatios = [["Domain Ratio 1", RATapi.utils.enums.Priors.Uniform, 0.0, np.inf]]
    priors.priorNames = [
        "Substrate Roughness",
        "Test Thickness",
        "Test SLD",
        "Test Roughness",
        "Background Param 1",
        "Scalefactor 1",
        "SLD Air",
        "SLD D2O",
        "Resolution Param 1",
        "Domain Ratio 1",
    ]
    priors.priorValues = [
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
        [1, 0.0, np.inf],
    ]

    return priors


@pytest.fixture
def standard_layers_controls():
    """The expected controls object for input to the compiled RAT code given the default inputs and
    "standard_layers_project".
    """
    controls = Control()
    controls.procedure = Procedures.Calculate
    controls.parallel = Parallel.Single
    controls.calcSldDuringFit = False
    controls.resampleMinAngle = 0.9
    controls.resampleNPoints = 50.0
    controls.display = Display.Iter
    controls.xTolerance = 1.0e-6
    controls.funcTolerance = 1.0e-6
    controls.maxFuncEvals = 10000
    controls.maxIterations = 1000
    controls.updateFreq = 1
    controls.updatePlotFreq = 20
    controls.populationSize = 20
    controls.fWeight = 0.5
    controls.crossoverProbability = 0.8
    controls.strategy = 4
    controls.targetValue = 1.0
    controls.numGenerations = 500
    controls.nLive = 150
    controls.nMCMC = 0.0
    controls.propScale = 0.1
    controls.nsTolerance = 0.1
    controls.nSamples = 20000
    controls.nChains = 10
    controls.jumpProbability = 0.5
    controls.pUnitGamma = 0.2
    controls.boundHandling = BoundHandling.Reflect
    controls.adaptPCR = True
    # controls.checks.fitParam = [1, 0, 0, 0]
    # controls.checks.fitBackgroundParam = [0]
    # controls.checks.fitQzshift = []
    # controls.checks.fitScalefactor = [0]
    # controls.checks.fitBulkIn = [0]
    # controls.checks.fitBulkOut = [0]
    # controls.checks.fitResolutionParam = [0]
    # controls.checks.fitDomainRatio = []

    return controls


@pytest.fixture
def custom_xy_controls():
    """The expected controls object for input to the compiled RAT code given the default inputs and
    "custom_xy_project".
    """
    controls = Control()
    controls.procedure = Procedures.Calculate
    controls.parallel = Parallel.Single
    controls.calcSldDuringFit = True
    controls.resampleMinAngle = 0.9
    controls.resampleNPoints = 50.0
    controls.display = Display.Iter
    controls.xTolerance = 1.0e-6
    controls.funcTolerance = 1.0e-6
    controls.maxFuncEvals = 10000
    controls.maxIterations = 1000
    controls.updateFreq = 1
    controls.updatePlotFreq = 20
    controls.populationSize = 20
    controls.fWeight = 0.5
    controls.crossoverProbability = 0.8
    controls.strategy = 4
    controls.targetValue = 1.0
    controls.numGenerations = 500
    controls.nLive = 150
    controls.nMCMC = 0.0
    controls.propScale = 0.1
    controls.nsTolerance = 0.1
    controls.nSamples = 20000
    controls.nChains = 10
    controls.jumpProbability = 0.5
    controls.pUnitGamma = 0.2
    controls.boundHandling = BoundHandling.Reflect
    controls.adaptPCR = True
    # controls.checks.fitParam = [1, 0, 0, 0]
    # controls.checks.fitBackgroundParam = [0]
    # controls.checks.fitQzshift = []
    # controls.checks.fitScalefactor = [0]
    # controls.checks.fitBulkIn = [0]
    # controls.checks.fitBulkOut = [0]
    # controls.checks.fitResolutionParam = [0]
    # controls.checks.fitDomainRatio = []

    return controls


@pytest.fixture
def test_checks():
    """The expected checks object from "standard_layers_project", "domains_project" and "custom_xy_project"."""
    checks = Checks()
    checks.params = [1, 0, 0, 0]
    checks.backgroundParams = [0]
    checks.qzshifts = []
    checks.scalefactors = [0]
    checks.bulkIns = [0]
    checks.bulkOuts = [0]
    checks.resolutionParams = [0]
    checks.domainRatios = []

    return checks


@pytest.mark.parametrize(
    ["test_project", "test_problem", "test_limits", "test_priors", "test_controls"],
    [
        (
            "standard_layers_project",
            "standard_layers_problem",
            "normal_limits",
            "normal_priors",
            "standard_layers_controls",
        ),
        (
            "custom_xy_project",
            "custom_xy_problem",
            "normal_limits",
            "normal_priors",
            "custom_xy_controls",
        ),
        (
            "domains_project",
            "domains_problem",
            "domains_limits",
            "domains_priors",
            "standard_layers_controls",
        ),
    ],
)
def test_make_input(test_project, test_problem, test_limits, test_priors, test_controls, request) -> None:
    """When converting the "project" and "controls", we should obtain the five input objects required for the compiled
    RAT code.
    """
    test_project = request.getfixturevalue(test_project)
    test_problem = request.getfixturevalue(test_problem)
    test_limits = request.getfixturevalue(test_limits)
    test_priors = request.getfixturevalue(test_priors)
    test_controls = request.getfixturevalue(test_controls)

    parameter_fields = [
        "params",
        "backgroundParams",
        "scalefactors",
        "qzshifts",
        "bulkIns",
        "bulkOuts",
        "resolutionParams",
        "domainRatios",
    ]

    problem, limits, priors, controls = make_input(test_project, RATapi.Controls())
    problem = pickle.loads(pickle.dumps(problem))
    check_problem_equal(problem, test_problem)

    limits = pickle.loads(pickle.dumps(limits))
    for limit_field in parameter_fields:
        assert np.all(getattr(limits, limit_field) == getattr(test_limits, limit_field))

    priors = pickle.loads(pickle.dumps(priors))
    for prior_field in parameter_fields:
        assert getattr(priors, prior_field) == getattr(test_priors, prior_field)

    assert priors.priorNames == test_priors.priorNames
    assert np.all(priors.priorValues == test_priors.priorValues)

    controls = pickle.loads(pickle.dumps(controls))
    check_controls_equal(controls, test_controls)


@pytest.mark.parametrize(
    ["test_project", "test_check", "test_problem"],
    [
        ("standard_layers_project", "test_checks", "standard_layers_problem"),
        ("custom_xy_project", "test_checks", "custom_xy_problem"),
        ("domains_project", "test_checks", "domains_problem"),
    ],
)
def test_make_problem(test_project, test_problem, test_check, request) -> None:
    """The problem object should contain the relevant parameters defined in the input project object."""
    test_project = request.getfixturevalue(test_project)
    test_problem = request.getfixturevalue(test_problem)
    test_check = request.getfixturevalue(test_check)

    problem = make_problem(test_project, test_check)
    check_problem_equal(problem, test_problem)


@pytest.mark.parametrize(
    "test_problem",
    [
        "standard_layers_problem",
        "custom_xy_problem",
        "domains_problem",
    ],
)
def test_check_indices(test_problem, request) -> None:
    """The check_indices routine should not raise errors for a properly defined ProblemDefinition object."""
    test_problem = request.getfixturevalue(test_problem)

    check_indices(test_problem)


@pytest.mark.parametrize(
    ["test_problem", "index_list", "bad_value"],
    [
        ("standard_layers_problem", "contrastBulkIns", [0.0]),
        ("standard_layers_problem", "contrastBulkIns", [2.0]),
        ("standard_layers_problem", "contrastBulkOuts", [0.0]),
        ("standard_layers_problem", "contrastBulkOuts", [2.0]),
        ("standard_layers_problem", "contrastScalefactors", [0.0]),
        ("standard_layers_problem", "contrastScalefactors", [2.0]),
        # ("standard_layers_problem", "contrastBackgroundParams", [0.0]),
        # ("standard_layers_problem", "contrastBackgroundParams", [2.0]),
        ("standard_layers_problem", "contrastResolutionParams", [0.0]),
        ("standard_layers_problem", "contrastResolutionParams", [2.0]),
        ("custom_xy_problem", "contrastBulkIns", [0.0]),
        ("custom_xy_problem", "contrastBulkIns", [2.0]),
        ("custom_xy_problem", "contrastBulkOuts", [0.0]),
        ("custom_xy_problem", "contrastBulkOuts", [2.0]),
        ("custom_xy_problem", "contrastScalefactors", [0.0]),
        ("custom_xy_problem", "contrastScalefactors", [2.0]),
        # ("custom_xy_problem", "contrastBackgroundParams", [0.0]),
        # ("custom_xy_problem", "contrastBackgroundParams", [2.0]),
        ("custom_xy_problem", "contrastResolutionParams", [0.0]),
        ("custom_xy_problem", "contrastResolutionParams", [2.0]),
        ("domains_problem", "contrastBulkIns", [0.0]),
        ("domains_problem", "contrastBulkIns", [2.0]),
        ("domains_problem", "contrastBulkOuts", [0.0]),
        ("domains_problem", "contrastBulkOuts", [2.0]),
        ("domains_problem", "contrastScalefactors", [0.0]),
        ("domains_problem", "contrastScalefactors", [2.0]),
        ("domains_problem", "contrastDomainRatios", [0.0]),
        ("domains_problem", "contrastDomainRatios", [2.0]),
        # ("domains_problem", "contrastBackgroundParams", [0.0]),
        # ("domains_problem", "contrastBackgroundParams", [2.0]),
        ("domains_problem", "contrastResolutionParams", [0.0]),
        ("domains_problem", "contrastResolutionParams", [2.0]),
    ],
)
def test_check_indices_error(test_problem, index_list, bad_value, request) -> None:
    """The check_indices routine should raise an IndexError if a contrast list contains an index that is out of the
    range of the corresponding parameter list in a ProblemDefinition object.
    """
    param_list = {
        "contrastBulkIns": "bulkIns",
        "contrastBulkOuts": "bulkOuts",
        "contrastScalefactors": "scalefactors",
        "contrastDomainRatios": "domainRatios",
        "contrastBackgroundParams": "backgroundParams",
        "contrastResolutionParams": "resolutionParams",
    }

    test_problem = request.getfixturevalue(test_problem)
    setattr(test_problem, index_list, bad_value)

    with pytest.raises(
        IndexError,
        match=f'The problem field "{index_list}" contains: {bad_value[0]}, which lie '
        f'outside of the range of "{param_list[index_list]}"',
    ):
        check_indices(test_problem)


def test_get_python_handle():
    path = pathlib.Path(__file__).parent.resolve()
    assert RATapi.inputs.get_python_handle("utils.py", "dummy_function", path).__code__ == dummy_function.__code__


def test_make_controls(standard_layers_controls) -> None:
    """The controls object should contain the full set of controls parameters, with the appropriate set defined by the
    input controls.
    """
    controls = make_controls(RATapi.Controls())
    check_controls_equal(controls, standard_layers_controls)


def check_problem_equal(actual_problem, expected_problem) -> None:
    """Compare two instances of the "problem" object for equality."""
    scalar_fields = [
        "TF",
        "modelType",
        "geometry",
        "useImaginary",
        "numberOfContrasts",
        "numberOfLayers",
        "numberOfDomainContrasts",
    ]

    array_fields = [
        "params",
        "backgroundParams",
        "qzshifts",
        "scalefactors",
        "bulkIns",
        "bulkOuts",
        "resolutionParams",
        "domainRatios",
        "contrastBackgroundParams",
        "contrastBackgroundActions",
        "contrastQzshifts",
        "contrastScalefactors",
        "contrastBulkIns",
        "contrastBulkOuts",
        "contrastResolutionParams",
        "contrastDomainRatios",
        "resample",
        "dataPresent",
        "oilChiDataPresent",
        "fitParams",
        "otherParams",
        "fitLimits",
        "otherLimits",
    ]

    for scalar_field in scalar_fields:
        assert getattr(actual_problem, scalar_field) == getattr(expected_problem, scalar_field)
    for array_field in array_fields:
        assert np.all(getattr(actual_problem, array_field) == getattr(expected_problem, array_field))

    # Need to account for "NaN" entries in contrastCustomFiles field
    assert (actual_problem.contrastCustomFiles == expected_problem.contrastCustomFiles).all() or [
        "NaN" if np.isnan(el) else el for el in actual_problem.contrastCustomFiles
    ] == ["NaN" if np.isnan(el) else el for el in expected_problem.contrastCustomFiles]


# def check_cells_equal(actual_cells, expected_cells) -> None:
#     """Compare two instances of the "cells" object for equality."""
#     assert actual_cells.f1 == expected_cells.f1

#     # Need to test equality of the numpy arrays separately
#     for a, b in zip(actual_cells.f2, expected_cells.f2):
#         assert (a == b).all()

#     # f6 may contain "NaN" values, so consider separately
#     assert actual_cells.f6 == expected_cells.f6 or [
#         "NaN" if np.isnan(el) else el for entry in actual_cells.f6 for el in entry
#     ] == ["NaN" if np.isnan(el) else el for entry in expected_cells.f6 for el in entry]

#     mocked_matlab_future = mock.MagicMock()
#     mocked_engine = mock.MagicMock()
#     mocked_matlab_future.result.return_value = mocked_engine
#     with (
#         mock.patch.object(
#             RATapi.wrappers.MatlabWrapper,
#             "loader",
#             mocked_matlab_future,
#         ),
#         mock.patch.object(RATapi.rat_core, "DylibEngine", mock.MagicMock()),
#         mock.patch.object(
#             RATapi.inputs,
#             "get_python_handle",
#             mock.MagicMock(return_value=dummy_function),
#         ),
#         mock.patch.object(
#             RATapi.wrappers.MatlabWrapper,
#             "getHandle",
#             mock.MagicMock(return_value=dummy_function),
#         ),
#         mock.patch.object(RATapi.wrappers.DylibWrapper, "getHandle", mock.MagicMock(return_value=dummy_function)),
#     ):
#         assert list(actual_cells.f14) == expected_cells.f14

#     for index in chain(range(3, 6), range(7, 14), range(15, 21)):
#         field = f"f{index}"
#         assert getattr(actual_cells, field) == getattr(expected_cells, field)


def check_controls_equal(actual_controls, expected_controls) -> None:
    """Compare two instances of the "controls" object used as input for the compiled RAT code for equality."""
    controls_fields = [
        "procedure",
        "parallel",
        "calcSldDuringFit",
        "resampleMinAngle",
        "resampleNPoints",
        "display",
        "xTolerance",
        "funcTolerance",
        "maxFuncEvals",
        "maxIterations",
        "updateFreq",
        "updatePlotFreq",
        "populationSize",
        "fWeight",
        "crossoverProbability",
        "strategy",
        "targetValue",
        "numGenerations",
        "nLive",
        "nMCMC",
        "propScale",
        "nsTolerance",
        "nSamples",
        "nChains",
        "jumpProbability",
        "pUnitGamma",
        "boundHandling",
        "adaptPCR",
    ]

    for field in controls_fields:
        assert getattr(actual_controls, field) == getattr(expected_controls, field)
