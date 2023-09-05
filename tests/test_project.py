"""Test the project module."""

import contextlib
import copy
import io
import numpy as np
import pydantic
import pytest
from typing import Callable

from RAT.classlist import ClassList
import RAT.models
import RAT.project


@pytest.fixture
def test_project():
    """Add parameters to the default project, so each ClassList can be tested properly."""
    test_project = RAT.project.Project()
    test_project.data[0] = {'data': np.array([[1, 1, 1]])}
    test_project.parameters.append(name='Test SLD')
    test_project.custom_files.append()
    test_project.layers.append(SLD='Test SLD')
    test_project.contrasts.append(data='Simulation', background='Background 1', nba='SLD Air', nbs='SLD D2O',
                                  scalefactor='Scalefactor 1', resolution='Resolution 1')
    return test_project


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


def test_classlists(test_project) -> None:
    """The ClassLists in the "Project" model should contain instances of the models given by the dictionary
    "model_in_classlist".
    """
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
def test_assign_wrong_classes(test_project, field: str, input_model: Callable) -> None:
    """If we assign incorrect classes to the "Project" model, we should raise a ValidationError."""
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
    empty_project = RAT.project.Project.model_construct()
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Project\n{field}\n  Input should be an '
                                                       f'instance of ClassList'):
        setattr(empty_project, field, input_model())


def test_wrapped_routines(test_project) -> None:
    """When initialising a project, several ClassList routines should be wrapped."""
    class_lists = ['parameters', 'bulk_in', 'bulk_out', 'qz_shifts', 'scalefactors', 'background_parameters',
                   'backgrounds', 'resolution_parameters', 'resolutions', 'custom_files', 'data', 'layers',
                   'contrasts']
    wrapped_methods = ['_setitem', '_delitem', '_iadd', 'append', 'insert', 'pop', 'remove', 'clear', 'extend']
    for class_list in class_lists:
        attribute = getattr(test_project, class_list)
        for methodName in wrapped_methods:
            assert hasattr(getattr(attribute, methodName), '__wrapped__')


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
    test_background = RAT.models.Background(**{field: 'undefined'})
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
    test_layer = RAT.models.Layer(**{field: 'undefined'})
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
    test_resolution = RAT.models.Resolution(**{field: 'undefined'})
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
    test_contrast = RAT.models.Contrast(**{field: 'undefined'})
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
    assert test_project.check_allowed_values("backgrounds", ["value_1"], ["Background Param 1"]) is None


@pytest.mark.parametrize("test_value", [
    "Undefined Param",
])
def test_check_allowed_values_not_on_list(test_value: str) -> None:
    """If string values are defined and are not included on the list of allowed values we should raise a ValueError."""
    test_project = RAT.project.Project.model_construct(backgrounds=ClassList(RAT.models.Background(value_1=test_value)))
    with pytest.raises(ValueError, match=f'The value "{test_value}" in the "value_1" field of "backgrounds" must be '
                                         f'defined in "background_parameters".'):
        test_project.check_allowed_values("backgrounds", ["value_1"], ["Background Param 1"])


@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_set(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we set the field values of a model in a ClassList as undefined values, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute[0] = {field: 'undefined'}
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "undefined" in '
                                    f'the "{field}" field of "{class_list}" must be defined in '
                                    f'"{RAT.project.values_defined_in[class_list+"."+field]}".\033[0m\n')
    # Ensure invalid model was not changed
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "parameter", "parent_list", "field"], [
    ('background_parameters', 'Background Param 1', 'backgrounds', 'value_1'),
    ('resolution_parameters', 'Resolution Param 1', 'resolutions', 'value_1'),
    ('parameters', 'Test SLD', 'layers', 'SLD'),
    #('data', 'Simulation', 'contrasts', 'data'),
    ('backgrounds', 'Background 1', 'contrasts', 'background'),
    ('bulk_in', 'SLD Air', 'contrasts', 'nba'),
    ('bulk_out', 'SLD D2O', 'contrasts', 'nbs'),
    ('scalefactors', 'Scalefactor 1', 'contrasts', 'scalefactor'),
    ('resolutions', 'Resolution 1', 'contrasts', 'resolution'),
])
def test_wrap_del(test_project, class_list: str, parameter: str, parent_list: str, field: str) -> None:
    """If we delete a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    index = test_attribute.index(parameter)
    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        del test_attribute[index]
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "{parameter}"'
                                    f' in the "{field}" field of "{parent_list}" must be defined in '
                                    f'"{class_list}".\033[0m\n')
    # Ensure model was not deleted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_iadd(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we add a model containing undefined values to a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute += [input_model(**{field: 'undefined'})]
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "undefined" in '
                                    f'the "{field}" field of "{class_list}" must be defined in '
                                    f'"{RAT.project.values_defined_in[class_list + "." + field]}".\033[0m\n')
    # Ensure invalid model was not added
    assert test_attribute == orig_class_list

@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_append(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we append a model containing undefined values to a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.append(input_model(**{field: 'undefined'}))
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "undefined" in '
                                    f'the "{field}" field of "{class_list}" must be defined in '
                                    f'"{RAT.project.values_defined_in[class_list + "." + field]}".\033[0m\n')
    # Ensure invalid model was not appended
    assert test_attribute == orig_class_list

@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_insert(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we insert a model containing undefined values into a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.insert(0, input_model(**{field: 'undefined'}))
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "undefined" in '
                                    f'the "{field}" field of "{class_list}" must be defined in '
                                    f'"{RAT.project.values_defined_in[class_list + "." + field]}".\033[0m\n')
    # Ensure invalid model was not inserted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_insert_type_error(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we raise a TypeError using the wrapped insert routine, we should re-raise the error."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with pytest.raises(TypeError):
        test_attribute.insert(input_model(**{field: 'undefined'}))

    # Ensure invalid model was not inserted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "parameter", "parent_list", "field"], [
    ('background_parameters', 'Background Param 1', 'backgrounds', 'value_1'),
    ('resolution_parameters', 'Resolution Param 1', 'resolutions', 'value_1'),
    ('parameters', 'Test SLD', 'layers', 'SLD'),
    #('data', 'Simulation', 'contrasts', 'data'),
    ('backgrounds', 'Background 1', 'contrasts', 'background'),
    ('bulk_in', 'SLD Air', 'contrasts', 'nba'),
    ('bulk_out', 'SLD D2O', 'contrasts', 'nbs'),
    ('scalefactors', 'Scalefactor 1', 'contrasts', 'scalefactor'),
    ('resolutions', 'Resolution 1', 'contrasts', 'resolution'),
])
def test_wrap_pop(test_project, class_list: str, parameter: str, parent_list: str, field: str) -> None:
    """If we pop a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    index = test_attribute.index(parameter)
    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.pop(index)
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "{parameter}"'
                                    f' in the "{field}" field of "{parent_list}" must be defined in '
                                    f'"{class_list}".\033[0m\n')
    # Ensure model was not popped
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "parameter", "parent_list", "field"], [
    ('background_parameters', 'Background Param 1', 'backgrounds', 'value_1'),
    ('resolution_parameters', 'Resolution Param 1', 'resolutions', 'value_1'),
    ('parameters', 'Test SLD', 'layers', 'SLD'),
    #('data', 'Simulation', 'contrasts', 'data'),
    ('backgrounds', 'Background 1', 'contrasts', 'background'),
    ('bulk_in', 'SLD Air', 'contrasts', 'nba'),
    ('bulk_out', 'SLD D2O', 'contrasts', 'nbs'),
    ('scalefactors', 'Scalefactor 1', 'contrasts', 'scalefactor'),
    ('resolutions', 'Resolution 1', 'contrasts', 'resolution'),
])
def test_wrap_remove(test_project, class_list: str, parameter: str, parent_list: str, field: str) -> None:
    """If we remove a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.remove(parameter)
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "{parameter}"'
                                    f' in the "{field}" field of "{parent_list}" must be defined in '
                                    f'"{class_list}".\033[0m\n')
    # Ensure model was not removed
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "parameter", "parent_list", "field"], [
    ('background_parameters', 'Background Param 1', 'backgrounds', 'value_1'),
    ('resolution_parameters', 'Resolution Param 1', 'resolutions', 'value_1'),
    ('parameters', 'Test SLD', 'layers', 'SLD'),
    #('data', 'Simulation', 'contrasts', 'data'),
    ('backgrounds', 'Background 1', 'contrasts', 'background'),
    ('bulk_in', 'SLD Air', 'contrasts', 'nba'),
    ('bulk_out', 'SLD D2O', 'contrasts', 'nbs'),
    ('scalefactors', 'Scalefactor 1', 'contrasts', 'scalefactor'),
    ('resolutions', 'Resolution 1', 'contrasts', 'resolution'),
])
def test_wrap_clear(test_project, class_list: str, parameter: str, parent_list: str, field: str) -> None:
    """If we clear a ClassList containing models with values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.clear()
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "{parameter}"'
                                    f' in the "{field}" field of "{parent_list}" must be defined in '
                                    f'"{class_list}".\033[0m\n')
    # Ensure list was not cleared
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(["class_list", "input_model", "field"], [
    ('backgrounds', RAT.models.Background, 'value_1'),
    ('backgrounds', RAT.models.Background, 'value_2'),
    ('backgrounds', RAT.models.Background, 'value_3'),
    ('backgrounds', RAT.models.Background, 'value_4'),
    ('backgrounds', RAT.models.Background, 'value_5'),
    ('resolutions', RAT.models.Resolution, 'value_1'),
    ('resolutions', RAT.models.Resolution, 'value_2'),
    ('resolutions', RAT.models.Resolution, 'value_3'),
    ('resolutions', RAT.models.Resolution, 'value_4'),
    ('resolutions', RAT.models.Resolution, 'value_5'),
    ('layers', RAT.models.Layer, 'thickness'),
    ('layers', RAT.models.Layer, 'SLD'),
    ('layers', RAT.models.Layer, 'roughness'),
    ('contrasts', RAT.models.Contrast, 'data'),
    ('contrasts', RAT.models.Contrast, 'background'),
    ('contrasts', RAT.models.Contrast, 'nba'),
    ('contrasts', RAT.models.Contrast, 'nbs'),
    ('contrasts', RAT.models.Contrast, 'scalefactor'),
    ('contrasts', RAT.models.Contrast, 'resolution'),
])
def test_wrap_extend(test_project, class_list: str, input_model: Callable, field: str) -> None:
    """If we extend a ClassList with model containing undefined values, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    with contextlib.redirect_stdout(io.StringIO()) as print_str:
        test_attribute.extend([input_model(**{field: 'undefined'})])
    assert print_str.getvalue() == (f'\033[31m1 validation error for Project\n  Value error, The value "undefined" in '
                                    f'the "{field}" field of "{class_list}" must be defined in '
                                    f'"{RAT.project.values_defined_in[class_list + "." + field]}".\033[0m\n')
    # Ensure invalid model was not appended
    assert test_attribute == orig_class_list
