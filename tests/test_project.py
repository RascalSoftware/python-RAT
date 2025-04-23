"""Test the project module."""

import copy
import re
import tempfile
import warnings
from pathlib import Path
from typing import Callable

import numpy as np
import pydantic
import pytest
from typing_extensions import get_args, get_origin

import RATapi
from RATapi.utils.enums import Calculations, LayerModels, TypeOptions

layer_params = {"thickness": "Test Thickness", "SLD": "Test SLD", "roughness": "Test Roughness"}
absorption_layer_params = {
    "thickness": "Test Thickness",
    "SLD_real": "Test SLD",
    "SLD_imaginary": "Test SLD",
    "roughness": "Test Roughness",
}

model_classes = {
    "parameters": RATapi.models.Parameter,
    "bulk_in": RATapi.models.Parameter,
    "bulk_out": RATapi.models.Parameter,
    "scalefactors": RATapi.models.Parameter,
    "domain_ratios": RATapi.models.Parameter,
    "background_parameters": RATapi.models.Parameter,
    "resolution_parameters": RATapi.models.Parameter,
    "backgrounds": RATapi.models.Background,
    "resolutions": RATapi.models.Resolution,
    "custom_files": RATapi.models.CustomFile,
    "data": RATapi.models.Data,
    "layers": RATapi.models.Layer,
    "domain_contrasts": RATapi.models.DomainContrast,
    "contrasts": RATapi.models.Contrast,
}


@pytest.fixture
def test_project():
    """Add parameters to the default project, so each ClassList can be tested properly."""
    test_project = RATapi.Project(
        data=RATapi.ClassList([RATapi.models.Data(name="Simulation", data=np.array([[1.0, 1.0, 1.0]]))]),
    )
    test_project.parameters.append(name="Test Thickness")
    test_project.parameters.append(name="Test SLD")
    test_project.parameters.append(name="Test Roughness")
    test_project.custom_files.append(name="Test Custom File")
    test_project.layers.append(
        name="Test Layer",
        thickness="Test Thickness",
        SLD="Test SLD",
        roughness="Test Roughness",
    )
    test_project.contrasts.append(
        name="Test Contrast",
        data="Simulation",
        background="Background 1",
        bulk_in="SLD Air",
        bulk_out="SLD D2O",
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        model=["Test Layer"],
    )
    return test_project


@pytest.fixture
def default_project_str():
    """A string of the output of str() for a Project model with no parameters specified."""
    return (
        "Calculation: ---------------------------------------------------------------------------------------\n\n"
        "normal\n\n"
        "Model: ---------------------------------------------------------------------------------------------\n\n"
        "standard layers\n\n"
        "Geometry: ------------------------------------------------------------------------------------------\n\n"
        "air/substrate\n\n"
        "Parameters: ----------------------------------------------------------------------------------------\n\n"
        "+-------+---------------------+-----+-------+-----+------+\n"
        "| index |         name        | min | value | max | fit  |\n"
        "+-------+---------------------+-----+-------+-----+------+\n"
        "|   0   | Substrate Roughness | 1.0 |  3.0  | 5.0 | True |\n"
        "+-------+---------------------+-----+-------+-----+------+\n\n"
        "Bulk In: -------------------------------------------------------------------------------------------\n\n"
        "+-------+---------+-----+-------+-----+-------+\n"
        "| index |   name  | min | value | max |  fit  |\n"
        "+-------+---------+-----+-------+-----+-------+\n"
        "|   0   | SLD Air | 0.0 |  0.0  | 0.0 | False |\n"
        "+-------+---------+-----+-------+-----+-------+\n\n"
        "Bulk Out: ------------------------------------------------------------------------------------------\n\n"
        "+-------+---------+---------+----------+----------+-------+\n"
        "| index |   name  |   min   |  value   |   max    |  fit  |\n"
        "+-------+---------+---------+----------+----------+-------+\n"
        "|   0   | SLD D2O | 6.2e-06 | 6.35e-06 | 6.35e-06 | False |\n"
        "+-------+---------+---------+----------+----------+-------+\n\n"
        "Scalefactors: --------------------------------------------------------------------------------------\n\n"
        "+-------+---------------+------+-------+------+-------+\n"
        "| index |      name     | min  | value | max  |  fit  |\n"
        "+-------+---------------+------+-------+------+-------+\n"
        "|   0   | Scalefactor 1 | 0.02 |  0.23 | 0.25 | False |\n"
        "+-------+---------------+------+-------+------+-------+\n\n"
        "Background Parameters: -----------------------------------------------------------------------------\n\n"
        "+-------+--------------------+-------+-------+-------+-------+\n"
        "| index |        name        |  min  | value |  max  |  fit  |\n"
        "+-------+--------------------+-------+-------+-------+-------+\n"
        "|   0   | Background Param 1 | 1e-07 | 1e-06 | 1e-05 | False |\n"
        "+-------+--------------------+-------+-------+-------+-------+\n\n"
        "Backgrounds: ---------------------------------------------------------------------------------------\n\n"
        "+-------+--------------+----------+--------------------+\n"
        "| index |     name     |   type   |       source       |\n"
        "+-------+--------------+----------+--------------------+\n"
        "|   0   | Background 1 | constant | Background Param 1 |\n"
        "+-------+--------------+----------+--------------------+\n\n"
        "Resolution Parameters: -----------------------------------------------------------------------------\n\n"
        "+-------+--------------------+------+-------+------+-------+\n"
        "| index |        name        | min  | value | max  |  fit  |\n"
        "+-------+--------------------+------+-------+------+-------+\n"
        "|   0   | Resolution Param 1 | 0.01 |  0.03 | 0.05 | False |\n"
        "+-------+--------------------+------+-------+------+-------+\n\n"
        "Resolutions: ---------------------------------------------------------------------------------------\n\n"
        "+-------+--------------+----------+--------------------+\n"
        "| index |     name     |   type   |       source       |\n"
        "+-------+--------------+----------+--------------------+\n"
        "|   0   | Resolution 1 | constant | Resolution Param 1 |\n"
        "+-------+--------------+----------+--------------------+\n\n"
        "Data: ----------------------------------------------------------------------------------------------\n\n"
        "+-------+------------+------+------------+------------------+\n"
        "| index |    name    | data | data range | simulation range |\n"
        "+-------+------------+------+------------+------------------+\n"
        "|   0   | Simulation |  []  |     []     |   [0.005, 0.7]   |\n"
        "+-------+------------+------+------------+------------------+\n\n"
    )


def test_classlists(test_project) -> None:
    """The ClassLists in the "Project" model should contain instances of the models given by the dictionary
    "model_in_classlist".
    """
    for model in (fields := test_project.model_fields):
        if get_origin(fields[model].annotation) == RATapi.ClassList:
            class_list = getattr(test_project, model)
            assert class_list._class_handle == get_args(fields[model].annotation)[0]


def test_classlists_specific_cases() -> None:
    """The ClassLists in the "Project" model should contain instances of specific models given various non-default
    options.
    """
    project = RATapi.Project(calculation=Calculations.Domains, absorption=True)
    assert project.layers._class_handle.__name__ == "AbsorptionLayer"
    assert project.contrasts._class_handle.__name__ == "ContrastWithRatio"


@pytest.mark.parametrize(
    ["input_model", "model_params"],
    [
        (RATapi.models.Background, {}),
        (RATapi.models.Contrast, {}),
        (RATapi.models.ContrastWithRatio, {}),
        (RATapi.models.CustomFile, {}),
        (RATapi.models.Data, {}),
        (RATapi.models.DomainContrast, {}),
        (RATapi.models.Layer, layer_params),
        (RATapi.models.AbsorptionLayer, absorption_layer_params),
        (RATapi.models.Resolution, {}),
    ],
)
def test_initialise_wrong_classes(input_model: Callable, model_params: dict) -> None:
    """If the "Project" model is initialised with incorrect classes, we should raise a ValidationError."""
    with pytest.raises(
        pydantic.ValidationError,
        match="1 validation error for Project\nparameters\n  "
        "Value error, This ClassList only supports elements of type Parameter. In the input list:\n"
        f"    index 0 is of type {input_model.__name__}",
    ):
        RATapi.Project(parameters=RATapi.ClassList(input_model(**model_params)))


@pytest.mark.parametrize(
    ["input_model", "model_params", "absorption", "actual_model_name"],
    [
        (RATapi.models.Layer, layer_params, True, "AbsorptionLayer"),
        (RATapi.models.AbsorptionLayer, absorption_layer_params, False, "Layer"),
    ],
)
def test_initialise_wrong_layers(
    input_model: Callable,
    model_params: dict,
    absorption: bool,
    actual_model_name: str,
) -> None:
    """If the "Project" model is initialised with the incorrect layer model given the value of absorption, we should
    raise a ValidationError.
    """
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\nlayers\n  Value error, "
        f'"The layers attribute contains {input_model.__name__}s, but the absorption parameter is '
        f'{absorption}. The attribute should be a ClassList of {actual_model_name} instead."',
    ):
        RATapi.Project(absorption=absorption, layers=RATapi.ClassList(input_model(**model_params)))


@pytest.mark.parametrize("absorption, model", [(False, RATapi.models.Layer), (True, RATapi.models.AbsorptionLayer)])
def test_initialise_ambiguous_layers(absorption: bool, model: RATapi.models.RATModel):
    """If a sequence of dictionaries is passed to 'contrasts', convert them to the correct model for the calculation."""
    proj = RATapi.Project(
        absorption=absorption,
        parameters=RATapi.ClassList(
            [
                RATapi.models.Parameter(name="Test Thickness"),
                RATapi.models.Parameter(name="Test SLD"),
                RATapi.models.Parameter(name="Test Roughness"),
            ]
        ),
        layers=RATapi.ClassList(
            [{"name": "Contrast 1", "thickness": "Test Thickness", "SLD": "Test SLD", "roughness": "Test Roughness"}]
        ),
    )
    assert proj.layers._class_handle == model


@pytest.mark.parametrize(
    ["input_model", "calculation", "actual_model_name"],
    [
        (RATapi.models.Contrast, Calculations.Domains, "ContrastWithRatio"),
        (RATapi.models.ContrastWithRatio, Calculations.Normal, "Contrast"),
    ],
)
def test_initialise_wrong_contrasts(
    input_model: RATapi.models.RATModel, calculation: Calculations, actual_model_name: str
) -> None:
    """If the "Project" model is initialised with the incorrect contrast model given the value of calculation, we
    should raise a ValidationError.
    """
    word = "without" if calculation == Calculations.Domains else "with"
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\ncontrasts\n"
        f'  Value error, "The contrasts attribute contains contrasts {word} ratio, '
        f'but the calculation is {calculation}"',
    ):
        RATapi.Project(calculation=calculation, contrasts=RATapi.ClassList(input_model()))


@pytest.mark.parametrize(
    "calculation, model",
    [(Calculations.Domains, RATapi.models.ContrastWithRatio), (Calculations.Normal, RATapi.models.Contrast)],
)
def test_initialise_ambiguous_contrasts(calculation: Calculations, model: RATapi.models.RATModel):
    """If a sequence of dictionaries is passed to 'contrasts', convert them to the correct model for the calculation."""
    proj = RATapi.Project(calculation=calculation, contrasts=RATapi.ClassList([{"name": "Contrast 1"}]))
    assert proj.contrasts._class_handle == model


def test_initialise_without_substrate_roughness() -> None:
    """If the "Project" model is initialised without "Substrate Roughness" as a parameter, add it as a protected
    parameter to the front of the "parameters" ClassList.
    """
    project = RATapi.Project(parameters=RATapi.ClassList(RATapi.models.Parameter(name="Test Parameter")))
    assert project.parameters[0] == RATapi.models.ProtectedParameter(
        name="Substrate Roughness",
        min=1.0,
        value=3.0,
        max=5.0,
        fit=True,
        prior_type=RATapi.models.Priors.Uniform,
        mu=0.0,
        sigma=np.inf,
    )


@pytest.mark.parametrize(
    "input_parameter",
    [
        RATapi.models.Parameter(name="Substrate Roughness"),
        RATapi.models.Parameter(name="SUBSTRATE ROUGHNESS"),
        RATapi.models.Parameter(name="substrate roughness"),
    ],
)
def test_initialise_without_protected_substrate_roughness(input_parameter: RATapi.models.Parameter) -> None:
    """If the "Project" model is initialised without "Substrate Roughness" as a protected parameter, add it to the
    front of the "parameters" ClassList.
    """
    project = RATapi.Project(parameters=RATapi.ClassList(input_parameter))
    assert project.parameters[0] == RATapi.models.ProtectedParameter(name=input_parameter.name)


def test_initialise_without_simulation() -> None:
    """If the "Project" model is initialised without "Simulation" in the "data" ClassList, add it to the front of the
    "data" ClassList.
    """
    project = RATapi.Project(parameters=RATapi.ClassList(RATapi.models.Parameter(name="Test Parameter")))
    assert project.data[0] == RATapi.models.Data(name="Simulation", simulation_range=[0.005, 0.7])


@pytest.mark.parametrize(
    ["field", "model_type", "wrong_input_model", "model_params"],
    [
        ("backgrounds", "Background", RATapi.models.Resolution, {}),
        ("contrasts", "Contrast", RATapi.models.Layer, layer_params),
        ("domain_contrasts", "DomainContrast", RATapi.models.Parameter, {}),
        ("custom_files", "CustomFile", RATapi.models.Data, {}),
        ("data", "Data", RATapi.models.Contrast, {}),
        ("layers", "Layer", RATapi.models.DomainContrast, {}),
        ("parameters", "Parameter", RATapi.models.CustomFile, {}),
        ("resolutions", "Resolution", RATapi.models.Background, {}),
    ],
)
def test_assign_wrong_classes(
    test_project, field: str, model_type: str, wrong_input_model: Callable, model_params: dict
) -> None:
    """If we assign incorrect classes to the "Project" model, we should raise a ValidationError."""
    if field == "contrasts":
        field_name = "contrasts.no_ratio"
    elif field == "layers":
        field_name = "layers.no_abs"
    else:
        field_name = field

    with pytest.raises(
        pydantic.ValidationError,
        match=(
            f"1 validation error for Project\n{field_name}\n"
            f"  Value error, This ClassList only supports elements of type {model_type}. In the input list:\n"
            f"    index 0 is of type {wrong_input_model.__name__}"
        ),
    ):
        setattr(test_project, field, RATapi.ClassList(wrong_input_model(**model_params)))


@pytest.mark.parametrize(
    ["wrong_input_model", "model_params", "absorption", "actual_model_name"],
    [
        (RATapi.models.Layer, layer_params, True, "AbsorptionLayer"),
        (RATapi.models.AbsorptionLayer, absorption_layer_params, False, "Layer"),
    ],
)
def test_assign_wrong_layers(
    wrong_input_model: Callable,
    model_params: dict,
    absorption: bool,
    actual_model_name: str,
) -> None:
    """If we assign incorrect classes to the "Project" model, we should raise a ValidationError."""
    project = RATapi.Project(absorption=absorption)
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\nlayers\n  Value error, "
        f'"The layers attribute contains {wrong_input_model.__name__}s, but the absorption parameter is '
        f'{absorption}. The attribute should be a ClassList of {actual_model_name} instead."',
    ):
        project.layers = RATapi.ClassList(wrong_input_model(**model_params))


@pytest.mark.parametrize(
    ["wrong_input_model", "calculation", "actual_model_name"],
    [
        (RATapi.models.Contrast, Calculations.Domains, "ContrastWithRatio"),
        (RATapi.models.ContrastWithRatio, Calculations.Normal, "Contrast"),
    ],
)
def test_assign_wrong_contrasts(wrong_input_model: Callable, calculation: Calculations, actual_model_name: str) -> None:
    """If we assign incorrect classes to the "Project" model, we should raise a ValidationError."""
    project = RATapi.Project(calculation=calculation)
    word = "without" if calculation == Calculations.Domains else "with"
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\ncontrasts\n"
        f'  Value error, "The contrasts attribute contains contrasts {word} ratio, but the calculation is '
        f'{calculation}"',
    ):
        project.contrasts = RATapi.ClassList(wrong_input_model())


@pytest.mark.parametrize(
    ["field", "model_params"],
    [
        ("backgrounds", {}),
        ("contrasts", {}),
        ("custom_files", {}),
        ("data", {}),
        ("layers", layer_params),
        ("parameters", {}),
        ("resolutions", {}),
    ],
)
def test_assign_models(test_project, field: str, model_params: dict) -> None:
    """If the "Project" model is initialised with models rather than ClassLists, we should raise a ValidationError."""
    input_model = model_classes[field]
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\n{field}\n  Input should be an instance of ClassList",
    ):
        setattr(test_project, field, input_model(**model_params))


def test_wrapped_routines(test_project) -> None:
    """When initialising a project, several ClassList routines should be wrapped."""
    wrapped_methods = [
        "_setitem",
        "_delitem",
        "_iadd",
        "append",
        "insert",
        "pop",
        "remove",
        "clear",
        "extend",
        "set_fields",
    ]
    for class_list in RATapi.project.class_lists:
        attribute = getattr(test_project, class_list)
        for methodName in wrapped_methods:
            assert hasattr(getattr(attribute, methodName), "__wrapped__")


def test_set_domain_ratios(test_project) -> None:
    """If we are not running a domains calculation, the "domain_ratios" field of the model should always be empty."""
    assert test_project.domain_ratios == []
    test_project.domain_ratios.append(name="New Domain Ratio")
    assert test_project.domain_ratios == []


@pytest.mark.parametrize(
    "project_parameters",
    [
        ({"calculation": Calculations.Normal, "model": LayerModels.StandardLayers}),
        ({"calculation": Calculations.Normal, "model": LayerModels.CustomLayers}),
        ({"calculation": Calculations.Normal, "model": LayerModels.CustomXY}),
        ({"calculation": Calculations.Domains, "model": LayerModels.CustomLayers}),
        ({"calculation": Calculations.Domains, "model": LayerModels.CustomXY}),
    ],
)
def test_set_domain_contrasts(project_parameters: dict) -> None:
    """If we are not running a domains calculation with standard layers, the "domain_contrasts" field of the model
    should always be empty.
    """
    project = RATapi.Project(**project_parameters)
    assert project.domain_contrasts == []
    project.domain_contrasts.append(name="New Domain Contrast")
    assert project.domain_contrasts == []


@pytest.mark.parametrize(
    "project_parameters",
    [
        ({"model": LayerModels.CustomLayers}),
        ({"model": LayerModels.CustomXY}),
    ],
)
def test_set_layers(project_parameters: dict) -> None:
    """If we are not using a standard layers model, the "layers" field of the model should always be empty."""
    project = RATapi.Project(**project_parameters)
    assert project.layers == []
    project.layers.append(name="New Layer", thickness="Test Thickness", SLD="Test SLD", roughness="Test Roughness")
    assert project.layers == []


@pytest.mark.parametrize(
    ["input_calculation", "input_contrast", "new_calculation", "new_contrast_model", "num_domain_ratios"],
    [
        (Calculations.Normal, RATapi.models.Contrast, Calculations.Domains, "ContrastWithRatio", 1),
        (Calculations.Domains, RATapi.models.ContrastWithRatio, Calculations.Normal, "Contrast", 0),
    ],
)
def test_set_calculation(
    input_calculation: Calculations,
    input_contrast: Callable,
    new_calculation: Calculations,
    new_contrast_model: str,
    num_domain_ratios: int,
) -> None:
    """When changing the value of the calculation option, the "contrasts" ClassList should switch to using the
    appropriate Contrast model.
    """
    project = RATapi.Project(calculation=input_calculation, contrasts=RATapi.ClassList(input_contrast()))
    project.calculation = new_calculation

    assert project.calculation is new_calculation
    assert type(project.contrasts[0]).__name__ == new_contrast_model
    assert project.contrasts._class_handle.__name__ == new_contrast_model
    assert len(project.domain_ratios) == num_domain_ratios


@pytest.mark.parametrize(
    ["new_calc", "new_model", "expected_contrast_model"],
    [
        (Calculations.Normal, LayerModels.StandardLayers, ["Test Layer"]),
        (Calculations.Normal, LayerModels.CustomLayers, []),
        (Calculations.Normal, LayerModels.CustomXY, []),
        (Calculations.Domains, LayerModels.StandardLayers, []),
        (Calculations.Domains, LayerModels.CustomLayers, []),
        (Calculations.Domains, LayerModels.CustomXY, []),
    ],
)
def test_set_contrast_model_field(
    test_project,
    new_calc: Calculations,
    new_model: LayerModels,
    expected_contrast_model: list[str],
) -> None:
    """If we change the calculation and model such that the values of the "model" field of the "contrasts" model come
    from a different field of the project, we should clear the contrast "model" field.
    """
    test_project.calculation = new_calc
    test_project.model = new_model
    assert test_project.contrasts[0].model == expected_contrast_model


@pytest.mark.parametrize(
    ["input_model", "test_contrast_model", "error_message"],
    [
        (
            LayerModels.StandardLayers,
            ["Test Domain Ratio"],
            'For a standard layers domains calculation the "model" field of "contrasts" must contain exactly two '
            "values.",
        ),
        (
            LayerModels.StandardLayers,
            ["Test Domain Ratio", "Test Domain Ratio", "Test Domain Ratio"],
            'For a standard layers domains calculation the "model" field of "contrasts" must contain exactly two '
            "values.",
        ),
        (
            LayerModels.CustomLayers,
            ["Test Custom File", "Test Custom File"],
            'For a custom model calculation the "model" field of "contrasts" cannot contain more than one value.',
        ),
    ],
)
def test_check_contrast_model_length(
    test_project,
    input_model: LayerModels,
    test_contrast_model: list[str],
    error_message: str,
) -> None:
    """If we are not running a domains calculation with standard layers, the "domain_contrasts" field of the model
    should always be empty.
    """
    test_domain_ratios = RATapi.ClassList(RATapi.models.Parameter(name="Test Domain Ratio"))
    test_contrasts = RATapi.ClassList(RATapi.models.ContrastWithRatio(model=test_contrast_model))
    with pytest.raises(
        pydantic.ValidationError,
        match=f"1 validation error for Project\n  Value error, {error_message}",
    ):
        RATapi.Project(
            calculation=Calculations.Domains,
            model=input_model,
            domain_ratios=test_domain_ratios,
            contrasts=test_contrasts,
        )


@pytest.mark.parametrize(
    ["input_layer", "model_params", "input_absorption", "new_layer_model"],
    [
        (RATapi.models.Layer, layer_params, False, "AbsorptionLayer"),
        (RATapi.models.AbsorptionLayer, absorption_layer_params, True, "Layer"),
    ],
)
def test_set_absorption(
    input_layer: Callable,
    model_params: dict,
    input_absorption: bool,
    new_layer_model: str,
) -> None:
    """When changing the value of the absorption option, the "layers" ClassList should switch to using the appropriate
    Layer model.
    """
    project = RATapi.Project(
        absorption=input_absorption,
        parameters=RATapi.ClassList(
            [
                RATapi.models.Parameter(name="Test Thickness"),
                RATapi.models.Parameter(name="Test SLD"),
                RATapi.models.Parameter(name="Test Roughness"),
            ],
        ),
        layers=RATapi.ClassList(input_layer(**model_params)),
    )
    project.absorption = not input_absorption

    assert project.absorption is not input_absorption
    assert type(project.layers[0]).__name__ == new_layer_model
    assert project.layers._class_handle.__name__ == new_layer_model


@pytest.mark.parametrize(
    "delete_operation",
    [
        "project.parameters.__delitem__(0)",
        "project.parameters.pop()",
        'project.parameters.remove("Substrate Roughness")',
        "project.parameters.clear()",
    ],
)
def test_check_protected_parameters(delete_operation) -> None:
    """If we try to remove a protected parameter, we should raise an error."""
    project = RATapi.Project()

    with pytest.raises(
        pydantic.ValidationError,
        match="1 validation error for Project\n  Value error, "
        "Can't delete the protected parameters: Substrate Roughness",
    ):
        eval(delete_operation)

    # Ensure model was not deleted
    assert project.parameters[0].name == "Substrate Roughness"


@pytest.mark.parametrize(
    ["model", "fields"],
    [
        ("background_parameters", ["source"]),
        ("resolution_parameters", ["source"]),
        ("parameters", ["roughness"]),
        ("data", ["data", "source"]),
        ("custom_files", ["source", "source"]),
        ("backgrounds", ["background"]),
        ("bulk_in", ["bulk_in"]),
        ("bulk_out", ["bulk_out"]),
        ("scalefactors", ["scalefactor"]),
        ("resolutions", ["resolution"]),
    ],
)
def test_rename_models(test_project, model: str, fields: list[str]) -> None:
    """When renaming a model in the project, the new name should be recorded when that model is referred to elsewhere
    in the project.
    """
    if model == "data":
        test_project.backgrounds[0] = RATapi.models.Background(type="data", source="Simulation")
    if model == "custom_files":
        test_project.backgrounds[0] = RATapi.models.Background(type="function", source="Test Custom File")
        # workaround until function resolutions are added
        # test_project.resolutions[0] = RATapi.models.Resolution(type="function", source="Test Custom File")
        test_project.resolution_parameters.append(RATapi.models.Parameter(name="New Name"))
        test_project.resolutions[0] = RATapi.models.Resolution(type="constant", source="New Name")

    getattr(test_project, model).set_fields(-1, name="New Name")
    model_name_lists = RATapi.project.model_names_used_in[model]
    for model_name_list, field in zip(model_name_lists, fields):
        attribute = model_name_list.attribute
        assert getattr(getattr(test_project, attribute)[-1], field) == "New Name"


@pytest.mark.parametrize(
    "background_type, expected_field",
    [
        [TypeOptions.Constant, "background_parameters"],
        [TypeOptions.Data, "data"],
        [TypeOptions.Function, "custom_files"],
    ],
)
def test_allowed_backgrounds(background_type, expected_field) -> None:
    """
    If the source field of the Background model is set to a value that is not specified in the appropriate ClassList,
    we should raise a ValidationError.
    """
    test_background = RATapi.models.Background(type=background_type, source="undefined")
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "source" field of '
            f'backgrounds[0] must be defined in "{expected_field}". Please add "undefined" to "{expected_field}" '
            f'before including it in "backgrounds".'
        ),
    ):
        RATapi.Project(backgrounds=RATapi.ClassList(test_background))


@pytest.mark.parametrize(
    "field",
    [
        "thickness",
        "SLD",
        "roughness",
    ],
)
def test_allowed_layers(field: str) -> None:
    """If the "thickness", "SLD", or "roughness" fields of the Layer model are set to values that are not specified in
    the parameters, we should raise a ValidationError.
    """
    test_layer = RATapi.models.Layer(**{**layer_params, field: "undefined"})

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" '
            f'field of layers[0] must be defined in "parameters". Please add "undefined" to "parameters" '
            f'before including it in "layers".'
        ),
    ):
        RATapi.Project(
            absorption=False,
            parameters=RATapi.ClassList(
                [
                    RATapi.models.Parameter(name="Test Thickness"),
                    RATapi.models.Parameter(name="Test SLD"),
                    RATapi.models.Parameter(name="Test Roughness"),
                ],
            ),
            layers=RATapi.ClassList(test_layer),
        )


@pytest.mark.parametrize(
    "field",
    [
        "thickness",
        "SLD_real",
        "SLD_imaginary",
        "roughness",
    ],
)
def test_allowed_absorption_layers(field: str) -> None:
    """If the "thickness", "SLD_real", "SLD_imaginary", or "roughness" fields of the AbsorptionLayer model are set to
    values that are not specified in the parameters, we should raise a ValidationError.
    """
    test_layer = RATapi.models.AbsorptionLayer(**{**absorption_layer_params, field: "undefined"})

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" field of '
            f'layers[0] must be defined in "parameters". Please add "undefined" to "parameters" before including it '
            f'in "layers".'
        ),
    ):
        RATapi.Project(
            absorption=True,
            parameters=RATapi.ClassList(
                [
                    RATapi.models.Parameter(name="Test Thickness"),
                    RATapi.models.Parameter(name="Test SLD"),
                    RATapi.models.Parameter(name="Test Roughness"),
                ],
            ),
            layers=RATapi.ClassList(test_layer),
        )


@pytest.mark.parametrize(
    "resolution_type, expected_field",
    [
        [TypeOptions.Constant, "resolution_parameters"],
        # uncomment when function resolutions are added!
        # [TypeOptions.Function, "custom_files"],
    ],
)
def test_allowed_resolutions(resolution_type, expected_field) -> None:
    """If the "value" fields of the Resolution model are set to values that are not specified in the resolution
    parameters, we should raise a ValidationError.
    """
    test_resolution = RATapi.models.Resolution(type=resolution_type, source="undefined")
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "source" field of '
            f'resolutions[0] must be defined in "{expected_field}". Please add "undefined" to "{expected_field}" '
            f'before including it in "resolutions".'
        ),
    ):
        RATapi.Project(resolutions=RATapi.ClassList(test_resolution))


@pytest.mark.parametrize(
    ["field", "model_name"],
    [
        ("data", "data"),
        ("background", "backgrounds"),
        ("bulk_in", "bulk_in"),
        ("bulk_out", "bulk_out"),
        ("scalefactor", "scalefactors"),
        ("resolution", "resolutions"),
    ],
)
def test_allowed_contrasts(field: str, model_name: str) -> None:
    """If the fields of the Contrast model are set to values not specified in the other respective models of the
    project, we should raise a ValidationError.
    """
    test_contrast = RATapi.models.Contrast(**{field: "undefined"})
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" field of '
            f'contrasts[0] must be defined in "{model_name}". Please add "undefined" to "{model_name}" before '
            f'including it in "contrasts".'
        ),
    ):
        RATapi.Project(calculation=Calculations.Normal, contrasts=RATapi.ClassList(test_contrast))


@pytest.mark.parametrize(
    ["field", "model_name"],
    [
        ("data", "data"),
        ("background", "backgrounds"),
        ("bulk_in", "bulk_in"),
        ("bulk_out", "bulk_out"),
        ("scalefactor", "scalefactors"),
        ("resolution", "resolutions"),
        ("domain_ratio", "domain_ratios"),
    ],
)
def test_allowed_contrasts_with_ratio(field: str, model_name: str) -> None:
    """If the fields of the ContrastWithRatio model are set to values not specified in the other respective models of
    the project, we should raise a ValidationError.
    """
    test_contrast = RATapi.models.ContrastWithRatio(**{field: "undefined"})
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" field of '
            f'contrasts[0] must be defined in "{model_name}". Please add "undefined" to "{model_name}" before '
            f'including it in "contrasts".'
        ),
    ):
        RATapi.Project(calculation=Calculations.Domains, contrasts=RATapi.ClassList(test_contrast))


@pytest.mark.parametrize(
    ["input_calc", "input_model", "test_contrast", "field_name"],
    [
        (
            Calculations.Domains,
            LayerModels.StandardLayers,
            RATapi.models.ContrastWithRatio(name="Test Contrast", model=["undefined", "undefined"]),
            "domain_contrasts",
        ),
        (
            Calculations.Domains,
            LayerModels.CustomLayers,
            RATapi.models.ContrastWithRatio(name="Test Contrast", model=["undefined"]),
            "custom_files",
        ),
        (
            Calculations.Domains,
            LayerModels.CustomXY,
            RATapi.models.ContrastWithRatio(name="Test Contrast", model=["undefined"]),
            "custom_files",
        ),
        (
            Calculations.Normal,
            LayerModels.StandardLayers,
            RATapi.models.Contrast(name="Test Contrast", model=["undefined", "undefined", "undefined"]),
            "layers",
        ),
        (
            Calculations.Normal,
            LayerModels.CustomLayers,
            RATapi.models.Contrast(name="Test Contrast", model=["undefined"]),
            "custom_files",
        ),
        (
            Calculations.Normal,
            LayerModels.CustomXY,
            RATapi.models.Contrast(name="Test Contrast", model=["undefined"]),
            "custom_files",
        ),
    ],
)
def test_allowed_contrast_models(
    input_calc: Calculations,
    input_model: LayerModels,
    test_contrast: "RATapi.models",
    field_name: str,
) -> None:
    """If any value in the model field of the contrasts is set to a value not specified in the appropriate part of the
    project, we should raise a ValidationError.
    """
    missing_values = list(set(test_contrast.model))
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f"1 validation error for Project\n  Value error, The value{'s' if len(missing_values) > 1 else ''}: "
            f'"{", ".join(missing_values)}" used in the "model" field of contrasts[0] must be defined in '
            f'"{field_name}". Please add all required values to "{field_name}" before including them in "contrasts".'
        ),
    ):
        RATapi.Project(calculation=input_calc, model=input_model, contrasts=RATapi.ClassList(test_contrast))


def test_allowed_domain_contrast_models() -> None:
    """If any value in the model field of the domain_contrasts is set to a value not specified in the "layers" field of
    the project, we should raise a ValidationError.
    """
    test_contrast = RATapi.models.DomainContrast(name="Test Domain Contrast", model=["undefined"])
    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            '1 validation error for Project\n  Value error, The value: "undefined" used in the "model" field of '
            'domain_contrasts[0] must be defined in "layers". Please add all required values to "layers" before '
            'including them in "domain_contrasts".'
        ),
    ):
        RATapi.Project(calculation=Calculations.Domains, domain_contrasts=RATapi.ClassList(test_contrast))


def test_str(default_project_str: str) -> None:
    """We should be able to print the "Project" model as a formatted list of the fields."""
    assert str(RATapi.Project()) == default_project_str


def test_get_all_names(test_project) -> None:
    """We should be able to get the names of all the models defined in the project."""
    assert test_project.get_all_names() == {
        "parameters": ["Substrate Roughness", "Test Thickness", "Test SLD", "Test Roughness"],
        "bulk_in": ["SLD Air"],
        "bulk_out": ["SLD D2O"],
        "scalefactors": ["Scalefactor 1"],
        "domain_ratios": [],
        "background_parameters": ["Background Param 1"],
        "backgrounds": ["Background 1"],
        "resolution_parameters": ["Resolution Param 1"],
        "resolutions": ["Resolution 1"],
        "custom_files": ["Test Custom File"],
        "data": ["Simulation"],
        "layers": ["Test Layer"],
        "domain_contrasts": [],
        "contrasts": ["Test Contrast"],
    }


def test_get_all_protected_parameters(test_project) -> None:
    """We should be able to get the names of all the protected parameters defined in the project."""
    assert test_project.get_all_protected_parameters() == {
        "parameters": ["Substrate Roughness"],
        "bulk_in": [],
        "bulk_out": [],
        "scalefactors": [],
        "domain_ratios": [],
        "background_parameters": [],
        "resolution_parameters": [],
    }


@pytest.mark.parametrize(
    "test_value",
    [
        "",
        "Substrate Roughness",
    ],
)
def test_check_allowed_values(test_value: str) -> None:
    """We should not raise an error if string values are defined and on the list of allowed values."""
    allowed_values = ["Substrate Roughness"]
    project = RATapi.Project.model_construct(
        layers=RATapi.ClassList(RATapi.models.Layer(**dict(layer_params, roughness=test_value)))
    )
    assert project.check_allowed_values("layers", ["roughness"], allowed_values, allowed_values) is None


@pytest.mark.parametrize(
    "test_value",
    [
        "Undefined Param",
    ],
)
def test_check_allowed_values_not_on_list(test_value: str) -> None:
    """If string values are defined and are not included on the list of allowed values we should raise a ValueError."""
    project = RATapi.Project.model_construct(
        layers=RATapi.ClassList(RATapi.models.Layer(**dict(layer_params, roughness=test_value)))
    )
    allowed_values = ["Substrate Roughness"]
    with pytest.raises(
        ValueError,
        match=re.escape(
            f'The value "{test_value}" used in the "roughness" field of layers[0] must be defined in "parameters". '
            f'Please add "{test_value}" to "parameters" before including it in "layers".'
        ),
    ):
        project.check_allowed_values("layers", ["roughness"], allowed_values, allowed_values)


@pytest.mark.parametrize(
    "test_value",
    [
        "",
        "Background Param 1",
    ],
)
def test_check_allowed_background_resolution_values_constant(test_value: str) -> None:
    """We should not raise an error if string values are defined and on the appropriate list of allowed values."""
    project = RATapi.Project.model_construct(
        background_parameters=RATapi.ClassList(RATapi.models.Parameter(name="Background Param 1")),
        backgrounds=RATapi.ClassList(RATapi.models.Background(type=TypeOptions.Constant, source=test_value)),
    )
    assert project.check_allowed_source("backgrounds") is None


@pytest.mark.parametrize(
    "test_value",
    [
        "",
        "Simulation",
    ],
)
def test_check_allowed_background_resolution_values_data(test_value: str) -> None:
    """We should not raise an error if string values are defined and on the appropriate list of allowed values."""
    project = RATapi.Project.model_construct(
        backgrounds=RATapi.ClassList(RATapi.models.Background(type=TypeOptions.Data, source=test_value))
    )
    assert project.check_allowed_source("backgrounds") is None


@pytest.mark.parametrize(
    "test_value",
    ["Undefined Param", "Simulation"],
)
def test_check_allowed_background_resolution_values_not_on_constant_list(test_value: str) -> None:
    """If string values are defined and are not included on the correct list of allowed values we should raise a
    ValueError.
    """
    project = RATapi.Project.model_construct(
        backgrounds=RATapi.ClassList(RATapi.models.Background(type=TypeOptions.Constant, source=test_value))
    )
    with pytest.raises(
        ValueError,
        match=re.escape(
            f'The value "{test_value}" used in the "source" field of backgrounds[0] must be defined in '
            f'"background_parameters". Please add "{test_value}" to "background_parameters" before including it in '
            f'"backgrounds".'
        ),
    ):
        project.check_allowed_source(
            "backgrounds",
        )


@pytest.mark.parametrize(
    "test_value",
    [
        "Undefined Param",
        "Background Param 1",
    ],
)
def test_check_allowed_background_resolution_values_on_data_list(test_value: str) -> None:
    """If string values are defined and are not included on the correct list of allowed values we should raise a
    ValueError.
    """
    project = RATapi.Project.model_construct(
        backgrounds=RATapi.ClassList(RATapi.models.Background(type=TypeOptions.Data, source=test_value))
    )
    with pytest.raises(
        ValueError,
        match=re.escape(
            f'The value "{test_value}" used in the "source" field of backgrounds[0] must be defined in "data". Please '
            f'add "{test_value}" to "data" before including it in "backgrounds".'
        ),
    ):
        project.check_allowed_source("backgrounds")


@pytest.mark.parametrize(
    "test_values",
    [
        [],
        ["Test Layer"],
    ],
)
def test_check_contrast_model_allowed_values(test_values: list[str]) -> None:
    """We should not raise an error if values are defined in a non-empty list and all are on the list of allowed
    values.
    """
    project = RATapi.Project.model_construct(
        contrasts=RATapi.ClassList(RATapi.models.Contrast(name="Test Contrast", model=test_values)),
    )
    assert project.check_contrast_model_allowed_values("contrasts", ["Test Layer"], ["Test Layer"], "layers") is None


@pytest.mark.parametrize(
    "test_values",
    [
        ["Undefined Param 1"],
        ["Test Layer", "Undefined Param 1"],
        ["Undefined Param 1 ", "Test Layer", "Undefined Param 2"],
    ],
)
def test_check_allowed_contrast_model_not_on_list(test_values: list[str]) -> None:
    """If string values are defined in a non-empty list and any of them are not included on the list of allowed values
    we should raise a ValueError.
    """
    project = RATapi.Project.model_construct(
        contrasts=RATapi.ClassList(RATapi.models.Contrast(name="Test Contrast", model=test_values)),
    )
    allowed_values = ["Test Layer"]
    missing_values = list(set(test_values) - set(allowed_values))
    with pytest.raises(
        ValueError,
        match=re.escape(
            f'The value{"s" if len(missing_values) > 1 else ""}: "{", ".join(str(i) for i in missing_values)}" used '
            f'in the "model" field of contrasts[0] must be defined in "layers". Please add all required values to '
            f'"layers" before including them in "contrasts".'
        ),
    ):
        project.check_contrast_model_allowed_values("contrasts", allowed_values, allowed_values, "layers")


@pytest.mark.parametrize(
    "test_values",
    [
        ["Undefined Param 1"],
        ["Test Layer", "Undefined Param 1"],
        ["Undefined Param 1", "Test Layer", "Undefined Param 2"],
    ],
)
def test_check_allowed_contrast_model_removed_from_list(test_values: list[str]) -> None:
    """If string values are defined in a non-empty list and any of them have been removed from the list of allowed
    values we should raise a ValueError.
    """
    project = RATapi.Project.model_construct(
        contrasts=RATapi.ClassList(RATapi.models.Contrast(name="Test Contrast", model=test_values)),
    )
    previous_values = ["Test Layer", "Undefined Param 1", "Undefined Param 2"]
    allowed_values = ["Test Layer"]
    missing_values = list(set(test_values) - set(allowed_values))
    with pytest.raises(
        ValueError,
        match=re.escape(
            f'The value{"s" if len(missing_values) > 1 else ""}: "{", ".join(str(i) for i in missing_values)}" used '
            f'in the "model" field of contrasts[0] must be defined in "layers". Please remove all unnecessary values '
            f'from "model" before attempting to delete them.'
        ),
    ):
        project.check_contrast_model_allowed_values("contrasts", allowed_values, previous_values, "layers")


@pytest.mark.parametrize(
    ["input_calc", "input_model", "expected_field_name"],
    [
        (Calculations.Domains, LayerModels.StandardLayers, "domain_contrasts"),
        (Calculations.Normal, LayerModels.StandardLayers, "layers"),
        (Calculations.Domains, LayerModels.CustomLayers, "custom_files"),
        (Calculations.Normal, LayerModels.CustomLayers, "custom_files"),
        (Calculations.Domains, LayerModels.CustomXY, "custom_files"),
        (Calculations.Normal, LayerModels.CustomXY, "custom_files"),
    ],
)
def test_get_contrast_model_field(input_calc: Calculations, input_model: LayerModels, expected_field_name: str) -> None:
    """Each combination of calculation and model determines the field where the values of "model" field of "contrasts"
    are defined.
    """
    project = RATapi.Project(calculation=input_calc, model=input_model)
    assert project.get_contrast_model_field() == expected_field_name


@pytest.mark.parametrize(
    "project",
    ["r1_default_project", "r1_monolayer", "input_project"],
)
@pytest.mark.parametrize(
    "input_filename",
    [
        "test_script.py",
        "test_script",
    ],
)
def test_write_script(project, request, temp_dir, input_filename: str) -> None:
    """Test the script we write to regenerate the project is created and runs as expected."""
    test_project = request.getfixturevalue(project)
    test_project.write_script("problem", Path(temp_dir, input_filename))

    # Test the file is written in the correct place
    script_path = Path(temp_dir, "test_script.py")
    assert script_path.exists()

    # Test we get the project object we expect when running the script
    local_dict = {}
    exec(script_path.read_text(), globals(), local_dict)
    new_project = local_dict["problem"]

    for class_list in RATapi.project.class_lists:
        assert getattr(new_project, class_list) == getattr(test_project, class_list)


@pytest.mark.parametrize(
    "extension",
    [
        ".txt",
        ".f90",
        ".m",
        ".pyc",
    ],
)
def test_write_script_wrong_extension(test_project, extension: str) -> None:
    """If we try to write the script to anything other than a ".py" file, we raise a ValueError."""
    with pytest.raises(ValueError, match='The script name provided to "write_script" must use the ".py" format'):
        test_project.write_script("problem", "test" + extension)


@pytest.mark.parametrize(
    ["class_list", "model_type", "field"],
    [
        ("backgrounds", "constant", "source"),
        ("resolutions", "constant", "source"),
        ("layers", "", "thickness"),
        ("layers", "", "SLD"),
        ("layers", "", "roughness"),
        ("contrasts", "", "data"),
        ("contrasts", "", "background"),
        ("contrasts", "", "bulk_in"),
        ("contrasts", "", "bulk_out"),
        ("contrasts", "", "scalefactor"),
        ("contrasts", "", "resolution"),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_set(test_project, class_list: str, model_type: str, field: str) -> None:
    """If we set the field values of a model in a ClassList as undefined values, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    class_list_str = f"{class_list}{f'.{model_type}' if model_type else ''}.{field}"
    index = 0

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" field of '
            f'{class_list}[{index}] must be defined in "{RATapi.project.values_defined_in[class_list_str]}". Please '
            f'add "undefined" to "{RATapi.project.values_defined_in[class_list_str]}" before including it in '
            f'"{class_list}".'
        ),
    ):
        test_attribute.set_fields(index, **{field: "undefined"})

    # Ensure invalid model was not changed
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "parameter", "field"],
    [
        ("background_parameters", "Background Param 1", "source"),
        ("resolution_parameters", "Resolution Param 1", "source"),
        ("parameters", "Test SLD", "SLD"),
        ("data", "Simulation", "data"),
        ("backgrounds", "Background 1", "background"),
        ("bulk_in", "SLD Air", "bulk_in"),
        ("bulk_out", "SLD D2O", "bulk_out"),
        ("scalefactors", "Scalefactor 1", "scalefactor"),
        ("resolutions", "Resolution 1", "resolution"),
    ],
)
def test_wrap_del(test_project, class_list: str, parameter: str, field: str) -> None:
    """If we delete a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    index = test_attribute.index(parameter)

    sub_attribute_name = RATapi.project.model_names_used_in[class_list][0].attribute
    sub_attribute = getattr(test_project, sub_attribute_name)
    sub_index = [i for i, _ in enumerate(sub_attribute) if getattr(sub_attribute[i], field) == parameter][0]

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "{parameter}" used in the "{field}" field of '
            f'{sub_attribute_name}[{sub_index}] must be defined in "{class_list}". Please remove "{parameter}" from '
            f'"{sub_attribute_name}[{sub_index}].{field}" before attempting to delete it.'
        ),
    ):
        del test_attribute[index]

    # Ensure model was not deleted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "model_type", "field", "model_params"],
    [
        ("backgrounds", "constant", "source", {}),
        ("resolutions", "constant", "source", {}),
        ("layers", "", "thickness", layer_params),
        ("layers", "", "SLD", layer_params),
        ("layers", "", "roughness", layer_params),
        ("contrasts", "", "data", {}),
        ("contrasts", "", "background", {}),
        ("contrasts", "", "bulk_in", {}),
        ("contrasts", "", "bulk_out", {}),
        ("contrasts", "", "scalefactor", {}),
        ("contrasts", "", "resolution", {}),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_iadd(test_project, class_list: str, model_type: str, field: str, model_params: dict) -> None:
    """If we add a model containing undefined values to a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    input_model = model_classes[class_list]
    class_list_str = f"{class_list}{f'.{model_type}' if model_type else ''}.{field}"

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" '
            f"field of {class_list}[{len(test_attribute)}] must be defined in "
            f'"{RATapi.project.values_defined_in[class_list_str]}". Please add "undefined" to '
            f'"{RATapi.project.values_defined_in[class_list_str]}" before including it in "{class_list}".'
        ),
    ):
        test_attribute += [input_model(**{**model_params, field: "undefined"})]

    # Ensure invalid model was not added
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "model_type", "field", "model_params"],
    [
        ("backgrounds", "constant", "source", {}),
        ("resolutions", "constant", "source", {}),
        ("layers", "", "thickness", layer_params),
        ("layers", "", "SLD", layer_params),
        ("layers", "", "roughness", layer_params),
        ("contrasts", "", "data", {}),
        ("contrasts", "", "background", {}),
        ("contrasts", "", "bulk_in", {}),
        ("contrasts", "", "bulk_out", {}),
        ("contrasts", "", "scalefactor", {}),
        ("contrasts", "", "resolution", {}),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_append(test_project, class_list: str, model_type: str, field: str, model_params: dict) -> None:
    """If we append a model containing undefined values to a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    input_model = model_classes[class_list]
    class_list_str = f"{class_list}{f'.{model_type}' if model_type else ''}.{field}"

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" '
            f"field of {class_list}[{len(test_attribute)}] must be defined in "
            f'"{RATapi.project.values_defined_in[class_list_str]}". Please add "undefined" to '
            f'"{RATapi.project.values_defined_in[class_list_str]}" before including it in "{class_list}".'
        ),
    ):
        test_attribute.append(input_model(**{**model_params, field: "undefined"}))

    # Ensure invalid model was not appended
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "model_type", "field", "model_params"],
    [
        ("backgrounds", "constant", "source", {}),
        ("resolutions", "constant", "source", {}),
        ("layers", "", "thickness", layer_params),
        ("layers", "", "SLD", layer_params),
        ("layers", "", "roughness", layer_params),
        ("contrasts", "", "data", {}),
        ("contrasts", "", "background", {}),
        ("contrasts", "", "bulk_in", {}),
        ("contrasts", "", "bulk_out", {}),
        ("contrasts", "", "scalefactor", {}),
        ("contrasts", "", "resolution", {}),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_insert(test_project, class_list: str, model_type: str, field: str, model_params: dict) -> None:
    """If we insert a model containing undefined values into a ClassList, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    input_model = model_classes[class_list]
    class_list_str = f"{class_list}{f'.{model_type}' if model_type else ''}.{field}"
    index = 0

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" '
            f"field of {class_list}[{index}] must be defined in "
            f'"{RATapi.project.values_defined_in[class_list_str]}". Please add "undefined" to '
            f'"{RATapi.project.values_defined_in[class_list_str]}" before including it in "{class_list}".'
        ),
    ):
        test_attribute.insert(index, input_model(**{**model_params, field: "undefined"}))

    # Ensure invalid model was not inserted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "field"],
    [
        ("backgrounds", "source"),
        ("backgrounds", "value_1"),
        ("backgrounds", "value_2"),
        ("backgrounds", "value_3"),
        ("backgrounds", "value_4"),
        ("backgrounds", "value_5"),
        ("resolutions", "source"),
        ("resolutions", "value_1"),
        ("resolutions", "value_2"),
        ("resolutions", "value_3"),
        ("resolutions", "value_4"),
        ("resolutions", "value_5"),
        ("contrasts", "data"),
        ("contrasts", "background"),
        ("contrasts", "bulk_in"),
        ("contrasts", "bulk_out"),
        ("contrasts", "scalefactor"),
        ("contrasts", "resolution"),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_insert_type_error(test_project, class_list: str, field: str) -> None:
    """If we raise a TypeError using the wrapped insert routine, we should re-raise the error."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    input_model = model_classes[class_list]

    with pytest.raises(TypeError):
        test_attribute.insert(input_model)

    # Ensure invalid model was not inserted
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "parameter", "field"],
    [
        ("background_parameters", "Background Param 1", "source"),
        ("resolution_parameters", "Resolution Param 1", "source"),
        ("parameters", "Test SLD", "SLD"),
        ("data", "Simulation", "data"),
        ("backgrounds", "Background 1", "background"),
        ("bulk_in", "SLD Air", "bulk_in"),
        ("bulk_out", "SLD D2O", "bulk_out"),
        ("scalefactors", "Scalefactor 1", "scalefactor"),
        ("resolutions", "Resolution 1", "resolution"),
    ],
)
def test_wrap_pop(test_project, class_list: str, parameter: str, field: str) -> None:
    """If we pop a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    index = test_attribute.index(parameter)

    sub_attribute_name = RATapi.project.model_names_used_in[class_list][0].attribute
    sub_attribute = getattr(test_project, sub_attribute_name)
    sub_index = [i for i, _ in enumerate(sub_attribute) if getattr(sub_attribute[i], field) == parameter][0]

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "{parameter}" used in the "{field}" field of '
            f'{sub_attribute_name}[{sub_index}] must be defined in "{class_list}". Please remove "{parameter}" from '
            f'"{sub_attribute_name}[{sub_index}].{field}" before attempting to delete it.'
        ),
    ):
        test_attribute.pop(index)

    # Ensure model was not popped
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "parameter", "field"],
    [
        ("background_parameters", "Background Param 1", "source"),
        ("resolution_parameters", "Resolution Param 1", "source"),
        ("parameters", "Test SLD", "SLD"),
        ("data", "Simulation", "data"),
        ("backgrounds", "Background 1", "background"),
        ("bulk_in", "SLD Air", "bulk_in"),
        ("bulk_out", "SLD D2O", "bulk_out"),
        ("scalefactors", "Scalefactor 1", "scalefactor"),
        ("resolutions", "Resolution 1", "resolution"),
    ],
)
def test_wrap_remove(test_project, class_list: str, parameter: str, field: str) -> None:
    """If we remove a model in a ClassList containing values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    sub_attribute_name = RATapi.project.model_names_used_in[class_list][0].attribute
    sub_attribute = getattr(test_project, sub_attribute_name)
    sub_index = [i for i, _ in enumerate(sub_attribute) if getattr(sub_attribute[i], field) == parameter][0]

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "{parameter}" used in the "{field}" field of '
            f'{sub_attribute_name}[{sub_index}] must be defined in "{class_list}". Please remove "{parameter}" from '
            f'"{sub_attribute_name}[{sub_index}].{field}" before attempting to delete it.'
        ),
    ):
        test_attribute.remove(parameter)

    # Ensure model was not removed
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "parameter", "field"],
    [
        ("background_parameters", "Background Param 1", "source"),
        ("resolution_parameters", "Resolution Param 1", "source"),
        ("parameters", "Test Thickness", "thickness"),
        ("data", "Simulation", "data"),
        ("backgrounds", "Background 1", "background"),
        ("bulk_in", "SLD Air", "bulk_in"),
        ("bulk_out", "SLD D2O", "bulk_out"),
        ("scalefactors", "Scalefactor 1", "scalefactor"),
        ("resolutions", "Resolution 1", "resolution"),
    ],
)
def test_wrap_clear(test_project, class_list: str, parameter: str, field: str) -> None:
    """If we clear a ClassList containing models with values defined elsewhere, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)

    sub_attribute_name = RATapi.project.model_names_used_in[class_list][0].attribute
    sub_attribute = getattr(test_project, sub_attribute_name)
    sub_index = [i for i, _ in enumerate(sub_attribute) if getattr(sub_attribute[i], field) == parameter][0]

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "{parameter}" used in the "{field}" field of '
            f'{sub_attribute_name}[{sub_index}] must be defined in "{class_list}". Please remove "{parameter}" from '
            f'"{sub_attribute_name}[{sub_index}].{field}" before attempting to delete it.'
        ),
    ):
        test_attribute.clear()

    # Ensure list was not cleared
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    ["class_list", "model_type", "field", "model_params"],
    [
        ("backgrounds", "constant", "source", {}),
        ("resolutions", "constant", "source", {}),
        ("layers", "", "thickness", layer_params),
        ("layers", "", "SLD", layer_params),
        ("layers", "", "roughness", layer_params),
        ("contrasts", "", "data", {}),
        ("contrasts", "", "background", {}),
        ("contrasts", "", "bulk_in", {}),
        ("contrasts", "", "bulk_out", {}),
        ("contrasts", "", "scalefactor", {}),
        ("contrasts", "", "resolution", {}),
    ],
)
@pytest.mark.filterwarnings("ignore:The following values are not recognised by this*:UserWarning")
def test_wrap_extend(test_project, class_list: str, model_type: str, field: str, model_params: dict) -> None:
    """If we extend a ClassList with model containing undefined values, we should raise a ValidationError."""
    test_attribute = getattr(test_project, class_list)
    orig_class_list = copy.deepcopy(test_attribute)
    input_model = model_classes[class_list]
    class_list_str = f"{class_list}{f'.{model_type}' if model_type else ''}.{field}"

    with pytest.raises(
        pydantic.ValidationError,
        match=re.escape(
            f'1 validation error for Project\n  Value error, The value "undefined" used in the "{field}" '
            f"field of {class_list}[{len(test_attribute)}] must be defined in "
            f'"{RATapi.project.values_defined_in[class_list_str]}". Please add "undefined" to '
            f'"{RATapi.project.values_defined_in[class_list_str]}" before including it in "{class_list}".'
        ),
    ):
        test_attribute.extend([input_model(**{**model_params, field: "undefined"})])

    # Ensure invalid model was not appended
    assert test_attribute == orig_class_list


@pytest.mark.parametrize(
    "project",
    [
        "r1_default_project",
        "r1_monolayer",
        "r1_monolayer_8_contrasts",
        "r1_orso_polymer",
        "r1_motofit_bench_mark",
        "dspc_standard_layers",
        "dspc_custom_layers",
        "dspc_custom_xy",
        "domains_standard_layers",
        "domains_custom_layers",
        "domains_custom_xy",
        "absorption",
    ],
)
def test_save_load(project, request):
    """Test that saving and loading a project returns the same project."""
    original_project = request.getfixturevalue(project)

    with tempfile.TemporaryDirectory() as tmp:
        # ignore relative path warnings
        path = Path(tmp, "project.json")
        with warnings.catch_warnings():
            warnings.simplefilter("ignore")
            original_project.save(path)
        converted_project = RATapi.Project.load(path)

    # resolve custom files in case the original project had unresolvable relative paths
    for file in original_project.custom_files:
        file.path = file.path.resolve()

    for field in original_project.model_fields:
        assert getattr(converted_project, field) == getattr(original_project, field)


def test_relative_paths():
    """Test that ``try_relative_to`` correctly creates relative paths to subfolders."""

    with tempfile.TemporaryDirectory() as tmp:
        data_path = Path(tmp, "data/myfile.dat")

        assert Path(RATapi.project.try_relative_to(data_path, tmp)) == Path("data/myfile.dat")


def test_relative_paths_warning():
    """Test that we get a warning for trying to walk up paths."""

    data_path = "/tmp/project/data/mydata.dat"
    relative_path = "/tmp/project/project_path/myproj.dat"

    with pytest.warns(
        match="Could not save custom file path as relative to the project directory, "
        "which means that it may not work on other devices. If you would like to share your project, "
        "make sure your custom files are in a subfolder of the project save location.",
    ):
        assert (
            Path(RATapi.project.try_relative_to(data_path, relative_path))
            == Path("/tmp/project/data/mydata.dat").resolve()
        )
