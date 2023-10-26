import prettytable
from pydantic import BaseModel, Field, field_validator, ValidationError
from typing import Literal, Union

from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class Calculate(BaseModel, validate_assignment=True, extra='forbid'):
    """Defines the class for the calculate procedure, which includes the properties used in all five procedures."""
    procedure: Literal[Procedures.Calculate] = Procedures.Calculate
    parallel: ParallelOptions = ParallelOptions.Single
    calcSldDuringFit: bool = False
    resamPars: list[float] = Field([0.9, 50], min_length=2, max_length=2)
    display: DisplayOptions = DisplayOptions.Iter

    @field_validator("resamPars")
    @classmethod
    def check_resamPars(cls, resamPars):
        if not 0 < resamPars[0] < 1:
            raise ValueError('resamPars[0] must be between 0 and 1')
        if resamPars[1] < 0:
            raise ValueError('resamPars[1] must be greater than or equal to 0')
        return resamPars

    def __repr__(self) -> str:
        table = prettytable.PrettyTable()
        table.field_names = ['Property', 'Value']
        table.add_rows([[k, v] for k, v in self.__dict__.items()])
        return table.get_string()


class Simplex(Calculate, validate_assignment=True, extra='forbid'):
    """Defines the additional fields for the simplex procedure."""
    procedure: Literal[Procedures.Simplex] = Procedures.Simplex
    tolX: float = Field(1.0e-6, gt=0.0)
    tolFun: float = Field(1.0e-6, gt=0.0)
    maxFunEvals: int = Field(10000, gt=0)
    maxIter: int = Field(1000, gt=0)
    updateFreq: int = -1
    updatePlotFreq: int = -1


class DE(Calculate, validate_assignment=True, extra='forbid'):
    """Defines the additional fields for the Differential Evolution procedure."""
    procedure: Literal[Procedures.DE] = Procedures.DE
    populationSize: int = Field(20, ge=1)
    fWeight: float = 0.5
    crossoverProbability: float = Field(0.8, gt=0.0, lt=1.0)
    strategy: StrategyOptions = StrategyOptions.RandomWithPerVectorDither
    targetValue: float = Field(1.0, ge=1.0)
    numGenerations: int = Field(500, ge=1)


class NS(Calculate, validate_assignment=True, extra='forbid'):
    """Defines the additional fields for the Nested Sampler procedure."""
    procedure: Literal[Procedures.NS] = Procedures.NS
    Nlive: int = Field(150, ge=1)
    Nmcmc: float = Field(0.0, ge=0.0)
    propScale: float = Field(0.1, gt=0.0, lt=1.0)
    nsTolerance: float = Field(0.1, ge=0.0)


class Dream(Calculate, validate_assignment=True, extra='forbid'):
    """Defines the additional fields for the Dream procedure."""
    procedure: Literal[Procedures.Dream] = Procedures.Dream
    nSamples: int = Field(50000, ge=0)
    nChains: int = Field(10, gt=0)
    jumpProb: float = Field(0.5, gt=0.0, lt=1.0)
    pUnitGamma: float = Field(0.2, gt=0.0, lt=1.0)
    boundHandling: BoundHandlingOptions = BoundHandlingOptions.Fold


def set_controls(procedure: Procedures = Procedures.Calculate, **properties)\
        -> Union[Calculate, Simplex, DE, NS, Dream]:
    """Returns the appropriate controls model given the specified procedure."""
    controls = {
        Procedures.Calculate: Calculate,
        Procedures.Simplex: Simplex,
        Procedures.DE: DE,
        Procedures.NS: NS,
        Procedures.Dream: Dream
    }

    try:
        model = controls[procedure](**properties)
    except KeyError:
        members = list(Procedures.__members__.values())
        allowed_values = ', '.join([repr(member.value) for member in members[:-1]]) + f' or {members[-1].value!r}'
        raise ValueError(f'The controls procedure must be one of: {allowed_values}') from None
    except ValidationError:
        raise

    return model
