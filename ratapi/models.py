"""The models module. Contains the pydantic models used by RAT to store project parameters."""

import pathlib
import warnings
from itertools import count
from typing import Any

import numpy as np
import prettytable
from pydantic import BaseModel, Field, ValidationInfo, field_validator, model_validator

from ratapi.utils.enums import BackgroundActions, Hydration, Languages, Priors, TypeOptions

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


# Create a counter for each model
background_number = count(1)
contrast_number = count(1)
custom_file_number = count(1)
data_number = count(1)
domain_contrast_number = count(1)
layer_number = count(1)
parameter_number = count(1)
resolution_number = count(1)


class RATModel(BaseModel, validate_assignment=True, extra="forbid"):
    """A BaseModel where enums are represented by their value."""

    def __repr__(self):
        fields_repr = ", ".join(
            repr(v) if a is None else f"{a}={v.value!r}" if isinstance(v, StrEnum) else f"{a}={v!r}"
            for a, v in self.__repr_args__()
        )
        return f"{self.__repr_name__()}({fields_repr})"

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.display_fields]
        table.add_row(list(self.display_fields.values()))
        return table.get_string()

    @property
    def display_fields(self) -> dict:
        """A dictionary of which fields should be displayed by this model and their values."""
        return self.__dict__


class Signal(RATModel):
    """Base model for background & resolution signals."""

    name: str = Field(default="New Signal", min_length=1)
    type: TypeOptions = TypeOptions.Constant
    source: str = ""
    value_1: str = ""
    value_2: str = ""
    value_3: str = ""
    value_4: str = ""
    value_5: str = ""

    def __setattr__(self, name, value):
        if name == "type":
            warnings.warn(f"Changing the type of {self.name} clears its source and value fields.", stacklevel=2)
            for attr in ["source", "value_1", "value_2", "value_3", "value_4", "value_5"]:
                with warnings.catch_warnings():
                    warnings.simplefilter("ignore")
                    super().__setattr__(attr, "")

        super().__setattr__(name, value)

    @property
    def display_fields(self) -> dict:
        """The fields which should be visible in a table and their values.

        Returns
        -------
        dict
            A dictionary of the fields which should be visible in a table and their values.

        """
        visible_fields = ["name", "type", "source"]
        if self.type != TypeOptions.Constant:
            visible_fields.append("value_1")
        if self.type == TypeOptions.Function:
            visible_fields.extend(["value_2", "value_3", "value_4", "value_5"])

        return {f: getattr(self, f) for f in visible_fields}


class Background(Signal):
    """A background signal.

    Parameters
    ----------
    name : str
        The name of the background.
    type : TypeOptions
        The type of background (constant, function or data)
    source : str
        The source of the background;

        - if type is 'constant', this should be the name of a background parameter.
        - if type is 'data', this should be the name of a dataset defined in `Project.data`.
        - if type is 'function', this should be the name of a custom function defined in `Project.custom_files`.

    value_1, value_2, value_3, value_4, value_5 : str
        Values required by the background.

        - if type is 'constant', all values will be ignored.
        - if type is 'data', value_1 may be the parameter name for an optional offset. Other values are ignored.
        - if type is 'function', these values may be the names of up to 5 parameters which are passed to the function.

    """

    name: str = Field(default_factory=lambda: f"New Background {next(background_number)}", min_length=1)

    @model_validator(mode="after")
    def check_unsupported_parameters(self):
        """Raise an error if the parameters given are not supported for the given type."""
        if self.type == TypeOptions.Constant:
            expected_empty_fields = ["value_1", "value_2", "value_3", "value_4", "value_5"]
        elif self.type == TypeOptions.Data:
            expected_empty_fields = ["value_2", "value_3", "value_4", "value_5"]
        else:
            return self

        non_empty_fields = [v for v in expected_empty_fields if getattr(self, v) != ""]
        if non_empty_fields:
            raise ValueError(
                f'The following values are not supported by the "{self.type}" Background type: '
                f"{', '.join(non_empty_fields)}"
            )

        return self


class Contrast(RATModel):
    """A group of all of the components of a model.

    Parameters
    ----------
    name : str
        The name of the contrast.
    data : str
        The name of the dataset used by the contrast.
    background : str
        The name of the background for the contrast.
    background_action : BackgroundActions
        Whether the background should be added ('add') or subtracted ('subtract') from the data.
    bulk_in : str
        The name of the bulk-in parameter which defines the SLD of the interface between the
        first layer and the environment.
    bulk_out : str
        The name of the bulk-out parameter which defines the SLD of the interface between the last
        layer and the environment.
    scalefactor : str
        The name of the scalefactor which defines how much the data for this contrast should be scaled.
    resolution : str
        The name of the instrument resolution for this contrast.
    resample : bool
        Whether adaptive resampling should be used for interface microslicing.
    repeat_layers : int
        For standard layers, the number of times the set of layers defined in the model should be repeated.
    model : list[str]
        If this is a standard layers model, this should be a list of layer names
        that make up the slab model for this contrast.
        For custom models, this should be a list containing just the custom file name for the
        custom model function.

    """

    name: str = Field(default_factory=lambda: f"New Contrast {next(contrast_number)}", min_length=1)
    data: str = ""
    background: str = ""
    background_action: BackgroundActions = BackgroundActions.Add
    bulk_in: str = ""
    bulk_out: str = ""
    scalefactor: str = ""
    resolution: str = ""
    resample: bool = False
    repeat_layers: int = Field(default=1, gt=0)
    model: list[str] = []

    @model_validator(mode="before")
    @classmethod
    def domain_ratio_error(cls, data: Any):
        """If the extra input 'domain_ratio' is given, give a more descriptive error."""
        if isinstance(data, dict) and data.get("domain_ratio", False):
            raise ValueError(
                "The Contrast class does not support domain ratios. Use the ContrastWithRatio class instead."
            )

        return data

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row(
            [
                self.name,
                self.data,
                self.background,
                self.background_action,
                self.bulk_in,
                self.bulk_out,
                self.scalefactor,
                self.resolution,
                self.resample,
                self.repeat_layers,
                model_entry,
            ]
        )
        return table.get_string()


class ContrastWithRatio(RATModel):
    """A group of all of the components of a model, including domain terms.

    Parameters
    ----------
    name : str
        The name of the contrast.
    data : str
        The name of the dataset used by the contrast.
    background : str
        The name of the background for the contrast.
    background_action : BackgroundActions
        Whether the background should be added ('add') or subtracted ('subtract') from the data.
    bulk_in : str
        The name of the bulk-in parameter which defines the SLD of the interface between the
        first layer and the environment.
    bulk_out : str
        The name of the bulk-out parameter which defines the SLD of the interface between the last
        layer and the environment.
    scalefactor : str
    resolution : str
        The name of the instrument resolution for this contrast.
    resample : bool
        Whether adaptive resampling should be used for interface microslicing.
    repeat_layers : int
        For standard layers, the number of times the set of layers defined in the model should be repeated.
    domain_ratio : str
        The name of the domain ratio parameter describing how the first domain should be weighted
        relative to the second.
    model : list[str]
        If this is a standard layers model, this should be a list of the names of the two domain contrasts
        which make up the domain model for this contrast.
        For custom models, this should be a list containing just the custom file name for the
        custom model function.

    """

    name: str = Field(default_factory=lambda: f"New Contrast {next(contrast_number)}", min_length=1)
    data: str = ""
    background: str = ""
    background_action: BackgroundActions = BackgroundActions.Add
    bulk_in: str = ""
    bulk_out: str = ""
    scalefactor: str = ""
    resolution: str = ""
    resample: bool = False
    repeat_layers: int = Field(default=1, gt=0)
    domain_ratio: str = ""
    model: list[str] = []

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row(
            [
                self.name,
                self.data,
                self.background,
                self.background_action,
                self.bulk_in,
                self.bulk_out,
                self.scalefactor,
                self.resolution,
                self.resample,
                self.repeat_layers,
                self.domain_ratio,
                model_entry,
            ]
        )
        return table.get_string()


class CustomFile(RATModel):
    """A file containing functions to use for a custom model or function background.

    Parameters
    ----------
    name : str
        The name of this custom file object.
    filename : str
        The name of the file containing the custom function.
    function_name : str
        The name of the custom function within the file.
    language : Languages
        What language the custom function is written in: 'matlab', 'python', or 'cpp' (C++)
    path : pathlib.Path
        The path to the custom file.

    """

    name: str = Field(default_factory=lambda: f"New Custom File {next(custom_file_number)}", min_length=1)
    filename: str = ""
    function_name: str = ""
    language: Languages = Languages.Python
    path: pathlib.Path = pathlib.Path(".").resolve()

    @field_validator("path")
    @classmethod
    def resolve_relative_paths(cls, path: pathlib.Path) -> pathlib.Path:
        """Return the absolute path of the given path."""
        return path.resolve()

    def model_post_init(self, __context: Any) -> None:
        """Autogenerate the function name from the filename if not set.

        If a filename is supplied but the ``function_name`` field is not set, the ``function_name`` should be set to
        the filename without the extension.

        """
        if "filename" in self.model_fields_set and "function_name" not in self.model_fields_set:
            self.function_name = pathlib.Path(self.filename).stem

    @model_validator(mode="after")
    def set_matlab_function_name(self):
        """For a matlab custom function, ``function_name`` should be set to the filename without the extension."""
        if self.language == Languages.Matlab and self.function_name != pathlib.Path(self.filename).stem:
            self.function_name = pathlib.Path(self.filename).stem

        return self


class Data(RATModel, arbitrary_types_allowed=True):
    """A dataset required for a contrast.

    Parameters
    ----------
    name : str
        The name of this dataset.
    data : np.ndarray[np.float64]
        The (x, y, error) data for this dataset, given as a Numpy array of three columns.
    data_range : list[float]
    simulation_range : list[float]

    """

    name: str = Field(default_factory=lambda: f"New Data {next(data_number)}", min_length=1)
    data: np.ndarray = np.empty([0, 3])
    data_range: list[float] = Field(default=[], min_length=2, max_length=2)
    simulation_range: list[float] = Field(default=[], min_length=2, max_length=2)

    @field_validator("data")
    @classmethod
    def check_data_dimension(cls, data: np.ndarray) -> np.ndarray:
        """Ensure the data is be a two-dimensional array containing at least three columns."""
        try:
            data.shape[1]
        except IndexError:
            raise ValueError('"data" must have at least two dimensions') from None
        else:
            if data.shape[1] < 3:
                raise ValueError('"data" must have at least three columns') from None
        return data

    @field_validator("data_range", "simulation_range")
    @classmethod
    def check_min_max(cls, limits: list[float], info: ValidationInfo) -> list[float]:
        """Ensure the data range and simulation range maximum is greater than the minimum."""
        if limits[0] > limits[1]:
            raise ValueError(f'{info.field_name} "min" value is greater than the "max" value')
        return limits

    def model_post_init(self, __context: Any) -> None:
        """Automatically generate ``data_range`` and ``simulation_range`` from the data.

        If the ``data_range`` and ``simulation_range`` fields are not set, but ``data`` is supplied, the ranges are
        set to the min and max values of the first column (assumed to be q) of the supplied data.
        """
        if self.data.shape[0] > 0:
            data_min = float(np.min(self.data[:, 0]))
            data_max = float(np.max(self.data[:, 0]))
            for field in ["data_range", "simulation_range"]:
                if field not in self.model_fields_set:
                    getattr(self, field).extend([data_min, data_max])

    @model_validator(mode="after")
    def check_ranges(self) -> "Data":
        """Check that ``data_range`` is within the q-range of the data, and ``simulation_range`` is outside it.

        The limits of the "data_range" field must lie within the range of the supplied data, whilst the limits
        of the "simulation_range" field must lie outside the range of the supplied data.
        """
        if self.data.shape[0] > 0:
            data_min = np.min(self.data[:, 0])
            data_max = np.max(self.data[:, 0])
            if "data_range" in self.model_fields_set and (
                self.data_range[0] < data_min or self.data_range[1] > data_max
            ):
                raise ValueError(
                    f"The data_range value of: {self.data_range} must lie within the min/max values of "
                    f"the data: [{data_min}, {data_max}]",
                )
            if "simulation_range" in self.model_fields_set and (
                self.simulation_range[0] > data_min or self.simulation_range[1] < data_max
            ):
                raise ValueError(
                    f"The simulation_range value of: {self.simulation_range} must lie outside of the "
                    f"min/max values of the data: [{data_min}, {data_max}]",
                )
        return self

    def __eq__(self, other: object) -> bool:
        if isinstance(other, BaseModel):
            # When comparing instances of generic types for equality, as long as all field values are equal,
            # only require their generic origin types to be equal, rather than exact type equality.
            # This prevents headaches like MyGeneric(x=1) != MyGeneric[Any](x=1).
            self_type = self.__pydantic_generic_metadata__["origin"] or self.__class__
            other_type = other.__pydantic_generic_metadata__["origin"] or other.__class__

            return (
                self_type == other_type
                and self.name == other.name
                and (self.data == other.data).all()
                and self.data_range == other.data_range
                and self.simulation_range == other.simulation_range
                and self.__pydantic_private__ == other.__pydantic_private__
                and self.__pydantic_extra__ == other.__pydantic_extra__
            )
        else:
            return NotImplemented  # delegate to the other item in the comparison

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        array_entry = f"{'Data array: [' + ' x '.join(str(i) for i in self.data.shape) if self.data.size > 0 else '['}]"
        table.add_row([self.name, array_entry, self.data_range, self.simulation_range])
        return table.get_string()


class DomainContrast(RATModel):
    """A group of layers required for a domain.

    Parameters
    ----------
    name : str
        The name of this domain contrast.
    model : list[str]
        A list of layer names that make up the slab model for this contrast.

    """

    name: str = Field(default_factory=lambda: f"New Domain Contrast {next(domain_contrast_number)}", min_length=1)
    model: list[str] = []

    def __str__(self):
        table = prettytable.PrettyTable()
        table.field_names = [key.replace("_", " ") for key in self.__dict__]
        model_entry = "\n".join(element for element in self.model)
        table.add_row([self.name, model_entry])
        return table.get_string()


class Layer(RATModel, populate_by_name=True):
    """A slab model layer with given physical properties.

    Parameters
    ----------
    name : str
        The name of this layer.
    thickness : str
        The name of the parameter describing the thickness of this layer.
    SLD : str
        The name of the parameter describing the scattering length density
        of this layer.
    roughness : str
        The name of the parameter describing the roughness of this layer.
    hydration : str
    hydrate_with : str

    """

    name: str = Field(default_factory=lambda: f"New Layer {next(layer_number)}", min_length=1)
    thickness: str
    SLD: str = Field(validation_alias="SLD_real")
    roughness: str
    hydration: str = ""
    hydrate_with: Hydration = Hydration.BulkOut

    @model_validator(mode="before")
    @classmethod
    def sld_imaginary_error(cls, data: Any):
        """If the extra input 'sld_imaginary' is given, give a more descriptive error."""
        if isinstance(data, dict) and data.get("SLD_imaginary", False):
            raise ValueError("The Layer class does not support imaginary SLD. Use the AbsorptionLayer class instead.")

        return data


class AbsorptionLayer(RATModel, populate_by_name=True):
    """A slab model layer with a non-negligible absorption term.

    Parameters
    ----------
    name : str
        The name of this layer.
    thickness : str
        The name of the parameter describing the thickness of this layer.
    SLD_real : str
        The name of the parameter describing the real (scattering) term
        for the scattering length density of this layer.
    SLD_imaginary : str
        The name of the parameter describing the imaginary (absorption) term
        for the scattering length density of this layer.
    roughness : str
        The name of the parameter describing the roughness of this layer.
    hydration : str
    hydrate_with : str

    """

    name: str = Field(default_factory=lambda: f"New Layer {next(layer_number)}", min_length=1)
    thickness: str
    SLD_real: str = Field(validation_alias="SLD")
    SLD_imaginary: str
    roughness: str
    hydration: str = ""
    hydrate_with: Hydration = Hydration.BulkOut


class Parameter(RATModel):
    """A parameter needed to specify the model.

    Parameters
    ----------
    name : str
        The name of this parameter.
    min : float
        The minimum value that this parameter could take when fitted.
    value : float
        The value of this parameter.
    max : float
        The maximum value that this parameter could take when fitted.
    fit : bool
        Whether this parameter should be fitted in a calculation.
    prior_type : Priors
        For Bayesian calculations, whether the prior likelihood
        is assumed to be 'uniform' or 'gaussian'.
    mu, sigma : float
        If the prior type is Gaussian, the mu and sigma values describing
        the Gaussian function for the prior likelihood.

    """

    name: str = Field(default_factory=lambda: f"New Parameter {next(parameter_number)}", min_length=1)
    min: float = 0.0
    value: float = 0.0
    max: float = 0.0
    fit: bool = False
    prior_type: Priors = Priors.Uniform
    mu: float = 0.0
    sigma: float = np.inf

    show_priors: bool = False

    def model_post_init(self, __context: Any) -> None:
        """Apply parameter value to limits if they are not set."""
        if "value" in self.model_fields_set:
            if self.value > 0.0:
                if "max" not in self.model_fields_set:
                    self.max = self.value
                if "min" not in self.model_fields_set:
                    self.min = self.value
            elif self.value < 0.0:
                if "min" not in self.model_fields_set:
                    self.min = self.value
                if "max" not in self.model_fields_set:
                    self.max = self.value

    @model_validator(mode="after")
    def check_min_max(self) -> "Parameter":
        """Ensure the maximum value of a parameter is greater than the minimum."""
        if self.min > self.max:
            raise ValueError(f"The maximum value {self.max} must be greater than the minimum value {self.min}")
        return self

    @model_validator(mode="after")
    def check_value_in_range(self) -> "Parameter":
        """Ensure the value of a parameter lies within its defined bounds."""
        if self.value < self.min or self.value > self.max:
            raise ValueError(f"value {self.value} is not within the defined range: {self.min} <= value <= {self.max}")
        return self

    @property
    def display_fields(self) -> dict:
        """Only display Prior information if ``show_priors`` is true."""
        visible_fields = ["name", "min", "value", "max", "fit"]
        if self.show_priors:
            visible_fields.append("prior_type")
            if self.prior_type == Priors.Gaussian:
                visible_fields.extend(["mu", "sigma"])

        return {f: getattr(self, f) for f in visible_fields}


class ProtectedParameter(Parameter):
    """A Parameter with a fixed name."""

    name: str = Field(frozen=True, min_length=1)


class Resolution(Signal):
    """An instrument resolution.

    Parameters
    ----------
    name : str
        The name of the resolution.
    type : TypeOptions
        The type of resolution: 'constant', 'data', or (NOT YET IMPLEMENTED) 'function'.
    source : str
        The source data for the resolution;

        - if type is 'constant', this should be the name of a background parameter.
        - if type is 'data', this should be empty (resolution data is in the contrast data).
        - if type is 'function' (NOT YET IMPLEMENTED),
          this should be the name of a custom function defined in `Project.custom_files`.

    value_1, value_2, value_3, value_4, value_5 : str
        Values required by the background.

        - if type is 'constant' or 'data', all values will be ignored.
        - if type is 'function' (NOT YET IMPLEMENTED),
          these values may be the names of up to 5 parameters which are passed to the function.

    """

    name: str = Field(default_factory=lambda: f"New Resolution {next(resolution_number)}", min_length=1)

    @field_validator("type")
    @classmethod
    def validate_unimplemented_resolutions(cls, type: TypeOptions):
        """Raise an error if currently unsupported function resolutions are used."""
        # when function resolutions are added, fix the commented-out parts of
        # test_project.py::test_rename_models
        # and test_project.py::test_allowed_resolutions
        if type == TypeOptions.Function:
            raise NotImplementedError("Function resolutions are not yet supported.")
        return type

    @model_validator(mode="after")
    def check_unsupported_parameters(self):
        """Raise an error if the parameters given are not supported for the given type."""
        if self.type == TypeOptions.Constant:
            expected_empty_fields = ["value_1", "value_2", "value_3", "value_4", "value_5"]
        elif self.type == TypeOptions.Data:
            expected_empty_fields = ["source", "value_1", "value_2", "value_3", "value_4", "value_5"]
        else:
            return self

        non_empty_fields = [v for v in expected_empty_fields if getattr(self, v) != ""]
        if non_empty_fields:
            raise ValueError(
                f'The following values are not supported by the "{self.type}" Resolution type: '
                f"{', '.join(non_empty_fields)}"
            )

        return self
