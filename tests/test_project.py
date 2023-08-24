"""Test the project module."""

import pydantic
import pytest
from typing import Callable

from RAT.classlist import ClassList
import RAT.models
import RAT.project


@pytest.fixture
def default_project_repr():
    """A string of the output of repr() for a Project model with no parameters specified."""
    return(
        'Calc Type: -----------------------------------------------------------------------------------------\n\n'
        'non polarised\n\n'
        'Model: ---------------------------------------------------------------------------------------------\n\n'
        'standard layers\n\n'
        'Geometry: ------------------------------------------------------------------------------------------\n\n'
        'air/substrate\n\n'
        'Parameters: ----------------------------------------------------------------------------------------\n\n'
        '    name                   min    value    max  fit    prior_type      mu    sigma\n'
        '--  -------------------  -----  -------  -----  -----  ------------  ----  -------\n'
        ' 0  Substrate Roughness      1        3      5  True   uniform          0      inf\n\n'
        'Bulk In: -------------------------------------------------------------------------------------------\n\n'
        '    name       min    value    max  fit    prior_type      mu    sigma\n'
        '--  -------  -----  -------  -----  -----  ------------  ----  -------\n'
        ' 0  SLD Air      0        0      0  False  uniform          0      inf\n\n'
        'Bulk Out: ------------------------------------------------------------------------------------------\n\n'
        '    name         min     value       max  fit    prior_type      mu    sigma\n'
        '--  -------  -------  --------  --------  -----  ------------  ----  -------\n'
        ' 0  SLD D2O  6.2e-06  6.35e-06  6.35e-06  False  uniform          0      inf\n\n'
        'Qz Shifts: -----------------------------------------------------------------------------------------\n\n'
        '    name            min    value     max  fit    prior_type      mu    sigma\n'
        '--  ----------  -------  -------  ------  -----  ------------  ----  -------\n'
        ' 0  Qz shift 1  -0.0001        0  0.0001  False  uniform          0      inf\n\n'
        'Scalefactors: --------------------------------------------------------------------------------------\n\n'
        '    name             min    value    max  fit    prior_type      mu    sigma\n'
        '--  -------------  -----  -------  -----  -----  ------------  ----  -------\n'
        ' 0  Scalefactor 1   0.02     0.23   0.25  False  uniform          0      inf\n\n'
        'Background Parameters: -----------------------------------------------------------------------------\n\n'
        '    name                  min    value    max  fit    prior_type      mu    sigma\n'
        '--  ------------------  -----  -------  -----  -----  ------------  ----  -------\n'
        ' 0  Background Param 1  1e-07    1e-06  1e-05  False  uniform          0      inf\n\n'
        'Backgrounds: ---------------------------------------------------------------------------------------\n\n'
        '    name          type      value_1             value_2    value_3    value_4    value_5\n'
        '--  ------------  --------  ------------------  ---------  ---------  ---------  ---------\n'
        ' 0  Background 1  constant  Background Param 1\n\n'
        'Resolution Parameters: -----------------------------------------------------------------------------\n\n'
        '    name                  min    value    max  fit    prior_type      mu    sigma\n'
        '--  ------------------  -----  -------  -----  -----  ------------  ----  -------\n'
        ' 0  Resolution Param 1   0.01     0.03   0.05  False  uniform          0      inf\n\n'
        'Resolutions: ---------------------------------------------------------------------------------------\n\n'
        '    name          type      value_1             value_2    value_3    value_4    value_5\n'
        '--  ------------  --------  ------------------  ---------  ---------  ---------  ---------\n'
        ' 0  Resolution 1  constant  Resolution Param 1\n\n'
        'Data: ----------------------------------------------------------------------------------------------\n\n'
        '    name        data    data_range    simulation_range\n'
        '--  ----------  ------  ------------  ------------------\n'
        ' 0  Simulation  []      []            [0.005, 0.7]\n\n')


def test_classlists() -> None:
    """The classlists in the "Project" model should contain instances of the models given by the dictionary
    "model_in_classlist".
    """
    test_project = RAT.project.Project()
    for key, value in RAT.project.model_in_classlist.items():
        class_list = getattr(test_project, key)
        assert class_list._class_handle.__name__ == value


@pytest.mark.parametrize("model", [
    RAT.models.Background,
    RAT.models.Contrast,
    RAT.models.CustomFile,
    RAT.models.Data,
    RAT.models.Layer,
    RAT.models.Resolution,
])
def test_initialise_wrong_classes(model: Callable) -> None:
    """If the "Project" model is initialised with incorrect classes, we should raise a ValidationError."""
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Project\nparameters\n  Assertion '
                                                       'failed, "parameters" ClassList contains objects other than '
                                                       '"Parameter"'):
        RAT.project.Project(parameters=ClassList(model()))


@pytest.mark.parametrize(["field", "input_model"], [
    ('backgrounds', RAT.models.Resolution),
    ('contrasts', RAT.models.Layer),
    ('custom_files', RAT.models.Data),
    ('data', RAT.models.Contrast),
    ('layers', RAT.models.Parameter),
    ('parameters', RAT.models.CustomFile),
    ('resolutions', RAT.models.Background),
])
def test_assign_wrong_classes(field: str, input_model: Callable) -> None:
    """If we assign incorrect classes to the "Project" model, we should raise a ValidationError."""
    test_project = RAT.project.Project.model_construct()
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n{field}\n  Assertion failed, '
                                                       f'"{field}" ClassList contains objects other than '
                                                       f'"{RAT.project.model_in_classlist[field]}"'):
        setattr(test_project, field, ClassList(input_model()))


@pytest.mark.parametrize(["field", "input_model"], [
    ('backgrounds', RAT.models.Background),
    ('contrasts', RAT.models.Contrast),
    ('custom_files', RAT.models.CustomFile),
    ('data', RAT.models.Data),
    ('layers', RAT.models.Layer),
    ('parameters', RAT.models.Parameter),
    ('resolutions', RAT.models.Resolution),
])
def test_assign_models(field: str, input_model: Callable) -> None:
    """If the "Project" model is initialised with models rather than ClassLists, we should raise a ValidationError.
    """
    test_project = RAT.project.Project.model_construct()
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n{field}\n  Input should be an '
                                                       f'instance of ClassList'):
        setattr(test_project, field, input_model())


@pytest.mark.parametrize("field", [
    'value_1',
    'value_2',
    'value_3',
    'value_4',
    'value_5',
])
def test_allowed_backgrounds(field: str) -> None:
    """If the "value" fields of the Background model are set to values that are not specified in the background
    parameters, we should raise a ValidationError.
    """
    test_background = RAT.models.Background()
    setattr(test_background, field, "undefined")
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n  Value error, The value '
                                                       f'"undefined" in the "{field}" field of "backgrounds" must be '
                                                       f'defined in "background_parameters".'):
        RAT.project.Project(backgrounds=ClassList(test_background))


@pytest.mark.parametrize("field", [
    'thickness',
    'SLD',
    'roughness',
])
def test_allowed_layers(field: str) -> None:
    """If the "thickness", "SLD", or "roughness" fields of the Layer model are set to values that are not specified in
    the parameters, we should raise a ValidationError.
    """
    test_layer = RAT.models.Layer()
    setattr(test_layer, field, "undefined")
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n  Value error, The value '
                                                       f'"undefined" in the "{field}" field of "layers" must be '
                                                       f'defined in "parameters".'):
        RAT.project.Project(layers=ClassList(test_layer))


@pytest.mark.parametrize("field", [
    'value_1',
    'value_2',
    'value_3',
    'value_4',
    'value_5',
])
def test_allowed_resolutions(field: str) -> None:
    """If the "value" fields of the Resolution model are set to values that are not specified in the background
    parameters, we should raise a ValidationError.
    """
    test_resolution = RAT.models.Resolution()
    setattr(test_resolution, field, "undefined")
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n  Value error, The value '
                                                       f'"undefined" in the "{field}" field of "resolutions" must be '
                                                       f'defined in "resolution_parameters".'):
        RAT.project.Project(resolutions=ClassList(test_resolution))


@pytest.mark.parametrize(["field", "model_name"], [
    ('data', 'data'),
    ('background', 'backgrounds'),
    ('nba', 'bulk_in'),
    ('nbs', 'bulk_out'),
    ('scalefactor', 'scalefactors'),
    ('resolution', 'resolutions'),
])
def test_allowed_contrasts(field: str, model_name: str) -> None:
    """If the fields of the Contrast model are set to values not specified in the other respective models of the
    project, we should raise a ValidationError.
    """
    test_contrast = RAT.models.Contrast()
    setattr(test_contrast, field, "undefined")
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n  Value error, The value '
                                                       f'"undefined" in the "{field}" field of "contrasts" must be '
                                                       f'defined in "{model_name}".'):
        RAT.project.Project(contrasts=ClassList(test_contrast))


def test_repr(default_project_repr: str) -> None:
    """We should be able to print the "Project" model as a formatted list of the fields."""
    assert repr(RAT.project.Project()) == default_project_repr


@pytest.mark.parametrize("test_value", [
    '',
    'Background Param 1',
])
def test_check_allowed_values(test_value: str) -> None:
    """We should not raise an error if string values are defined and on the list of allowed values."""
    test_project = RAT.project.Project.model_construct(backgrounds=ClassList(RAT.models.Background(value_1=test_value)))
    assert test_project.check_allowed_values("backgrounds", ["value_1"], ["Background Param 1"],
                                             "background_parameters") is None


@pytest.mark.parametrize("test_value", [
    "Undefined Param",
])
def test_check_allowed_values_not_on_list(test_value: str) -> None:
    """If string values are defined and are not included on the list of allowed values we should raise a ValueError."""
    test_project = RAT.project.Project.model_construct(backgrounds=ClassList(RAT.models.Background(value_1=test_value)))
    with pytest.raises(ValueError, match=f'The value "{test_value}" in the "value_1" field of "backgrounds" must be '
                                         f'defined in "background_parameters".'):
        test_project.check_allowed_values("backgrounds", ["value_1"], ["Background Param 1"], "background_parameters")
