from enum import Enum
from pydantic import BaseModel, ConfigDict, Field, field_validator, model_validator
from typing import ClassVar, List, Literal

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


class Background(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Background ' + next(background_id))
    type: Types = Types.Constant
    value1: str = ''
    value2: str = ''
    value3: str = ''
    value4: str = ''
    value5: str = ''


class Contrast(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Contrast ' + next(contrast_id))
    data: str = ''
    background: str = ''
    nba: str = ''
    nbs: str = ''
    scalefactor: str = ''
    resolution: str = ''
    resample: bool = False
    model: List[str] = []  # But how many strings? How to deal with this?


class CustomFile(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Custom File ' + next(custom_file_id))
    filename: str = ''
    language: Languages = Languages.Python
    path: str = 'pwd'  # Should later expand to find current file path


class Data(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Data ' + next(data_id))
    data: str = ''  # pandas dataframe?
    data_range: List[float] = []
    simulation_range: List[float] = [0.005, 0.7]

    @field_validator('data_range', 'simulation_range')
    @classmethod
    def check_list_elements(cls, range_) -> List[int]:
        if len(range_) != 2:
            raise ValueError(f'{range_.__name__} must contain two values')
        return range_


class DomainContrast(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Domain Contrast ' + next(domain_contrast_id))
    model: List[str] = []


class Layer(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    name: str = Field(default_factory=lambda: 'New Layer ' + next(layer_id))
    thickness: str = ''
    SLD: str = ''
    roughness: str = ''
    hydration: str = ''
    hydrate_with: Hydration = Hydration.BulkOut


class Parameter(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

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


class Resolution(BaseModel, validate_assignment=True):
    model_config = ConfigDict(extra='forbid')

    param_list: ClassVar[List[str]] = []
    name: str = Field(default_factory=lambda: 'New Resolution ' + next(resolution_id))
    type: Types = Types.Constant
    value_1: str = ''
    value_2: str = ''
    value_3: str = ''
    value_4: str = ''
    value_5: str = ''

    @field_validator('value_1', 'value_2', 'value_3', 'value_4', 'value_5')
    @classmethod
    def check_on_list(cls, value) -> str:
        if value not in Resolution.param_list:
            raise ValueError('Not on the list!')
        return value

