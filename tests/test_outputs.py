"""Test the outputs module using the example calculation from "DSPC_standard_layers.py".

We use the example for both a reflectivity calculation, and Bayesian analysis using the Dream algorithm.
"""

import pytest

import RATapi
import RATapi.outputs
import RATapi.rat_core
from RATapi.utils.enums import Procedures
from tests.utils import check_results_equal


@pytest.mark.parametrize(
    ["test_procedure", "test_output_results", "test_bayes", "test_results"],
    [
        (Procedures.Calculate, "reflectivity_calculation_output_results", None, "reflectivity_calculation_results"),
        (Procedures.Dream, "dream_output_results", "dream_bayes", "dream_results"),
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
