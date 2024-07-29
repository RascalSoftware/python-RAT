"""Test conversion to Project files."""

import os

import pytest

from RATapi.utils.convert import r1_to_project_class

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
