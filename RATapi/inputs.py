"""Converts python models to the necessary inputs for the compiled RAT code"""

import importlib
import os
import pathlib
from typing import Callable, Union

import numpy as np

import RATapi
import RATapi.controls
import RATapi.wrappers
from RATapi.rat_core import Checks, Control, Limits, NameStore, Priors, ProblemDefinition
from RATapi.utils.enums import Calculations, Languages, LayerModels, TypeOptions


def get_python_handle(file_name: str, function_name: str, path: Union[str, pathlib.Path] = "") -> Callable:
    """Get the function handle from a function defined in a python module located anywhere within the filesystem.

    Parameters
    ----------
    file_name : str
        The name of the file containing the function of interest.
    function_name : str
        The name of the function we wish to obtain the handle for within the module.
    path : str
        The path to the file containing the function (default is "", which represent the working directory).

    Returns
    -------
    handle : Callable
        The handle of the function defined in the python module file.

    """
    spec = importlib.util.spec_from_file_location(pathlib.Path(file_name).stem, os.path.join(path, file_name))
    custom_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(custom_module)
    handle = getattr(custom_module, function_name)
    return handle


class FileHandles:
    """Class to defer creation of custom file handles.

    Parameters
    ----------
    files : ClassList[CustomFile]
        A list of custom file models.
    """

    def __init__(self, files=None):
        self.index = 0
        self.files = [] if files is None else [file.model_dump() for file in files]

    def __iter__(self):
        self.index = 0
        return self

    def get_handle(self, index):
        """Returns file handle for a given custom file.

        Parameters
        ----------
        index : int
            The index of the custom file.

        """
        custom_file = self.files[index]
        full_path = os.path.join(custom_file["path"], custom_file["filename"])
        if custom_file["language"] == Languages.Python:
            file_handle = get_python_handle(custom_file["filename"], custom_file["function_name"], custom_file["path"])
        elif custom_file["language"] == Languages.Matlab:
            file_handle = RATapi.wrappers.MatlabWrapper(full_path).getHandle()
        elif custom_file["language"] == Languages.Cpp:
            file_handle = RATapi.wrappers.DylibWrapper(full_path, custom_file["function_name"]).getHandle()

        return file_handle

    def copy(self):
        handles = FileHandles()
        handles.files = [file.copy() for file in self.files]

        return handles

    def __next__(self):
        if self.index < len(self.files):
            custom_file = self.get_handle(self.index)
            self.index += 1
            return custom_file
        else:
            raise StopIteration

    def __len__(self):
        return len(self.files)


def make_input(project: RATapi.Project, controls: RATapi.Controls) -> tuple[ProblemDefinition, Limits, Priors, Control]:
    """Constructs the inputs required for the compiled RAT code using the data defined in the input project and
    controls.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.
    controls : RAT.Controls
        The controls model, which defines algorithmic properties.

    Returns
    -------
    problem : RAT.rat_core.ProblemDefinition
        The problem input used in the compiled RAT code.
    limits : RAT.rat_core.Limits
        A list of min/max values for each parameter defined in the project.
    priors : RAT.rat_core.Priors
        The priors defined for each parameter in the project.
    cpp_controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.

    """
    parameter_field = {
        "parameters": "params",
        "bulk_in": "bulkIns",
        "bulk_out": "bulkOuts",
        "scalefactors": "scalefactors",
        "domain_ratios": "domainRatios",
        "background_parameters": "backgroundParams",
        "resolution_parameters": "resolutionParams",
    }
    checks_field = {
        "parameters": "params",
        "bulk_in": "bulkIns",
        "bulk_out": "bulkOuts",
        "scalefactors": "scalefactors",
        "domain_ratios": "domainRatios",
        "background_parameters": "backgroundParams",
        "resolution_parameters": "resolutionParams",
    }

    prior_id = {"uniform": 1, "gaussian": 2, "jeffreys": 3}

    checks = Checks()
    limits = Limits()
    priors = Priors()

    for class_list in RATapi.project.parameter_class_lists:
        setattr(checks, checks_field[class_list], [int(element.fit) for element in getattr(project, class_list)])
        setattr(
            limits,
            parameter_field[class_list],
            [[element.min, element.max] for element in getattr(project, class_list)],
        )
        setattr(
            priors,
            parameter_field[class_list],
            [[element.name, element.prior_type, element.mu, element.sigma] for element in getattr(project, class_list)],
        )

    # Use dummy values for qzshifts
    checks.qzshifts = []
    limits.qzshifts = []
    priors.qzshifts = []

    priors.priorNames = [
        param.name for class_list in RATapi.project.parameter_class_lists for param in getattr(project, class_list)
    ]
    priors.priorValues = [
        [prior_id[param.prior_type], param.mu, param.sigma]
        for class_list in RATapi.project.parameter_class_lists
        for param in getattr(project, class_list)
    ]

    if project.model == LayerModels.CustomXY:
        controls.calcSldDuringFit = True

    problem = make_problem(project, checks)
    cpp_controls = make_controls(controls)

    return problem, limits, priors, cpp_controls


def make_problem(project: RATapi.Project, checks: Checks) -> ProblemDefinition:
    """Constructs the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
    problem : RAT.rat_core.ProblemDefinition
        The problem input used in the compiled RAT code.

    """
    hydrate_id = {"bulk in": 1, "bulk out": 2}

    # Set contrast parameters according to model type
    if project.model == LayerModels.StandardLayers:
        if project.calculation == Calculations.Domains:
            contrast_models = [
                [project.domain_contrasts.index(domain_contrast, True) for domain_contrast in contrast.model]
                for contrast in project.contrasts
            ]
        else:
            contrast_models = [
                [project.layers.index(layer, True) for layer in contrast.model] for contrast in project.contrasts
            ]
    else:
        contrast_models = [[]] * len(project.contrasts)

    # Set contrast parameters according to model type
    if project.model == LayerModels.StandardLayers:
        contrast_custom_files = [float("NaN")] * len(project.contrasts)
    else:
        contrast_custom_files = [project.custom_files.index(contrast.model[0], True) for contrast in project.contrasts]

    # Set background parameters, with -1 used to indicate a data background
    contrast_background_params = []
    contrast_background_types = []

    # Get details of defined layers
    layer_details = []
    for layer in project.layers:
        layer_params = [
            project.parameters.index(getattr(layer, attribute), True)
            for attribute in list(layer.model_fields.keys())[1:-2]
        ]
        layer_params.append(project.parameters.index(layer.hydration, True) if layer.hydration else float("NaN"))
        layer_params.append(hydrate_id[layer.hydrate_with])

        layer_details.append(layer_params)

    for contrast in project.contrasts:
        background = project.backgrounds[contrast.background]
        contrast_background_types.append(background.type)
        contrast_background_param = []
        if background.type == TypeOptions.Data:
            contrast_background_param.append(project.data.index(background.source, True))
            if background.value_1 != "":
                contrast_background_param.append(project.background_parameters.index(background.value_1))
        elif background.type == TypeOptions.Function:
            contrast_background_param.append(project.custom_files.index(background.source, True))
            contrast_background_param.extend(
                [
                    project.background_parameters.index(value, True)
                    for value in [
                        background.value_1,
                        background.value_2,
                        background.value_3,
                        background.value_4,
                        background.value_5,
                    ]
                    if value != ""
                ]
            )

        else:
            contrast_background_param.append(project.background_parameters.index(background.source, True))

        contrast_background_params.append(contrast_background_param)

    # Set resolution parameters, with -1 used to indicate a data resolution
    all_data = []
    data_limits = []
    simulation_limits = []
    contrast_resolution_params = []

    for contrast in project.contrasts:
        resolution = project.resolutions[contrast.resolution]
        if resolution.type == TypeOptions.Data:
            contrast_resolution_params.append(-1)
        else:
            contrast_resolution_params.append(project.resolution_parameters.index(resolution.source, True))

        data_index = project.data.index(contrast.data)
        data = project.data[data_index].data
        all_data.append(np.column_stack((data, np.zeros((data.shape[0], 6 - data.shape[1])))))
        data_range = project.data[data_index].data_range
        simulation_range = project.data[data_index].simulation_range

        if data_range:
            data_limits.append(data_range)
        else:
            data_limits.append([0.0, 0.0])

        if simulation_range:
            simulation_limits.append(simulation_range)
        else:
            simulation_limits.append([0.0, 0.0])

    problem = ProblemDefinition()

    problem.TF = project.calculation
    problem.resample = make_resample(project)
    problem.data = all_data
    problem.dataPresent = make_data_present(project)
    problem.dataLimits = data_limits
    problem.simulationLimits = simulation_limits
    problem.oilChiDataPresent = [0] * len(project.contrasts)
    problem.numberOfContrasts = len(project.contrasts)
    problem.geometry = project.geometry
    problem.useImaginary = project.absorption
    problem.repeatLayers = [[0, 1]] * len(project.contrasts)  # This is marked as "to do" in RAT
    problem.contrastBackgroundParams = contrast_background_params
    problem.contrastBackgroundTypes = contrast_background_types
    problem.contrastBackgroundActions = [contrast.background_action for contrast in project.contrasts]
    problem.contrastQzshifts = [1] * len(project.contrasts)  # This is marked as "to do" in RAT
    problem.contrastScalefactors = [
        project.scalefactors.index(contrast.scalefactor, True) for contrast in project.contrasts
    ]
    problem.contrastBulkIns = [project.bulk_in.index(contrast.bulk_in, True) for contrast in project.contrasts]
    problem.contrastBulkOuts = [project.bulk_out.index(contrast.bulk_out, True) for contrast in project.contrasts]
    problem.contrastResolutionParams = contrast_resolution_params
    problem.backgroundParams = [param.value for param in project.background_parameters]
    problem.qzshifts = [0.0]
    problem.scalefactors = [param.value for param in project.scalefactors]
    problem.bulkIns = [param.value for param in project.bulk_in]
    problem.bulkOuts = [param.value for param in project.bulk_out]
    problem.resolutionParams = [param.value for param in project.resolution_parameters]
    problem.params = [param.value for param in project.parameters]
    problem.numberOfLayers = len(project.layers)
    problem.contrastLayers = [contrast_model if contrast_model else [] for contrast_model in contrast_models]
    problem.layersDetails = layer_details if project.model == LayerModels.StandardLayers else []
    problem.customFiles = FileHandles(project.custom_files)
    problem.modelType = project.model
    problem.contrastCustomFiles = contrast_custom_files

    problem.contrastDomainRatios = [
        project.domain_ratios.index(contrast.domain_ratio, True) if hasattr(contrast, "domain_ratio") else 0
        for contrast in project.contrasts
    ]

    problem.domainRatios = [param.value for param in project.domain_ratios]
    problem.numberOfDomainContrasts = len(project.domain_contrasts)

    domain_contrast_models = [
        [project.layers.index(layer, True) for layer in domain_contrast.model]
        for domain_contrast in project.domain_contrasts
    ]

    problem.domainContrastLayers = [
        domain_contrast_model if domain_contrast_model else [] for domain_contrast_model in domain_contrast_models
    ]
    problem.fitParams = [
        param.value
        for class_list in RATapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if param.fit
    ]
    problem.fitLimits = [
        [param.min, param.max]
        for class_list in RATapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if param.fit
    ]
    problem.otherParams = [
        param.value
        for class_list in RATapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if not param.fit
    ]
    problem.otherLimits = [
        [param.min, param.max]
        for class_list in RATapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if not param.fit
    ]

    # Names
    problem.names = NameStore()
    problem.names.params = [param.name for param in project.parameters]
    problem.names.backgroundParams = [param.name for param in project.background_parameters]
    problem.names.scalefactors = [param.name for param in project.scalefactors]
    problem.names.qzshifts = []  # Placeholder for qzshifts
    problem.names.bulkIns = [param.name for param in project.bulk_in]
    problem.names.bulkOuts = [param.name for param in project.bulk_out]
    problem.names.resolutionParams = [param.name for param in project.resolution_parameters]
    problem.names.domainRatios = [param.name for param in project.domain_ratios]
    problem.names.contrasts = [contrast.name for contrast in project.contrasts]

    # Checks
    problem.checks = checks
    check_indices(problem)

    return problem


def make_resample(project: RATapi.Project) -> list[int]:
    """Constructs the "resample" field of the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
     : list[int]
        The "resample" field of the problem input used in the compiled RAT code.

    """
    return [contrast.resample for contrast in project.contrasts]


def make_data_present(project: RATapi.Project) -> list[int]:
    """Constructs the "dataPresent" field of the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
     : list[int]
        The "dataPresent" field of the problem input used in the compiled RAT code.

    """
    return [1 if project.data[contrast.data].data.size != 0 else 0 for contrast in project.contrasts]


def check_indices(problem: ProblemDefinition) -> None:
    """Checks the indices in contrast lists in a ProblemDefinition object lie within the range of the corresponding
    parameter lists.

    Parameters
    ----------
    problem : RAT.rat_core.ProblemDefinition
        The problem input used in the compiled RAT code.

    """
    index_list = {
        "bulkIns": "contrastBulkIns",
        "bulkOuts": "contrastBulkOuts",
        "scalefactors": "contrastScalefactors",
        "domainRatios": "contrastDomainRatios",
        "resolutionParams": "contrastResolutionParams",
    }

    # Check the indices -- note we have switched to 1-based indexing at this point
    for params in index_list:
        param_list = getattr(problem, params)
        if len(param_list) > 0:
            elements = [
                element
                for element in getattr(problem, index_list[params])
                if (element != -1) and not (0 < element <= len(param_list))
            ]
            if elements:
                raise IndexError(
                    f'The problem field "{index_list[params]}" contains: {", ".join(str(i) for i in elements)}'
                    f', which lie{"s"*(len(elements)==1)} outside of the range of "{params}"',
                )

    # backgroundParams has a different structure, so is handled separately:
    # it is of type list[list[int]], where each list[int] is the indices for
    # source, value_1, value_2, value_3, value_4, value_5 where they are defined
    # e.g. for a data background with offset it is [source value_1], for a function
    # with 3 values it is [source value_1 value_2 value_3], etc.

    source_param_lists = {
        "constant": "backgroundParams",
        "data": "data",
        "function": "customFiles",
    }

    for i, background_data in enumerate(problem.contrastBackgroundParams):
        background_type = problem.contrastBackgroundTypes[i]

        # check source param is in range of the relevant parameter list
        param_list = getattr(problem, source_param_lists[background_type])
        source_index = background_data[0]
        if not 0 < source_index <= len(param_list):
            raise IndexError(
                f'Entry {i} of contrastBackgroundParams has type "{background_type}" '
                f"and source index {source_index}, "
                f'which is outside the range of "{source_param_lists[background_type]}".'
            )

        # check value params are in range for background params
        if len(background_data) > 1:
            elements = [element for element in background_data[1:] if not 0 < element <= len(problem.backgroundParams)]
            if elements:
                raise IndexError(
                    f'Entry {i} of contrastBackgroundParams contains: {", ".join(str(i) for i in elements)}'
                    f', which lie{"s"*(len(elements)==1)} outside of the range of "backgroundParams"',
                )


def make_controls(input_controls: RATapi.Controls) -> Control:
    """Converts the controls object to the format required by the compiled RAT code.

    Parameters
    ----------
    input_controls : RAT.Controls
        The controls model, which defines algorithmic properties.
    checks : Rat.rat_core.Checks
        States whether or not to fit each parameter defined in the project.

    Returns
    -------
    controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.

    """
    controls = Control()

    controls.procedure = input_controls.procedure
    controls.parallel = input_controls.parallel
    controls.calcSldDuringFit = input_controls.calcSldDuringFit
    controls.resampleMinAngle = input_controls.resampleMinAngle
    controls.resampleNPoints = input_controls.resampleNPoints
    controls.display = input_controls.display
    # Simplex
    controls.xTolerance = input_controls.xTolerance
    controls.funcTolerance = input_controls.funcTolerance
    controls.maxFuncEvals = input_controls.maxFuncEvals
    controls.maxIterations = input_controls.maxIterations
    controls.updateFreq = input_controls.updateFreq
    controls.updatePlotFreq = input_controls.updatePlotFreq
    # DE
    controls.populationSize = input_controls.populationSize
    controls.fWeight = input_controls.fWeight
    controls.crossoverProbability = input_controls.crossoverProbability
    controls.strategy = int(input_controls.strategy)  # RAT core expects strategy as an integer
    controls.targetValue = input_controls.targetValue
    controls.numGenerations = input_controls.numGenerations
    # NS
    controls.nLive = input_controls.nLive
    controls.nMCMC = input_controls.nMCMC
    controls.propScale = input_controls.propScale
    controls.nsTolerance = input_controls.nsTolerance
    # Dream
    controls.nSamples = input_controls.nSamples
    controls.nChains = input_controls.nChains
    controls.jumpProbability = input_controls.jumpProbability
    controls.pUnitGamma = input_controls.pUnitGamma
    controls.boundHandling = input_controls.boundHandling
    controls.adaptPCR = input_controls.adaptPCR
    # IPC
    controls.IPCFilePath = ""

    controls.IPCFilePath = input_controls._IPCFilePath

    return controls
