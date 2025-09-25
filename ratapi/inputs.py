"""Converts python models to the necessary inputs for the compiled RAT code."""

import importlib
import os
import pathlib
from collections.abc import Callable

import numpy as np

import ratapi
import ratapi.wrappers
from ratapi.rat_core import Checks, Control, NameStore, ProblemDefinition
from ratapi.utils.enums import Calculations, Languages, LayerModels, TypeOptions

parameter_field = {
    "parameters": "params",
    "bulk_in": "bulkIns",
    "bulk_out": "bulkOuts",
    "scalefactors": "scalefactors",
    "domain_ratios": "domainRatios",
    "background_parameters": "backgroundParams",
    "resolution_parameters": "resolutionParams",
}


def get_python_handle(file_name: str, function_name: str, path: str | pathlib.Path = "") -> Callable:
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

    def get_handle(self, index: int):
        """Return file handle for a given custom file.

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
            file_handle = ratapi.wrappers.MatlabWrapper(full_path).getHandle()
        elif custom_file["language"] == Languages.Cpp:
            file_handle = ratapi.wrappers.DylibWrapper(full_path, custom_file["function_name"]).getHandle()

        return file_handle

    def copy(self) -> "FileHandles":
        """Create a copy of the FileHandles object.

        Returns
        -------
        FileHandles
            The copy of this FileHandles object.

        """
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


def make_input(project: ratapi.Project, controls: ratapi.Controls) -> tuple[ProblemDefinition, Control]:
    """Construct the inputs required for the compiled RAT code using the data defined in the input project and controls.

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
    cpp_controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.

    """
    problem = make_problem(project)
    cpp_controls = make_controls(controls)

    return problem, cpp_controls


def make_problem(project: ratapi.Project) -> ProblemDefinition:
    """Construct the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
    problem : RAT.rat_core.ProblemDefinition
        The problem input used in the compiled RAT code.

    """
    prior_id = {"uniform": 1, "gaussian": 2, "jeffreys": 3}

    # Ensure all contrast fields are properly defined
    for contrast in project.contrasts:
        contrast_fields = ["data", "background", "bulk_in", "bulk_out", "scalefactor", "resolution"]

        if project.calculation == Calculations.Domains:
            contrast_fields.append("domain_ratio")

        for field in contrast_fields:
            if getattr(contrast, field) == "":
                raise ValueError(
                    f'In the input project, the "{field}" field of contrast "{contrast.name}" does not have a '
                    f"value defined. A value must be supplied before running the project."
                )

        # Ensure backgrounds and resolutions have a source defined
        background = project.backgrounds[contrast.background]
        resolution = project.resolutions[contrast.resolution]
        if background.source == "":
            raise ValueError(
                f"All backgrounds must have a source defined. For a {background.type} type background, "
                f"the source must be defined in "
                f'"{ratapi.project.values_defined_in[f"backgrounds.{background.type}.source"]}"'
            )
        if resolution.source == "" and resolution.type != TypeOptions.Data:
            raise ValueError(
                f"Constant resolutions must have a source defined. The source must be defined in "
                f'"{ratapi.project.values_defined_in[f"resolutions.{resolution.type}.source"]}"'
            )

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

    # Get details of defined layers
    layer_details = get_layer_details(project)

    contrast_background_params = []
    contrast_background_types = []
    all_data = []
    data_limits = []
    simulation_limits = []
    contrast_resolution_params = []
    contrast_resolution_types = []

    # Set data, background and resolution for each contrast
    for contrast in project.contrasts:
        # Set data
        data_index = project.data.index(contrast.data)
        data = project.data[data_index].data
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

        # Set background parameters
        background = project.backgrounds[contrast.background]
        contrast_background_types.append(background.type)
        contrast_background_param = []

        if background.type == TypeOptions.Data:
            contrast_background_param.append(project.data.index(background.source, True))
            if background.value_1 != "":
                contrast_background_param.append(project.background_parameters.index(background.value_1))
            # If we are using a data background, we add the background data to the contrast data
            data = append_data_background(data, project.data[background.source].data)

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

        # Set resolution parameters
        resolution = project.resolutions[contrast.resolution]
        contrast_resolution_types.append(resolution.type)
        contrast_resolution_param = []
        if resolution.type == TypeOptions.Function:
            contrast_resolution_param.append(project.custom_files.index(resolution.source, True))
            contrast_resolution_param.extend(
                [
                    project.resolution_parameters.index(value, True)
                    for value in [
                        resolution.value_1,
                        resolution.value_2,
                        resolution.value_3,
                        resolution.value_4,
                        resolution.value_5,
                    ]
                    if value != ""
                ]
            )

        elif resolution.type == TypeOptions.Constant:
            contrast_resolution_param.append(project.resolution_parameters.index(resolution.source, True))

        contrast_resolution_params.append(contrast_resolution_param)

        # Contrast data has exactly six columns to include background data if relevant
        all_data.append(np.column_stack((data, np.zeros((data.shape[0], 6 - data.shape[1])))))

    problem = ProblemDefinition()

    problem.TF = project.calculation
    problem.resample = make_resample(project)
    problem.data = all_data
    problem.dataPresent = make_data_present(project)
    problem.dataLimits = data_limits
    problem.simulationLimits = simulation_limits
    problem.numberOfContrasts = len(project.contrasts)
    problem.geometry = project.geometry
    problem.useImaginary = project.absorption
    problem.repeatLayers = [contrast.repeat_layers for contrast in project.contrasts]
    problem.contrastBackgroundParams = contrast_background_params
    problem.contrastBackgroundTypes = contrast_background_types
    problem.contrastBackgroundActions = [contrast.background_action for contrast in project.contrasts]
    problem.contrastScalefactors = [
        project.scalefactors.index(contrast.scalefactor, True) for contrast in project.contrasts
    ]
    problem.contrastBulkIns = [project.bulk_in.index(contrast.bulk_in, True) for contrast in project.contrasts]
    problem.contrastBulkOuts = [project.bulk_out.index(contrast.bulk_out, True) for contrast in project.contrasts]

    problem.contrastResolutionParams = contrast_resolution_params
    problem.contrastResolutionTypes = contrast_resolution_types

    problem.backgroundParams = [param.value for param in project.background_parameters]
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
        for class_list in ratapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if param.fit
    ]
    problem.fitLimits = [
        [param.min, param.max]
        for class_list in ratapi.project.parameter_class_lists
        for param in getattr(project, class_list)
        if param.fit
    ]
    problem.priorNames = [
        param.name for class_list in ratapi.project.parameter_class_lists for param in getattr(project, class_list)
    ]
    problem.priorValues = [
        [prior_id[param.prior_type], param.mu, param.sigma]
        for class_list in ratapi.project.parameter_class_lists
        for param in getattr(project, class_list)
    ]

    # Names
    problem.names = NameStore()
    for class_list in ratapi.project.parameter_class_lists:
        setattr(problem.names, parameter_field[class_list], [param.name for param in getattr(project, class_list)])
    problem.names.contrasts = [contrast.name for contrast in project.contrasts]

    # Checks
    problem.checks = Checks()
    for class_list in ratapi.project.parameter_class_lists:
        setattr(
            problem.checks, parameter_field[class_list], [int(element.fit) for element in getattr(project, class_list)]
        )

    check_indices(problem)

    return problem


def get_layer_details(project: ratapi.Project) -> list[int]:
    """Get parameter indices for all layers defined in the project."""
    hydrate_id = {"bulk in": 1, "bulk out": 2}
    layer_details = []

    # Get the thickness, SLD, roughness fields from the appropriate model
    if project.absorption:
        layer_fields = list(ratapi.models.AbsorptionLayer.model_fields.keys())[1:-2]
    else:
        layer_fields = list(ratapi.models.Layer.model_fields.keys())[1:-2]

    for layer in project.layers:
        for field in layer_fields:
            if getattr(layer, field) == "":
                raise ValueError(
                    f'In the input project, the "{field}" field of layer {layer.name} does not have a value '
                    f"defined. A value must be supplied before running the project."
                )

        layer_params = [project.parameters.index(getattr(layer, attribute), True) for attribute in list(layer_fields)]

        layer_params.append(project.parameters.index(layer.hydration, True) if layer.hydration else float("NaN"))
        layer_params.append(hydrate_id[layer.hydrate_with])

        layer_details.append(layer_params)

    return layer_details


def make_resample(project: ratapi.Project) -> list[int]:
    """Construct the "resample" field of the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
    list[int]
        The "resample" field of the problem input used in the compiled RAT code.

    """
    return [contrast.resample for contrast in project.contrasts]


def make_data_present(project: ratapi.Project) -> list[int]:
    """Construct the "dataPresent" field of the problem input required for the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
    list[int]
        The "dataPresent" field of the problem input used in the compiled RAT code.

    """
    return [1 if project.data[contrast.data].data.size != 0 else 0 for contrast in project.contrasts]


def check_indices(problem: ProblemDefinition) -> None:
    """Check the indices given in a problem's contrasts lie within the range of the corresponding parameter lists.

    Parameters
    ----------
    problem : RAT.rat_core.ProblemDefinition
        The problem input used in the compiled RAT code.

    """
    index_list = {
        "scalefactors": "contrastScalefactors",
        "bulkIns": "contrastBulkIns",
        "bulkOuts": "contrastBulkOuts",
        "domainRatios": "contrastDomainRatios",
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
                    f', which lie{"s" * (len(elements) == 1)} outside of the range of "{params}"',
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
                    f"Entry {i} of contrastBackgroundParams contains: {', '.join(str(i) for i in elements)}"
                    f', which lie{"s" * (len(elements) == 1)} outside of the range of "backgroundParams"',
                )


def append_data_background(data: np.array, background: np.array) -> np.array:
    """Append background data to contrast data.

    Parameters
    ----------
    data : np.array
        The contrast data to which we are appending a background.
    background : np.array
        The background data to append to the contrast.

    Returns
    -------
    np.array
        The contrast data with background data appended as two additional columns.

    """
    if not np.allclose(data[:, 0], background[:, 0]):
        raise ValueError("The q-values of the data and background must be equal.")

    return np.hstack((data, np.zeros((data.shape[0], 4 - data.shape[1])), background[:, 1:]))


def make_controls(input_controls: ratapi.Controls) -> Control:
    """Convert the controls object to the format required by the compiled RAT code.

    Parameters
    ----------
    input_controls : RAT.Controls
        The controls model, which defines algorithmic properties.

    Returns
    -------
    controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.

    """
    controls = Control()

    controls.procedure = input_controls.procedure
    controls.parallel = input_controls.parallel
    controls.numSimulationPoints = input_controls.numSimulationPoints
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

    controls.IPCFilePath = input_controls._IPCFilePath

    return controls
