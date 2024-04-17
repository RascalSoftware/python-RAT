"""Defines dataclasses used for the inputs to the compiled RAT code."""

from dataclasses import dataclass, field

from RAT.utils.enums import Parallel, Procedures, Display, BoundHandling, Strategies


@dataclass(frozen=True)
class Controls:
    """The full set of controls parameters required for RATMain."""
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
    nSamples: int = 50000
    nChains: int = 10
    jumpProbability: float = 0.5
    pUnitGamma: float = 0.2
    boundHandling: BoundHandling = BoundHandling.Fold
    adaptPCR: bool = False
