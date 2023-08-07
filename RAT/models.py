from enum import Enum
from pydantic import BaseModel, Field, FieldValidationInfo, field_validator, model_validator
from typing import ClassVar

import math  # replace with np


def int_sequence():
    num = 1
    while True:
        yield str(num)
        num += 1


background_id = int_sequence()
contrast_id = int_sequence()
custom_file_id = int_sequence()
data_id = int_sequence()
domain_contrast_id = int_sequence()
layer_id = int_sequence()
parameter_id = int_sequence()
resolution_id = int_sequence()


class Hydration(str, Enum):
    None_ = 'none'
    BulkIn = 'bulk in'
    BulkOut = 'bulk out'
    Oil = 'oil'


class Languages(str, Enum):
    Python = 'python'
    Matlab = 'matlab'


class Priors(str, Enum):
    Uniform = 'uniform'
    Gaussian = 'gaussian'
    Jeffreys = 'jeffreys'


class Types(str, Enum):
    Constant = 'constant'
    Data = 'data'
    Function = 'function'


class Background(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Background ' + next(background_id))
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''

    parameter_names: ClassVar[list[str]] = []

    @field_validator('value_1', 'value_2', 'value_3', 'value_4', 'value_5')
    @classmethod
    def check_parameter_is_defined(cls, value) -> str:
        if value not in Background.parameter_names:
            raise ValueError(f'The parameter "{value}" has not been defined in the background parameters.')
        return value


class Contrast(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Contrast ' + next(contrast_id))
    data: str = ''
    background: str = ''
    nba: str = ''
    nbs: str = ''
    scalefactor: str = ''
    resolution: str = ''
    resample: bool = False
    model: list[str] = []  # But how many strings? How to deal with this?

    all_names: ClassVar[dict[str, list[str]]] = []

    @field_validator('data', 'background', 'nba', 'nbs', 'scalefactor', 'resolution')
    @classmethod
    def check_parameter_is_defined(cls, value: str, info: FieldValidationInfo) -> str:
        if value not in Contrast.all_names[info.field_name]:
            raise ValueError(f'The parameter "{value}" has not been defined in the "{info.field_name}" parameters.')
        return value


class CustomFile(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Custom File ' + next(custom_file_id))
    filename: str = ''
    language: Languages = Languages.Python
    path: str = 'pwd'  # Should later expand to find current file path


class Data(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Data ' + next(data_id))
    data: str = ''  # pandas dataframe?
    data_range: list[float] = []
    simulation_range: list[float] = [0.005, 0.7]

    @field_validator('data_range', 'simulation_range')
    @classmethod
    def check_list_elements(cls, range_) -> list[int]:
        if len(range_) != 2:
            raise ValueError(f'{range_.__name__} must contain two values')
        return range_


class DomainContrast(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Domain Contrast ' + next(domain_contrast_id))
    model: list[str] = []

    layer_names: ClassVar[list[str]] = []

    @field_validator('model')
    @classmethod
    def check_parameter_is_defined(cls, value) -> str:
        if value not in DomainContrast.layer_names:
            raise ValueError(f'The parameter "{value}" has not been defined in the layers.')
        return value


class Layer(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_id))
    thickness: str = ''
    SLD: str = ''
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut

    parameter_names: ClassVar[list[str]] = []

    @field_validator('thickness', 'SLD', 'roughness')
    @classmethod
    def check_parameter_is_defined(cls, value) -> str:
        if value not in Layer.parameter_names:
            raise ValueError(f'The parameter "{value}" has not been defined in the parameters.')
        return value


class Parameter(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Parameter ' + next(parameter_id))
    min: float = 0.0
    value: float = 0.0
    max: float = 0.0
    fit: bool = False
    prior_type: Priors = Priors.Uniform
    mu: float = 0.0
    sigma: float = math.inf

    @model_validator(mode='after')
    def check_value_in_range(self) -> 'Parameter':
        if self.value < self.min or self.value > self.max:
            raise ValueError(f'value {self.value} is not within the defined range: {self.min} <= value <= {self.max}')
        return self


class ProtectedParameter(Parameter, validate_assignment=True, extra='forbid'):
    name: str = Field(frozen=True)


class Resolution(BaseModel, validate_assignment=True, extra='forbid'):
    name: str = Field(default_factory=lambda: 'New Resolution ' + next(resolution_id))
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''

    parameter_names: ClassVar[list[str]] = []

    @field_validator('value_1', 'value_2', 'value_3', 'value_4', 'value_5')
    @classmethod
    def check_parameter_is_defined(cls, value) -> str:
        if value not in Resolution.parameter_names:
            raise ValueError(f'The parameter "{value}" has not been defined in the resolution parameters.')
        return value
