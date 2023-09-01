"""The project module. Defines and stores all the input data required for reflectivity calculations in RAT."""

import copy
import functools
import numpy as np
from pydantic import BaseModel, FieldValidationInfo, field_validator, model_validator, ValidationError
from typing import Any, Callable

from RAT.classlist import ClassList
import RAT.models
from RAT.utils.CustomErrors import formatted_pydantic_error

try:
    from enum import StrEnum
except ImportError:
    from strenum import StrEnum


class CalcTypes(StrEnum):
    NonPolarised = 'non polarised'
    Domains = 'domains'
    OilWater = 'oil water'


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
                      'background_parameters': 'Parameter',
                      'resolution_parameters': 'Parameter',
                      'backgrounds': 'Background',
                      'resolutions': 'Resolution',
                      'custom_files': 'CustomFile',
                      'data': 'Data',
                      'layers': 'Layer',
                      'contrasts': 'Contrast'
                      }


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

    bulk_in: ClassList = ClassList(RAT.models.Parameter(name='SLD Air', min=0, value=0, max=0, fit=False,
                                                        prior_type=RAT.models.Priors.Uniform, mu=0, sigma=np.inf))

    bulk_out: ClassList = ClassList(RAT.models.Parameter(name='SLD D2O', min=6.2e-6, value=6.35e-6, max=6.35e-6,
                                                         fit=False, prior_type=RAT.models.Priors.Uniform, mu=0,
                                                         sigma=np.inf))

    qz_shifts: ClassList = ClassList(RAT.models.Parameter(name='Qz shift 1', min=-1e-4, value=0, max=1e-4, fit=False,
                                                          prior_type=RAT.models.Priors.Uniform, mu=0, sigma=np.inf))

    scalefactors: ClassList = ClassList(RAT.models.Parameter(name='Scalefactor 1', min=0.02, value=0.23, max=0.25,
                                                             fit=False, prior_type=RAT.models.Priors.Uniform, mu=0,
                                                             sigma=np.inf))

    background_parameters: ClassList = ClassList(RAT.models.Parameter(name='Background Param 1', min=1e-7, value=1e-6,
                                                                      max=1e-5, fit=False,
                                                                      prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                      sigma=np.inf))

    backgrounds: ClassList = ClassList(RAT.models.Background(name='Background 1', type=RAT.models.Types.Constant.value,
                                                             value_1='Background Param 1'))

    resolution_parameters: ClassList = ClassList(RAT.models.Parameter(name='Resolution Param 1', min=0.01, value=0.03,
                                                                      max=0.05, fit=False,
                                                                      prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                      sigma=np.inf))

    resolutions: ClassList = ClassList(RAT.models.Resolution(name='Resolution 1', type=RAT.models.Types.Constant.value,
                                                             value_1='Resolution Param 1'))

    custom_files: ClassList = ClassList()
    data: ClassList = ClassList(RAT.models.Data(name='Simulation'))
    layers: ClassList = ClassList()
    contrasts: ClassList = ClassList()

    @field_validator('parameters', 'bulk_in', 'bulk_out', 'qz_shifts', 'scalefactors', 'background_parameters',
                     'backgrounds', 'resolution_parameters', 'resolutions', 'custom_files', 'data', 'layers',
                     'contrasts')
    @classmethod
    def check_class(cls, value: ClassList, info: FieldValidationInfo) -> ClassList:
        """Each of the data fields should be a ClassList of the appropriate model."""
        model_name = model_in_classlist[info.field_name]
        model = getattr(RAT.models, model_name)
        assert all(isinstance(element, model) for element in value), \
            f'"{info.field_name}" ClassList contains objects other than "{model_name}"'
        return value

    def model_post_init(self, __context: Any) -> None:
        """Initialises the class in the ClassLists for empty data fields, sets protected parameters, and wraps
        ClassList routines to control revalidation."""
        for field_name, model in model_in_classlist.items():
            field = getattr(self, field_name)
            if not hasattr(field, "_class_handle"):
                setattr(field, "_class_handle", getattr(RAT.models, model))

        self.parameters.insert(0, RAT.models.ProtectedParameter(name='Substrate Roughness', min=1, value=3, max=5,
                                                                fit=True, prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                sigma=np.inf))

        # Wrap ClassList routines - when any of these routines are called, the wrapper will force revalidation of the
        # model, handle errors and reset previous values if necessary.
        class_lists = ['parameters', 'bulk_in', 'bulk_out', 'qz_shifts', 'scalefactors', 'background_parameters',
                       'backgrounds', 'resolution_parameters', 'resolutions', 'custom_files', 'data', 'layers',
                       'contrasts']
        methods_to_wrap = ['_setitem', '_delitem', '_iadd', 'append', 'insert', 'pop', 'remove', 'clear', 'extend']
        for class_list in class_lists:
            attribute = getattr(self, class_list)
            for methodName in methods_to_wrap:
                setattr(attribute, methodName, self._classlist_wrapper(attribute, getattr(attribute, methodName)))

    @model_validator(mode='after')
    def cross_check_model_values(self) -> 'Project':
        """Certain model fields should contain values defined elsewhere in the project."""
        value_fields = ['value_1', 'value_2', 'value_3', 'value_4', 'value_5']
        self.check_allowed_values('backgrounds', value_fields, self.background_parameters.get_names(),
                                  'background_parameters')
        self.check_allowed_values('resolutions', value_fields, self.resolution_parameters.get_names(),
                                  'resolution_parameters')
        self.check_allowed_values('layers', ['thickness', 'SLD', 'roughness'], self.parameters.get_names(),
                                  'parameters')

        self.check_allowed_values('contrasts', ['data'], self.data.get_names(), 'data')
        self.check_allowed_values('contrasts', ['background'], self.backgrounds.get_names(), 'backgrounds')
        self.check_allowed_values('contrasts', ['nba'], self.bulk_in.get_names(), 'bulk_in')
        self.check_allowed_values('contrasts', ['nbs'], self.bulk_out.get_names(), 'bulk_out')
        self.check_allowed_values('contrasts', ['scalefactor'], self.scalefactors.get_names(), 'scalefactors')
        self.check_allowed_values('contrasts', ['resolution'], self.resolutions.get_names(), 'resolutions')
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

    def check_allowed_values(self, attribute: str, field_list: list[str], allowed_values: list[str],
                             allowed_values_classlist: str) -> None:
        """Check the values of the given fields in the given model are in the supplied list of allowed values.

        Parameters
        ----------
        attribute : str
            The attribute of Project being validated.
        field_list : list [str]
            The fields of the attribute to be checked for valid values.
        allowed_values : list [str]
            The list of allowed values for the fields given in field_list.
        allowed_values_classlist : str
            The name of the ClassList containing the allowed values/

        Raises
        ------
        ValueError
            Raised if any field in field_list has a value not specified in allowed_values.
        """
        class_list = getattr(self, attribute)
        for model in class_list:
            for field in field_list:
                value = getattr(model, field)
                if value and value not in allowed_values:
                    raise ValueError(f'The value "{value}" in the "{field}" field of "{attribute}" must be defined in '
                                     f'"{allowed_values_classlist}".')

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
