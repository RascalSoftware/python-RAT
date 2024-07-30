"""Test conversion to Project files."""

import os

import pytest
import scipy.io

from RATapi.utils.convert import project_class_to_r1, r1_to_project_class

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


@pytest.mark.parametrize(
    ["file", "expected"],
    [["R1defaultProject.mat", "r1_default_project"], ["R1monolayerVolumeModel.mat", "r1_monolayer"]],
)
def test_r1_to_project_class(file, expected, request):
    """Test that R1 to Project class conversion returns the expected Project."""
    output_project = r1_to_project_class(os.path.join(TEST_DIR_PATH, file))
    expected_project = request.getfixturevalue(expected)

    assert output_project == expected_project


@pytest.mark.parametrize("project", ["r1_default_project", "r1_monolayer"])
def test_r1_involution(project, request, monkeypatch):
    """Test that converting a Project to an R1 struct and back returns the same project."""
    original_project = request.getfixturevalue(project)
    r1_struct = project_class_to_r1(original_project, "")

    def mock_load():
        """Load the generated R1 struct instead of reading a file."""
        return r1_struct

    monkeypatch.setattr(scipy.io, "loadmat", mock_load)

    converted_project = r1_to_project_class(project)  # not that the parameter matters...

    assert original_project == converted_project
