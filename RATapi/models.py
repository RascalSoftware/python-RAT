"""The models module. Contains the pydantic models used by RAT to store project parameters."""

import pathlib
from typing import Any

import numpy as np
import prettytable
from pydantic import BaseModel, Field, ValidationInfo, field_validator, model_validator

from RATapi.utils.enums import BackgroundActions, Hydration, Languages, Priors, TypeOptions

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


def int_sequence():
    """Iterate through integers for use as model counters."""
    num = 1
    while True:
        yield str(num)
        num += 1


# Create a counter for each model
background_number = int_sequence()
contrast_number = int_sequence()
custom_file_number = int_sequence()
data_number = int_sequence()
domain_contrast_number = int_sequence()
layer_number = int_sequence()
parameter_number = int_sequence()
resolution_number = int_sequence()


class RATModel(BaseModel, validate_assignment=True, extra="forbid"):
    """A BaseModel where enums are represented by their value."""

    def __repr__(self):
        fields_repr = ", ".join(
            repr(v) if a is None else f"{a}={v.value!r}" if isinstance(v, StrEnum) else f"{a}={v!r}"
            for a, v in self.__repr_args__()
        )
        return f"{self.__repr_name__()}({fields_repr})"

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        table.add_row(list(self.__dict__.values()))
        return table.get_string()


class Background(RATModel):
    """Defines the Backgrounds in RAT."""

    name: str = Field(default_factory=lambda: "New Background " + next(background_number), min_length=1)
    type: TypeOptions = TypeOptions.Constant
    value_1: str = ""
    value_2: str = ""
    value_3: str = ""
    value_4: str = ""
    value_5: str = ""

    @field_validator("type")
    @classmethod
    def validate_unimplemented_backgrounds(cls, type: TypeOptions):
        """Raise an error if currently unsupported Data or Function backgrounds are used."""
        # FIXME: once data/function backgrounds have been implemented,
        # please remember to remove the @pytest.mark.skip decorators used to skip the tests:
        # - tests/test_project.py::test_check_allowed_background_resolution_values_data
        # - tests/test_project.py::test_check_allowed_background_resolution_values_on_data_list
        if type == TypeOptions.Data:
            raise NotImplementedError("Data backgrounds are not yet supported.")
        if type == TypeOptions.Function:
            raise NotImplementedError("Function backgrounds are not yet supported.")
        return type


class Contrast(RATModel):
    """Groups together all of the components of the model."""

    name: str = Field(default_factory=lambda: "New Contrast " + next(contrast_number), min_length=1)
    data: str = ""
    background: str = ""
    background_action: BackgroundActions = BackgroundActions.Add
    bulk_in: str = ""
    bulk_out: str = ""
    scalefactor: str = ""
    resolution: str = ""
    resample: bool = False
    model: list[str] = []

    @model_validator(mode="before")
    @classmethod
    def domain_ratio_error(cls, data: Any):
        """If the extra input 'domain_ratio' is given, give a more descriptive error."""

        if isinstance(data, dict) and data.get("domain_ratio", False):
            raise ValueError(
                "The Contrast class does not support domain ratios. Use the ContrastWithRatio class instead."
            )

        return data

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row(
            [
                self.name,
                self.data,
                self.background,
                self.background_action,
                self.bulk_in,
                self.bulk_out,
                self.scalefactor,
                self.resolution,
                self.resample,
                model_entry,
            ]
        )
        return table.get_string()


class ContrastWithRatio(RATModel):
    """Groups together all of the components of the model including domain terms."""

    name: str = Field(default_factory=lambda: "New Contrast " + next(contrast_number), min_length=1)
    data: str = ""
    background: str = ""
    background_action: BackgroundActions = BackgroundActions.Add
    bulk_in: str = ""
    bulk_out: str = ""
    scalefactor: str = ""
    resolution: str = ""
    resample: bool = False
    domain_ratio: str = ""
    model: list[str] = []

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row(
            [
                self.name,
                self.data,
                self.background,
                self.background_action,
                self.bulk_in,
                self.bulk_out,
                self.scalefactor,
                self.resolution,
                self.resample,
                model_entry,
            ]
        )
        return table.get_string()


class CustomFile(RATModel):
    """Defines the files containing functions to run when using custom models."""

    name: str = Field(default_factory=lambda: "New Custom File " + next(custom_file_number), min_length=1)
    filename: str = ""
    function_name: str = ""
    language: Languages = Languages.Python
    path: pathlib.Path = pathlib.Path(".")

    def model_post_init(self, __context: Any) -> None:
        """If a "filename" is supplied but the "function_name" field is not set, the "function_name" should be set to
        the file name without the extension.
        """
        if "filename" in self.model_fields_set and "function_name" not in self.model_fields_set:
            self.function_name = pathlib.Path(self.filename).stem

    @model_validator(mode="after")
    def set_matlab_function_name(self):
        """If we have a matlab custom function, the "function_name" should be set to the filename without the
        extension.
        """
        if self.language == Languages.Matlab and self.function_name != pathlib.Path(self.filename).stem:
            self.function_name = pathlib.Path(self.filename).stem

        return self


class Data(RATModel, arbitrary_types_allowed=True):
    """Defines the dataset required for each contrast."""

    name: str = Field(default_factory=lambda: "New Data " + next(data_number), min_length=1)
    data: np.ndarray[np.float64] = np.empty([0, 3])
    data_range: list[float] = Field(default=[], min_length=2, max_length=2)
    simulation_range: list[float] = Field(default=[], min_length=2, max_length=2)

    @field_validator("data")
    @classmethod
    def check_data_dimension(cls, data: np.ndarray[float]) -> np.ndarray[float]:
        """The data must be a two-dimensional array containing at least three columns."""
        try:
            data.shape[1]
        except IndexError:
            raise ValueError('"data" must have at least two dimensions') from None
        else:
            if data.shape[1] < 3:
                raise ValueError('"data" must have at least three columns') from None
        return data

    @field_validator("data_range", "simulation_range")
    @classmethod
    def check_min_max(cls, limits: list[float], info: ValidationInfo) -> list[float]:
        """The data range and simulation range maximum must be greater than the minimum."""
        if limits[0] > limits[1]:
            raise ValueError(f'{info.field_name} "min" value is greater than the "max" value')
        return limits

    def model_post_init(self, __context: Any) -> None:
        """If the "data_range" and "simulation_range" fields are not set, but "data" is supplied, the ranges should be
        set to the min and max values of the first column (assumed to be q) of the supplied data.
        """
        if self.data.shape[0] > 0:
            data_min = float(np.min(self.data[:, 0]))
            data_max = float(np.max(self.data[:, 0]))
            for field in ["data_range", "simulation_range"]:
                if field not in self.model_fields_set:
                    getattr(self, field).extend([data_min, data_max])

    @model_validator(mode="after")
    def check_ranges(self) -> "Data":
        """The limits of the "data_range" field must lie within the range of the supplied data, whilst the limits
        of the "simulation_range" field must lie outside the range of the supplied data.
        """
        if self.data.shape[0] > 0:
            data_min = np.min(self.data[:, 0])
            data_max = np.max(self.data[:, 0])
            if "data_range" in self.model_fields_set and (
                self.data_range[0] < data_min or self.data_range[1] > data_max
            ):
                raise ValueError(
                    f"The data_range value of: {self.data_range} must lie within the min/max values of "
                    f"the data: [{data_min}, {data_max}]",
                )
            if "simulation_range" in self.model_fields_set and (
                self.simulation_range[0] > data_min or self.simulation_range[1] < data_max
            ):
                raise ValueError(
                    f"The simulation_range value of: {self.simulation_range} must lie outside of the "
                    f"min/max values of the data: [{data_min}, {data_max}]",
                )
        return self

    def __eq__(self, other: object) -> bool:
        if isinstance(other, BaseModel):
            # When comparing instances of generic types for equality, as long as all field values are equal,
            # only require their generic origin types to be equal, rather than exact type equality.
            # This prevents headaches like MyGeneric(x=1) != MyGeneric[Any](x=1).
            self_type = self.__pydantic_generic_metadata__["origin"] or self.__class__
            other_type = other.__pydantic_generic_metadata__["origin"] or other.__class__

            return (
                self_type == other_type
                and self.name == other.name
                and (self.data == other.data).all()
                and self.data_range == other.data_range
                and self.simulation_range == other.simulation_range
                and self.__pydantic_private__ == other.__pydantic_private__
                and self.__pydantic_extra__ == other.__pydantic_extra__
            )
        else:
            return NotImplemented  # delegate to the other item in the comparison

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        array_entry = f"{'Data array: ['+' x '.join(str(i) for i in self.data.shape) if self.data.size > 0 else '['}]"
        table.add_row([self.name, array_entry, self.data_range, self.simulation_range])
        return table.get_string()


class DomainContrast(RATModel):
    """Groups together the layers required for each domain."""

    name: str = Field(default_factory=lambda: "New Domain Contrast " + next(domain_contrast_number), min_length=1)
    model: list[str] = []

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row([self.name, model_entry])
        return table.get_string()


class Layer(RATModel, populate_by_name=True):
    """Combines parameters into defined layers."""

    name: str = Field(default_factory=lambda: "New Layer " + next(layer_number), min_length=1)
    thickness: str
    SLD: str = Field(validation_alias="SLD_real")
    roughness: str
    hydration: str = ""
    hydrate_with: Hydration = Hydration.BulkOut

    @model_validator(mode="before")
    @classmethod
    def sld_imaginary_error(cls, data: Any):
        """If the extra input 'sld_imaginary' is given, give a more descriptive error."""

        if isinstance(data, dict) and data.get("SLD_imaginary", False):
            raise ValueError("The Layer class does not support imaginary SLD. Use the AbsorptionLayer class instead.")

        return data


class AbsorptionLayer(RATModel, populate_by_name=True):
    """Combines parameters into defined layers including absorption terms."""

    name: str = Field(default_factory=lambda: "New Layer " + next(layer_number), min_length=1)
    thickness: str
    SLD_real: str = Field(validation_alias="SLD")
    SLD_imaginary: str = ""
    roughness: str
    hydration: str = ""
    hydrate_with: Hydration = Hydration.BulkOut


class Parameter(RATModel):
    """Defines parameters needed to specify the model."""

    name: str = Field(default_factory=lambda: "New Parameter " + next(parameter_number), min_length=1)
    min: float = 0.0
    value: float = 0.0
    max: float = 0.0
    fit: bool = False
    prior_type: Priors = Priors.Uniform
    mu: float = 0.0
    sigma: float = np.inf

    @model_validator(mode="after")
    def check_min_max(self) -> "Parameter":
        """The maximum value of a parameter must be greater than the minimum."""
        if self.min > self.max:
            raise ValueError(f"The maximum value {self.max} must be greater than the minimum value {self.min}")
        return self

    @model_validator(mode="after")
    def check_value_in_range(self) -> "Parameter":
        """The value of a parameter must lie within its defined bounds."""
        if self.value < self.min or self.value > self.max:
            raise ValueError(f"value {self.value} is not within the defined range: {self.min} <= value <= {self.max}")
        return self


class ProtectedParameter(Parameter):
    """A Parameter with a fixed name."""

    name: str = Field(frozen=True, min_length=1)


class Resolution(RATModel):
    """Defines Resolutions in RAT."""

    name: str = Field(default_factory=lambda: "New Resolution " + next(resolution_number), min_length=1)
    type: TypeOptions = TypeOptions.Constant
    value_1: str = ""
    value_2: str = ""
    value_3: str = ""
    value_4: str = ""
    value_5: str = ""
