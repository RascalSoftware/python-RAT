"""The project module. Defines and stores all the input data required for reflectivity calculations in RAT."""

import collections
import copy
import functools
import numpy as np
from pydantic import BaseModel, ValidationInfo, field_validator, model_validator, ValidationError
from typing import Any, Callable

from RAT.classlist import ClassList
import RAT.models
from RAT.utils.custom_errors import formatted_pydantic_error

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


class CalcTypes(StrEnum):
    NonPolarised = 'non polarised'
    Domains = 'domains'


class ModelTypes(StrEnum):
    CustomLayers = 'custom layers'
    CustomXY = 'custom xy'
    StandardLayers = 'standard layers'


class Geometries(StrEnum):
    AirSubstrate = 'air/substrate'
    SubstrateLiquid = 'substrate/liquid'


# Map project fields to pydantic models
model_in_classlist = {'parameters': 'Parameter',
                      'bulk_in': 'Parameter',
                      'bulk_out': 'Parameter',
                      'qz_shifts': 'Parameter',
                      'scalefactors': 'Parameter',
                      'domain_ratios': 'Parameter',
                      'background_parameters': 'Parameter',
                      'resolution_parameters': 'Parameter',
                      'backgrounds': 'Background',
                      'resolutions': 'Resolution',
                      'custom_files': 'CustomFile',
                      'data': 'Data',
                      'layers': 'Layer',
                      'domain_contrasts': 'DomainContrast',
                      'contrasts': 'Contrast',
                      }

values_defined_in = {'backgrounds.value_1': 'background_parameters',
                     'backgrounds.value_2': 'background_parameters',
                     'backgrounds.value_3': 'background_parameters',
                     'backgrounds.value_4': 'background_parameters',
                     'backgrounds.value_5': 'background_parameters',
                     'resolutions.value_1': 'resolution_parameters',
                     'resolutions.value_2': 'resolution_parameters',
                     'resolutions.value_3': 'resolution_parameters',
                     'resolutions.value_4': 'resolution_parameters',
                     'resolutions.value_5': 'resolution_parameters',
                     'layers.thickness': 'parameters',
                     'layers.SLD': 'parameters',
                     'layers.SLD_real': 'parameters',
                     'layers.SLD_imaginary': 'parameters',
                     'layers.roughness': 'parameters',
                     'contrasts.data': 'data',
                     'contrasts.background': 'backgrounds',
                     'contrasts.nba': 'bulk_in',
                     'contrasts.nbs': 'bulk_out',
                     'contrasts.scalefactor': 'scalefactors',
                     'contrasts.resolution': 'resolutions',
                     'contrasts.domain_ratio': 'domain_ratios',
                     }

AllFields = collections.namedtuple('AllFields', ['attribute', 'fields'])
model_names_used_in = {'background_parameters': AllFields('backgrounds', ['value_1', 'value_2', 'value_3', 'value_4',
                                                                          'value_5']),
                       'resolution_parameters': AllFields('resolutions', ['value_1', 'value_2', 'value_3', 'value_4',
                                                                          'value_5']),
                       'parameters': AllFields('layers', ['thickness', 'SLD', 'SLD_real', 'SLD_imaginary',
                                                          'roughness']),
                       'data': AllFields('contrasts', ['data']),
                       'backgrounds': AllFields('contrasts', ['background']),
                       'bulk_in': AllFields('contrasts', ['nba']),
                       'bulk_out': AllFields('contrasts', ['nbs']),
                       'scalefactors': AllFields('contrasts', ['scalefactor']),
                       'domain_ratios': AllFields('contrasts', ['domain_ratio']),
                       'resolutions': AllFields('contrasts', ['resolution']),
                       }

class_lists = ['parameters', 'bulk_in', 'bulk_out', 'qz_shifts', 'scalefactors', 'domain_ratios',
               'background_parameters', 'backgrounds', 'resolution_parameters', 'resolutions', 'custom_files', 'data',
               'layers', 'domain_contrasts', 'contrasts']


class Project(BaseModel, validate_assignment=True, extra='forbid', arbitrary_types_allowed=True):
    """Defines the input data for a reflectivity calculation in RAT.

    This class combines the data defined in each of the pydantic models included in "models.py" into the full set of
    inputs required for a reflectivity calculation.
    """
    name: str = ''
    calc_type: CalcTypes = CalcTypes.NonPolarised
    model: ModelTypes = ModelTypes.StandardLayers
    geometry: Geometries = Geometries.AirSubstrate
    absorption: bool = False

    parameters: ClassList = ClassList()

    bulk_in: ClassList = ClassList(RAT.models.Parameter(name='SLD Air', min=0.0, value=0.0, max=0.0, fit=False,
                                                        prior_type=RAT.models.Priors.Uniform, mu=0.0, sigma=np.inf))

    bulk_out: ClassList = ClassList(RAT.models.Parameter(name='SLD D2O', min=6.2e-6, value=6.35e-6, max=6.35e-6,
                                                         fit=False, prior_type=RAT.models.Priors.Uniform, mu=0.0,
                                                         sigma=np.inf))

    qz_shifts: ClassList = ClassList(RAT.models.Parameter(name='Qz shift 1', min=-1e-4, value=0.0, max=1e-4, fit=False,
                                                          prior_type=RAT.models.Priors.Uniform, mu=0.0, sigma=np.inf))

    scalefactors: ClassList = ClassList(RAT.models.Parameter(name='Scalefactor 1', min=0.02, value=0.23, max=0.25,
                                                             fit=False, prior_type=RAT.models.Priors.Uniform, mu=0.0,
                                                             sigma=np.inf))

    domain_ratios: ClassList = ClassList(RAT.models.Parameter(name='Domain Ratio 1', min=0.4, value=0.5, max=0.6,
                                                              fit=False, prior_type=RAT.models.Priors.Uniform, mu=0.0,
                                                              sigma=np.inf))

    background_parameters: ClassList = ClassList(RAT.models.Parameter(name='Background Param 1', min=1e-7, value=1e-6,
                                                                      max=1e-5, fit=False,
                                                                      prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                      sigma=np.inf))

    backgrounds: ClassList = ClassList(RAT.models.Background(name='Background 1', type=RAT.models.Types.Constant,
                                                             value_1='Background Param 1'))

    resolution_parameters: ClassList = ClassList(RAT.models.Parameter(name='Resolution Param 1', min=0.01, value=0.03,
                                                                      max=0.05, fit=False,
                                                                      prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                      sigma=np.inf))

    resolutions: ClassList = ClassList(RAT.models.Resolution(name='Resolution 1', type=RAT.models.Types.Constant,
                                                             value_1='Resolution Param 1'))

    custom_files: ClassList = ClassList()
    data: ClassList = ClassList(RAT.models.Data(name='Simulation'))
    layers: ClassList = ClassList()
    domain_contrasts: ClassList = ClassList()
    contrasts: ClassList = ClassList()

    _all_names: dict
    _contrast_model_field: str

    @field_validator('parameters', 'bulk_in', 'bulk_out', 'qz_shifts', 'scalefactors', 'background_parameters',
                     'backgrounds', 'resolution_parameters', 'resolutions', 'custom_files', 'data', 'layers',
                     'domain_contrasts', 'contrasts')
    @classmethod
    def check_class(cls, value: ClassList, info: ValidationInfo) -> ClassList:
        """Each of the data fields should be a ClassList of the appropriate model."""
        model_name = model_in_classlist[info.field_name]
        # Correct model name if necessary
        if info.field_name == 'layers' and info.data['absorption']:
            model_name = 'AbsorptionLayer'
        if info.field_name == 'contrasts' and info.data['calc_type'] == CalcTypes.Domains:
            model_name = 'ContrastWithRatio'

        model = getattr(RAT.models, model_name)
        if not all(isinstance(element, model) for element in value):
            raise ValueError(f'"{info.field_name}" ClassList contains objects other than "{model_name}"')
        return value

    def model_post_init(self, __context: Any) -> None:
        """Initialises the class in the ClassLists for empty data fields, sets protected parameters, gets names of all
        defined parameters, determines the contents of the "model" field in contrasts, and wraps ClassList routines to
        control revalidation.
        """
        # Ensure all ClassLists have the correct _class_handle defined
        layer_field = getattr(self, 'layers')
        if not hasattr(layer_field, "_class_handle"):
            if self.absorption:
                setattr(layer_field, "_class_handle", getattr(RAT.models, 'AbsorptionLayer'))
            else:
                setattr(layer_field, "_class_handle", getattr(RAT.models, 'Layer'))

        contrast_field = getattr(self, 'contrasts')
        if not hasattr(contrast_field, "_class_handle"):
            if self.calc_type == CalcTypes.Domains:
                setattr(contrast_field, "_class_handle", getattr(RAT.models, 'ContrastWithRatio'))
            else:
                setattr(contrast_field, "_class_handle", getattr(RAT.models, 'Contrast'))

        for field_name, model in model_in_classlist.items():
            field = getattr(self, field_name)
            if not hasattr(field, "_class_handle"):
                setattr(field, "_class_handle", getattr(RAT.models, model))

        self.parameters.insert(0, RAT.models.ProtectedParameter(name='Substrate Roughness', min=1.0, value=3.0, max=5.0,
                                                                fit=True, prior_type=RAT.models.Priors.Uniform, mu=0.0,
                                                                sigma=np.inf))

        self._all_names = self.get_all_names()
        self._contrast_model_field = self.get_contrast_model_field()

        # Wrap ClassList routines - when any of these routines are called, the wrapper will force revalidation of the
        # model, handle errors and reset previous values if necessary.
        methods_to_wrap = ['_setitem', '_delitem', '_iadd', 'append', 'insert', 'pop', 'remove', 'clear', 'extend',
                           'set_fields']

        for class_list in class_lists:
            attribute = getattr(self, class_list)
            for methodName in methods_to_wrap:
                setattr(attribute, methodName, self._classlist_wrapper(attribute, getattr(attribute, methodName)))

    @model_validator(mode='after')
    def set_domain_ratios(self) -> 'Project':
        """If we are not running a domains calculation, ensure the domain_ratios component of the model is empty."""
        if self.calc_type != CalcTypes.Domains:
            self.domain_ratios.data = []
        return self

    @model_validator(mode='after')
    def set_domain_contrasts(self) -> 'Project':
        """If we are not running a domains calculation with standard layers, ensure the domain_contrasts component of
        the model is empty.
        """
        if not (self.calc_type == CalcTypes.Domains and self.model == ModelTypes.StandardLayers):
            self.domain_contrasts.data = []
        return self

    @model_validator(mode='after')
    def set_layers(self) -> 'Project':
        """If we are not using a standard layers model, ensure the layers component of the model is empty."""
        if self.model != ModelTypes.StandardLayers:
            self.layers.data = []
        return self

    @model_validator(mode='after')
    def set_calc_type(self) -> 'Project':
        """Apply the calc_type setting to the project."""
        contrast_list = []
        handle = getattr(self.contrasts, '_class_handle').__name__
        if self.calc_type == CalcTypes.Domains and handle == 'Contrast':
            for contrast in self.contrasts:
                contrast_list.append(RAT.models.ContrastWithRatio(**contrast.model_dump()))
            self.contrasts.data = contrast_list
            setattr(self.contrasts, '_class_handle', getattr(RAT.models, 'ContrastWithRatio'))
        elif self.calc_type != CalcTypes.Domains and handle == 'ContrastWithRatio':
            for contrast in self.contrasts:
                contrast_params = contrast.model_dump()
                del contrast_params['domain_ratio']
                contrast_list.append(RAT.models.Contrast(**contrast_params))
            self.contrasts.data = contrast_list
            setattr(self.contrasts, '_class_handle', getattr(RAT.models, 'Contrast'))
        return self

    @model_validator(mode='after')
    def set_contrast_model_field(self) -> 'Project':
        """The contents of the "model" field of "contrasts" depend on the values of the "calc_type" and "model_type"
        defined in the project. If they have changed, clear the contrast models.
        """
        model_field = self.get_contrast_model_field()
        if model_field != self._contrast_model_field:
            for contrast in self.contrasts:
                contrast.model = []
            self._contrast_model_field = model_field
        return self

    @model_validator(mode='after')
    def check_contrast_model_length(self) -> 'Project':
        """Given certain values of the "calc_type" and "model" defined in the project, the "model" field of "contrasts"
        may be constrained in its length.
        """
        if self.model == ModelTypes.StandardLayers and self.calc_type == CalcTypes.Domains:
            for contrast in self.contrasts:
                if contrast.model and len(contrast.model) != 2:
                    raise ValueError('For a standard layers domains calculation the "model" field of "contrasts" must '
                                     'contain exactly two values.')
        elif self.model != ModelTypes.StandardLayers:
            for contrast in self.contrasts:
                if len(contrast.model) > 1:
                    raise ValueError('For a custom model calculation the "model" field of "contrasts" cannot contain '
                                     'more than one value.')
        return self

    @model_validator(mode='after')
    def set_absorption(self) -> 'Project':
        """Apply the absorption setting to the project."""
        layer_list = []
        handle = getattr(self.layers, '_class_handle').__name__
        if self.absorption and handle == 'Layer':
            for layer in self.layers:
                layer_list.append(RAT.models.AbsorptionLayer(**layer.model_dump()))
            self.layers.data = layer_list
            setattr(self.layers, '_class_handle', getattr(RAT.models, 'AbsorptionLayer'))
        elif not self.absorption and handle == 'AbsorptionLayer':
            for layer in self.layers:
                layer_params = layer.model_dump()
                del layer_params['SLD_imaginary']
                layer_list.append(RAT.models.Layer(**layer_params))
            self.layers.data = layer_list
            setattr(self.layers, '_class_handle', getattr(RAT.models, 'Layer'))
        return self

    @model_validator(mode='after')
    def update_renamed_models(self) -> 'Project':
        """When models defined in the ClassLists are renamed, we need to update that name elsewhere in the project."""
        for class_list in model_names_used_in.keys():
            old_names = self._all_names[class_list]
            new_names = getattr(self, class_list).get_names()
            if len(old_names) == len(new_names):
                name_diff = [(old, new) for (old, new) in zip(old_names, new_names) if old != new]
                for (old_name, new_name) in name_diff:
                    model_names_list = getattr(self, model_names_used_in[class_list].attribute)
                    all_matches = model_names_list.get_all_matches(old_name)
                    fields = model_names_used_in[class_list].fields
                    for (index, field) in all_matches:
                        if field in fields:
                            setattr(model_names_list[index], field, new_name)
        self._all_names = self.get_all_names()
        return self

    @model_validator(mode='after')
    def cross_check_model_values(self) -> 'Project':
        """Certain model fields should contain values defined elsewhere in the project."""
        value_fields = ['value_1', 'value_2', 'value_3', 'value_4', 'value_5']
        self.check_allowed_values('backgrounds', value_fields, self.background_parameters.get_names())
        self.check_allowed_values('resolutions', value_fields, self.resolution_parameters.get_names())
        self.check_allowed_values('layers', ['thickness', 'SLD', 'SLD_real', 'SLD_imaginary', 'roughness'],
                                  self.parameters.get_names())

        self.check_allowed_values('contrasts', ['data'], self.data.get_names())
        self.check_allowed_values('contrasts', ['background'], self.backgrounds.get_names())
        self.check_allowed_values('contrasts', ['nba'], self.bulk_in.get_names())
        self.check_allowed_values('contrasts', ['nbs'], self.bulk_out.get_names())
        self.check_allowed_values('contrasts', ['scalefactor'], self.scalefactors.get_names())
        self.check_allowed_values('contrasts', ['resolution'], self.resolutions.get_names())
        self.check_allowed_values('contrasts', ['domain_ratio'], self.domain_ratios.get_names())

        self.check_contrast_model_allowed_values('contrasts', getattr(self, self._contrast_model_field).get_names(),
                                                 self._contrast_model_field)
        self.check_contrast_model_allowed_values('domain_contrasts', self.layers.get_names(), 'layers')
        return self

    def __repr__(self):
        output = ''
        for key, value in self.__dict__.items():
            if value:
                output += f'{key.replace("_", " ").title() + ": " :-<100}\n\n'
                try:
                    value.value  # For enums
                except AttributeError:
                    output += repr(value) + '\n\n'
                else:
                    output += value.value + '\n\n'
        return output

    def get_all_names(self):
        """Record the names of all models defined in the project."""
        return {class_list: getattr(self, class_list).get_names() for class_list in class_lists}

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
                value = getattr(model, field, '')
                if value and value not in allowed_values:
                    raise ValueError(f'The value "{value}" in the "{field}" field of "{attribute}" must be defined in '
                                     f'"{values_defined_in[f"{attribute}.{field}"]}".')

    def check_contrast_model_allowed_values(self, contrast_attribute: str, allowed_values: list[str],
                                            allowed_field: str) -> None:
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
            model_values = getattr(contrast, 'model')
            if model_values and not all(value in allowed_values for value in model_values):
                raise ValueError(f'The values: "{", ".join(str(i) for i in model_values)}" in the "model" field of '
                                 f'"{contrast_attribute}" must be defined in "{allowed_field}".')

    def get_contrast_model_field(self):
        """Get the field used to define the contents of the "model" field in contrasts.

        Returns
        -------
        model_field : str
            The name of the field used to define the contrasts' model field.
        """
        if self.model == ModelTypes.StandardLayers:
            if self.calc_type == CalcTypes.Domains:
                model_field = 'domain_contrasts'
            else:
                model_field = 'layers'
        else:
            model_field = 'custom_files'
        return model_field

    def _classlist_wrapper(self, class_list: 'ClassList', func: Callable):
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
            previous_state = copy.deepcopy(getattr(class_list, 'data'))
            return_value = None
            try:
                return_value = func(*args, **kwargs)
                Project.model_validate(self)
            except ValidationError as e:
                setattr(class_list, 'data', previous_state)
                error_string = formatted_pydantic_error(e)
                # Use ANSI escape sequences to print error text in red
                print('\033[31m' + error_string + '\033[0m')
            except (TypeError, ValueError):
                setattr(class_list, 'data', previous_state)
                raise
            finally:
                del previous_state
            return return_value
        return wrapped_func
