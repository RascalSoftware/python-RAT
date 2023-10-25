import prettytable
from pydantic import BaseModel, Field, field_validator
from typing import Union

from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class BaseControls(BaseModel, validate_assignment=True, extra='forbid'):
    """Defines the base class with properties used in all five procedures."""
    procedure: Procedures = Procedures.Calculate
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


class Calculate(BaseControls, validate_assignment=True, extra='forbid'):
    """Defines the class for the calculate procedure."""
    procedure: Procedures = Field(Procedures.Calculate, frozen=True)


class Simplex(BaseControls, validate_assignment=True, extra='forbid'):
    """Defines the class for the simplex procedure."""
    procedure: Procedures = Field(Procedures.Simplex, frozen=True)
    tolX: float = Field(1.0e-6, gt=0.0)
    tolFun: float = Field(1.0e-6, gt=0.0)
    maxFunEvals: int = Field(10000, gt=0)
    maxIter: int = Field(1000, gt=0)
    updateFreq: int = -1
    updatePlotFreq: int = -1


class DE(BaseControls, validate_assignment=True, extra='forbid'):
    """Defines the class for the Differential Evolution procedure."""
    procedure: Procedures = Field(Procedures.DE, frozen=True)
    populationSize: int = Field(20, ge=1)
    fWeight: float = 0.5
    crossoverProbability: float = Field(0.8, gt=0.0, lt=1.0)
    strategy: StrategyOptions = StrategyOptions.RandomWithPerVectorDither
    targetValue: float = Field(1.0, ge=1.0)
    numGenerations: int = Field(500, ge=1)


class NS(BaseControls, validate_assignment=True, extra='forbid'):
    """Defines the class for the  Nested Sampler procedure."""
    procedure: Procedures = Field(Procedures.NS, frozen=True)
    Nlive: int = Field(150, ge=1)
    Nmcmc: float = Field(0.0, ge=0.0)
    propScale: float = Field(0.1, gt=0.0, lt=1.0)
    nsTolerance: float = Field(0.1, ge=0.0)


class Dream(BaseControls, validate_assignment=True, extra='forbid'):
    """Defines the class for the Dream procedure."""
    procedure: Procedures = Field(Procedures.Dream, frozen=True)
    nSamples: int = Field(50000, ge=0)
    nChains: int = Field(10, gt=0)
    jumpProb: float = Field(0.5, gt=0.0, lt=1.0)
    pUnitGamma: float = Field(0.2, gt=0.0, lt=1.0)
    boundHandling: BoundHandlingOptions = BoundHandlingOptions.Fold


def set_controls(procedure: Procedures = Procedures.Calculate, **properties)\
        -> Union[Calculate, Simplex, DE, NS, Dream]:

    properties.update(procedure=procedure)
    controls = {
        Procedures.Calculate: Calculate(**properties),
        Procedures.Simplex: Simplex(**properties),
        Procedures.DE: DE(**properties),
        Procedures.NS: NS(**properties),
        Procedures.Dream: Dream(**properties)
    }

    return controls[procedure]
