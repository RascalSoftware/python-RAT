"""Test conversion to Project files."""

import os

import pytest

from RATapi.utils.convert import project_class_to_r1, r1_to_project_class

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


@pytest.mark.parametrize(
    ["file", "expected"],
    [
        ["R1defaultProject.mat", "r1_default_project"],
        ["R1monolayerVolumeModel.mat", "r1_monolayer"],
        ["R1DSPCBilayer.mat", "dspc_bilayer"],
    ],
)
def test_r1_to_project_class(file, expected, request):
    """Test that R1 to Project class conversion returns the expected Project."""
    output_project = r1_to_project_class(os.path.join(TEST_DIR_PATH, file))
    expected_project = request.getfixturevalue(expected)

    # assert statements have to be more careful due to R1 missing features
    # e.g. R1 doesn't support background parameter names, mu, sigma...
    for attr in ["resolutions", "data", "layers", "contrasts"]:
        assert getattr(output_project, attr) == getattr(expected_project, attr)
    for param_type in ["parameters", "bulk_in", "bulk_out", "scalefactors", "resolution_parameters"]:
        exp_params = getattr(expected_project, param_type)
        act_params = getattr(output_project, param_type)
        assert [p.name for p in exp_params] == [p.name for p in act_params]
        for param in exp_params:
            for attr in ["min", "value", "max", "fit"]:
                assert getattr(param, attr) == getattr(act_params[param.name], attr)

    for i in range(0, len(expected_project.background_parameters)):
        expected_param = expected_project.background_parameters[i]
        actual_param = output_project.background_parameters[i]
        for attr in ["min", "value", "max", "fit"]:
            assert getattr(expected_param, attr) == getattr(actual_param, attr)


@pytest.mark.parametrize("project", ["r1_default_project", "r1_monolayer", "dspc_bilayer"])
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

    for attr in ["resolutions", "data", "layers", "contrasts"]:
        assert getattr(converted_project, attr) == getattr(original_project, attr)
    for param_type in ["parameters", "bulk_in", "bulk_out", "scalefactors", "resolution_parameters"]:
        exp_params = getattr(original_project, param_type)
        act_params = getattr(converted_project, param_type)
        assert [p.name for p in exp_params] == [p.name for p in act_params]
        for param in exp_params:
            for attr in ["min", "value", "max", "fit"]:
                assert getattr(param, attr) == getattr(act_params[param.name], attr)

    for i in range(0, len(original_project.background_parameters)):
        expected_param = original_project.background_parameters[i]
        actual_param = converted_project.background_parameters[i]
        for attr in ["min", "value", "max", "fit"]:
            assert getattr(expected_param, attr) == getattr(actual_param, attr)
