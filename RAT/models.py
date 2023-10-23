"""The models module. Contains the pydantic models used by RAT to store project parameters."""

import numpy as np
from pydantic import BaseModel, Field, ValidationInfo, field_validator, model_validator
from typing import Any

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


class Hydration(StrEnum):
    None_ = 'none'
    BulkIn = 'bulk in'
    BulkOut = 'bulk out'
    Oil = 'oil'


class Languages(StrEnum):
    Python = 'python'
    Matlab = 'matlab'


class Priors(StrEnum):
    Uniform = 'uniform'
    Gaussian = 'gaussian'


class Types(StrEnum):
    Constant = 'constant'
    Data = 'data'
    Function = 'function'


class RATModel(BaseModel):
    """A BaseModel where enums are represented by their value."""
    def __repr__(self):
        fields_repr = (', '.join(repr(v) if a is None else
                                 f'{a}={v.value!r}' if isinstance(v, StrEnum) else
                                 f'{a}={v!r}'
                                 for a, v in self.__repr_args__()
                                 )
                       )
        return f'{self.__repr_name__()}({fields_repr})'


class Background(RATModel, validate_assignment=True, extra='forbid'):
    """Defines the Backgrounds in RAT."""
    name: str = Field(default_factory=lambda: 'New Background ' + next(background_number), min_length=1)
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''


class Contrast(RATModel, validate_assignment=True, extra='forbid'):
    """Groups together all of the components of the model."""
    name: str = Field(default_factory=lambda: 'New Contrast ' + next(contrast_number), min_length=1)
    data: str = ''
    background: str = ''
    nba: str = ''
    nbs: str = ''
    scalefactor: str = ''
    resolution: str = ''
    resample: bool = False
    model: list[str] = []


class ContrastWithRatio(RATModel, validate_assignment=True, extra='forbid'):
    """Groups together all of the components of the model including domain terms."""
    name: str = Field(default_factory=lambda: 'New Contrast ' + next(contrast_number), min_length=1)
    data: str = ''
    background: str = ''
    nba: str = ''
    nbs: str = ''
    scalefactor: str = ''
    resolution: str = ''
    resample: bool = False
    domain_ratio: str = ''
    model: list[str] = []


class CustomFile(RATModel, validate_assignment=True, extra='forbid'):
    """Defines the files containing functions to run when using custom models."""
    name: str = Field(default_factory=lambda: 'New Custom File ' + next(custom_file_number), min_length=1)
    filename: str = ''
    language: Languages = Languages.Python
    path: str = 'pwd'  # Should later expand to find current file path


class Data(RATModel, validate_assignment=True, extra='forbid', arbitrary_types_allowed=True):
    """Defines the dataset required for each contrast."""
    name: str = Field(default_factory=lambda: 'New Data ' + next(data_number), min_length=1)
    data: np.ndarray[np.float64] = np.empty([0, 3])
    data_range: list[float] = Field(default=[], min_length=2, max_length=2)
    simulation_range: list[float] = Field(default=[], min_length=2, max_length=2)

    @field_validator('data')
    @classmethod
    def check_data_dimension(cls, data: np.ndarray[float]) -> np.ndarray[float]:
        """The data must be a two-dimensional array containing at least three columns."""
        try:
            data.shape[1]
        except IndexError:
            raise ValueError('"data" must have at least two dimensions')
        else:
            if data.shape[1] < 3:
                raise ValueError('"data" must have at least three columns')
        return data

    @field_validator('data_range', 'simulation_range')
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
        if len(self.data[:, 0]) > 0:
            data_min = np.min(self.data[:, 0])
            data_max = np.max(self.data[:, 0])
            for field in ["data_range", "simulation_range"]:
                if field not in self.model_fields_set:
                    getattr(self, field).extend([data_min, data_max])

    @model_validator(mode='after')
    def check_ranges(self) -> 'Data':
        """The limits of the "data_range" field must lie within the range of the supplied data, whilst the limits
        of the "simulation_range" field must lie outside of the range of the supplied data.
        """
        if len(self.data[:, 0]) > 0:
            data_min = np.min(self.data[:, 0])
            data_max = np.max(self.data[:, 0])
            if "data_range" in self.model_fields_set and (self.data_range[0] < data_min or
                                                          self.data_range[1] > data_max):
                raise ValueError(f'The data_range value of: {self.data_range} must lie within the min/max values of '
                                 f'the data: [{data_min}, {data_max}]')
            if "simulation_range" in self.model_fields_set and (self.simulation_range[0] > data_min or
                                                                self.simulation_range[1] < data_max):
                raise ValueError(f'The simulation_range value of: {self.simulation_range} must lie outside of the '
                                 f'min/max values of the data: [{data_min}, {data_max}]')
        return self

    def __eq__(self, other: Any) -> bool:
        if isinstance(other, BaseModel):
            # When comparing instances of generic types for equality, as long as all field values are equal,
            # only require their generic origin types to be equal, rather than exact type equality.
            # This prevents headaches like MyGeneric(x=1) != MyGeneric[Any](x=1).
            self_type = self.__pydantic_generic_metadata__['origin'] or self.__class__
            other_type = other.__pydantic_generic_metadata__['origin'] or other.__class__

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

    def __repr__(self):
        """Only include the name if the data is empty."""
        fields_repr = (f"name={self.name!r}" if self.data.size == 0 else
                       ", ".join(repr(v) if a is None else
                                 f"{a}={v!r}"
                                 for a, v in self.__repr_args__()
                                 )
                       )
        return f'{self.__repr_name__()}({fields_repr})'


class DomainContrast(RATModel, validate_assignment=True, extra='forbid'):
    """Groups together the layers required for each domain."""
    name: str = Field(default_factory=lambda: 'New Domain Contrast ' + next(domain_contrast_number), min_length=1)
    model: list[str] = []


class Layer(RATModel, validate_assignment=True, extra='forbid', populate_by_name=True):
    """Combines parameters into defined layers."""
    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_number), min_length=1)
    thickness: str = ''
    SLD: str = Field('', validation_alias='SLD_real')
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut


class AbsorptionLayer(RATModel, validate_assignment=True, extra='forbid', populate_by_name=True):
    """Combines parameters into defined layers including absorption terms."""
    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_number), min_length=1)
    thickness: str = ''
    SLD_real: str = Field('', validation_alias='SLD')
    SLD_imaginary: str = ''
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut


class Parameter(RATModel, validate_assignment=True, extra='forbid'):
    """Defines parameters needed to specify the model."""
    name: str = Field(default_factory=lambda: 'New Parameter ' + next(parameter_number), min_length=1)
    min: float = 0.0
    value: float = 0.0
    max: float = 0.0
    fit: bool = False
    prior_type: Priors = Priors.Uniform
    mu: float = 0.0
    sigma: float = np.inf

    @model_validator(mode='after')
    def check_value_in_range(self) -> 'Parameter':
        """The value of a parameter must lie within its defined bounds."""
        if self.value < self.min or self.value > self.max:
            raise ValueError(f'value {self.value} is not within the defined range: {self.min} <= value <= {self.max}')
        return self


class ProtectedParameter(Parameter, validate_assignment=True, extra='forbid'):
    """A Parameter with a fixed name."""
    name: str = Field(frozen=True, min_length=1)


class Resolution(RATModel, validate_assignment=True, extra='forbid'):
    """Defines Resolutions in RAT."""
    name: str = Field(default_factory=lambda: 'New Resolution ' + next(resolution_number), min_length=1)
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''
