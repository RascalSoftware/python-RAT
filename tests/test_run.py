"""
Test the run module using the example calculation from "DSPC_standard_layers.py".

We use the example for both a reflectivity calculation, and Bayesian analysis using the Dream algorithm.
"""

import numpy as np
import pytest
import unittest.mock as mock

import RAT
import RAT.outputs
import RAT.rat_core
from RAT.utils.enums import Procedures

from tests.utils import check_results_equal


@pytest.fixture
def input_project():
    """A cut-down version of the input Project object for a reflectivity calculation set out in
    "DSPC_standard_layers.py".
    """
    project = RAT.Project(name="original_dspc_bilayer", calculation="non polarised", model="standard layers",
                          geometry="substrate/liquid", absorption=False)

    # Set up the relevant parameters
    project.parameters.append(name="Oxide Thickness", min=5.0, value=19.54, max=60.0, fit=True, prior_type="uniform",
                              mu=0.0, sigma=np.inf)
    project.parameters.append(name="Oxide SLD", min=3.39e-06, value=3.39e-06, max=3.41e-06, fit=False,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="SAM Tails Thickness", min=15.0, value=22.66, max=40.0, fit=True,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="SAM Tails SLD", min=-5e-07, value=-4.01e-07, max=-3e-07, fit=False,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="SAM Tails Hydration", min=1.0, value=5.252, max=50.0, fit=True,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="SAM Roughness", min=1.0, value=5.64, max=15.0, fit=True, prior_type="uniform",
                              mu=0.0, sigma=np.inf)
    project.parameters.append(name="CW Thickness", min=10.0, value=17.12, max=28.0, fit=True, prior_type="uniform",
                              mu=0.0, sigma=np.inf)
    project.parameters.append(name="CW SLD", min=0.0, value=0.0, max=1e-09, fit=False, prior_type="uniform", mu=0.0,
                              sigma=np.inf)
    project.parameters.append(name="SAM Heads Thickness", min=5.0, value=8.56, max=17.0, fit=True,
                              prior_type="gaussian", mu=10.0, sigma=2.0)
    project.parameters.append(name="SAM Heads SLD", min=1.0e-07, value=1.75e-06, max=2.0e-06, fit=False,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="SAM Heads Hydration", min=10.0, value=45.45, max=50.0, fit=True,
                              prior_type="uniform", mu=30.0, sigma=3.0)
    project.parameters.append(name="Bilayer Heads Thickness", min=7.0, value=10.7, max=17.0, fit=True,
                              prior_type="gaussian", mu=10.0, sigma=2.0)
    project.parameters.append(name="Bilayer Heads SLD", min=5.0e-07, value=1.47e-06, max=1.5e-06, fit=False,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="Bilayer Roughness", min=2.0, value=6.014, max=15.0, fit=True, prior_type="uniform",
                              mu=0.0, sigma=np.inf)
    project.parameters.append(name="Bilayer Tails Thickness", min=14.0, value=17.82, max=22.0, fit=True,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="Bilayer Tails SLD", min=-5.0e-07, value=-4.61e-07, max=0.0, fit=False,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="Bilayer Tails Hydration", min=10.0, value=17.64, max=50.0, fit=True,
                              prior_type="uniform", mu=0.0, sigma=np.inf)
    project.parameters.append(name="Bilayer Heads Hydration", min=10.0, value=36.15, max=50.0, fit=True,
                              prior_type="gaussian", mu=30.0, sigma=3.0)
    project.parameters.append(name="CW Hydration", min=99.9, value=100.0, max=100.0, fit=False, prior_type="uniform",
                              mu=0.0, sigma=np.inf)
    project.parameters.append(name="Oxide Hydration", min=0.0, value=23.61, max=60.0, fit=True, prior_type="uniform",
                              mu=0.0, sigma=np.inf)

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
    project.background_parameters.append(name="Background parameter D2O", min=5.0e-10, value=2.23e-06, max=7.0e-06,
                                         fit=True)
    project.background_parameters.append(name="Background parameter SMW", min=1.0e-10, value=3.38e-06, max=4.99e-06,
                                         fit=True)

    return project


@pytest.mark.parametrize(["test_procedure", "test_output_problem", "test_output_results", "test_bayes", "test_results"], [
    (Procedures.Calculate, "reflectivity_calculation_problem", "reflectivity_calculation_output_results", None, "reflectivity_calculation_results"),
    (Procedures.Dream, "dream_problem", "dream_output_results", "dream_bayes", "dream_results"),
])
def test_run(test_procedure, test_output_problem, test_output_results, test_bayes, test_results, request) -> None:
    input_project = request.getfixturevalue("input_project")
    test_output_problem = request.getfixturevalue(test_output_problem)
    test_output_results = request.getfixturevalue(test_output_results)
    if test_bayes:
        test_bayes = request.getfixturevalue(test_bayes)

    test_results = request.getfixturevalue(test_results)

    with mock.patch.object(RAT.rat_core, "RATMain", mock.MagicMock(return_value=(test_output_problem,
                                                                                 test_output_results,
                                                                                 test_bayes))):
        # Use default project as we patch RATMain to give the desired outputs
        project, results = RAT.run(input_project, RAT.set_controls(procedure=test_procedure))

    check_results_equal(test_results, results)
