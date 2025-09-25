"""The project module. Defines and stores all the input data required for reflectivity calculations in RAT."""

import collections
import copy
import functools
import json
import warnings
from collections.abc import Callable
from enum import Enum
from pathlib import Path
from textwrap import indent
from typing import Annotated, Any, get_args, get_origin

import numpy as np
from pydantic import (
    BaseModel,
    Discriminator,
    Field,
    Tag,
    ValidationError,
    ValidationInfo,
    field_validator,
    model_validator,
)

import ratapi.models
from ratapi.classlist import ClassList
from ratapi.utils.custom_errors import custom_pydantic_validation_error
from ratapi.utils.enums import Calculations, Geometries, LayerModels, Priors, TypeOptions


# note for these discriminators that the before-validator discriminate_ambiguous_dicts
# guarantees we don't run into the ambiguous case of a sequence of dicts
def discriminate_layers(layer_input):
    """Union discriminator for layers."""
    if isinstance(layer_input, collections.abc.Sequence):
        # if classlist is empty, just label it as no absorption and it'll get fixed in post_init
        if len(layer_input) > 0 and isinstance(layer_input[0], ratapi.models.AbsorptionLayer):
            return "abs"
        return "no_abs"


def discriminate_contrasts(contrast_input):
    """Union discriminator for contrasts."""
    if isinstance(contrast_input, collections.abc.Sequence):
        # if classlist is empty, just label it as no ratio and it'll get fixed in post_init
        if len(contrast_input) > 0 and isinstance(contrast_input[0], ratapi.models.ContrastWithRatio):
            return "ratio"
        return "no_ratio"


values_defined_in = {
    "backgrounds.value_1": "background_parameters",
    "backgrounds.value_2": "background_parameters",
    "backgrounds.value_3": "background_parameters",
    "backgrounds.value_4": "background_parameters",
    "backgrounds.value_5": "background_parameters",
    "backgrounds.constant.source": "background_parameters",
    "backgrounds.data.source": "data",
    "backgrounds.function.source": "custom_files",
    "resolutions.value_1": "resolution_parameters",
    "resolutions.value_2": "resolution_parameters",
    "resolutions.value_3": "resolution_parameters",
    "resolutions.value_4": "resolution_parameters",
    "resolutions.value_5": "resolution_parameters",
    "resolutions.constant.source": "resolution_parameters",
    "resolutions.function.source": "custom_files",
    "layers.thickness": "parameters",
    "layers.SLD": "parameters",
    "layers.SLD_real": "parameters",
    "layers.SLD_imaginary": "parameters",
    "layers.roughness": "parameters",
    "contrasts.data": "data",
    "contrasts.background": "backgrounds",
    "contrasts.bulk_in": "bulk_in",
    "contrasts.bulk_out": "bulk_out",
    "contrasts.scalefactor": "scalefactors",
    "contrasts.resolution": "resolutions",
    "contrasts.domain_ratio": "domain_ratios",
}

AllFields = collections.namedtuple("AllFields", ["attribute", "fields"])
model_names_used_in = {
    "background_parameters": [
        AllFields("backgrounds", ["source", "value_1", "value_2", "value_3", "value_4", "value_5"])
    ],
    "resolution_parameters": [
        AllFields("resolutions", ["source", "value_1", "value_2", "value_3", "value_4", "value_5"])
    ],
    "parameters": [AllFields("layers", ["thickness", "SLD", "SLD_real", "SLD_imaginary", "roughness", "hydration"])],
    "data": [
        AllFields("contrasts", ["data"]),
        AllFields("backgrounds", ["source"]),
    ],
    "custom_files": [AllFields("backgrounds", ["source"]), AllFields("resolutions", ["source"])],
    "backgrounds": [AllFields("contrasts", ["background"])],
    "bulk_in": [AllFields("contrasts", ["bulk_in"])],
    "bulk_out": [AllFields("contrasts", ["bulk_out"])],
    "scalefactors": [AllFields("contrasts", ["scalefactor"])],
    "domain_ratios": [AllFields("contrasts", ["domain_ratio"])],
    "resolutions": [AllFields("contrasts", ["resolution"])],
}

# Note that the order of these parameters is hard-coded into RAT
parameter_class_lists = [
    "parameters",
    "background_parameters",
    "scalefactors",
    "bulk_in",
    "bulk_out",
    "resolution_parameters",
    "domain_ratios",
]
class_lists = [
    *parameter_class_lists,
    "backgrounds",
    "resolutions",
    "custom_files",
    "data",
    "layers",
    "domain_contrasts",
    "contrasts",
]


class Project(BaseModel, validate_assignment=True, extra="forbid", use_attribute_docstrings=True):
    """Defines the input data for a reflectivity calculation in RAT.

    This class combines the data defined in each of the pydantic models included in "models.py" into the full set of
    inputs required for a reflectivity calculation.
    """

    name: str = ""
    """The name of the project."""

    calculation: Calculations = Calculations.Normal
    """What calculation type should be used. Can be 'normal' or 'domains'."""

    model: LayerModels = LayerModels.StandardLayers
    """What layer model should be used. Can be 'standard layers', 'custom layers', or 'custom xy'."""

    geometry: Geometries = Geometries.AirSubstrate
    """What geometry should be used. Can be 'air/substrate' or 'substrate/liquid'"""

    absorption: bool = False
    """Whether imaginary SLD (absorption) should be accounted for."""

    parameters: ClassList[ratapi.models.Parameter] = ClassList()
    """The list of parameters used in the layers of a model."""

    bulk_in: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="SLD Air",
            min=0.0,
            value=0.0,
            max=0.0,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for SLD of the entry interfaces of a model."""

    bulk_out: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="SLD D2O",
            min=6.2e-6,
            value=6.35e-6,
            max=6.35e-6,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for SLD of the exit interfaces of a model."""

    scalefactors: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="Scalefactor 1",
            min=0.02,
            value=0.23,
            max=0.25,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for scale factors to handle systematic error in model data."""

    domain_ratios: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="Domain Ratio 1",
            min=0.4,
            value=0.5,
            max=0.6,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for weighting between domains of a domains model."""

    background_parameters: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="Background Param 1",
            min=1e-7,
            value=1e-6,
            max=1e-5,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for models of backgrounds."""

    backgrounds: ClassList[ratapi.models.Background] = ClassList(
        ratapi.models.Background(name="Background 1", type=TypeOptions.Constant, source="Background Param 1"),
    )
    """The list of models for background noise in the project."""

    resolution_parameters: ClassList[ratapi.models.Parameter] = ClassList(
        ratapi.models.Parameter(
            name="Resolution Param 1",
            min=0.01,
            value=0.03,
            max=0.05,
            fit=False,
            prior_type=Priors.Uniform,
            mu=0.0,
            sigma=np.inf,
        ),
    )
    """The list of parameters for models of resolutions."""

    resolutions: ClassList[ratapi.models.Resolution] = ClassList(
        ratapi.models.Resolution(name="Resolution 1", type=TypeOptions.Constant, source="Resolution Param 1"),
    )
    """The list of models for instrument resolution in the project."""

    custom_files: ClassList[ratapi.models.CustomFile] = ClassList()
    """Handles for custom files used by the project."""

    data: ClassList[ratapi.models.Data] = ClassList()
    """Experimental data for a model."""

    layers: (
        Annotated[ClassList[ratapi.models.Layer], Tag("no_abs")]
        | Annotated[ClassList[ratapi.models.AbsorptionLayer], Tag("abs")]
    ) = Field(
        default=ClassList(),
        discriminator=Discriminator(
            discriminate_layers,
            custom_error_type="invalid_union_member",
            custom_error_message="Input should be an instance of ClassList",
            custom_error_context={"discriminator": "absorption_or_no"},
        ),
    )
    """The layers of a standard layer model."""

    domain_contrasts: ClassList[ratapi.models.DomainContrast] = ClassList()
    """The groups of layers required by each domain in a domains model."""

    contrasts: (
        Annotated[ClassList[ratapi.models.Contrast], Tag("no_ratio")]
        | Annotated[ClassList[ratapi.models.ContrastWithRatio], Tag("ratio")]
    ) = Field(
        default=ClassList(),
        discriminator=Discriminator(
            discriminate_contrasts,
            custom_error_type="invalid_union_member",
            custom_error_message="Input should be an instance of ClassList.",
            custom_error_context={"discriminator": "ratio_or_no_ratio"},
        ),
    )
    """All groups of components used to define each model in the project."""

    _all_names: dict
    _contrast_model_field: str
    _protected_parameters: dict

    @model_validator(mode="before")
    @classmethod
    def discriminate_ambiguous_dicts(cls, data: Any) -> Any:
        """If layers or contrasts contain a dict, convert it to the relevant model."""
        # pydantic docs says data can be anything, but i can't see anywhere where it isn't a dict.
        # if it's not a dict, just return and let the library handle it
        if isinstance(data, dict):
            layer_model = ratapi.models.AbsorptionLayer if data.get("absorption", False) else ratapi.models.Layer
            if data.get("calculation", Calculations.Normal) == Calculations.Domains:
                contrast_model = ratapi.models.ContrastWithRatio
            else:
                contrast_model = ratapi.models.Contrast

            # note we aren't modifying the layers and contrasts in-place:
            # if a ClassList of dicts is passed, in-place conversion would make the ClassList heterogenous
            # & it'd throw an error
            if layers := data.get("layers", False):
                new_layers = ClassList()
                for layer in layers:
                    if isinstance(layer, dict):
                        layer = layer_model.model_validate(layer)
                    new_layers.append(layer)
                data["layers"] = new_layers

            if contrasts := data.get("contrasts", False):
                new_contrasts = ClassList()
                for contrast in contrasts:
                    if isinstance(contrast, dict):
                        contrast = contrast_model.model_validate(contrast)
                    new_contrasts.append(contrast)
                data["contrasts"] = new_contrasts

        return data

    @field_validator("layers")
    @classmethod
    def check_layers(cls, value: ClassList, info: ValidationInfo):
        """Check that layers are AbsorptionLayers if doing absorption, and Layers otherwise."""
        if info.data["absorption"]:
            model_name = "AbsorptionLayer"
            other_model = "Layer"
        else:
            model_name = "Layer"
            other_model = "AbsorptionLayer"
        model = getattr(ratapi.models, model_name)
        if not all(isinstance(element, model) for element in value):
            raise ValueError(
                f'"The layers attribute contains {other_model}s, '
                f"but the absorption parameter is {info.data['absorption']}. "
                f'The attribute should be a ClassList of {model_name} instead."'
            )

        return value

    @field_validator("contrasts")
    @classmethod
    def check_contrasts(cls, value: ClassList, info: ValidationInfo):
        """Check that contrasts are with ratio if calculating domains, and without otherwise."""
        if info.data["calculation"] == Calculations.Domains:
            model_name = "ContrastWithRatio"
            error_word = "without"
        else:
            model_name = "Contrast"
            error_word = "with"
        model = getattr(ratapi.models, model_name)
        if not all(isinstance(element, model) for element in value):
            raise ValueError(
                f'"The contrasts attribute contains contrasts {error_word} ratio, '
                f'but the calculation is {str(info.data["calculation"])}", '
                f"The attribute should be a ClassList of {model_name} instead."
            )

        return value

    def model_post_init(self, __context: Any) -> None:
        """Set up the Class to protect against disallowed modification.

        We initialise the class handle in the ClassLists for empty data fields, set protected parameters, get names of
        all defined parameters, determine the contents of the "model" field in contrasts, and wrap ClassList routines
        to control revalidation.
        """
        # Ensure all ClassLists have the correct _class_handle defined
        for field in (fields := Project.model_fields):
            annotation = fields[field].annotation
            if get_origin(annotation) == ClassList:
                classlist = getattr(self, field)
                if not hasattr(field, "_class_handle"):
                    classlist._class_handle = get_args(annotation)[0]

        layers_field = self.layers
        if not hasattr(layers_field, "_class_handle"):
            if self.absorption:
                layers_field._class_handle = ratapi.models.AbsorptionLayer
            else:
                layers_field._class_handle = ratapi.models.Layer

        contrast_field = self.contrasts
        if not hasattr(contrast_field, "_class_handle"):
            if self.calculation == Calculations.Domains:
                contrast_field._class_handle = ratapi.models.ContrastWithRatio
            else:
                contrast_field._class_handle = ratapi.models.Contrast

        if "Substrate Roughness" not in [name.title() for name in self.parameters.get_names()]:
            self.parameters.insert(
                0,
                ratapi.models.ProtectedParameter(
                    name="Substrate Roughness",
                    min=1.0,
                    value=3.0,
                    max=5.0,
                    fit=True,
                    prior_type=ratapi.models.Priors.Uniform,
                    mu=0.0,
                    sigma=np.inf,
                ),
            )
        elif "Substrate Roughness" not in [name.title() for name in self.get_all_protected_parameters()["parameters"]]:
            # If substrate roughness is included as a standard parameter replace it with a protected parameter
            substrate_roughness_values = self.parameters["Substrate Roughness"].model_dump()
            self.parameters.remove("Substrate Roughness")
            self.parameters.insert(0, ratapi.models.ProtectedParameter(**substrate_roughness_values))

        if "Simulation" not in [name.title() for name in self.data.get_names()]:
            self.data.insert(0, ratapi.models.Data(name="Simulation", simulation_range=[0.005, 0.7]))

        self._all_names = self.get_all_names()
        self._contrast_model_field = self.get_contrast_model_field()
        self._protected_parameters = self.get_all_protected_parameters()

        # Wrap ClassList routines - when any of these routines are called, the wrapper will force revalidation of the
        # model, handle errors and reset previous values if necessary.
        methods_to_wrap = [
            "_setitem",
            "_delitem",
            "_iadd",
            "append",
            "insert",
            "pop",
            "remove",
            "clear",
            "extend",
            "set_fields",
        ]

        for class_list in class_lists:
            attribute = getattr(self, class_list)
            for methodName in methods_to_wrap:
                setattr(attribute, methodName, self._classlist_wrapper(attribute, getattr(attribute, methodName)))

    @model_validator(mode="after")
    def set_domain_ratios(self) -> "Project":
        """If we are not running a domains calculation, ensure the domain_ratios component of the model is empty."""
        if self.calculation != Calculations.Domains:
            self.domain_ratios.data = []
        return self

    @model_validator(mode="after")
    def set_domain_contrasts(self) -> "Project":
        """Ensure ``domain_contrasts`` is empty if we are not running a standard layer domains calculation."""
        if not (self.calculation == Calculations.Domains and self.model == LayerModels.StandardLayers):
            self.domain_contrasts.data = []
        return self

    @model_validator(mode="after")
    def set_layers(self) -> "Project":
        """If we are not using a standard layers model, ensure the layers component of the model is empty."""
        if self.model != LayerModels.StandardLayers:
            self.layers.data = []
        return self

    @model_validator(mode="after")
    def set_repeat_layers(self) -> "Project":
        """If we are not using a standard layers model, warn that the repeat layers setting is not valid."""
        if self.model != LayerModels.StandardLayers:
            for contrast in self.contrasts:
                if "repeat_layers" in contrast.model_fields_set and contrast.repeat_layers != 1:
                    warnings.warn(
                        'For a custom layers or custom XY calculation, the "repeat_layers" setting for each '
                        "contrast is not valid - resetting to 1.",
                        stacklevel=2,
                    )
                contrast.repeat_layers = 1
        return self

    @model_validator(mode="after")
    def set_resample(self) -> "Project":
        """If we are using a custom XY model, warn that the resample setting for each contrast must always be True."""
        if self.model == LayerModels.CustomXY:
            for contrast in self.contrasts:
                if "resample" in contrast.model_fields_set and contrast.resample is False:
                    warnings.warn(
                        'For a custom XY calculation, "resample" must be True for each contrast - resetting to True.',
                        stacklevel=2,
                    )
                contrast.resample = True
        return self

    @model_validator(mode="after")
    def set_calculation(self) -> "Project":
        """Apply the calc setting to the project."""
        contrast_list = []
        handle = self.contrasts._class_handle.__name__
        if self.calculation == Calculations.Domains and handle == "Contrast":
            for contrast in self.contrasts:
                contrast_list.append(ratapi.models.ContrastWithRatio(**contrast.model_dump()))
            self.contrasts.data = contrast_list
            self.domain_ratios.data = [
                ratapi.models.Parameter(
                    name="Domain Ratio 1",
                    min=0.4,
                    value=0.5,
                    max=0.6,
                    fit=False,
                    prior_type=ratapi.models.Priors.Uniform,
                    mu=0.0,
                    sigma=np.inf,
                ),
            ]
            self.contrasts._class_handle = ratapi.models.ContrastWithRatio
        elif self.calculation != Calculations.Domains and handle == "ContrastWithRatio":
            for contrast in self.contrasts:
                contrast_params = contrast.model_dump()
                del contrast_params["domain_ratio"]
                contrast_list.append(ratapi.models.Contrast(**contrast_params))
            self.contrasts.data = contrast_list
            self.contrasts._class_handle = ratapi.models.Contrast
        return self

    @model_validator(mode="after")
    def set_contrast_model_field(self) -> "Project":
        """Clear the contrast models if ``calculation`` or ``model_type`` has changed.

        The contents of the "model" field of "contrasts" depend on the values of the "calculation" and "model_type"
        defined in the project.
        """
        model_field = self.get_contrast_model_field()
        if model_field != self._contrast_model_field:
            for contrast in self.contrasts:
                contrast.model = []
            self._contrast_model_field = model_field
        return self

    @model_validator(mode="after")
    def check_contrast_model_length(self) -> "Project":
        """Ensure the contrast model isn't too long for a domains, custom layers, or custom XY calculation.

        If a custom model is used, the ``model`` field of the contrast should just be one item long. For
        a standard layers domain calculation, it should be exactly two items long.
        """
        if self.model == LayerModels.StandardLayers and self.calculation == Calculations.Domains:
            for contrast in self.contrasts:
                if contrast.model and len(contrast.model) != 2:
                    raise ValueError(
                        'For a standard layers domains calculation the "model" field of "contrasts" must '
                        "contain exactly two values.",
                    )
        elif self.model != LayerModels.StandardLayers:
            for contrast in self.contrasts:
                if len(contrast.model) > 1:
                    raise ValueError(
                        'For a custom model calculation the "model" field of "contrasts" cannot contain '
                        "more than one value.",
                    )
        return self

    @model_validator(mode="after")
    def set_absorption(self) -> "Project":
        """Apply the absorption setting to the project."""
        layer_list = []
        handle = self.layers._class_handle.__name__
        if self.absorption and handle == "Layer":
            for layer in self.layers:
                layer_params = layer.model_dump()
                layer_params["SLD_imaginary"] = ""
                layer_list.append(ratapi.models.AbsorptionLayer(**layer_params))
            self.layers.data = layer_list
            self.layers._class_handle = ratapi.models.AbsorptionLayer
        elif not self.absorption and handle == "AbsorptionLayer":
            for layer in self.layers:
                layer_params = layer.model_dump()
                del layer_params["SLD_imaginary"]
                layer_list.append(ratapi.models.Layer(**layer_params))
            self.layers.data = layer_list
            self.layers._class_handle = ratapi.models.Layer
        return self

    @model_validator(mode="after")
    def update_renamed_models(self) -> "Project":
        """When models defined in the ClassLists are renamed, we need to update that name elsewhere in the project."""
        for class_list, fields_to_update in model_names_used_in.items():
            old_names = self._all_names[class_list]
            new_names = getattr(self, class_list).get_names()
            if len(old_names) == len(new_names):
                name_diff = [(old, new) for (old, new) in zip(old_names, new_names, strict=False) if old != new]
                for old_name, new_name in name_diff:
                    for field in fields_to_update:
                        project_field = getattr(self, field.attribute)
                        all_matches = project_field.get_all_matches(old_name)
                        params = field.fields
                        for index, param in all_matches:
                            if param in params:
                                setattr(project_field[index], param, new_name)
        return self

    @model_validator(mode="after")
    def cross_check_model_values(self) -> "Project":
        """Certain model fields should contain values defined elsewhere in the project."""
        values = ["value_1", "value_2", "value_3", "value_4", "value_5"]
        for field in ["backgrounds", "resolutions"]:
            self.check_allowed_source(field)
            self.check_allowed_values(
                field,
                values,
                getattr(self, f"{field[:-1]}_parameters").get_names(),
                self._all_names[f"{field[:-1]}_parameters"],
            )

        self.check_allowed_values(
            "layers",
            ["thickness", "SLD", "SLD_real", "SLD_imaginary", "roughness"],
            self.parameters.get_names(),
            self._all_names["parameters"],
        )

        self.check_allowed_values("contrasts", ["data"], self.data.get_names(), self._all_names["data"])
        self.check_allowed_values(
            "contrasts", ["background"], self.backgrounds.get_names(), self._all_names["backgrounds"]
        )
        self.check_allowed_values("contrasts", ["bulk_in"], self.bulk_in.get_names(), self._all_names["bulk_in"])
        self.check_allowed_values("contrasts", ["bulk_out"], self.bulk_out.get_names(), self._all_names["bulk_out"])
        self.check_allowed_values(
            "contrasts", ["scalefactor"], self.scalefactors.get_names(), self._all_names["scalefactors"]
        )
        self.check_allowed_values(
            "contrasts", ["resolution"], self.resolutions.get_names(), self._all_names["resolutions"]
        )
        self.check_allowed_values(
            "contrasts", ["domain_ratio"], self.domain_ratios.get_names(), self._all_names["domain_ratios"]
        )

        self.check_contrast_model_allowed_values(
            "contrasts",
            getattr(self, self._contrast_model_field).get_names(),
            self._all_names[self._contrast_model_field],
            self._contrast_model_field,
        )
        self.check_contrast_model_allowed_values(
            "domain_contrasts", self.layers.get_names(), self._all_names["layers"], "layers"
        )
        return self

    @model_validator(mode="after")
    def check_protected_parameters(self) -> "Project":
        """Protected parameters should not be deleted. If this is attempted, raise an error."""
        for class_list in parameter_class_lists:
            protected_parameters = [
                param.name for param in getattr(self, class_list) if isinstance(param, ratapi.models.ProtectedParameter)
            ]
            # All previously existing protected parameters should be present in new list
            if not all(element in protected_parameters for element in self._protected_parameters[class_list]):
                removed_params = [
                    param for param in self._protected_parameters[class_list] if param not in protected_parameters
                ]
                raise ValueError(f"Can't delete the protected parameters: {', '.join(str(i) for i in removed_params)}")
        self._protected_parameters = self.get_all_protected_parameters()
        return self

    @model_validator(mode="after")
    def update_names(self) -> "Project":
        """Following validation, update the list of all parameter names."""
        self._all_names = self.get_all_names()
        return self

    def __str__(self):
        output = ""
        for key, value in self.__dict__.items():
            if value:
                output += f"{key.replace('_', ' ').title() + ': ':-<100}\n\n"
                try:
                    output += value.value + "\n\n"  # For enums
                except AttributeError:
                    output += str(value) + "\n\n"
        return output

    def get_all_names(self):
        """Record the names of all models defined in the project."""
        return {class_list: getattr(self, class_list).get_names() for class_list in class_lists}

    def get_all_protected_parameters(self):
        """Record the protected parameters defined in the project."""
        return {
            class_list: [
                param.name for param in getattr(self, class_list) if isinstance(param, ratapi.models.ProtectedParameter)
            ]
            for class_list in parameter_class_lists
        }

    def check_allowed_values(
        self, attribute: str, field_list: list[str], allowed_values: list[str], previous_values: list[str]
    ) -> None:
        """Check the values of the given fields in the given model are in the supplied list of allowed values.

        Parameters
        ----------
        attribute : str
            The attribute of Project being validated.
        field_list : list [str]
            The fields of the attribute to be checked for valid values.
        allowed_values : list [str]
            The list of allowed values for the fields given in field_list.
        previous_values : list [str]
            The list of allowed values for the fields given in field_list after the previous validation.

        Raises
        ------
        ValueError
            Raised if any field in field_list has a value not specified in allowed_values.

        """
        class_list = getattr(self, attribute)
        for index, model in enumerate(class_list):
            for field in field_list:
                value = getattr(model, field, "")
                if value and value not in allowed_values:
                    if value in previous_values:
                        raise ValueError(
                            f'The value "{value}" used in the "{field}" field of {attribute}[{index}] must be defined '
                            f'in "{values_defined_in[f"{attribute}.{field}"]}". Please remove "{value}" from '
                            f'"{attribute}[{index}].{field}" before attempting to delete it.',
                        )
                    else:
                        raise ValueError(
                            f'The value "{value}" used in the "{field}" field of {attribute}[{index}] must be defined '
                            f'in "{values_defined_in[f"{attribute}.{field}"]}". Please add "{value}" to '
                            f'"{values_defined_in[f"{attribute}.{field}"]}" before including it in "{attribute}".',
                        )

    def check_allowed_source(self, attribute: str) -> None:
        """Check that the source of a background or resolution is defined in the relevant field for its type.

        - A constant background or resolution source should be defined in
          `background_parameters` or `resolution_parameters` respectively;
        - A data background source should be defined in `data`
        - A function background source should be defined in `custom_files`

        Parameters
        ----------
        attribute : str
            The attribute of Project being validated.

        Raises
        ------
        ValueError
            Raised if any field in field_list has a value not specified in allowed_constants or allowed_data as
            appropriate.

        """
        class_list = getattr(self, attribute)
        for index, model in enumerate(class_list):
            if model.type == TypeOptions.Constant:
                allowed_values = getattr(self, f"{attribute[:-1]}_parameters").get_names()
                previous_values = self._all_names[f"{attribute[:-1]}_parameters"]
            elif model.type == TypeOptions.Data:
                allowed_values = self.data.get_names()
                previous_values = self._all_names["data"]
            else:
                allowed_values = self.custom_files.get_names()
                previous_values = self._all_names["custom_files"]

            if (value := model.source) != "" and value not in allowed_values:
                if value in previous_values:
                    raise ValueError(
                        f'The value "{value}" used in the "source" field of {attribute}[{index}] must be defined in '
                        f'"{values_defined_in[f"{attribute}.{model.type}.source"]}". Please remove "{value}" from '
                        f'"{attribute}[{index}].source" before attempting to delete it.',
                    )
                else:
                    raise ValueError(
                        f'The value "{value}" used in the "source" field of {attribute}[{index}] must be defined in '
                        f'"{values_defined_in[f"{attribute}.{model.type}.source"]}". Please add "{value}" to '
                        f'"{values_defined_in[f"{attribute}.{model.type}.source"]}" before including it in '
                        f'"{attribute}".',
                    )

    def check_contrast_model_allowed_values(
        self,
        contrast_attribute: str,
        allowed_values: list[str],
        previous_values: list[str],
        allowed_field: str,
    ) -> None:
        """Ensure the contents of the ``model`` for a contrast or domain contrast exist in the required project fields.

        Parameters
        ----------
        contrast_attribute : str
            The specific contrast attribute of Project being validated (either "contrasts" or "domain_contrasts").
        allowed_values : list [str]
            The list of allowed values for the model of the contrast_attribute.
        previous_values : list [str]
            The list of allowed values for the model of the contrast_attribute after the previous validation.
        allowed_field : str
            The name of the field in the project in which the allowed_values are defined.

        Raises
        ------
        ValueError
            Raised if any model in contrast_attribute has a value not specified in allowed_values.

        """
        class_list = getattr(self, contrast_attribute)
        for index, contrast in enumerate(class_list):
            if (model_values := contrast.model) and (missing_values := list(set(model_values) - set(allowed_values))):
                if all(value in previous_values for value in model_values):
                    raise ValueError(
                        f"The value{'s' if len(missing_values) > 1 else ''}: "
                        f'"{", ".join(str(i) for i in missing_values)}" used in the "model" field of '
                        f'{contrast_attribute}[{index}] must be defined in "{allowed_field}". Please remove all '
                        f'unnecessary values from "model" before attempting to delete them.',
                    )
                else:
                    raise ValueError(
                        f"The value{'s' if len(missing_values) > 1 else ''}: "
                        f'"{", ".join(str(i) for i in missing_values)}" used in the "model" field of '
                        f'{contrast_attribute}[{index}] must be defined in "{allowed_field}". Please add all '
                        f'required values to "{allowed_field}" before including them in "{contrast_attribute}".',
                    )

    def get_contrast_model_field(self):
        """Get the field used to define the contents of the "model" field in contrasts.

        Returns
        -------
        model_field : str
            The name of the field used to define the contrasts' model field.

        """
        if self.model == LayerModels.StandardLayers:
            if self.calculation == Calculations.Domains:
                model_field = "domain_contrasts"
            else:
                model_field = "layers"
        else:
            model_field = "custom_files"
        return model_field

    def set_prior_visibility(self, priors_visible: bool):
        """Set whether priors are visible or invisible for all parameters.

        Parameters
        ----------
        priors_visible : bool
            Whether priors should be shown.

        """
        for classlist_name in parameter_class_lists:
            classlist = getattr(self, classlist_name)
            for i in range(0, len(classlist)):
                classlist[i].show_priors = priors_visible

    def show_priors(self):
        """Show priors for all parameters in the model."""
        # convenience function from set_prior_visibility
        self.set_prior_visibility(True)

    def hide_priors(self):
        """Hide priors for all parameters in the model."""
        # convenience function from set_prior_visibility
        self.set_prior_visibility(False)

    def write_script(self, obj_name: str = "problem", script: str = "project_script.py"):
        """Write a python script that can be run to reproduce this project object.

        Parameters
        ----------
        obj_name : str, optional
            The name given to the project object under construction (default is "problem").
        script : str, optional
            The filepath of the generated script (default is "project_script.py").

        """
        # Need to ensure correct format for script name
        script_path = Path(script)
        if script_path.suffix == "":
            script_path = script_path.with_suffix(".py")
        elif script_path.suffix != ".py":
            raise ValueError('The script name provided to "write_script" must use the ".py" format')

        def write_item(item):
            """Write a model item as a string of a dictionary."""
            if isinstance(item, ratapi.models.Data):
                item_str = "{" + f"'name': '{item.name}',"
                if not np.any(item.data):  # if array is empty, e.g. in simulation data
                    item_str += "'data': empty([0, 3]), "
                else:
                    item_str += f"'data': {repr(item.data)}, "
                if len(item.data_range) != 0:
                    item_str += f"'data_range': {item.data_range}, "
                if len(item.simulation_range) != 0:
                    item_str += f"'simulation_range': {item.simulation_range}"
                item_str += "}"
                return item_str

            if isinstance(item, ratapi.models.CustomFile):
                # convert file paths to strings because pathlib gets too specific
                item_str = (
                    "{"
                    + f"'name': '{item.name}', "
                    + f"'filename': '{item.filename}', "
                    + f"'function_name': '{item.function_name}', "
                    + f"'language': '{str(item.language)}', "
                    # Raw string to ensure backslash is not interpreted as escape
                    + f"'path': r'{str(try_relative_to(item.path, script_path.parent))}'"
                    + "}"
                )
                return item_str
            # converting a dict to a string doesn't automatically convert StrEnums to str :(
            return str({key: str(value) if isinstance(value, Enum) else value for key, value in dict(item).items()})

        def classlist_script(name, classlist):
            """Write a classlist as a script."""
            return f"{name}=[\n    " + "\n    ".join([write_item(item) + "," for item in classlist.data]) + "],"

        script_path.write_text(
            f"""# THIS FILE IS GENERATED FROM RAT VIA THE "WRITE_SCRIPT" ROUTINE. IT IS NOT PART OF THE RAT CODE.

import ratapi
from numpy import array, empty, inf

{obj_name} = ratapi.Project(
    name="{self.name}",
    calculation="{self.calculation}",
    model="{self.model}",
    geometry="{self.geometry}",
    absorption="{self.absorption}",
"""
            + indent(
                "\n".join([classlist_script(class_list, getattr(self, class_list)) for class_list in class_lists]),
                " " * 4,
            )
            + "\n)"
        )

    def save(self, filepath: str | Path = "./project.json"):
        """Save a project to a JSON file.

        Parameters
        ----------
        filepath : str or Path
            The path to where the project file will be written.
        """
        filepath = Path(filepath).with_suffix(".json")

        json_dict = {}
        for field in self.model_fields:
            attr = getattr(self, field)

            if field == "data":

                def make_data_dict(item):
                    return {
                        "name": item.name,
                        "data": item.data.tolist(),
                        "data_range": item.data_range,
                        "simulation_range": item.simulation_range,
                    }

                json_dict["data"] = [make_data_dict(data) for data in attr]

            elif field == "custom_files":

                def make_custom_file_dict(item):
                    return {
                        "name": item.name,
                        "filename": item.filename,
                        "language": item.language,
                        "path": try_relative_to(item.path, filepath.parent),
                    }

                json_dict["custom_files"] = [make_custom_file_dict(file) for file in attr]

            elif isinstance(attr, ClassList):
                json_dict[field] = [item.model_dump() for item in attr]
            else:
                json_dict[field] = attr

        filepath.write_text(json.dumps(json_dict))

    @classmethod
    def load(cls, path: str | Path) -> "Project":
        """Load a project from file.

        Parameters
        ----------
        path : str or Path
            The path to the project file.

        """
        path = Path(path)
        input_data = path.read_text()
        model_dict = json.loads(input_data)
        for dataset in model_dict["data"]:
            if dataset["name"] == "Simulation":
                dataset["data"] = np.empty([0, 3])
                del dataset["data_range"]
            else:
                data = dataset["data"]
                dataset["data"] = np.array(data)

        # file paths are saved as relative to the project directory
        for file in model_dict["custom_files"]:
            if not Path(file["path"]).is_absolute():
                file["path"] = Path(path.parent, file["path"]).resolve()

        return cls.model_validate(model_dict)

    def _classlist_wrapper(self, class_list: ClassList, func: Callable):
        """Define the function used to wrap around ClassList routines to force revalidation.

        Parameters
        ----------
        class_list : ClassList
            The ClassList defined in the "Project" model that is being modified.
        func : Callable
            The routine being wrapped.

        Returns
        -------
        wrapped_func : Callable
            The wrapped routine.

        """

        @functools.wraps(func)
        def wrapped_func(*args, **kwargs):
            """Run the given function and then revalidate the "Project" model.

            If any exception is raised, restore the previous state of the given ClassList
            and report details of the exception.

            """
            previous_state = copy.deepcopy(class_list.data)
            return_value = None
            try:
                return_value = func(*args, **kwargs)
                Project.model_validate(self)
            except ValidationError as exc:
                class_list.data = previous_state
                custom_error_list = custom_pydantic_validation_error(exc.errors(include_url=False))
                raise ValidationError.from_exception_data(exc.title, custom_error_list, hide_input=True) from None
            except (TypeError, ValueError):
                class_list.data = previous_state
                raise
            finally:
                del previous_state
            return return_value

        return wrapped_func


def try_relative_to(path: Path, relative_to: Path) -> str:
    """Attempt to create a relative path and warn the user if it isn't possible.

    Parameters
    ----------
    path : Path
        The path to try to find a relative path for.
    relative_to: Path
        The path to which we find a relative path for ``path``.

    Returns
    -------
    str
        The relative path if successful, else the absolute path.

    """
    path = Path(path)
    relative_to = Path(relative_to)
    if path.is_relative_to(relative_to):
        return str(path.relative_to(relative_to))
    else:
        warnings.warn(
            "Could not write custom file path as relative to the project directory, "
            "which means that it may not work on other devices. If you would like to share your project, "
            "make sure your custom files are in a subfolder of the project save location.",
            stacklevel=2,
        )
        return str(path.resolve())
