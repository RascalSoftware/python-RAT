from enum import Enum
import math
from pydantic import AfterValidator, BaseModel
from typing import Annotated, Any

from RAT.classlist import ClassList
import RAT.models


class CalcTypes(str, Enum):
    NonPolarised = 'non polarised'
    Domains = 'domains'
    OilWater = 'oil water'
    Magnetic = 'magnetic'
    MagneticDomains = 'magnetic domains'


class ModelTypes(str, Enum):
    CustomLayers = 'custom layers'
    CustomXY = 'custom xy'
    StandardLayers = 'standard layers'


class Geometries(str, Enum):
    AirSubstrate = 'air/substrate'
    SubstrateLiquid = 'substrate/liquid'


def check_background(value):
    assert all(isinstance(element, RAT.models.Background) for element in value),\
        f'ClassList contains objects other than "Background"'
    return value


def check_contrast(value):
    assert all(isinstance(element, RAT.models.Contrast) for element in value),\
        f'ClassList contains objects other than "Contrast"'
    return value


def check_custom_file(value):
    assert all(isinstance(element, RAT.models.CustomFile) for element in value),\
        f'ClassList contains objects other than "CustomFile"'
    return value


def check_data(value):
    assert all(isinstance(element, RAT.models.Data) for element in value),\
        f'ClassList contains objects other than "Data"'
    return value


def check_layer(value):
    assert all(isinstance(element, RAT.models.Layer) for element in value),\
        f'ClassList contains objects other than "Layer"'
    return value


def check_parameter(value):
    assert all(isinstance(element, RAT.models.Parameter) for element in value),\
        f'ClassList contains objects other than "Parameter"'
    return value


def check_resolution(value):
    assert all(isinstance(element, RAT.models.Resolution) for element in value),\
        f'ClassList contains objects other than "Resolution"'
    return value


ParameterList = Annotated[ClassList, AfterValidator(check_parameter)]


class Project(BaseModel, validate_assignment=True, extra='forbid', arbitrary_types_allowed=True):
    name: str = ''
    calc_type: CalcTypes = CalcTypes.NonPolarised
    model: ModelTypes = ModelTypes.StandardLayers
    geometry: Geometries = Geometries.AirSubstrate
    absorption: bool = False

    parameters: ParameterList = ClassList(RAT.models.Parameter(), empty_list=True)

    bulk_in: ParameterList = ClassList(RAT.models.Parameter(name='SLD Air', min=0, value=0, max=0, fit=False,
                                                            prior_type=RAT.models.Priors.Uniform, mu=0, sigma=math.inf))

    bulk_out: ParameterList = ClassList(RAT.models.Parameter(name='SLD D2O', min=6.2e-6, value=6.35e-6, max=6.35e-6,
                                                             fit=False, prior_type=RAT.models.Priors.Uniform, mu=0,
                                                             sigma=math.inf))

    qz_shifts: ParameterList = ClassList(RAT.models.Parameter(name='Qz shift 1', min=-1e-4, value=0, max=1e-4,
                                                              fit=False, prior_type=RAT.models.Priors.Uniform,
                                                              mu=0, sigma=math.inf))

    scalefactors: ParameterList = ClassList(RAT.models.Parameter(name='Scalefactor 1', min=0.02, value=0.23, max=0.25,
                                                                 fit=False, prior_type=RAT.models.Priors.Uniform,
                                                                 mu=0, sigma=math.inf))

    background_parameters: ParameterList = ClassList(RAT.models.Parameter(name='Background Param 1', min=1e-7,
                                                                          value=1e-6, max=1e-5, fit=False,
                                                                          prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                          sigma=math.inf))

    resolution_parameters: ParameterList = ClassList(RAT.models.Parameter(name='Resolution Param 1', min=0.01,
                                                                          value=0.03, max=0.05, fit=False,
                                                                          prior_type=RAT.models.Priors.Uniform, mu=0,
                                                                          sigma=math.inf))

    backgrounds: Annotated[ClassList, AfterValidator(check_background)] = ClassList(RAT.models.Background(),
                                                                                    empty_list=True)
    custom_files: Annotated[ClassList, AfterValidator(check_custom_file)] = ClassList(RAT.models.CustomFile(),
                                                                                      empty_list=True)
    data: Annotated[ClassList, AfterValidator(check_data)] = ClassList(RAT.models.Data(name='Simulation'))
    layers: Annotated[ClassList, AfterValidator(check_layer)] = ClassList(RAT.models.Layer(), empty_list=True)
    resolutions: Annotated[ClassList, AfterValidator(check_resolution)] = ClassList(RAT.models.Resolution(),
                                                                                    empty_list=True)
    contrasts: Annotated[ClassList, AfterValidator(check_contrast)] = ClassList(RAT.models.Contrast(), empty_list=True)

    def model_post_init(self, __context: Any) -> None:

        # Add protected parameters here
        self.parameters.extend([RAT.models.ProtectedParameter(name='Substrate Roughness', min=1, value=3, max=5,
                                                              fit=True, prior_type=RAT.models.Priors.Uniform, mu=0,
                                                              sigma=math.inf)])

        RAT.models.Background.parameter_names = self.background_parameters.get_names()
        #RAT.models.Contrast.all_names = self.parameters.get_names()
        RAT.models.Layer.parameter_names = self.parameters.get_names()
        RAT.models.Resolution.parameter_names = self.resolution_parameters.get_names()

        self.backgrounds.append(name='Background 1', type=RAT.models.Types.Constant.value, value_1='Background Param 1')
        self.resolutions.append(name='Resolution 1', type=RAT.models.Types.Constant.value, value_1='Resolution Param 1')
