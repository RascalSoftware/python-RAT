"""Test conversion to Project files."""

import os

import pytest

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

    # rather than writing the struct to a file and reading the file, just directly
    # hand the struct over
    def mock_load(ignored_filename, **ignored_settings):
        """Load the generated R1 struct instead of reading a file."""
        return {"problem": r1_struct}

    monkeypatch.setattr("RATapi.utils.convert.loadmat", mock_load, raising=True)

    converted_project = r1_to_project_class(project)

    assert original_project == converted_project
