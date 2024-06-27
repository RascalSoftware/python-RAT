from dataclasses import dataclass, field
from typing import Literal, Union

import prettytable
from pydantic import BaseModel, Field, ValidationError, field_validator

from RATpy.utils.custom_errors import custom_pydantic_validation_error
from RATpy.utils.enums import BoundHandling, Display, Parallel, Procedures, Strategies


@dataclass(frozen=True)
class Controls:
    """The full set of controls parameters required for the compiled RAT code."""

    # All Procedures
    procedure: Procedures = Procedures.Calculate
    parallel: Parallel = Parallel.Single
    calcSldDuringFit: bool = False
    resampleParams: list[float] = field(default_factory=list[0.9, 50.0])
    display: Display = Display.Iter
    # Simplex
    xTolerance: float = 1.0e-6
    funcTolerance: float = 1.0e-6
    maxFuncEvals: int = 10000
    maxIterations: int = 1000
    updateFreq: int = -1
    updatePlotFreq: int = 1
    # DE
    populationSize: int = 20
    fWeight: float = 0.5
    crossoverProbability: float = 0.8
    strategy: Strategies = Strategies.RandomWithPerVectorDither.value
    targetValue: float = 1.0
    numGenerations: int = 500
    # NS
    nLive: int = 150
    nMCMC: float = 0.0
    propScale: float = 0.1
    nsTolerance: float = 0.1
    # Dream
    nSamples: int = 2000
    nChains: int = 10
    jumpProbability: float = 0.5
    pUnitGamma: float = 0.2
    boundHandling: BoundHandling = BoundHandling.Reflect
    adaptPCR: bool = True


class Calculate(BaseModel, validate_assignment=True, extra="forbid"):
    """Defines the class for the calculate procedure, which includes the properties used in all five procedures."""

    procedure: Literal[Procedures.Calculate] = Procedures.Calculate
    parallel: Parallel = Parallel.Single
    calcSldDuringFit: bool = False
    resampleParams: list[float] = Field([0.9, 50], min_length=2, max_length=2)
    display: Display = Display.Iter

    @field_validator("resampleParams")
    @classmethod
    def check_resample_params(cls, resampleParams):
        if not 0 < resampleParams[0] < 1:
            raise ValueError("resampleParams[0] must be between 0 and 1")
        if resampleParams[1] < 0:
            raise ValueError("resampleParams[1] must be greater than or equal to 0")
        return resampleParams

    def __repr__(self) -> str:
        table = prettytable.PrettyTable()
        table.field_names = ["Property", "Value"]
        table.add_rows([[k, v] for k, v in self.__dict__.items()])
        return table.get_string()


class Simplex(Calculate):
    """Defines the additional fields for the simplex procedure."""

    procedure: Literal[Procedures.Simplex] = Procedures.Simplex
    xTolerance: float = Field(1.0e-6, gt=0.0)
    funcTolerance: float = Field(1.0e-6, gt=0.0)
    maxFuncEvals: int = Field(10000, gt=0)
    maxIterations: int = Field(1000, gt=0)
    updateFreq: int = -1
    updatePlotFreq: int = 1


class DE(Calculate):
    """Defines the additional fields for the Differential Evolution procedure."""

    procedure: Literal[Procedures.DE] = Procedures.DE
    populationSize: int = Field(20, ge=1)
    fWeight: float = 0.5
    crossoverProbability: float = Field(0.8, gt=0.0, lt=1.0)
    strategy: Strategies = Strategies.RandomWithPerVectorDither
    targetValue: float = Field(1.0, ge=1.0)
    numGenerations: int = Field(500, ge=1)


class NS(Calculate):
    """Defines the additional fields for the Nested Sampler procedure."""

    procedure: Literal[Procedures.NS] = Procedures.NS
    nLive: int = Field(150, ge=1)
    nMCMC: float = Field(0.0, ge=0.0)
    propScale: float = Field(0.1, gt=0.0, lt=1.0)
    nsTolerance: float = Field(0.1, ge=0.0)


class Dream(Calculate):
    """Defines the additional fields for the Dream procedure."""

    procedure: Literal[Procedures.Dream] = Procedures.Dream
    nSamples: int = Field(2000, ge=0)
    nChains: int = Field(10, gt=0)
    jumpProbability: float = Field(0.5, gt=0.0, lt=1.0)
    pUnitGamma: float = Field(0.2, gt=0.0, lt=1.0)
    boundHandling: BoundHandling = BoundHandling.Reflect
    adaptPCR: bool = True


def set_controls(
    procedure: Procedures = Procedures.Calculate,
    **properties,
) -> Union[Calculate, Simplex, DE, NS, Dream]:
    """Returns the appropriate controls model given the specified procedure."""
    controls = {
        Procedures.Calculate: Calculate,
        Procedures.Simplex: Simplex,
        Procedures.DE: DE,
        Procedures.NS: NS,
        Procedures.Dream: Dream,
    }

    try:
        model = controls[procedure](**properties)
    except KeyError:
        members = list(Procedures.__members__.values())
        allowed_values = f'{", ".join([repr(member.value) for member in members[:-1]])} or {members[-1].value!r}'
        raise ValueError(f"The controls procedure must be one of: {allowed_values}") from None
    except ValidationError as exc:
        custom_error_msgs = {
            "extra_forbidden": f'Extra inputs are not permitted. The fields for the {procedure}'
            f' controls procedure are:\n    '
            f'{", ".join(controls[procedure].model_fields.keys())}\n',
        }
        custom_error_list = custom_pydantic_validation_error(exc.errors(), custom_error_msgs)
        raise ValidationError.from_exception_data(exc.title, custom_error_list) from None

    return model
