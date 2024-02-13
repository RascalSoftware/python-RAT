"""Test the pydantic models."""

import numpy as np
import pydantic
import pytest
import re
from typing import Callable

import RAT.models


@pytest.mark.parametrize(["model", "model_name"], [
    (RAT.models.Background, "Background"),
    (RAT.models.Contrast, "Contrast"),
    (RAT.models.CustomFile, "Custom File"),
    (RAT.models.Data, "Data"),
    (RAT.models.DomainContrast, "Domain Contrast"),
    (RAT.models.Layer, "Layer"),
    (RAT.models.Parameter, "Parameter"),
    (RAT.models.Resolution, "Resolution"),
])
def test_default_names(model: Callable, model_name: str) -> None:
    """When initialising multiple models without specifying a name, they should be given a default name with the
    format: "New <model name> <integer>".
    """
    model_1 = model()
    model_2 = model()
    model_3 = model(name='Given Name')
    model_4 = model()

    assert model_1.name == f"New {model_name} 1"
    assert model_2.name == f"New {model_name} 2"
    assert model_3.name == "Given Name"
    assert model_4.name == f"New {model_name} 3"


@pytest.mark.parametrize("model", [
    RAT.models.Background,
    RAT.models.Contrast,
    RAT.models.ContrastWithRatio,
    RAT.models.CustomFile,
    RAT.models.Data,
    RAT.models.DomainContrast,
    RAT.models.Layer,
    RAT.models.AbsorptionLayer,
    RAT.models.Parameter,
    RAT.models.Resolution,
])
class TestModels(object):
    def test_initialise_with_wrong_type(self, model: Callable) -> None:
        """When initialising a model with the wrong type for the "name" field, we should raise a ValidationError."""
        with pytest.raises(pydantic.ValidationError, match=f'1 validation error for {model.__name__}\nname\n  Input should be a valid string'):
            model(name=1)

    def test_assignment_with_wrong_type(self, model: Callable) -> None:
        """When assigning the "name" field of a model with the wrong type, we should raise a ValidationError."""
        test_model = model()
        with pytest.raises(pydantic.ValidationError, match=f'1 validation error for {model.__name__}\nname\n  Input should be a valid string'):
            test_model.name = 1

    def test_initialise_with_zero_length_name(self, model: Callable) -> None:
        """When initialising a model with a zero length name, we should raise a ValidationError."""
        with pytest.raises(pydantic.ValidationError, match=f'1 validation error for {model.__name__}\nname\n  String should have at least 1 character'):
            model(name='')

    def test_initialise_with_extra_fields(self, model: Callable) -> None:
        """When initialising a model with unspecified fields, we should raise a ValidationError."""
        with pytest.raises(pydantic.ValidationError, match=f'1 validation error for {model.__name__}\nnew_field\n  Extra inputs are not permitted'):
            model(new_field=1)


def test_data_eq() -> None:
    """If we use the Data.__eq__ method with an object that is not a pydantic BaseModel, we should return
    "NotImplemented".
    """
    assert RAT.models.Data().__eq__('data') == NotImplemented


@pytest.mark.parametrize("input_data", [
    (np.array([[1.0, 1.0, 1.0]])),
])
def test_data_dimension(input_data: np.ndarray[float]) -> None:
    """The "data" field of the "Data" model should be a two-dimensional numpy array with at least three values in the
    second dimension.
    """
    test_data = RAT.models.Data(data=input_data)
    assert (test_data.data == input_data).all()


@pytest.mark.parametrize("input_data", [
    (np.array([])),
    (np.array([1.0, 1.0])),
])
def test_data_too_few_dimensions(input_data: np.ndarray[float]) -> None:
    """If the "data" field of the "Data" model is not a two-dimensional numpy array we should raise a ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Data\ndata\n  Value error, "data" must '
                                                       'have at least two dimensions'):
        RAT.models.Data(data=input_data)


@pytest.mark.parametrize("input_data", [
    (np.array([[]])),
    (np.array([[1.0]])),
    (np.array([[1.0, 1.0]])),
])
def test_data_too_few_values(input_data: np.ndarray[float]) -> None:
    """If the second dimension of the array in the "data" field of the "Data" model has fewer than three values we
    should raise a ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Data\ndata\n  Value error, "data" must '
                                                       'have at least three columns'):
        RAT.models.Data(data=input_data)


@pytest.mark.parametrize("input_range", [
    ([1.0, 2.0]),
])
def test_data_ranges(input_range: list[float]) -> None:
    """The "data_range" and "simulation_range" fields of the "Data" model should contain exactly two values."""
    assert RAT.models.Data(data_range=input_range).data_range == input_range
    assert RAT.models.Data(simulation_range=input_range).simulation_range == input_range


@pytest.mark.parametrize("input_range", [
    ([]),
    ([1.0]),
    ([1.0, 2.0, 3.0]),
])
def test_two_values_in_data_range(input_range: list[float]) -> None:
    """If the "data_range" field of the "Data" model contains more or less than two values, we should raise a
    ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Data\ndata_range\n  List should have '
                                                       f'at {"least" if len(input_range) < 2 else "most"} 2 items '
                                                       f'after validation, not {len(input_range)}'):
        RAT.models.Data(data_range=input_range)


@pytest.mark.parametrize("input_range", [
    ([]),
    ([1.0]),
    ([1.0, 2.0, 3.0]),
])
def test_two_values_in_simulation_range(input_range: list[float]) -> None:
    """If the "simulation_range" field of the "Data" model contains more or less than two values, we should raise a
    ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Data\nsimulation_range\n  List should '
                                                       f'have at {"least" if len(input_range) < 2 else "most"} 2 items '
                                                       f'after validation, not {len(input_range)}'):
        RAT.models.Data(simulation_range=input_range)


@pytest.mark.parametrize("field", [
    'data_range',
    'simulation_range'
])
def test_min_max_in_range(field: str) -> None:
    """If the maximum value of the "data_range" or "simulation_range" fields of the "Data" model is greater than the
    minimum value, we should raise a ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Data\n{field}\n  Value error, {field} '
                                                       f'"min" value is greater than the "max" value'):
        RAT.models.Data(**{field: [1.0, 0.0]})


def test_default_ranges() -> None:
    """If "data" is specified but either the "data_range" or "simulation_range" fields are not, we set the ranges to
    the minimum and maximum values of the first column of the data.
    """
    test_data = RAT.models.Data(data=np.array([[1.0, 0.0, 0.0], [3.0, 0.0, 0.0]]))
    assert test_data.data_range == [1.0, 3.0]
    assert test_data.simulation_range == [1.0, 3.0]


@pytest.mark.parametrize("test_range", [
    [0.0, 2.0],
    [2.0, 4.0],
    [0.0, 4.0],
])
def test_data_range(test_range) -> None:
    """If "data" is specified but the "data_range" lies outside of the limits of the data we should raise a
    ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=re.escape(f'1 validation error for Data\n  Value error, The '
                                                                 f'data_range value of: {test_range} must lie within '
                                                                 f'the min/max values of the data: [1.0, 3.0]')):
        RAT.models.Data(data=np.array([[1.0, 0.0, 0.0], [3.0, 0.0, 0.0]]), data_range=test_range)


@pytest.mark.parametrize("test_range", [
    [0.0, 2.0],
    [2.0, 4.0],
    [1.5, 2.5],
])
def test_simulation_range(test_range) -> None:
    """If "data" is specified but the "simulation_range" lies within the limits of the data we should raise a
    ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=re.escape(f'1 validation error for Data\n  Value error, The '
                                                                 f'simulation_range value of: {test_range} must lie '
                                                                 f'outside of the min/max values of the data: '
                                                                 f'[1.0, 3.0]')):
        RAT.models.Data(data=np.array([[1.0, 0.0, 0.0], [3.0, 0.0, 0.0]]), simulation_range=test_range)


@pytest.mark.parametrize(["minimum", "value", "maximum"], [
    (0.0, 2.0, 1.0),
    (0, 1, 0),
    (1, -1, 1),
])
def test_parameter_range(minimum: float, value: float, maximum: float) -> None:
    """For the "Parameter" model, if the value of the "value" field does not lie with the values given in the "min" and
    "max" fields, we should raise a ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for Parameter\n  Value error, value '
                                                       f'{float(value)} is not within the defined range: '
                                                       f'{float(minimum)} <= value <= {float(maximum)}'):
        RAT.models.Parameter(min=minimum, value=value, max=maximum)
