"""Converts python models to the necessary inputs for the compiled RAT code"""

import importlib
import os
import pathlib
from typing import Callable, Union

import RATapi
import RATapi.controls
import RATapi.wrappers
from RATapi.rat_core import Cells, Checks, Control, Limits, Priors, ProblemDefinition
from RATapi.utils.enums import Calculations, Languages, LayerModels, TypeOptions


def make_input(
    project: RATapi.Project, controls: RATapi.Controls
) -> tuple[ProblemDefinition, Cells, Limits, Priors, Control]:
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
    cells : RAT.rat_core.Cells
        The set of inputs that are defined in MATLAB as cell arrays.
    limits : RAT.rat_core.Limits
        A list of min/max values for each parameter defined in the project.
    priors : RAT.rat_core.Priors
        The priors defined for each parameter in the project.
    cpp_controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.

    """
    parameter_field = {
        "parameters": "param",
        "bulk_in": "bulkIn",
        "bulk_out": "bulkOut",
        "scalefactors": "scalefactor",
        "domain_ratios": "domainRatio",
        "background_parameters": "backgroundParam",
        "resolution_parameters": "resolutionParam",
    }
    checks_field = {
        "parameters": "fitParam",
        "bulk_in": "fitBulkIn",
        "bulk_out": "fitBulkOut",
        "scalefactors": "fitScalefactor",
        "domain_ratios": "fitDomainRatio",
        "background_parameters": "fitBackgroundParam",
        "resolution_parameters": "fitResolutionParam",
    }

    prior_id = {"uniform": 1, "gaussian": 2, "jeffreys": 3}

    problem = make_problem(project)
    cells = make_cells(project)

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
    checks.fitQzshift = []
    limits.qzshift = []
    priors.qzshift = []

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

    cpp_controls = make_controls(controls, checks)

    return problem, cells, limits, priors, cpp_controls


def make_problem(project: RATapi.Project) -> ProblemDefinition:
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
    action_id = {"add": 1, "subtract": 2}

    # Set contrast parameters according to model type
    if project.model == LayerModels.StandardLayers:
        contrast_custom_files = [float("NaN")] * len(project.contrasts)
    else:
        contrast_custom_files = [project.custom_files.index(contrast.model[0], True) for contrast in project.contrasts]

    # Set background parameters, with -1 used to indicate a data background
    contrast_background_params = []

    for contrast in project.contrasts:
        background = project.backgrounds[contrast.background]
        if background.type == TypeOptions.Data:
            contrast_background_params.append(-1)
        else:
            contrast_background_params.append(project.background_parameters.index(background.value_1, True))

    # Set resolution parameters, with -1 used to indicate a data resolution
    contrast_resolution_params = []

    for contrast in project.contrasts:
        resolution = project.resolutions[contrast.resolution]
        if resolution.type == TypeOptions.Data:
            contrast_resolution_params.append(-1)
        else:
            contrast_resolution_params.append(project.resolution_parameters.index(resolution.value_1, True))

    problem = ProblemDefinition()

    problem.TF = project.calculation
    problem.modelType = project.model
    problem.geometry = project.geometry
    problem.useImaginary = project.absorption
    problem.params = [param.value for param in project.parameters]
    problem.bulkIn = [param.value for param in project.bulk_in]
    problem.bulkOut = [param.value for param in project.bulk_out]
    problem.qzshifts = [0.0]
    problem.scalefactors = [param.value for param in project.scalefactors]
    problem.domainRatio = [param.value for param in project.domain_ratios]
    problem.backgroundParams = [param.value for param in project.background_parameters]
    problem.resolutionParams = [param.value for param in project.resolution_parameters]
    problem.contrastBulkIns = [project.bulk_in.index(contrast.bulk_in, True) for contrast in project.contrasts]
    problem.contrastBulkOuts = [project.bulk_out.index(contrast.bulk_out, True) for contrast in project.contrasts]
    problem.contrastQzshifts = [1] * len(project.contrasts)  # This is marked as "to do" in RAT
    problem.contrastScalefactors = [
        project.scalefactors.index(contrast.scalefactor, True) for contrast in project.contrasts
    ]
    problem.contrastDomainRatios = [
        project.domain_ratios.index(contrast.domain_ratio, True) if hasattr(contrast, "domain_ratio") else 0
        for contrast in project.contrasts
    ]
    problem.contrastBackgroundParams = contrast_background_params
    problem.contrastBackgroundActions = [action_id[contrast.background_action] for contrast in project.contrasts]
    problem.contrastResolutionParams = contrast_resolution_params
    problem.contrastCustomFiles = contrast_custom_files
    problem.resample = make_resample(project)
    problem.dataPresent = make_data_present(project)
    problem.oilChiDataPresent = [0] * len(project.contrasts)
    problem.numberOfContrasts = len(project.contrasts)
    problem.numberOfLayers = len(project.layers)
    problem.numberOfDomainContrasts = len(project.domain_contrasts)
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
        "bulkIn": "contrastBulkIns",
        "bulkOut": "contrastBulkOuts",
        "scalefactors": "contrastScalefactors",
        "domainRatio": "contrastDomainRatios",
        "backgroundParams": "contrastBackgroundParams",
        "resolutionParams": "contrastResolutionParams",
    }

    # Check the indices -- note we have switched to 1-based indexing at this point
    for params in index_list:
        param_list = getattr(problem, params)
        if len(param_list) > 0 and not all(
            (element > 0 or element == -1) and element <= len(param_list)
            for element in getattr(problem, index_list[params])
        ):
            elements = [
                element
                for element in getattr(problem, index_list[params])
                if not ((element > 0 or element == -1) and element <= len(param_list))
            ]
            raise IndexError(
                f'The problem field "{index_list[params]}" contains: {", ".join(str(i) for i in elements)}'
                f', which lie outside of the range of "{params}"',
            )


class FileHandles:
    """Class to defer creation of custom file handles.

    Parameters
    ----------
    files : ClassList[CustomFile]
        A list of custom file models.
    """

    def __init__(self, files):
        self.index = 0
        self.files = [*files]

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
        full_path = os.path.join(custom_file.path, custom_file.filename)
        if custom_file.language == Languages.Python:
            file_handle = get_python_handle(custom_file.filename, custom_file.function_name, custom_file.path)
        elif custom_file.language == Languages.Matlab:
            file_handle = RATapi.wrappers.MatlabWrapper(full_path).getHandle()
        elif custom_file.language == Languages.Cpp:
            file_handle = RATapi.wrappers.DylibWrapper(full_path, custom_file.function_name).getHandle()

        return file_handle

    def __next__(self):
        if self.index < len(self.files):
            custom_file = self.get_handle(self.index)
            self.index += 1
            return custom_file
        else:
            raise StopIteration


def make_cells(project: RATapi.Project) -> Cells:
    """Constructs the cells input required for the compiled RAT code.

    Note that the order of the inputs (i.e, f1 to f20) has been hard--coded into the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.

    Returns
    -------
    cells : RAT.rat_core.Cells
        The set of inputs that are defined in MATLAB as cell arrays.

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

    # Find contrast data in project.data classlist
    all_data = []
    data_limits = []
    simulation_limits = []

    for contrast in project.contrasts:
        data_index = project.data.index(contrast.data)
        all_data.append(project.data[data_index].data)
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

    # Populate the set of cells
    cells = Cells()
    cells.f1 = [[0, 1]] * len(project.contrasts)  # This is marked as "to do" in RAT
    cells.f2 = all_data
    cells.f3 = data_limits
    cells.f4 = simulation_limits
    cells.f5 = [contrast_model if contrast_model else [] for contrast_model in contrast_models]
    cells.f6 = layer_details if project.model == LayerModels.StandardLayers else []
    cells.f7 = [param.name for param in project.parameters]
    cells.f8 = [param.name for param in project.background_parameters]
    cells.f9 = [param.name for param in project.scalefactors]
    cells.f10 = []  # Placeholder for qzshifts
    cells.f11 = [param.name for param in project.bulk_in]
    cells.f12 = [param.name for param in project.bulk_out]
    cells.f13 = [param.name for param in project.resolution_parameters]
    cells.f14 = FileHandles(project.custom_files)
    cells.f15 = [param.type for param in project.backgrounds]
    cells.f16 = [param.type for param in project.resolutions]

    cells.f17 = [[[]]] * len(project.contrasts)  # Placeholder for oil chi data
    cells.f18 = [[0, 1]] * len(project.domain_contrasts)  # This is marked as "to do" in RAT

    domain_contrast_models = [
        [project.layers.index(layer, True) for layer in domain_contrast.model]
        for domain_contrast in project.domain_contrasts
    ]
    cells.f19 = [
        domain_contrast_model if domain_contrast_model else [] for domain_contrast_model in domain_contrast_models
    ]

    cells.f20 = [param.name for param in project.domain_ratios]
    cells.f21 = [contrast.name for contrast in project.contrasts]

    return cells


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


def make_controls(input_controls: RATapi.Controls, checks: Checks) -> Control:
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
    # Checks
    controls.checks = checks
    # IPC
    controls.IPCFilePath = ""

    controls.IPCFilePath = input_controls._IPCFilePath

    return controls
