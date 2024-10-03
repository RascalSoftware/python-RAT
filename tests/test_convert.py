"""Test conversion to Project files."""

import importlib
import os
import tempfile

import pytest

import RATapi
from RATapi.utils.convert import project_class_to_r1, project_from_json, project_to_json, r1_to_project_class

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


@pytest.fixture
def dspc_bilayer():
    """The Project from the DSPC standard layers example,
    with some changes to make it compatible with R1.

    """
    project, _ = RATapi.examples.DSPC_standard_layers()

    # change parameters to standardise arguments not in R1
    for class_list in RATapi.project.parameter_class_lists:
        params = getattr(project, class_list)
        for param in params:
            param.prior_type = "uniform"
            param.mu = 0.0
            param.sigma = float("inf")

    for i in range(0, len(project.background_parameters)):
        param = project.background_parameters[i]
        for background in project.backgrounds:
            if background.value_1 == param.name:
                background.value_1 = f"Background parameter {i+1}"
        param.name = f"Background parameter {i+1}"

    for i in range(0, len(project.resolution_parameters)):
        param = project.resolution_parameters[i]
        for resolution in project.resolutions:
            if resolution.value_1 == param.name:
                resolution.value_1 = f"Resolution parameter {i+1}"
        param.name = f"Resolution parameter {i+1}"

    return project


@pytest.mark.parametrize(
    ["file", "project"],
    [
        ["R1defaultProject.mat", "r1_default_project"],
        ["R1monolayerVolumeModel.mat", "r1_monolayer"],
        ["R1DSPCBilayer.mat", "dspc_bilayer"],
    ],
)
def test_r1_to_project_class(file, project, request):
    """Test that R1 to Project class conversion returns the expected Project."""
    output_project = r1_to_project_class(os.path.join(TEST_DIR_PATH, file))
    expected_project = request.getfixturevalue(project)

    # assert statements have to be more careful due to R1 missing features
    # e.g. R1 doesn't support background parameter names, mu, sigma...
    for class_list in RATapi.project.class_lists:
        assert getattr(output_project, class_list) == getattr(expected_project, class_list)


@pytest.mark.parametrize(
    "project",
    [
        "r1_default_project",
        "r1_monolayer",
        "dspc_bilayer",
    ],
)
def test_r1_involution(project, request, monkeypatch):
    """Test that converting a Project to an R1 struct and back returns the same project."""
    original_project = request.getfixturevalue(project)
    r1_struct = project_class_to_r1(original_project, return_struct=True)

    # rather than writing the struct to a file and reading the file, just directly
    # hand the struct over
    def mock_load(ignored_filename, **ignored_settings):
        """Load the generated R1 struct instead of reading a file."""
        return {"problem": r1_struct}

    monkeypatch.setattr("RATapi.utils.convert.loadmat", mock_load, raising=True)

    converted_project = r1_to_project_class(project)

    for class_list in RATapi.project.class_lists:
        assert getattr(converted_project, class_list) == getattr(original_project, class_list)


@pytest.mark.parametrize(
    "project",
    [
        "r1_default_project",
        "r1_monolayer",
        "dspc_bilayer",
        "dspc_standard_layers",
        "dspc_custom_layers",
        "dspc_custom_xy",
        "domains_standard_layers",
        "domains_custom_layers",
        "domains_custom_xy",
        "absorption",
    ],
)
def test_json_involution(project, request):
    """Test that converting a Project to JSON and back returns the same project."""
    original_project = request.getfixturevalue(project)
    json_data = project_to_json(original_project)

    converted_project = project_from_json(json_data)

    for field in RATapi.Project.model_fields:
        assert getattr(converted_project, field) == getattr(original_project, field)


@pytest.mark.skipif(importlib.util.find_spec("matlab") is None, reason="Matlab not installed")
def test_matlab_save(request):
    """Test that MATLAB correctly saves the .mat file."""
    project = request.getfixturevalue("r1_default_project")
    with tempfile.TemporaryDirectory() as temp:
        matfile = os.path.join(temp, "testfile.mat")
        project_class_to_r1(project, filename=matfile)
        converted_project = r1_to_project_class(matfile)

    assert project == converted_project
