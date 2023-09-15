from enum import Enum


class ParallelOptions(Enum):
    """Defines the avaliable options for parallelization"""
    Single = 'single'
    Points = 'points'
    Contrasts = 'contrasts'
    All = 'all'


class Procedures(Enum):
    """Defines the avaliable options for procedures"""
    Calculate = 'calculate'
    Simplex = 'simplex'
    DE = 'de'
    NS = 'ns'
    Dream = 'dream'


class DisplayOptions(Enum):
    """Defines the avaliable options for display"""
    Off = 'off'
    Iter = 'iter'
    Notify = 'notify'
    Final = 'final'


class BoundHandlingOptions(Enum):
    """Defines the avaliable options for bound handling"""
    No = 'no'
    Reflect = 'reflect'
    Bound = 'bound'
    Fold = 'fold'


class StrategyOptions(Enum):
    """Defines the avaliable options for strategies"""
    Random = 1
    LocalToBest = 2
    BestWithJitter = 3
    RandomWithPerVectorDither = 4
    RandomWithPerGenerationDither = 5
    RandomEitherOrAlgorithm = 6