"""The Enum values used in the parameters of various ratapi classes and functions."""

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


class RATEnum(StrEnum):
    """A subclass of StrEnum with some adjustments for variable spellings."""

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
    """The available options for procedures."""

    Calculate = "calculate"
    """Run an Abelès reflectivity calculation and calculate chi-squared to the data."""

    Simplex = "simplex"
    """Run a Nelder-Mead simplex optimisation over the fit parameters."""

    DE = "de"
    """Run a Differential Evolution optimisation over the fit parameters."""

    NS = "ns"
    """Run Bayesian Nested Sampling over the fit parameters."""

    DREAM = "dream"
    """Run the Bayesian DREAM algorithm over the fit parameters."""


class Parallel(RATEnum):
    """The available options for parallelisation."""

    Single = "single"
    """Do not parallelise."""

    Points = "points"
    """Split all contrasts into groups of points, and assign a process to each group."""

    Contrasts = "contrasts"
    """Assign one process to each contrast."""


class Display(RATEnum):
    """The available options for terminal output."""

    Off = "off"
    Iter = "iter"
    Notify = "notify"
    Final = "final"


class Strategies(RATEnum):
    """The available strategies for generating base vectors in differential evolution."""

    Random = "random"
    """The base vector is random."""

    LocalToBest = "local to best"
    """The base vector is a combination of one randomly-selected local solution
    and the best solution of the previous iteration."""

    BestWithJitter = "best jitter"
    """The base vector is the best solution of the previous iteration, with a small random perturbation applied."""

    RandomWithPerVectorDither = "vector dither"
    """The base vector is random, with a random scaling factor applied to each mutant.
    This scaling factor is different for each mutant."""

    RandomWithPerGenerationDither = "generation dither"
    """The base vector is random, with a random scaling factor applied to each mutant.
    This scaling factor is the same for every mutant, and randomised every generation."""

    RandomEitherOrAlgorithm = "either or"
    """The base vector is randomly chosen from either a pure random mutation,
    or a pure recombination of parent parameter values."""

    @classmethod
    def _missing_(cls, value: int | str):
        # legacy compatibility with strategies being 1-indexed ints under the hood
        if isinstance(value, int):
            if value < 1 or value > 6:
                raise IndexError("Strategy indices must be between 1 and 6.")
            return list(cls)[value - 1]
        return super()._missing_(value)

    def __int__(self):
        """Convert the DE strategy to its hardcoded index in the internal code.

        RAT core expects strategy to be an integer, as this is how it is given to
        the internal DE algorithm.
        """
        return list(Strategies).index(self) + 1


class BoundHandling(RATEnum):
    """The available options for bound handling in DREAM."""

    Off = "off"
    """Allow points to be sampled out of the parameter bounds."""

    Reflect = "reflect"
    """If a step passes a boundary, reflect it back across the boundary."""

    Bound = "bound"
    """If a step passes a boundary, set it equal to the nearest point on the boundary."""

    Fold = "fold"
    """Treat the boundary as periodic and ‘wrap the step around’ to the other side of the space."""


# Models
class TypeOptions(RATEnum):
    """The types of signal (``Background`` and ``Resolution``)."""

    Constant = "constant"
    """A uniform constant signal given by a parameter."""

    Data = "data"
    """A signal for each q-value given by a dataset."""

    Function = "function"
    """A signal defined by a custom function."""


class BackgroundActions(RATEnum):
    """Ways that the background can be applied to the model."""

    Add = "add"
    Subtract = "subtract"


class Languages(RATEnum):
    """Language options for custom files."""

    Cpp = "cpp"
    Python = "python"
    Matlab = "matlab"


class Hydration(RATEnum):
    """Options for the 'hydrate with' parameter of a Layer."""

    BulkIn = "bulk in"
    BulkOut = "bulk out"


class Priors(RATEnum):
    """Prior distributions for parameters."""

    Uniform = "uniform"
    """A uniform distribution over the parameter range."""

    Gaussian = "gaussian"
    """A Gaussian distribution centred on the parameter value,
    with shape defined by ``mu`` and ``sigma`` for the parameter."""

    Jeffreys = "jeffreys"
    """A Jeffreys' prior distribution over the parameter range."""


# Project
class Calculations(RATEnum):
    """Types of calculations that can be performed by RAT."""

    Normal = "normal"
    Domains = "domains"


class LayerModels(RATEnum):
    """Types of layer model supported by RAT."""

    CustomLayers = "custom layers"
    """The layer model is given by a custom function."""

    CustomXY = "custom xy"
    """The continuous SLD of the layer model is given by a custom function."""

    StandardLayers = "standard layers"
    """The layer model is given by a list of ``Layer``s or ``DomainContrast``s."""


class Geometries(RATEnum):
    """Where the substrate roughness is placed."""

    AirSubstrate = "air/substrate"
    """The substrate roughness is placed at the end of the stack."""

    SubstrateLiquid = "substrate/liquid"
    """The substrate roughness is placed at the beginning of the stack."""
