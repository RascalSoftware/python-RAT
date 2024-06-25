from enum import Enum

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


# Controls
class Parallel(StrEnum):
    """Defines the available options for parallelization"""
    Single = 'single'
    Points = 'points'
    Contrasts = 'contrasts'


class Procedures(StrEnum):
    """Defines the available options for procedures"""
    Calculate = 'calculate'
    Simplex = 'simplex'
    DE = 'de'
    NS = 'ns'
    Dream = 'dream'


class Display(StrEnum):
    """Defines the available options for display"""
    Off = 'off'
    Iter = 'iter'
    Notify = 'notify'
    Final = 'final'


class BoundHandling(StrEnum):
    """Defines the available options for bound handling"""
    Off = 'off'
    Reflect = 'reflect'
    Bound = 'bound'
    Fold = 'fold'


class Strategies(Enum):
    """Defines the available options for strategies"""
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
    Cpp = 'cpp'
    Python = 'python'
    Matlab = 'matlab'


class Priors(StrEnum):
    Uniform = 'uniform'
    Gaussian = 'gaussian'


class TypeOptions(StrEnum):
    Constant = 'constant'
    Data = 'data'
    Function = 'function'


class BackgroundActions(StrEnum):
    Add = 'add'
    Subtract = 'subtract'


# Project
class Calculations(StrEnum):
    NonPolarised = 'non polarised'
    Domains = 'domains'


class Geometries(StrEnum):
    AirSubstrate = 'air/substrate'
    SubstrateLiquid = 'substrate/liquid'


class LayerModels(StrEnum):
    CustomLayers = 'custom layers'
    CustomXY = 'custom xy'
    StandardLayers = 'standard layers'
