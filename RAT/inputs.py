"""Converts python models to the necessary inputs for the compiled RAT code"""

import pathlib
from typing import Union

import RAT
import RAT.controls
from RAT.utils.enums import Calculations, LayerModels

from RAT.rat_core import Cells, Checks, Control, Limits, Priors, ProblemDefinition


def make_input(project: RAT.Project, controls: Union[RAT.controls.Calculate, RAT.controls.Simplex, RAT.controls.DE,
                                                     RAT.controls.NS, RAT.controls.Dream]
               ) -> tuple[ProblemDefinition, Cells, Limits, Priors, Control]:
    """Constructs the inputs required for the compiled RAT code using the data defined in the input project and controls.

    Parameters
    ----------
    project : RAT.Project
        The project model, which defines the physical system under study.
    controls : Union[RAT.controls.Calculate, RAT.controls.Simplex, RAT.controls.DE, RAT.controls.NS, RAT.controls.Dream]
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

    parameter_field = {'parameters': 'param',
                       'bulk_in': 'bulkIn',
                       'bulk_out': 'bulkOut',
                       'scalefactors': 'scalefactor',
                       'domain_ratios': 'domainRatio',
                       'background_parameters': 'backgroundParam',
                       'resolution_parameters': 'resolutionParam',
                       }
    checks_field = {'parameters': 'fitParam',
                    'bulk_in': 'fitBulkIn',
                    'bulk_out': 'fitBulkOut',
                    'scalefactors': 'fitScalefactor',
                    'domain_ratios': 'fitDomainRatio',
                    'background_parameters': 'fitBackgroundParam',
                    'resolution_parameters': 'fitResolutionParam',
                    }

    prior_id = {'uniform': 1, 'gaussian': 2, 'jeffreys': 3}

    problem = make_problem(project)
    cells = make_cells(project)

    checks = Checks()
    limits = Limits()
    priors = Priors()

    for class_list in RAT.project.parameter_class_lists:
        setattr(checks, checks_field[class_list], [int(element.fit) for element in getattr(project, class_list)])
        setattr(limits, parameter_field[class_list], [[element.min, element.max]
                                                      for element in getattr(project, class_list)])
        setattr(priors, parameter_field[class_list], [[element.name, element.prior_type, element.mu, element.sigma]
                                                      for element in getattr(project, class_list)])

    # Use dummy values for qzshifts
    checks.fitQzshift = []
    limits.qzshift = []
    priors.qzshift = []

    priors.priorNames = [param.name for class_list in RAT.project.parameter_class_lists
                         for param in getattr(project, class_list)]
    priors.priorValues = [[prior_id[param.prior_type], param.mu, param.sigma]
                          for class_list in RAT.project.parameter_class_lists
                          for param in getattr(project, class_list)]

    if project.model == LayerModels.CustomXY:
        controls.calcSldDuringFit = True

    cpp_controls = make_controls(controls, checks)

    return problem, cells, limits, priors, cpp_controls


def make_problem(project: RAT.Project) -> ProblemDefinition:
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
    action_id = {'add': 1, 'subtract': 2}

    # Set contrast parameters according to model type
    if project.model == LayerModels.StandardLayers:
        contrast_custom_files = [float('NaN')] * len(project.contrasts)
    else:
        contrast_custom_files = [project.custom_files.index(contrast.model[0], True) for contrast in project.contrasts]

    problem = ProblemDefinition()

    problem.TF = project.calculation
    problem.modelType = project.model
    problem.geometry = project.geometry
    problem.useImaginary = project.absorption
    problem.params = [param.value for param in project.parameters]
    problem.bulkIn = [param.value for param in project.bulk_in]
    problem.bulkOut = [param.value for param in project.bulk_out]
    problem.qzshifts = []
    problem.scalefactors = [param.value for param in project.scalefactors]
    problem.domainRatio = [param.value for param in project.domain_ratios]
    problem.backgroundParams = [param.value for param in project.background_parameters]
    problem.resolutionParams = [param.value for param in project.resolution_parameters]
    problem.contrastBulkIns = [project.bulk_in.index(contrast.bulk_in, True) for contrast in project.contrasts]
    problem.contrastBulkOuts = [project.bulk_out.index(contrast.bulk_out, True) for contrast in project.contrasts]
    problem.contrastQzshifts = [1] * len(project.contrasts)  # This is marked as "to do" in RAT
    problem.contrastScalefactors = [project.scalefactors.index(contrast.scalefactor, True) for contrast in project.contrasts]
    problem.contrastDomainRatios = [project.domain_ratios.index(contrast.domain_ratio, True)
                                    if hasattr(contrast, 'domain_ratio') else 0 for contrast in project.contrasts]
    problem.contrastBackgrounds = [project.backgrounds.index(contrast.background, True) for contrast in project.contrasts]
    problem.contrastBackgroundActions = [action_id[contrast.background_action] for contrast in project.contrasts]
    problem.contrastResolutions = [project.resolutions.index(contrast.resolution, True) for contrast in project.contrasts]
    problem.contrastCustomFiles = contrast_custom_files
    problem.resample = [contrast.resample for contrast in project.contrasts]
    problem.dataPresent = [1 if contrast.data else 0 for contrast in project.contrasts]
    problem.oilChiDataPresent = [0] * len(project.contrasts)
    problem.numberOfContrasts = len(project.contrasts)
    problem.numberOfLayers = len(project.layers)
    problem.numberOfDomainContrasts = len(project.domain_contrasts)
    problem.fitParams = [param.value for class_list in RAT.project.parameter_class_lists
                         for param in getattr(project, class_list) if param.fit]
    problem.fitLimits = [[param.min, param.max] for class_list in RAT.project.parameter_class_lists
                         for param in getattr(project, class_list) if param.fit]
    problem.otherParams = [param.value for class_list in RAT.project.parameter_class_lists
                           for param in getattr(project, class_list) if not param.fit]
    problem.otherLimits = [[param.min, param.max] for class_list in RAT.project.parameter_class_lists
                           for param in getattr(project, class_list) if not param.fit]

    return problem


def make_cells(project: RAT.Project) -> Cells:
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

    hydrate_id = {'bulk in': 1, 'bulk out': 2}

    # Set contrast parameters according to model type
    if project.model == LayerModels.StandardLayers:
        if project.calculation == Calculations.Domains:
            contrast_models = [[project.domain_contrasts.index(domain_contrast, True) for domain_contrast in contrast.model]
                               for contrast in project.contrasts]
        else:
            contrast_models = [[project.layers.index(layer, True) for layer in contrast.model]
                               for contrast in project.contrasts]
    else:
        contrast_models = [[]] * len(project.contrasts)

    # Get details of defined layers
    layer_details = []
    for layer in project.layers:

        layer_params = [project.parameters.index(getattr(layer, attribute), True) for attribute in list(layer.model_fields.keys())[1:-2]]
        layer_params.append(project.parameters.index(layer.hydration, True) if layer.hydration else float('NaN'))
        layer_params.append(hydrate_id[layer.hydrate_with])

        layer_details.append(layer_params)

    # Find contrast data in project.data classlist
    all_data = []
    data_limits = []
    simulation_limits = []

    for contrast in project.contrasts:

        data_index = project.data.index(contrast.data)

        if 'data' in project.data[data_index].model_fields_set:
            all_data.append(project.data[data_index].data)
            data_limits.append(project.data[data_index].data_range)
            simulation_limits.append(project.data[data_index].simulation_range)
        else:
            all_data.append([0.0, 0.0, 0.0])
            data_limits.append([0.0, 0.0])
            simulation_limits.append([0.0, 0.0])

    # Populate the set of cells
    cells = Cells()
    cells.f1 = [[0, 1]] * len(project.contrasts)  # This is marked as "to do" in RAT
    cells.f2 = all_data
    cells.f3 = data_limits
    cells.f4 = simulation_limits
    cells.f5 = [contrast_model if contrast_model else 0 for contrast_model in contrast_models]
    cells.f6 = layer_details if project.model == LayerModels.StandardLayers else [0]
    cells.f7 = [param.name for param in project.parameters]
    cells.f8 = [param.name for param in project.background_parameters]
    cells.f9 = [param.name for param in project.scalefactors]
    cells.f10 = []  # Placeholder for qzshifts
    cells.f11 = [param.name for param in project.bulk_in]
    cells.f12 = [param.name for param in project.bulk_out]
    cells.f13 = [param.name for param in project.resolution_parameters]
    cells.f14 = [pathlib.Path(file.filename).stem for file in project.custom_files]
    cells.f15 = [param.type for param in project.backgrounds]
    cells.f16 = [param.type for param in project.resolutions]

    cells.f17 = [[0.0, 0.0, 0.0]] * len(project.contrasts)  # Placeholder for oil chi data
    cells.f18 = [[0, 1]] * len(project.domain_contrasts)  # This is marked as "to do" in RAT

    domain_contrast_models = [[project.layers.index(layer, True) for layer in domain_contrast.model]
                              for domain_contrast in project.domain_contrasts]
    cells.f19 = [domain_contrast_model if domain_contrast_model else 0
                 for domain_contrast_model in domain_contrast_models]

    cells.f20 = [param.name for param in project.domain_ratios]

    return cells


def make_controls(controls: Union[RAT.controls.Calculate, RAT.controls.Simplex, RAT.controls.DE, RAT.controls.NS,
                                  RAT.controls.Dream], checks: Checks) -> Control:
    """Converts the controls object to the format required by the compiled RAT code.

    Parameters
    ----------
    controls : Union[RAT.controls.Calculate, RAT.controls.Simplex, RAT.controls.DE, RAT.controls.NS, RAT.controls.Dream]
        The controls model, which defines algorithmic properties.
    checks : Rat.rat_core.Checks
        States whether or not to fit each parameter defined in the project.

    Returns
    -------
    controls : RAT.rat_core.Control
        The controls object used in the compiled RAT code.
    """

    full_controls = RAT.controls.Controls(**controls.model_dump())
    cpp_controls = Control()

    cpp_controls.procedure = full_controls.procedure
    cpp_controls.parallel = full_controls.parallel
    cpp_controls.calcSldDuringFit = full_controls.calcSldDuringFit
    cpp_controls.resampleParams = full_controls.resampleParams
    cpp_controls.display = full_controls.display
    # Simplex
    cpp_controls.xTolerance = full_controls.xTolerance
    cpp_controls.funcTolerance = full_controls.funcTolerance
    cpp_controls.maxFuncEvals = full_controls.maxFuncEvals
    cpp_controls.maxIterations = full_controls.maxIterations
    cpp_controls.updateFreq = full_controls.updateFreq
    cpp_controls.updatePlotFreq = full_controls.updatePlotFreq
    # DE
    cpp_controls.populationSize = full_controls.populationSize
    cpp_controls.fWeight = full_controls.fWeight
    cpp_controls.crossoverProbability = full_controls.crossoverProbability
    cpp_controls.strategy = full_controls.strategy
    cpp_controls.targetValue = full_controls.targetValue
    cpp_controls.numGenerations = full_controls.numGenerations
    # NS
    cpp_controls.nLive = full_controls.nLive
    cpp_controls.nMCMC = full_controls.nMCMC
    cpp_controls.propScale = full_controls.propScale
    cpp_controls.nsTolerance = full_controls.nsTolerance
    # Dream
    cpp_controls.nSamples = full_controls.nSamples
    cpp_controls.nChains = full_controls.nChains
    cpp_controls.jumpProbability = full_controls.jumpProbability
    cpp_controls.pUnitGamma = full_controls.pUnitGamma
    cpp_controls.boundHandling = full_controls.boundHandling
    cpp_controls.adaptPCR = full_controls.adaptPCR
    # Checks
    cpp_controls.checks = checks

    return cpp_controls
