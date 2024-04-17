"""Test the inputs module."""

import numpy as np
import pytest

from RAT.controls import set_controls
from RAT.inputs import make_input, make_problem, make_cells, make_controls
import RAT.project
import RAT.utils.enums

from RAT.rat_core import Cells, Checks, Control, Limits, Priors, ProblemDefinition


@pytest.fixture
def test_project():
    """Add parameters to the default project."""
    test_project = RAT.Project(data=RAT.ClassList([RAT.models.Data(name='Simulation', data=np.array([[1.0, 1.0, 1.0]]))]))
    test_project.parameters.append(name='Test SLD')
    test_project.custom_files.append(name='Test Custom File')
    test_project.layers.append(name='Test Layer', SLD='Test SLD')
    test_project.contrasts.append(name='Test Contrast', data='Simulation', background='Background 1', bulk_in='SLD Air',
                                  bulk_out='SLD D2O', scalefactor='Scalefactor 1', resolution='Resolution 1',
                                  model=['Test Layer'])
    return test_project


@pytest.fixture
def test_problem():
    """The expected problem object from "test_project"."""
    problem = ProblemDefinition()
    problem.TF = RAT.utils.enums.Calculations.NonPolarised
    problem.modelType = RAT.utils.enums.Models.StandardLayers
    problem.geometry = RAT.utils.enums.Geometries.AirSubstrate
    problem.useImaginary = False
    problem.params = [3.0, 0.0]
    problem.bulkIn = [0.0]
    problem.bulkOut = [6.35e-06]
    problem.qzshifts = [0.0]
    problem.scalefactors = [0.23]
    problem.domainRatio = []
    problem.backgroundParams = [1e-06]
    problem.resolutionParams = [0.03]
    problem.contrastBulkIns = [1]
    problem.contrastBulkOuts = [1]
    problem.contrastQzshifts = [1]
    problem.contrastScalefactors = [1]
    problem.contrastBackgrounds = [1]
    problem.contrastBackgroundActions = [1]
    problem.contrastResolutions = [1]
    problem.contrastCustomFiles = [float('NaN')]
    problem.contrastDomainRatios = [0]
    problem.resample = [False]
    problem.dataPresent = [1]
    problem.oilChiDataPresent = [0]
    problem.numberOfContrasts = 1
    problem.numberOfLayers = 1
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [3.0]
    problem.otherParams = [0.0, 1e-06, 0.23, 0.0, 0.0, 6.35e-06, 0.03]
    problem.fitLimits = [[1.0, 5.0]]
    problem.otherLimits = [[0.0, 0.0], [1e-07, 1e-05], [0.02, 0.25], [-0.0001, 0.0001], [0.0, 0.0], [6.2e-06, 6.35e-06],
                           [0.01, 0.05]]

    return problem


@pytest.fixture
def test_cells():
    """The expected cells object from "test_project"."""
    cells = Cells()
    cells.f1 = [[0, 1]]
    cells.f2 = [np.array([[1.0, 1.0, 1.0]])]
    cells.f3 = [[1.0, 1.0]]
    cells.f4 = [[1.0, 1.0]]
    cells.f5 = [[1]]
    #cells.f6 = []
    cells.f7 = ['Substrate Roughness', 'Test SLD']
    cells.f8 = ['Background Param 1']
    cells.f9 = ['Scalefactor 1']
    cells.f10 = ['Qz shift 1']
    cells.f11 = ['SLD Air']
    cells.f12 = ['SLD D2O']
    cells.f13 = ['Resolution Param 1']
    cells.f14 = ['']
    cells.f15 = [RAT.models.Types.Constant]
    cells.f16 = [RAT.models.Types.Constant]
    cells.f17 = [[0.0, 0.0, 0.0]]
    cells.f18 = []
    cells.f19 = []
    cells.f20 = []

    return cells


@pytest.fixture
def test_limits():
    """The expected limits object from "test_project"."""
    limits = Limits()
    limits.param = [[1.0, 5.0], [0.0, 0.0]]
    limits.backgroundParam = [[1e-7, 1e-5]]
    limits.qzshift = [[-1e-4, 1e-4]]
    limits.scalefactor = [[0.02, 0.25]]
    limits.bulkIn = [[0.0, 0.0]]
    limits.bulkOut = [[6.2e-6, 6.35e-6]]
    limits.resolutionParam = [[0.01, 0.05]]
    limits.domainRatio = []

    return limits


@pytest.fixture
def test_priors():
    """The expected priors object from "test_project"."""
    priors = Priors()
    priors.param = [['Substrate Roughness', RAT.models.Priors.Uniform, 0.0, np.inf],
                    ['Test SLD', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.backgroundParam = [['Background Param 1', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.qzshift = [['Qz shift 1', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.scalefactor = [['Scalefactor 1', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.bulkIn = [['SLD Air', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.bulkOut = [['SLD D2O', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.resolutionParam = [['Resolution Param 1', RAT.models.Priors.Uniform, 0.0, np.inf]]
    priors.domainRatio = []
    priors.priorNames = ['Substrate Roughness', 'Test SLD', 'Background Param 1', 'Scalefactor 1', 'Qz shift 1',
                         'SLD Air', 'SLD D2O', 'Resolution Param 1']
    priors.priorValues = [[1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf],
                          [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf]]

    return priors


@pytest.fixture
def test_controls():
    """The expected controls object for input to the compiled RAT code given the default inputs and "test_project"."""
    controls = Control()
    controls.procedure = RAT.utils.enums.Procedures.Calculate
    controls.parallel = RAT.utils.enums.Parallel.Single
    controls.calcSldDuringFit = False
    controls.resampleParams = [0.9, 50.0]
    controls.display = RAT.utils.enums.Display.Iter
    controls.xTolerance = 1.0e-6
    controls.funcTolerance = 1.0e-6
    controls.maxFuncEvals = 10000
    controls.maxIterations = 1000
    controls.updateFreq = -1
    controls.updatePlotFreq = 1
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
    controls.nSamples = 50000
    controls.nChains = 10
    controls.jumpProbability = 0.5
    controls.pUnitGamma = 0.2
    controls.boundHandling = RAT.utils.enums.BoundHandling.Fold
    controls.checks.fitParam = [1, 0]
    controls.checks.fitBackgroundParam = [0]
    controls.checks.fitQzshift = [0]
    controls.checks.fitScalefactor = [0]
    controls.checks.fitBulkIn = [0]
    controls.checks.fitBulkOut = [0]
    controls.checks.fitResolutionParam = [0]
    controls.checks.fitDomainRatio = []

    return controls


@pytest.fixture
def test_checks():
    """The expected checks object from "test_project"."""
    checks = Checks()
    checks.fitParam = [1, 0]
    checks.fitBackgroundParam = [0]
    checks.fitQzshift = [0]
    checks.fitScalefactor = [0]
    checks.fitBulkIn = [0]
    checks.fitBulkOut = [0]
    checks.fitResolutionParam = [0]
    checks.fitDomainRatio = []

    return checks


def test_make_input(test_project, test_problem, test_cells, test_limits, test_priors, test_controls) -> None:
    """When converting the "project" and "controls", we should obtain the five input objects required for the compiled
    RAT code.
    """
    parameter_fields = ["param", "backgroundParam", "scalefactor", "qzshift", "bulkIn", "bulkOut", "resolutionParam",
                        "domainRatio"]

    problem, cells, limits, priors, controls = make_input(test_project, set_controls())

    assert problem_equal(problem, test_problem)
    assert cells_equal(cells, test_cells)

    for limit_field in parameter_fields:
        assert (getattr(limits, limit_field) == getattr(test_limits, limit_field)).all()

    for prior_field in parameter_fields:
        assert getattr(priors, prior_field) == getattr(test_priors, prior_field)

    assert priors.priorNames == test_priors.priorNames
    assert (priors.priorValues == test_priors.priorValues).all()

    assert controls_equal(controls, test_controls)


def test_make_problem(test_project, test_problem) -> None:
    """The problem object should contain the relevant parameters defined in the input project object."""
    problem = make_problem(test_project)
    assert problem_equal(problem, test_problem)


def test_make_cells(test_project, test_cells) -> None:
    """The cells object should be populated according to the input project object."""
    cells = make_cells(test_project)
    assert cells_equal(cells, test_cells)


def test_make_controls(test_controls, test_checks) -> None:
    """The controls object should contain the full set of controls parameters, with the appropriate set defined by the
    input controls.
    """
    controls = make_controls(set_controls(), test_checks)
    assert controls_equal(controls, test_controls)


def problem_equal(actual_problem, expected_problem) -> bool:
    """Compare two instances of the "problem" object."""
    scalar_fields = ["TF", "modelType", "geometry", "useImaginary", "numberOfContrasts", "numberOfLayers",
                     "numberOfDomainContrasts"]

    array_fields = ["params", "backgroundParams", "qzshifts", "scalefactors", "bulkIn", "bulkOut", "resolutionParams",
                    "domainRatio", "contrastBackgrounds", "contrastBackgroundActions", "contrastQzshifts",
                    "contrastScalefactors", "contrastBulkIns", "contrastBulkOuts", "contrastResolutions",
                    "contrastDomainRatios", "resample", "dataPresent", "oilChiDataPresent", "fitParams", "otherParams",
                    "fitLimits", "otherLimits"]

    fields_equal = []
    for scalar_field in scalar_fields:
        fields_equal.append(getattr(actual_problem, scalar_field) == getattr(expected_problem, scalar_field))
    for array_field in array_fields:
        fields_equal.append((getattr(actual_problem, array_field) == getattr(expected_problem, array_field)).all())

    # Need to account for "NaN" entries in contrastCustomFiles field
    fields_equal.append(((actual_problem.contrastCustomFiles == expected_problem.contrastCustomFiles).all() or
                         ['NaN' if np.isnan(el) else el for el in actual_problem.contrastCustomFiles] ==
                         ['NaN' if np.isnan(el) else el for el in expected_problem.contrastCustomFiles]
                         ))

    return all(fields_equal)


def cells_equal(actual_cells, expected_cells) -> bool:
    """Compare two instances of the "cells" object."""
    elements_equal = [actual_cells.f1 == expected_cells.f1]

    # Need to test equality of the numpy arrays separately
    for (a, b) in zip(actual_cells.f2, expected_cells.f2):
        elements_equal.append((a == b).all())

    for index in range(3, 21):
        field = f"f{index}"
        elements_equal.append(getattr(actual_cells, field) == getattr(expected_cells, field))

    return all(elements_equal)


def controls_equal(actual_controls, expected_controls) -> bool:
    """Compare two instances of the "controls" object used as input for the compiled RAT code."""
    controls_fields = ["procedure", "parallel", "calcSldDuringFit", "display", "xTolerance",
                       "funcTolerance", "maxFuncEvals", "maxIterations", "updateFreq", "updatePlotFreq",
                       "populationSize", "fWeight", "crossoverProbability", "strategy", "targetValue", "numGenerations",
                       "nLive", "nMCMC", "propScale", "nsTolerance", "nSamples", "nChains", "jumpProbability",
                       "pUnitGamma", "boundHandling", "adaptPCR"]
    checks_fields = ["fitParam", "fitBackgroundParam", "fitQzshift", "fitScalefactor", "fitBulkIn", "fitBulkOut",
                     "fitResolutionParam", "fitDomainRatio"]

    # Check "resampleParams" separately as it is an array
    fields_equal = [(actual_controls.resampleParams == expected_controls.resampleParams).all()]
    for field in controls_fields:
        fields_equal.append(getattr(actual_controls, field) == getattr(expected_controls, field))
    for field in checks_fields:
        fields_equal.append((getattr(actual_controls.checks, field) == getattr(expected_controls.checks, field)).all())

    return all(fields_equal)
