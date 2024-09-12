import contextlib
import os
import tempfile
import warnings

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


class Controls(BaseModel, validate_assignment=True, extra="forbid"):
    """The full set of controls parameters for all five procedures that are required for the compiled RAT code."""

    # All Procedures
    procedure: Procedures = Procedures.Calculate
    parallel: Parallel = Parallel.Single
    calcSldDuringFit: bool = False
    resampleMinAngle: float = Field(0.9, le=1, gt=0)
    resampleNPoints: int = Field(50, gt=0)
    display: Display = Display.Iter
    # Simplex
    xTolerance: float = Field(1.0e-6, gt=0.0)
    funcTolerance: float = Field(1.0e-6, gt=0.0)
    maxFuncEvals: int = Field(10000, gt=0)
    maxIterations: int = Field(1000, gt=0)
    # Simplex and DE
    updateFreq: int = 1
    updatePlotFreq: int = 20
    # DE
    populationSize: int = Field(20, ge=1)
    fWeight: float = 0.5
    crossoverProbability: float = Field(0.8, gt=0.0, lt=1.0)
    strategy: Strategies = Strategies.RandomWithPerVectorDither
    targetValue: float = Field(1.0, ge=1.0)
    numGenerations: int = Field(500, ge=1)
    # NS
    nLive: int = Field(150, ge=1)
    nMCMC: int = Field(0, ge=0)
    propScale: float = Field(0.1, gt=0.0, lt=1.0)
    nsTolerance: float = Field(0.1, ge=0.0)
    # Dream
    nSamples: int = Field(20000, ge=0)
    nChains: int = Field(10, gt=0)
    jumpProbability: float = Field(0.5, gt=0.0, lt=1.0)
    pUnitGamma: float = Field(0.2, gt=0.0, lt=1.0)
    boundHandling: BoundHandling = BoundHandling.Reflect
    adaptPCR: bool = True
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
                f' controls procedure are:\n    '
                f'{", ".join(fields.get("procedure", []))}\n',
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
                    f'    {", ".join(fields[new_procedure])}\n',
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
