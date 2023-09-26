"""The models module. Contains the pydantic models used by RAT to store project parameters."""

import numpy as np
from pydantic import BaseModel, Field, ValidationInfo, field_validator, model_validator

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
    Jeffreys = 'jeffreys'


class Types(StrEnum):
    Constant = 'constant'
    Data = 'data'
    Function = 'function'


class Background(BaseModel, validate_assignment=True, extra='forbid'):
    """Defines the Backgrounds in RAT."""
    name: str = Field(default_factory=lambda: 'New Background ' + next(background_number), min_length=1)
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''


class Contrast(BaseModel, validate_assignment=True, extra='forbid'):
    """Groups together all of the components of the model."""
    name: str = Field(default_factory=lambda: 'New Contrast ' + next(contrast_number), min_length=1)
    data: str = ''
    background: str = ''
    nba: str = ''
    nbs: str = ''
    scalefactor: str = ''
    resolution: str = ''
    resample: bool = False
    model: list[str] = []  # But how many strings? How to deal with this?


class CustomFile(BaseModel, validate_assignment=True, extra='forbid'):
    """Defines the files containing functions to run when using custom models."""
    name: str = Field(default_factory=lambda: 'New Custom File ' + next(custom_file_number), min_length=1)
    filename: str = ''
    language: Languages = Languages.Python
    path: str = 'pwd'  # Should later expand to find current file path


class Data(BaseModel, validate_assignment=True, extra='forbid', arbitrary_types_allowed=True):
    """Defines the dataset required for each contrast."""
    name: str = Field(default_factory=lambda: 'New Data ' + next(data_number), min_length=1)
    data: np.ndarray[float] = np.empty([0, 3])
    data_range: list[float] = []
    simulation_range: list[float] = [0.005, 0.7]

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
    def check_list_elements(cls, limits: list[float], info: ValidationInfo) -> list[float]:
        """The data range and simulation range must contain exactly two parameters."""
        if len(limits) != 2:
            raise ValueError(f'{info.field_name} must contain exactly two values')
        return limits

    # Also need model validators for data range compared to data etc -- need more details.


class DomainContrast(BaseModel, validate_assignment=True, extra='forbid'):
    """Groups together the layers required for each domain."""
    name: str = Field(default_factory=lambda: 'New Domain Contrast ' + next(domain_contrast_number), min_length=1)
    model: list[str] = []


class Layer(BaseModel, validate_assignment=True, extra='forbid', populate_by_name=True):
    """Combines parameters into defined layers."""
    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_number), min_length=1)
    thickness: str = ''
    SLD: str = Field('', validation_alias='SLD_real')
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut


class AbsorptionLayer(BaseModel, validate_assignment=True, extra='forbid', populate_by_name=True):
    """Combines parameters into defined layers including absorption terms."""
    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_number), min_length=1)
    thickness: str = ''
    SLD_real: str = Field('', validation_alias='SLD')
    SLD_imaginary: str = ''
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut


class Parameter(BaseModel, validate_assignment=True, extra='forbid'):
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


class Resolution(BaseModel, validate_assignment=True, extra='forbid'):
    """Defines Resolutions in RAT."""
    name: str = Field(default_factory=lambda: 'New Resolution ' + next(resolution_number), min_length=1)
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''
