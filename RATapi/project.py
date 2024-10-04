"""The project module. Defines and stores all the input data required for reflectivity calculations in RAT."""

import collections
import copy
import functools
from enum import Enum
from pathlib import Path
from textwrap import indent
from typing import Annotated, Any, Callable, Union

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
from typing_extensions import get_args, get_origin

import RATapi.models
from RATapi.classlist import ClassList
from RATapi.utils.custom_errors import custom_pydantic_validation_error
from RATapi.utils.enums import Calculations, Geometries, LayerModels, Priors, TypeOptions


# note for these discriminators that the before-validator discriminate_ambiguous_dicts
# guarantees we don't run into the ambiguous case of a sequence of dicts
def discriminate_layers(layer_input):
    """Union discriminator for layers."""
    if isinstance(layer_input, collections.abc.Sequence):
        # if classlist is empty, just label it as no absorption and it'll get fixed in post_init
        if len(layer_input) > 0 and isinstance(layer_input[0], RATapi.models.AbsorptionLayer):
            return "abs"
        return "no_abs"


def discriminate_contrasts(contrast_input):
    """Union discriminator for contrasts."""
    if isinstance(contrast_input, collections.abc.Sequence):
        # if classlist is empty, just label it as no ratio and it'll get fixed in post_init
        if len(contrast_input) > 0 and isinstance(contrast_input[0], RATapi.models.ContrastWithRatio):
            return "ratio"
        return "no_ratio"


values_defined_in = {
    "backgrounds.constant.value_1": "background_parameters",
    "backgrounds.constant.value_2": "background_parameters",
    "backgrounds.constant.value_3": "background_parameters",
    "backgrounds.constant.value_4": "background_parameters",
    "backgrounds.constant.value_5": "background_parameters",
    "backgrounds.data.value_1": "data",
    "backgrounds.data.value_2": "data",
    "backgrounds.data.value_3": "data",
    "backgrounds.data.value_4": "data",
    "backgrounds.data.value_5": "data",
    "resolutions.constant.value_1": "resolution_parameters",
    "resolutions.constant.value_2": "resolution_parameters",
    "resolutions.constant.value_3": "resolution_parameters",
    "resolutions.constant.value_4": "resolution_parameters",
    "resolutions.constant.value_5": "resolution_parameters",
    "resolutions.data.value_1": "data",
    "resolutions.data.value_2": "data",
    "resolutions.data.value_3": "data",
    "resolutions.data.value_4": "data",
    "resolutions.data.value_5": "data",
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
    "background_parameters": AllFields("backgrounds", ["value_1", "value_2", "value_3", "value_4", "value_5"]),
    "resolution_parameters": AllFields("resolutions", ["value_1", "value_2", "value_3", "value_4", "value_5"]),
    "parameters": AllFields("layers", ["thickness", "SLD", "SLD_real", "SLD_imaginary", "roughness", "hydration"]),
    "data": AllFields("contrasts", ["data"]),
    "backgrounds": AllFields("contrasts", ["background"]),
    "bulk_in": AllFields("contrasts", ["bulk_in"]),
    "bulk_out": AllFields("contrasts", ["bulk_out"]),
    "scalefactors": AllFields("contrasts", ["scalefactor"]),
    "domain_ratios": AllFields("contrasts", ["domain_ratio"]),
    "resolutions": AllFields("contrasts", ["resolution"]),
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


class Project(BaseModel, validate_assignment=True, extra="forbid"):
    """Defines the input data for a reflectivity calculation in RAT.

    This class combines the data defined in each of the pydantic models included in "models.py" into the full set of
    inputs required for a reflectivity calculation.
    """

    name: str = ""
    calculation: Calculations = Calculations.NonPolarised
    model: LayerModels = LayerModels.StandardLayers
    geometry: Geometries = Geometries.AirSubstrate
    absorption: bool = False

    parameters: ClassList[RATapi.models.Parameter] = ClassList()

    bulk_in: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    bulk_out: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    scalefactors: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    domain_ratios: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    background_parameters: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    backgrounds: ClassList[RATapi.models.Background] = ClassList(
        RATapi.models.Background(name="Background 1", type=TypeOptions.Constant, value_1="Background Param 1"),
    )

    resolution_parameters: ClassList[RATapi.models.Parameter] = ClassList(
        RATapi.models.Parameter(
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

    resolutions: ClassList[RATapi.models.Resolution] = ClassList(
        RATapi.models.Resolution(name="Resolution 1", type=TypeOptions.Constant, value_1="Resolution Param 1"),
    )

    custom_files: ClassList[RATapi.models.CustomFile] = ClassList()
    data: ClassList[RATapi.models.Data] = ClassList()
    layers: Union[
        Annotated[ClassList[RATapi.models.Layer], Tag("no_abs")],
        Annotated[ClassList[RATapi.models.AbsorptionLayer], Tag("abs")],
    ] = Field(
        default=ClassList(),
        discriminator=Discriminator(
            discriminate_layers,
            custom_error_type="invalid_union_member",
            custom_error_message="Input should be an instance of ClassList",
            custom_error_context={"discriminator": "absorption_or_no"},
        ),
    )
    domain_contrasts: ClassList[RATapi.models.DomainContrast] = ClassList()
    contrasts: Union[
        Annotated[ClassList[RATapi.models.Contrast], Tag("no_ratio")],
        Annotated[ClassList[RATapi.models.ContrastWithRatio], Tag("ratio")],
    ] = Field(
        default=ClassList(),
        discriminator=Discriminator(
            discriminate_contrasts,
            custom_error_type="invalid_union_member",
            custom_error_message="Input should be an instance of ClassList.",
            custom_error_context={"discriminator": "ratio_or_no_ratio"},
        ),
    )

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
            layer_model = RATapi.models.AbsorptionLayer if data.get("absorption", False) else RATapi.models.Layer
            if data.get("calculation", Calculations.NonPolarised) == Calculations.Domains:
                contrast_model = RATapi.models.ContrastWithRatio
            else:
                contrast_model = RATapi.models.Contrast

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
        model = getattr(RATapi.models, model_name)
        if not all(isinstance(element, model) for element in value):
            raise ValueError(
                f'"The layers attribute contains {other_model}s, '
                f'but the absorption parameter is {info.data["absorption"]}. '
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
        model = getattr(RATapi.models, model_name)
        if not all(isinstance(element, model) for element in value):
            raise ValueError(
                f'"The contrasts attribute contains contrasts {error_word} ratio, '
                f'but the calculation is {str(info.data["calculation"])}", '
                f"The attribute should be a ClassList of {model_name} instead."
            )

        return value

    def model_post_init(self, __context: Any) -> None:
        """Initialises the class in the ClassLists for empty data fields, sets protected parameters, gets names of all
        defined parameters, determines the contents of the "model" field in contrasts, and wraps ClassList routines to
        control revalidation.
        """
        # Ensure all ClassLists have the correct _class_handle defined
        for field in (fields := self.model_fields):
            type = fields[field].annotation
            if get_origin(type) == ClassList:
                classlist = getattr(self, field)
                if not hasattr(field, "_class_handle"):
                    classlist._class_handle = get_args(type)[0]

        layers_field = self.layers
        if not hasattr(layers_field, "_class_handle"):
            if self.absorption:
                layers_field._class_handle = RATapi.models.AbsorptionLayer
            else:
                layers_field._class_handle = RATapi.models.Layer

        contrast_field = self.contrasts
        if not hasattr(contrast_field, "_class_handle"):
            if self.calculation == Calculations.Domains:
                contrast_field._class_handle = RATapi.models.ContrastWithRatio
            else:
                contrast_field._class_handle = RATapi.models.Contrast

        if "Substrate Roughness" not in [name.title() for name in self.parameters.get_names()]:
            self.parameters.insert(
                0,
                RATapi.models.ProtectedParameter(
                    name="Substrate Roughness",
                    min=1.0,
                    value=3.0,
                    max=5.0,
                    fit=True,
                    prior_type=RATapi.models.Priors.Uniform,
                    mu=0.0,
                    sigma=np.inf,
                ),
            )
        elif "Substrate Roughness" not in [name.title() for name in self.get_all_protected_parameters()["parameters"]]:
            # If substrate roughness is included as a standard parameter replace it with a protected parameter
            substrate_roughness_values = self.parameters["Substrate Roughness"].model_dump()
            self.parameters.remove("Substrate Roughness")
            self.parameters.insert(0, RATapi.models.ProtectedParameter(**substrate_roughness_values))

        if "Simulation" not in [name.title() for name in self.data.get_names()]:
            self.data.insert(0, RATapi.models.Data(name="Simulation", simulation_range=[0.005, 0.7]))

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
        """If we are not running a domains calculation with standard layers, ensure the domain_contrasts component of
        the model is empty.
        """
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
    def set_calculation(self) -> "Project":
        """Apply the calc setting to the project."""
        contrast_list = []
        handle = self.contrasts._class_handle.__name__
        if self.calculation == Calculations.Domains and handle == "Contrast":
            for contrast in self.contrasts:
                contrast_list.append(RATapi.models.ContrastWithRatio(**contrast.model_dump()))
            self.contrasts.data = contrast_list
            self.domain_ratios.data = [
                RATapi.models.Parameter(
                    name="Domain Ratio 1",
                    min=0.4,
                    value=0.5,
                    max=0.6,
                    fit=False,
                    prior_type=RATapi.models.Priors.Uniform,
                    mu=0.0,
                    sigma=np.inf,
                ),
            ]
            self.contrasts._class_handle = RATapi.models.ContrastWithRatio
        elif self.calculation != Calculations.Domains and handle == "ContrastWithRatio":
            for contrast in self.contrasts:
                contrast_params = contrast.model_dump()
                del contrast_params["domain_ratio"]
                contrast_list.append(RATapi.models.Contrast(**contrast_params))
            self.contrasts.data = contrast_list
            self.contrasts._class_handle = RATapi.models.Contrast
        return self

    @model_validator(mode="after")
    def set_contrast_model_field(self) -> "Project":
        """The contents of the "model" field of "contrasts" depend on the values of the "calculation" and "model_type"
        defined in the project. If they have changed, clear the contrast models.
        """
        model_field = self.get_contrast_model_field()
        if model_field != self._contrast_model_field:
            for contrast in self.contrasts:
                contrast.model = []
            self._contrast_model_field = model_field
        return self

    @model_validator(mode="after")
    def check_contrast_model_length(self) -> "Project":
        """Given certain values of the "calculation" and "model" defined in the project, the "model" field of
        "contrasts" may be constrained in its length.
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
                layer_list.append(RATapi.models.AbsorptionLayer(**layer.model_dump()))
            self.layers.data = layer_list
            self.layers._class_handle = RATapi.models.AbsorptionLayer
        elif not self.absorption and handle == "AbsorptionLayer":
            for layer in self.layers:
                layer_params = layer.model_dump()
                del layer_params["SLD_imaginary"]
                layer_list.append(RATapi.models.Layer(**layer_params))
            self.layers.data = layer_list
            self.layers._class_handle = RATapi.models.Layer
        return self

    @model_validator(mode="after")
    def update_renamed_models(self) -> "Project":
        """When models defined in the ClassLists are renamed, we need to update that name elsewhere in the project."""
        for class_list in model_names_used_in:
            old_names = self._all_names[class_list]
            new_names = getattr(self, class_list).get_names()
            if len(old_names) == len(new_names):
                name_diff = [(old, new) for (old, new) in zip(old_names, new_names) if old != new]
                for old_name, new_name in name_diff:
                    model_names_list = getattr(self, model_names_used_in[class_list].attribute)
                    all_matches = model_names_list.get_all_matches(old_name)
                    fields = model_names_used_in[class_list].fields
                    for index, field in all_matches:
                        if field in fields:
                            setattr(model_names_list[index], field, new_name)
        self._all_names = self.get_all_names()
        return self

    @model_validator(mode="after")
    def cross_check_model_values(self) -> "Project":
        """Certain model fields should contain values defined elsewhere in the project."""
        value_fields = ["value_1", "value_2", "value_3", "value_4", "value_5"]
        self.check_allowed_background_resolution_values(
            "backgrounds", value_fields, self.background_parameters.get_names(), self.data.get_names()
        )
        self.check_allowed_background_resolution_values(
            "resolutions", value_fields, self.resolution_parameters.get_names(), self.data.get_names()
        )

        self.check_allowed_values(
            "layers",
            ["thickness", "SLD", "SLD_real", "SLD_imaginary", "roughness"],
            self.parameters.get_names(),
        )

        self.check_allowed_values("contrasts", ["data"], self.data.get_names())
        self.check_allowed_values("contrasts", ["background"], self.backgrounds.get_names())
        self.check_allowed_values("contrasts", ["bulk_in"], self.bulk_in.get_names())
        self.check_allowed_values("contrasts", ["bulk_out"], self.bulk_out.get_names())
        self.check_allowed_values("contrasts", ["scalefactor"], self.scalefactors.get_names())
        self.check_allowed_values("contrasts", ["resolution"], self.resolutions.get_names())
        self.check_allowed_values("contrasts", ["domain_ratio"], self.domain_ratios.get_names())

        self.check_contrast_model_allowed_values(
            "contrasts",
            getattr(self, self._contrast_model_field).get_names(),
            self._contrast_model_field,
        )
        self.check_contrast_model_allowed_values("domain_contrasts", self.layers.get_names(), "layers")
        return self

    @model_validator(mode="after")
    def check_protected_parameters(self) -> "Project":
        """Protected parameters should not be deleted. If this is attempted, raise an error."""
        for class_list in parameter_class_lists:
            protected_parameters = [
                param.name for param in getattr(self, class_list) if isinstance(param, RATapi.models.ProtectedParameter)
            ]
            # All previously existing protected parameters should be present in new list
            if not all(element in protected_parameters for element in self._protected_parameters[class_list]):
                removed_params = [
                    param for param in self._protected_parameters[class_list] if param not in protected_parameters
                ]
                raise ValueError(f'Can\'t delete the protected parameters: {", ".join(str(i) for i in removed_params)}')
        self._protected_parameters = self.get_all_protected_parameters()
        return self

    def __str__(self):
        output = ""
        for key, value in self.__dict__.items():
            if value:
                output += f'{key.replace("_", " ").title() + ": " :-<100}\n\n'
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
                param.name for param in getattr(self, class_list) if isinstance(param, RATapi.models.ProtectedParameter)
            ]
            for class_list in parameter_class_lists
        }

    def check_allowed_values(self, attribute: str, field_list: list[str], allowed_values: list[str]) -> None:
        """Check the values of the given fields in the given model are in the supplied list of allowed values.

        Parameters
        ----------
        attribute : str
            The attribute of Project being validated.
        field_list : list [str]
            The fields of the attribute to be checked for valid values.
        allowed_values : list [str]
            The list of allowed values for the fields given in field_list.

        Raises
        ------
        ValueError
            Raised if any field in field_list has a value not specified in allowed_values.

        """
        class_list = getattr(self, attribute)
        for model in class_list:
            for field in field_list:
                value = getattr(model, field, "")
                if value and value not in allowed_values:
                    raise ValueError(
                        f'The value "{value}" in the "{field}" field of "{attribute}" must be defined in '
                        f'"{values_defined_in[f"{attribute}.{field}"]}".',
                    )

    def check_allowed_background_resolution_values(
        self, attribute: str, field_list: list[str], allowed_constants: list[str], allowed_data: list[str]
    ) -> None:
        """Check the values of the given fields in the given model are in the supplied list of allowed values.

        For backgrounds and resolutions, the list of allowed values depends on whether the type of the
        background/resolution is "constant" or "data".

        Parameters
        ----------
        attribute : str
            The attribute of Project being validated.
        field_list : list [str]
            The fields of the attribute to be checked for valid values.
        allowed_constants : list [str]
            The list of allowed values for the fields given in field_list if the type is "constant".
        allowed_data : list [str]
            The list of allowed values for the fields given in field_list if the type is "data".

        Raises
        ------
        ValueError
            Raised if any field in field_list has a value not specified in allowed_constants or allowed_data as
            appropriate.

        """
        class_list = getattr(self, attribute)
        for model in class_list:
            if model.type == TypeOptions.Constant:
                allowed_values = allowed_constants
            elif model.type == TypeOptions.Data:
                allowed_values = allowed_data
            else:
                raise ValueError('"Function" type backgrounds and resolutions are not yet supported.')

            for field in field_list:
                value = getattr(model, field, "")
                if value and value not in allowed_values:
                    raise ValueError(
                        f'The value "{value}" in the "{field}" field of "{attribute}" must be defined in '
                        f'"{values_defined_in[f"{attribute}.{model.type}.{field}"]}".',
                    )

    def check_contrast_model_allowed_values(
        self,
        contrast_attribute: str,
        allowed_values: list[str],
        allowed_field: str,
    ) -> None:
        """The contents of the "model" field of "contrasts" and "domain_contrasts" must be defined elsewhere in the
        project.

        Parameters
        ----------
        contrast_attribute : str
            The specific contrast attribute of Project being validated (either "contrasts" or "domain_contrasts").
        allowed_values : list [str]
            The list of allowed values for the model of the contrast_attribute.
        allowed_field : str
            The name of the field in the project in which the allowed_values are defined.

        Raises
        ------
        ValueError
            Raised if any model in contrast_attribute has a value not specified in allowed_values.

        """
        class_list = getattr(self, contrast_attribute)
        for contrast in class_list:
            model_values = contrast.model
            if model_values and not all(value in allowed_values for value in model_values):
                raise ValueError(
                    f'The values: "{", ".join(str(i) for i in model_values)}" in the "model" field of '
                    f'"{contrast_attribute}" must be defined in "{allowed_field}".',
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
            if isinstance(item, RATapi.models.Data):
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

            if isinstance(item, RATapi.models.CustomFile):
                # convert file paths to strings because pathlib gets too specific
                item_str = (
                    "{"
                    + f"'name': '{item.name}', "
                    + f"'filename': '{item.filename}', "
                    + f"'function_name': '{item.function_name}', "
                    + f"'language': '{str(item.language)}', "
                    + f"'path': '{str(item.path)}'"
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

import RATapi
from numpy import array, empty, inf

{obj_name} = RATapi.Project(
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

    def _classlist_wrapper(self, class_list: ClassList, func: Callable):
        """Defines the function used to wrap around ClassList routines to force revalidation.

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
            """Run the given function and then revalidate the "Project" model. If any exception is raised, restore
            the previous state of the given ClassList and report details of the exception.
            """
            previous_state = copy.deepcopy(class_list.data)
            return_value = None
            try:
                return_value = func(*args, **kwargs)
                Project.model_validate(self)
            except ValidationError as exc:
                class_list.data = previous_state
                custom_error_list = custom_pydantic_validation_error(exc.errors())
                raise ValidationError.from_exception_data(exc.title, custom_error_list, hide_input=True) from None
            except (TypeError, ValueError):
                class_list.data = previous_state
                raise
            finally:
                del previous_state
            return return_value

        return wrapped_func
