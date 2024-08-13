try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


class RATEnum(StrEnum):
    @classmethod
    def _missing_(cls, value: str):
        value = value.lower()

        # Replace common alternative spellings
        value = value.replace("-", " ").replace("_", " ").replace("++", "pp").replace("polarized", "polarised")

        for member in cls:
            if member.value.lower() == value:
                return member
        return None


# Controls
class Procedures(RATEnum):
    """Defines the available options for procedures"""

    Calculate = "calculate"
    Simplex = "simplex"
    DE = "de"
    NS = "ns"
    DREAM = "dream"


class Parallel(RATEnum):
    """Defines the available options for parallelization"""

    Single = "single"
    Points = "points"
    Contrasts = "contrasts"


class Display(RATEnum):
    """Defines the available options for display"""

    Off = "off"
    Iter = "iter"
    Notify = "notify"
    Final = "final"


class Strategies(RATEnum):
    """Defines the available options for differential evolution strategies"""

    Random = "random"
    LocalToBest = "local to best"
    BestWithJitter = "best jitter"
    RandomWithPerVectorDither = "vector dither"
    RandomWithPerGenerationDither = "generation dither"
    RandomEitherOrAlgorithm = "either or"

    def __int__(self):
        # as RAT core expects strategy as an integer
        return [s for s in Strategies].index(self) + 1


class BoundHandling(RATEnum):
    """Defines the available options for bound handling"""

    Off = "off"
    Reflect = "reflect"
    Bound = "bound"
    Fold = "fold"


# Models
class TypeOptions(RATEnum):
    Constant = "constant"
    Data = "data"
    Function = "function"


class BackgroundActions(RATEnum):
    Add = "add"
    Subtract = "subtract"


class Languages(RATEnum):
    Cpp = "cpp"
    Python = "python"
    Matlab = "matlab"


class Hydration(RATEnum):
    None_ = "none"
    BulkIn = "bulk in"
    BulkOut = "bulk out"


class Priors(RATEnum):
    Uniform = "uniform"
    Gaussian = "gaussian"


# Project
class Calculations(RATEnum):
    NonPolarised = "non polarised"
    Domains = "domains"


class LayerModels(RATEnum):
    CustomLayers = "custom layers"
    CustomXY = "custom xy"
    StandardLayers = "standard layers"


class Geometries(RATEnum):
    AirSubstrate = "air/substrate"
    SubstrateLiquid = "substrate/liquid"
