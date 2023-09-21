import tabulate
from typing import Union
from pydantic import BaseModel, Field, field_validator
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class BaseProcedure(BaseModel, validate_assignment=True, extra='forbid'):
    """
    Defines the base class with properties used in all five procedures.
    """
    parallel: ParallelOptions = ParallelOptions.Single
    calcSldDuringFit: bool = False
    resamPars: list[Union[int, float]] = Field([0.9, 50], min_length=2, max_length=2)
    display: DisplayOptions = DisplayOptions.Iter

    @field_validator("resamPars")
    def check_resamPars(cls, resamPars):
        if not 0 < resamPars[0] < 1:
            raise ValueError('resamPars[0] must be between 0 and 1')
        if not 0 <= resamPars[1]:
            raise ValueError('resamPars[1] must be greater than 0')
        return resamPars


class Calculate(BaseProcedure, validate_assignment=True, extra='forbid'):
    """
    Defines the class for the calculate procedure
    """
    procedure: Procedures = Field(Procedures.Calculate, frozen=True)

        
class Simplex(BaseProcedure, validate_assignment=True, extra='forbid'):
    """
    Defines the class for the simplex procedure
    """
    procedure: Procedures = Field(Procedures.Simplex, frozen=True)
    tolX: float = Field(1e-6, gt=0)
    tolFun: float = Field(1e-6, gt=0)
    maxFunEvals: int = Field(10000, gt=0)
    maxIter: int = Field(1000, gt=0)
    updateFreq: int = -1
    updatePlotFreq: int = -1

class DE(BaseProcedure, validate_assignment=True, extra='forbid'):
    """
    Defines the class for the Differential Evolution procedure
    """
    procedure: Procedures = Field(Procedures.DE, frozen=True)
    populationSize: int = Field(20, ge=1)
    fWeight: float = 0.5
    crossoverProbability: float = Field(0.8, gt=0, lt=1)
    strategy: StrategyOptions = StrategyOptions.RandomWithPerVectorDither
    targetValue: Union[int, float] = Field(1.0, ge=1)
    numGenerations: int = Field(500, ge=1)

class NS(BaseProcedure, validate_assignment=True, extra='forbid'):
    """
    Defines the class for the  Nested Sampler procedure
    """
    procedure: Procedures = Field(Procedures.NS, frozen=True)
    Nlive: int = Field(150, ge=1)
    Nmcmc: Union[float, int] = Field(0.0, ge=0)
    propScale: float = Field(0.1, gt=0, lt=1)
    nsTolerance: Union[float, int] = Field(0.1, ge=0)

class Dream(BaseProcedure, validate_assignment=True, extra='forbid'):
    """
    Defines the class for the Dream procedure
    """
    procedure: Procedures = Field(Procedures.Dream, frozen=True)
    nSamples: int = Field(50000, ge=0)
    nChains: int = Field(10, gt=0)
    jumpProb: float = Field(0.5, gt=0, lt=1)
    pUnitGamma:float = Field(0.2, gt=0, lt=1)
    boundHandling: BoundHandlingOptions = BoundHandlingOptions.Fold


class ControlsClass:

    def __init__(self,
                 procedure: Procedures = Procedures.Calculate,
                 **properties) -> None:
        
        self._procedure = procedure
        self._validate_properties(**properties)

        if self._procedure == Procedures.Calculate:
            self._controls = Calculate(**properties)
                 
        elif self._procedure == Procedures.Simplex:
            self._controls = Simplex(**properties)
        
        elif self._procedure == Procedures.DE:
            self._controls = DE(**properties)
        
        elif self._procedure == Procedures.NS:
            self._controls = NS(**properties)
        
        elif self._procedure == Procedures.Dream:
            self._controls = Dream(**properties)
    
    @property
    def controls(self) -> Union[Calculate, Simplex, DE, NS, Dream]:
        """
        Gets the controls.

        Returns
        -------
        controls : Union[Calculate,
                         Simplex,
                         DE,
                         NS,
                         Dream]
            The class with control properties.
        """
        return self._controls

    def _validate_properties(self, **properties) -> None:
        """
        Validates the inputs for the procedure.

        Parameters
        ----------
        properties : dict[str, Any]
            The properties of the procedure.

        Raises
        ------
        ValueError
            Raised if properties are not validated.
        """
        property_names = {Procedures.Calculate: {'parallel',
                                                 'calcSLdDuringFit',
                                                 'resamPars',
                                                 'display'},
                          Procedures.Simplex: {'parallel',
                                               'calcSLdDuringFit',
                                               'resamPars',
                                               'display',
                                               'tolX',
                                               'tolFun',
                                               'maxFunEvals',
                                               'maxIter',
                                               'updateFreq',
                                               'updatePlotFreq'},
                          Procedures.DE: {'parallel',
                                          'calcSLdDuringFit',
                                          'resamPars',
                                          'display',
                                          'populationSize',
                                          'fWeight',
                                          'crossoverProbability',
                                          'strategy',
                                          'targetValue',
                                          'numGenerations'},
                          Procedures.NS: {'parallel',
                                          'calcSLdDuringFit',
                                          'resamPars',
                                          'display',
                                          'Nlive',
                                          'Nmcmc',
                                          'propScale',
                                          'nsTolerance'},
                          Procedures.Dream: {'parallel',
                                             'calcSLdDuringFit',
                                             'resamPars',
                                             'display',
                                             'nSamples',
                                             'nChains',
                                             'jumpProb',
                                             'pUnitGamma',
                                             'boundHandling'}}
        expected_properties = property_names[self._procedure]
        input_properties = properties.keys()
        if not (expected_properties | input_properties == expected_properties):
            raise ValueError((f"Properties that can be set for {self._procedure} are "
                              f"{', '.join(sorted(expected_properties))}"))
