import contextlib
import os
import tempfile
import warnings
from pathlib import Path

import prettytable
from pydantic import (
    BaseModel,
    Field,
    ValidationError,
    ValidatorFunctionWrapHandler,
    model_serializer,
    model_validator,
)

from RATapi.utils.custom_errors import custom_pydantic_validation_error
from RATapi.utils.enums import BoundHandling, Display, Parallel, Procedures, Strategies

common_fields = ["procedure", "parallel", "calcSldDuringFit", "resampleMinAngle", "resampleNPoints", "display"]
update_fields = ["updateFreq", "updatePlotFreq"]
fields = {
    "calculate": common_fields,
    "simplex": [*common_fields, "xTolerance", "funcTolerance", "maxFuncEvals", "maxIterations", *update_fields],
    "de": [
        *common_fields,
        "populationSize",
        "fWeight",
        "crossoverProbability",
        "strategy",
        "targetValue",
        "numGenerations",
        *update_fields,
    ],
    "ns": [*common_fields, "nLive", "nMCMC", "propScale", "nsTolerance"],
    "dream": [*common_fields, "nSamples", "nChains", "jumpProbability", "pUnitGamma", "boundHandling", "adaptPCR"],
}


class Controls(BaseModel, validate_assignment=True, extra="forbid", use_attribute_docstrings=True):
    """The full set of controls parameters for all five procedures that are required for the compiled RAT code."""

    # All Procedures
    procedure: Procedures = Procedures.Calculate
    """Which procedure RAT should execute. Can be 'calculate', 'simplex', 'de', 'ns', or 'dream'."""

    parallel: Parallel = Parallel.Single
    """How the calculation should be parallelised. Can be 'single', 'contrasts' or 'points'."""

    calcSldDuringFit: bool = False
    """Whether SLD will be calculated during fit (for live plotting etc.)"""

    resampleMinAngle: float = Field(0.9, le=1, gt=0)
    """The upper threshold on the angle between three sampled points for resampling, in units of radians over pi."""

    resampleNPoints: int = Field(50, gt=0)
    """The number of initial points to use for resampling."""

    display: Display = Display.Iter
    """How much RAT should print to the terminal. Can be 'off', 'iter', 'notify', or 'final'."""

    # Simplex
    xTolerance: float = Field(1.0e-6, gt=0.0)
    """[SIMPLEX] The termination tolerance for step size."""

    funcTolerance: float = Field(1.0e-6, gt=0.0)
    """[SIMPLEX] The termination tolerance for change in chi-squared."""

    maxFuncEvals: int = Field(10000, gt=0)
    """[SIMPLEX] The maximum number of function evaluations before the algorithm terminates."""

    maxIterations: int = Field(1000, gt=0)
    """[SIMPLEX] The maximum number of iterations before the algorithm terminates."""

    # Simplex and DE
    updateFreq: int = 1
    """[SIMPLEX, DE] Number of iterations between printing progress updates to the terminal."""

    updatePlotFreq: int = 20
    """[SIMPLEX, DE] Number of iterations between updates to live plots."""

    # DE
    populationSize: int = Field(20, ge=1)
    """[DE] The number of candidate solutions that exist at any time."""

    fWeight: float = Field(0.5, gt=0.0)
    """[DE] The step size for how different mutations are to their parents."""

    crossoverProbability: float = Field(0.8, gt=0.0, lt=1.0)
    """[DE] The probability of exchange of parameters between individuals at any iteration."""

    strategy: Strategies = Strategies.RandomWithPerVectorDither
    """[DE] The algorithm used to generate new candidates."""

    targetValue: float = Field(1.0, ge=1.0)
    """[DE] The value of chi-squared at which the algorithm will terminate."""

    numGenerations: int = Field(500, ge=1)
    """[DE] The maximum number of iterations before the algorithm terminates."""

    # NS
    nLive: int = Field(150, ge=1)
    """[NS] The number of points to sample."""

    nMCMC: int = Field(0, ge=0)
    """[NS] If non-zero, an MCMC process with ``nMCMC`` chains will be used instead of MultiNest."""

    propScale: float = Field(0.1, gt=0.0, lt=1.0)
    """[NS] A scaling factor for the ellipsoid generated by MultiNest."""

    nsTolerance: float = Field(0.1, ge=0.0)
    """[NS] The tolerance threshold for when the algorithm should terminate."""

    # Dream
    nSamples: int = Field(20000, ge=0)
    """[DREAM] The number of samples in the initial population for each chain."""

    nChains: int = Field(10, gt=0)
    """[DREAM] The number of Markov chains to use in the algorithm."""

    jumpProbability: float = Field(0.5, gt=0.0, lt=1.0)
    """[DREAM] The probability range for the size of jumps in sampling. Larger values mean more variable jumps."""

    pUnitGamma: float = Field(0.2, gt=0.0, lt=1.0)
    """[DREAM] The probability that the scaling-down factor of jumps will be ignored and a larger jump will be taken."""

    boundHandling: BoundHandling = BoundHandling.Reflect
    """[DREAM] How steps past the space boundaries should be handled. Can be 'off', 'reflect', 'bound', or 'fold'."""

    adaptPCR: bool = True
    """[DREAM] Whether the crossover probability for differential evolution should be adapted during the run."""

    # Private field for IPC file
    _IPCFilePath: str = ""

    @model_validator(mode="wrap")
    def warn_setting_incorrect_properties(self, handler: ValidatorFunctionWrapHandler) -> "Controls":
        """Raise a warning if the user sets fields that apply to other procedures."""
        model_input = self
        try:
            input_dict = model_input.__dict__
        except AttributeError:
            input_dict = model_input

        validated_self = None
        try:
            validated_self = handler(self)
        except ValidationError as exc:
            procedure = input_dict.get("procedure", Procedures.Calculate)
            custom_error_msgs = {
                "extra_forbidden": f'Extra inputs are not permitted. The fields for the "{procedure}"'
                f" controls procedure are:\n    "
                f"{', '.join(fields.get('procedure', []))}\n",
            }
            custom_error_list = custom_pydantic_validation_error(exc.errors(), custom_error_msgs)
            raise ValidationError.from_exception_data(exc.title, custom_error_list, hide_input=True) from None

        if isinstance(model_input, validated_self.__class__):
            # This is for changing fields in a defined model
            changed_fields = [key for key in input_dict if input_dict[key] != validated_self.__dict__[key]]
        elif isinstance(model_input, dict):
            # This is for a newly-defined model
            changed_fields = input_dict.keys()
        else:
            raise ValueError('The input to the "Controls" model is invalid.')

        new_procedure = validated_self.procedure
        allowed_fields = fields[new_procedure]
        for field in changed_fields:
            if field not in allowed_fields:
                incorrect_procedures = [key for (key, value) in fields.items() if field in value]
                warnings.warn(
                    f'\nThe current controls procedure is "{new_procedure}", but the property'
                    f' "{field}" applies instead to the {", ".join(incorrect_procedures)} procedure.\n\n'
                    f' The fields for the "{new_procedure}" controls procedure are:\n'
                    f"    {', '.join(fields[new_procedure])}\n",
                    stacklevel=2,
                )

        return validated_self

    @model_serializer
    def serialize(self):
        """Filter fields so only those applying to the chosen procedure are serialized."""
        return {model_field: getattr(self, model_field) for model_field in fields[self.procedure]}

    def __repr__(self) -> str:
        fields_repr = ", ".join(repr(v) if a is None else f"{a}={v!r}" for a, v in self.model_dump().items())
        return f"{self.__repr_name__()}({fields_repr})"

    def __str__(self) -> str:
        table = prettytable.PrettyTable()
        table.field_names = ["Property", "Value"]
        table.add_rows([[k, v] for k, v in self.model_dump().items()])
        return table.get_string()

    def initialise_IPC(self):
        """Setup the inter-process communication file."""
        IPC_obj, self._IPCFilePath = tempfile.mkstemp()
        os.write(IPC_obj, b"0")
        os.close(IPC_obj)
        return None

    def sendStopEvent(self):
        """Sends the stop event via the inter-process communication file.

        Warnings
        --------
        UserWarning
            Raised if we try to delete an IPC file that was not initialised.
        """
        if os.path.isfile(self._IPCFilePath):
            with open(self._IPCFilePath, "wb") as f:
                f.write(b"1")
        else:
            warnings.warn("An IPC file was not initialised.", UserWarning, stacklevel=2)
        return None

    def delete_IPC(self):
        """Delete the inter-process communication file."""
        with contextlib.suppress(FileNotFoundError):
            os.remove(self._IPCFilePath)
        return None

    def save(self, path: str | Path, filename: str = "controls"):
        """Save a controls object to a JSON file.

        Parameters
        ----------
        path : str or Path
            The directory in which the controls object will be written.

        """
        file = Path(path, f"{filename.removesuffix('.json')}.json")
        file.write_text(self.model_dump_json())

    @classmethod
    def load(cls, path: str | Path) -> "Controls":
        """Load a controls object from file.

        Parameters
        ----------
        path : str or Path
            The path to the controls object file.

        """
        file = Path(path)
        return cls.model_validate_json(file.read_text())
