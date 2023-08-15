"""Test the pydantic models."""

import numpy as np
import pydantic
import pytest
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
class TestModels(object):

    def test_default_names(self, model: Callable, model_name: str) -> None:
        """When initialising multiple models without specifying a name, they should be given a default name with the
        format: "New <model name> <integer>".
        """
        model_1 = model()
        model_2 = model()
        model_3 = model(name='Given Name')
        model_4 = model()

        assert model_1.name == "New " + model_name + " 1"
        assert model_2.name == "New " + model_name + " 2"
        assert model_3.name == "Given Name"
        assert model_4.name == "New " + model_name + " 3"

    def test_initialise_with_wrong_type(self, model: Callable, model_name: str) -> None:
        """When initialising a model with the wrong type for the "name" field, we should raise a ValidationError."""
        with pytest.raises(pydantic.ValidationError, match='Input should be a valid string'):
            model(name=1)

    def test_assignment_with_wrong_type(self, model: Callable, model_name: str) -> None:
        """When assigning the "name" field of a model with the wrong type, we should raise a ValidationError."""
        test_model = model()
        with pytest.raises(pydantic.ValidationError, match='Input should be a valid string'):
            test_model.name = 1

    def test_initialise_with_extra_fields(self, model: Callable, model_name: str) -> None:
        """When initialising a model with unspecified fields, we should raise a ValidationError."""
        with pytest.raises(pydantic.ValidationError, match='Extra inputs are not permitted'):
            model(new_field=1)


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
def test_data_dimension(input_data: np.ndarray[float]) -> None:
    """If the second dimension of the array in the "data" field of the "Data" model has fewer than three values we
    should raise a ValidationError.
    """
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Data\ndata\n  Value error, "data" must '
                                                       'have at least three columns'):
        RAT.models.Data(data=input_data)


@pytest.mark.parametrize("input_range", [
    ([]),
    ([1.0]),
    ([1.0, 2.0, 3.0]),
])
def test_two_values_in_data_range(input_range: list[float]) -> None:
    """If the "data_range" field of the "Data" model contains more or less than two values, we should raise a
    ValueError.
    """
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Data\ndata_range\n  Value error, '
                                                       'data_range must contain exactly two values'):
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
    with pytest.raises(pydantic.ValidationError, match='1 validation error for Data\nsimulation_range\n  Value error, '
                                                       'simulation_range must contain exactly two values'):
        RAT.models.Data(simulation_range=input_range)


@pytest.mark.parametrize("minimum, value, maximum", [
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
