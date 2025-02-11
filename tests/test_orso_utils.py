"""Tests for the RATapi.utils.orso module."""

import os
from io import StringIO
from pathlib import Path

import numpy as np
import pytest
from orsopy.fileio.model_language import SampleModel

from RATapi.examples.bayes_benchmark.bayes_benchmark import get_project
from RATapi.project import Project, class_lists
from RATapi.utils.orso import load_ort_data, orso_model_to_rat, ort_to_project

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


@pytest.fixture
def bare_subs():
    """The bare substrate project from the Bayes Benchmark example."""
    return get_project()


@pytest.fixture
def prist():
    """The project from the model data from prist5_10K_m_025.Rqz.ort"""
    return Project.load(Path(TEST_DIR_PATH, "prist.json"))


@pytest.mark.parametrize(
    "model",
    [
        "air | FeO2 0.75 | Fe 10 | Si",
        "vacuum | 5 (O3 3 | He2 4) | SiO2 0.75 | Si",
        "Si | 5 (O2 2 | 3 (D2O 1 | H2O 1)) | air",
    ],
)
@pytest.mark.parametrize("absorption", [True, False])
def test_orso_model_to_rat(model, absorption):
    """Test that orso_model_to_rat gives the expected parameters, layers and model."""

    expected = SampleModel(model).resolve_to_layers()[1:-1]
    expected_layers = [layer.material.formula for layer in expected]
    expected_thicknesses = {layer.material.formula: layer.thickness for layer in expected}
    expected_roughnesses = {layer.material.formula: layer.roughness for layer in expected}
    actual = orso_model_to_rat(model, absorption=absorption)

    assert actual.model == expected_layers

    for layer in actual.layers:
        assert layer.name in expected_layers
    for layer in expected_layers:
        assert layer in [actual_layer.name for actual_layer in actual.layers]

    expected_parameters = []
    # get set of parameters
    for layer in set(expected_layers):
        expected_parameters.extend([f"{layer} Thickness", f"{layer} Roughness", f"{layer} SLD"])
        if absorption:
            expected_parameters.append(f"{layer} SLD imaginary")

        assert actual.parameters[f"{layer} Thickness"].value == expected_thicknesses[layer].as_unit("angstrom")
        assert actual.parameters[f"{layer} Roughness"].value == expected_roughnesses[layer].as_unit("angstrom")

    assert set(p.name for p in actual.parameters) == set(expected_parameters)


@pytest.mark.parametrize(
    "test_data",
    [
        "bare_substrate.ort",
        "inter_data.ort",
        "polref_data.ort",
        "prist5_10K_m_025.Rqz.ort",
    ],
)
def test_load_ort_data(test_data):
    """Test that .ort data is loaded correctly."""
    # manually get the test data for comparison
    data_strings = [""]
    parsing_data = False
    with Path(TEST_DIR_PATH, test_data).open() as file:
        for line in file:
            if line[0] == "#":
                if parsing_data:
                    parsing_data = False
                    data_strings.append("")
                else:
                    continue
            else:
                parsing_data = True
                data_strings[-1] += line

    expected_data = list(map(lambda s: np.loadtxt(StringIO(s)), data_strings))
    actual_data = load_ort_data(Path(TEST_DIR_PATH, test_data))

    if not isinstance(actual_data, list):
        actual_data = [actual_data]

    assert len(actual_data) == len(expected_data)
    for actual_dataset, expected_dataset in zip(actual_data, expected_data):
        np.testing.assert_array_equal(actual_dataset.data, expected_dataset)


@pytest.mark.parametrize(
    "test_data, expected_data",
    [
        ["bare_substrate.ort", "bare_substrate.json"],
        ["prist5_10K_m_025.Rqz.ort", "prist.json"],
    ],
)
def test_load_ort_project(test_data, expected_data):
    """Test that a project with model data is loaded correctly."""
    project = ort_to_project(Path(TEST_DIR_PATH, test_data))
    exp_project = Project.load(Path(TEST_DIR_PATH, expected_data))

    for class_list in class_lists:
        assert getattr(project, class_list) == getattr(exp_project, class_list)

    for data, exp_data in zip(project.data, exp_project.data):
        np.testing.assert_array_equal(data.data, exp_data.data)
