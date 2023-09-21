from enum import Enum
try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


class ParallelOptions(StrEnum):
    """Defines the avaliable options for parallelization"""
    Single = 'single'
    Points = 'points'
    Contrasts = 'contrasts'
    All = 'all'


class Procedures(StrEnum):
    """Defines the avaliable options for procedures"""
    Calculate = 'calculate'
    Simplex = 'simplex'
    DE = 'de'
    NS = 'ns'
    Dream = 'dream'


class DisplayOptions(StrEnum):
    """Defines the avaliable options for display"""
    Off = 'off'
    Iter = 'iter'
    Notify = 'notify'
    Final = 'final'


class BoundHandlingOptions(StrEnum):
    """Defines the avaliable options for bound handling"""
    Off = 'off'
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