from enum import Enum
try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


# Controls
class ParallelOptions(StrEnum):
    """Defines the available options for parallelization"""
    Single = 'single'
    Points = 'points'
    Contrasts = 'contrasts'
    All = 'all'


class Procedures(StrEnum):
    """Defines the available options for procedures"""
    Calculate = 'calculate'
    Simplex = 'simplex'
    DE = 'de'
    NS = 'ns'
    Dream = 'dream'


class DisplayOptions(StrEnum):
    """Defines the available options for display"""
    Off = 'off'
    Iter = 'iter'
    Notify = 'notify'
    Final = 'final'


class BoundHandlingOptions(StrEnum):
    """Defines the available options for bound handling"""
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


# Models
class Hydration(StrEnum):
    None_ = 'none'
    BulkIn = 'bulk in'
    BulkOut = 'bulk out'
    Oil = 'oil'


class Languages(StrEnum):
    Python = 'python'
    Matlab = 'matlab'


class Priors(StrEnum):
    Uniform = 'uniform'
    Gaussian = 'gaussian'


class Types(StrEnum):
    Constant = 'constant'
    Data = 'data'
    Function = 'function'


# Project
class CalcTypes(StrEnum):
    NonPolarised = 'non polarised'
    Domains = 'domains'


class Geometries(StrEnum):
    AirSubstrate = 'air/substrate'
    SubstrateLiquid = 'substrate/liquid'


class ModelTypes(StrEnum):
    CustomLayers = 'custom layers'
    CustomXY = 'custom xy'
    StandardLayers = 'standard layers'
