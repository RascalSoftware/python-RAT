"""Defines dataclasses used for the inputs to the compiled RAT code."""

from dataclasses import dataclass, field
from RAT.utils.enums import Parallel, Procedures, Display, BoundHandling, Strategies, Calc, Geometries, Models


@dataclass
class Checks:
    """Fit parameters for all parameter sets defined in the project."""
    fitParam: list[int] = field(default_factory=list)
    fitBackgroundParam: list[int] = field(default_factory=list)
    fitQzshift: list[int] = field(default_factory=list)
    fitScalefactor: list[int] = field(default_factory=list)
    fitBulkIn: list[int] = field(default_factory=list)
    fitBulkOut: list[int] = field(default_factory=list)
    fitResolutionParam: list[int] = field(default_factory=list)
    fitDomainRatio: list[int] = field(default_factory=list)


@dataclass
class Limits:
    """Parameters limits for all parameter sets defined in the project."""
    param: list[float] = field(default_factory=list)
    backgroundParam: list[float] = field(default_factory=list)
    qzshift: list[float] = field(default_factory=list)
    scalefactor: list[float] = field(default_factory=list)
    bulkIn: list[float] = field(default_factory=list)
    bulkOut: list[float] = field(default_factory=list)
    resolutionParam: list[float] = field(default_factory=list)
    domainRatio: list[float] = field(default_factory=list)


@dataclass
class Priors:
    """Priors parameters for all parameter sets defined in the project."""
    param: list = field(default_factory=list)
    backgroundParam: list = field(default_factory=list)
    qzshift: list = field(default_factory=list)
    scalefactor: list = field(default_factory=list)
    bulkIn: list = field(default_factory=list)
    bulkOut: list = field(default_factory=list)
    resolutionParam: list = field(default_factory=list)
    domainRatio: list = field(default_factory=list)
    priorNames: list[str] = field(default_factory=list)
    priorValues: list = field(default_factory=list)


@dataclass(frozen=True)
class Controls:
    """The full set of controls parameters required for RATMain."""
    # All Procedures
    procedure: Procedures = Procedures.Calculate
    parallel: Parallel = Parallel.Single
    calcSldDuringFit: bool = False
    resamPars: list[float] = field(default_factory=list[0.9, 50.0])
    display: Display = Display.Iter
    # Simplex
    tolX: float = 1.0e-6
    tolFun: float = 1.0e-6
    maxFunEvals: int = 10000
    maxIter: int = 1000
    updateFreq: int = -1
    updatePlotFreq: int = -1
    # DE
    populationSize: int = 20
    fWeight: float = 0.5
    crossoverProbability: float = 0.8
    strategy: Strategies = Strategies.RandomWithPerVectorDither
    targetValue: float = 1.0
    numGenerations: int = 500
    # NS
    Nlive: int = 150
    Nmcmc: float = 0.0
    propScale: float = 0.1
    nsTolerance: float = 0.1
    # Dream
    nSamples: int = 50000
    nChains: int = 10
    jumpProbability: float = 0.5
    pUnitGamma: float = 0.2
    boundHandling: BoundHandling = BoundHandling.Fold
    checks: Checks = Checks()


@dataclass(frozen=True)
class Problem:
    """The full set of problemDef parameters required for RATMain."""
    # General
    TF: str = Calc.NonPolarised.value
    modelType: str = Models.StandardLayers.value
    geometry: str = Geometries.AirSubstrate.value
    useImaginary: bool = False
    # Parameter Values
    params: list[float] = field(default_factory=list)
    bulkIn: list[float] = field(default_factory=list)
    bulkOut: list[float] = field(default_factory=list)
    qzshifts: list[float] = field(default_factory=list)
    scalefactors: list[float] = field(default_factory=list)
    domainRatio: list[int] = field(default_factory=list)
    backgroundParams: list[float] = field(default_factory=list)
    resolutionParams: list[float] = field(default_factory=list)
    # Contrast Parameters
    contrastBulkIns: list[int] = field(default_factory=list)
    contrastBulkOuts: list[int] = field(default_factory=list)
    contrastQzshifts: list[int] = field(default_factory=list)
    contrastScalefactors: list[int] = field(default_factory=list)
    contrastDomainRatios: list[int] = field(default_factory=list)
    contrastBackgrounds: list[int] = field(default_factory=list)
    contrastBackgroundsType: list[int] = field(default_factory=list)
    contrastResolutions: list[int] = field(default_factory=list)
    contrastCustomFiles: list[int] = field(default_factory=list)
    resample: list[int] = field(default_factory=list)
    dataPresent: list[int] = field(default_factory=list)
    oilChiDataPresent: list[int] = field(default_factory=list)
    numberOfContrasts: int = 0
    # Other
    numberOfLayers: int = 0
    numberOfDomainContrasts: int = 0
    fitParams: list = field(default_factory=list)
    otherParams: list = field(default_factory=list)
    fitLimits: list = field(default_factory=list)
    otherLimits: list = field(default_factory=list)
