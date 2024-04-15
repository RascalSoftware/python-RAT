"""Test the inputs module."""

import numpy as np
import pytest

from RAT.controls import set_controls
from RAT.inputs import make_input, make_problem, make_cells
import RAT.project
from RAT.utils.dataclasses import Checks, Limits, Priors, Controls, Problem
import RAT.utils.enums
from tests.utils import NaNList


@pytest.fixture
def test_project():
    """Add parameters to the default project, so each ClassList can be tested properly."""
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
    """Add parameters to the default problem, so each ClassList can be tested properly."""
    return Problem(params=[3.0, 0.0], bulkIn=[0.0], bulkOut=[6.35e-06], qzshifts=[0.0], scalefactors=[0.23],
                   backgroundParams=[1e-06], resolutionParams=[0.03], contrastBulkIns=[1], contrastBulkOuts=[1],
                   contrastQzshifts=[1], contrastScalefactors=[1], contrastBackgrounds=[1],
                   contrastBackgroundsActions=[1], contrastResolutions=[1], contrastCustomFiles=NaNList([float('NaN')]),
                   contrastDomainRatios=[0], resample=[False], dataPresent=[1], oilChiDataPresent=[0],
                   numberOfContrasts=1, numberOfLayers=1, fitParams=[3.0],
                   otherParams=[0.0, 1e-06, 0.23, 0.0, 0.0, 6.35e-06, 0.03], fitLimits=[[1.0, 5.0]],
                   otherLimits=[[0.0, 0.0], [1e-07, 1e-05], [0.02, 0.25], [-0.0001, 0.0001], [0.0, 0.0],
                                [6.2e-06, 6.35e-06], [0.01, 0.05]]
                   )


@pytest.fixture
def test_cells():
    """Add parameters to the default problem, so each ClassList can be tested properly."""
    return [[[0, 1]],
            [np.array([[1.0, 1.0, 1.0]])],
            [[1.0, 1.0]],
            [[1.0, 1.0]],
            [[1]],
            #[],
            ['Substrate Roughness', 'Test SLD'],
            ['Background Param 1'],
            ['Scalefactor 1'],
            ['Qz shift 1'],
            ['SLD Air'],
            ['SLD D2O'],
            ['Resolution Param 1'],
            [''],
            [RAT.models.Types.Constant],
            [RAT.models.Types.Constant],
            [[0.0, 0.0, 0.0]],
            [],
            [],
            [],
            ]


@pytest.fixture
def test_limits():
    """Add parameters to the default problem, so each ClassList can be tested properly."""
    return Limits(param=[[1.0, 5.0], [0.0, 0.0]],
                  backgroundParam=[[1e-7, 1e-5]],
                  qzshift=[[-1e-4, 1e-4]],
                  scalefactor=[[0.02, 0.25]],
                  bulkIn=[[0.0, 0.0]],
                  bulkOut=[[6.2e-6, 6.35e-6]],
                  resolutionParam=[[0.01, 0.05]],
                  domainRatio=[],
                  )


@pytest.fixture
def test_priors():
    """Add parameters to the default problem, so each ClassList can be tested properly."""
    return Priors(param=[['Substrate Roughness', RAT.models.Priors.Uniform, 0.0, np.inf],
                         ['Test SLD', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  backgroundParam=[['Background Param 1', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  qzshift=[['Qz shift 1', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  scalefactor=[['Scalefactor 1', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  bulkIn=[['SLD Air', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  bulkOut=[['SLD D2O', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  resolutionParam=[['Resolution Param 1', RAT.models.Priors.Uniform, 0.0, np.inf]],
                  domainRatio=[],
                  priorNames=['Substrate Roughness', 'Test SLD', 'Background Param 1', 'Scalefactor 1', 'Qz shift 1',
                              'SLD Air', 'SLD D2O', 'Resolution Param 1'],
                  priorValues=[[1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf],
                               [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf], [1, 0.0, np.inf]]
                  )


@pytest.fixture
def test_controls():
    """Add parameters to the default problem, so each ClassList can be tested properly."""
    return Controls(procedure=RAT.utils.enums.Procedures.Calculate,
                    parallel=RAT.utils.enums.Parallel.Single,
                    calcSldDuringFit=False,
                    resampleParams=[0.9, 50.0],
                    display=RAT.utils.enums.Display.Iter,
                    xTolerance=1.0e-6,
                    funcTolerance=1.0e-6,
                    maxFuncEvals=10000,
                    maxIterations=1000,
                    updateFreq=-1,
                    updatePlotFreq=1,
                    populationSize=20,
                    fWeight=0.5,
                    crossoverProbability=0.8,
                    strategy=4,
                    targetValue=1.0,
                    numGenerations=500,
                    nLive=150,
                    nMCMC=0.0,
                    propScale=0.1,
                    nsTolerance=0.1,
                    nSamples=50000,
                    nChains=10,
                    jumpProbability=0.5,
                    pUnitGamma=0.2,
                    boundHandling=RAT.utils.enums.BoundHandling.Fold,
                    checks=Checks(fitParam=[1, 0], fitBackgroundParam=[0], fitQzshift=[0], fitScalefactor=[0],
                                  fitBulkIn=[0], fitBulkOut=[0], fitResolutionParam=[0], fitDomainRatio=[])
                    )


def test_make_input(test_project, test_problem, test_cells, test_limits, test_priors, test_controls) -> None:
    """The problem object should contain the parameters defined in the input project object."""
    problem, cells, limits, priors, controls = make_input(test_project, set_controls())

    assert problem == test_problem
    assert cells_equal(cells, test_cells)
    assert limits == test_limits
    assert priors == test_priors
    assert controls == test_controls


def test_make_problem(test_project, test_problem) -> None:
    """The problem object should contain the parameters defined in the input project object."""
    assert make_problem(test_project) == test_problem


def test_make_cells(test_project, test_cells) -> None:
    """The problem object should contain the parameters defined in the input project object."""
    cells = make_cells(test_project)
    assert cells_equal(cells, test_cells)


def cells_equal(actual_cells, expected_cells) -> bool:
    """Compare two instances of the "cells" input for RATMain."""
    elements = [actual_cells[0] == expected_cells[0]]

    # Need to test equality of the numpy arrays separately
    for (a, b) in zip(actual_cells[1], expected_cells[1]):
        elements.append((a == b).all())

    for i in range(2, len(actual_cells)):
        elements.append(actual_cells[i] == expected_cells[i])

    return all(elements)
