"""Handles inputs to the compiled RAT code"""

from dataclasses import asdict
from typing import Union

import RAT
import RAT.project
from RAT.utils.dataclasses import Checks, Limits, Priors, Controls, Problem
from RAT.utils.enums import Calc, Models


def make_input(project: RAT.Project, controls: Union[RAT.controls.Calculate, RAT.controls.Simplex, RAT.controls.DE,
                                                     RAT.controls.NS, RAT.controls.Dream]
               ) -> tuple[RAT.utils.dataclasses.Problem, list, RAT.utils.dataclasses.Limits,
                          RAT.utils.dataclasses.Priors, RAT.utils.dataclasses.Controls]:
    """Constructs the inputs required for the compiled RAT code using the data defined in the input project and
    controls.

    Parameters
    ----------
    project : RAT.Project
        The input project.
    controls : RAT.Controls
        The input controls.

    Returns
    -------
    problem : RAT.utils.dataclasses.Problem
        The problem input used in the compiled code
    cells : list
        The list of lists (cell arrays).
    limits : RAT.utils.dataclasses.Limits
        The limits.
    priors : RAT.utils.dataclasses.Priors
        The priors.
    controls : RAT.utils.dataclasses.Controls
        The controls object used in the compiled code.
    """
    # Need to convert project and controls to the format expected by RATMain
    dataclass_field = {'parameters': 'param',
                       'bulk_in': 'bulkIn',
                       'bulk_out': 'bulkOut',
                       'qz_shifts': 'qzshift',
                       'scalefactors': 'scalefactor',
                       'domain_ratios': 'domainRatio',
                       'background_parameters': 'backgroundParam',
                       'resolution_parameters': 'resolutionParam',
                       }

    prior_id = {'uniform': 1, 'gaussian': 2, 'jeffreys': 3}

    checks = Checks()
    limits = Limits()
    priors = Priors()

    for class_list in RAT.project.parameter_class_lists:
        setattr('checks', 'fit' + dataclass_field[class_list], [int(element.fit) for element in class_list])
        setattr('limits', dataclass_field[class_list], [[element.min, element.max] for element in class_list])
        setattr('priors', dataclass_field[class_list], [[element.name, element.prior_type, element.mu, element.sigma]
                                                        for element in class_list])

    priors.priorNames = [param.name for class_list in RAT.project.parameter_class_lists
                         for param in getattr(project, class_list)]
    priors.priorValues = [[prior_id[param.prior_type], param.mu, param.sigma]
                          for class_list in RAT.project.parameter_class_lists
                          for param in getattr(project, class_list)]

    if project.model == Models.CustomXY:
        controls.calcSldDuringFit = True

    input_controls = Controls(**{**controls.model_dump(), **asdict(checks)})

    # Set the values in the problem dataclass
    problem = make_problem(project)
    cells = make_cells(project)

    return problem, cells, limits, priors, input_controls


def make_problem(project: RAT.Project) -> RAT.utils.dataclasses.Problem:
    """Constructs the inputs required for the compiled RAT code using the data defined in the input project and
    controls.

    Parameters
    ----------
    project : RAT.Project
        The input project.

    Returns
    -------
    problem : RAT.utils.Problem
        The problem object required for RATMain
    """

    # Set contrast parameters according to model type
    if project.model == Models.StandardLayers:
        contrast_custom_files = [float('NaN')] * len(project.contrasts)
    else:
        contrast_custom_files = [project.custom_files.index(contrast.model, 1) for contrast in project.contrasts]

    problem = Problem(
        TF=project.calculation,
        modelType=project.model,
        geometry=project.geometry,
        useImaginary=project.absorption,
        params=[param.value for param in project.parameters],
        bulkIn=[param.value for param in project.bulk_in],
        bulkOut=[param.value for param in project.bulk_out],
        qzshifts=[param.value for param in project.qz_shifts],
        scalefactors=[param.value for param in project.scalefactors],
        domainRatio=[param.value for param in project.domain_ratios],
        backgroundParams=[param.value for param in project.background_parameters],
        resolutionParams=[param.value for param in project.resolution_parameters],
        contrastBulkIns=[project.bulk_in.index(contrast.bulkIn, 1) for contrast in project.contrasts],
        contrastBulkOuts=[project.bulk_out.index(contrast.bulkOut, 1) for contrast in project.contrasts],
        contrastQzshifts=[],  # This is marked as "to do" in RAT
        contrastScalefactors=[project.scalefactors.index(contrast.scalefactor, 1) for contrast in project.contrasts],
        contrastDomainRatios=[project.domain_ratios.index(contrast.domain_ratio, 1) for contrast in project.contrasts
                              if hasattr(contrast, 'domain_ratio')],
        contrastBackgrounds=[project.backgrounds.index(contrast.background, 1) for contrast in project.contrasts],
        contrastBackgroundsType=[],
        contrastResolutions=[project.resolutions.index(contrast.resolution, 1) for contrast in project.contrasts],
        contrastCustomFiles=contrast_custom_files,
        resample=[contrast.resample for contrast in project.contrasts],
        dataPresent=[1 if contrast.data else 0 for contrast in project.contrasts],
        oilChiDataPresent=[],
        numberOfContrasts=len(project.contrasts),
        numberOfLayers=len(project.layers),
        numberOfDomainContrasts=len(project.domain_contrasts),
        fitParams=[param.value for class_list in RAT.project.parameter_class_lists
                   for param in getattr(project, class_list) if param.fit],
        fitLimits=[[param.min, param.max] for class_list in RAT.project.parameter_class_lists
                   for param in getattr(project, class_list) if param.fit],
        otherParams=[param.value for class_list in RAT.project.parameter_class_lists
                     for param in getattr(project, class_list) if not param.fit],
        otherLimits=[[param.min, param.max] for class_list in RAT.project.parameter_class_lists
                     for param in getattr(project, class_list) if not param.fit],
    )

    return problem


def make_cells(project: RAT.Project) -> list:
    """Constructs the inputs required for the compiled RAT code using the data defined in the input project and
    controls.

    Note that the order of the inputs has been hard--coded into the compiled RAT code.

    Parameters
    ----------
    project : RAT.Project
        The input project.

    Returns
    -------
    cells : list
        The list of lists (cell arrays) required for RATMain.
    """
    hydrate_id = {'bulk in': 1, 'bulk out': 2}

    # Set contrast parameters according to model type
    if project.model == Models.StandardLayers:
        if project.calc_type == Calc.Domains:
            contrast_models = [[project.domain_contrasts.index(domain_contrast, 1) for domain_contrast in contrast.model]
                               for contrast in project.contrasts]
        else:
            contrast_models = [[project.layers.index(layer, 1) for layer in contrast.model]
                               for contrast in project.contrasts]
    else:
        contrast_models = [[]] * len(project.contrasts)

    # Get details of defined layers
    layer_details = []
    for layer in project.layers:

        layer_params = [project.parameters.index(attribute, 1) for attribute in layer.model_fields[1:-2]]
        layer_params.append(project.parameters.index(layer.hydration, 1) if layer.hydration else float('NaN'))
        layer_params.append(hydrate_id[layer.hydrate_with])

        layer_details.append(layer_params)

    # Find contrast data in project.data classlist
    all_data = []
    data_limits = []
    simulation_limits = []

    for contrast in project.contrasts:

        data_index = project.data.index(contrast.data)

        if project.data[data_index].data:
            all_data.append(project.data[data_index].data)
            data_limits.append(project.data[data_index].data_limits)
            simulation_limits.append(project.data[data_index].simulation_limits)
        else:
            all_data.append([0, 0, 0])
            data_limits.append([0, 0])
            simulation_limits.append([0, 0])

    # Populate the list of cells
    cells = []
    cells.append([[0, 1]] * len(project.contrasts))  # This is marked as "to do" in RAT
    cells.append(all_data)
    cells.append(data_limits)
    cells.append(simulation_limits)
    cells.append([contrast_model if contrast_model else 0 for contrast_model in contrast_models])
    cells.append(layer_details if project.model == Models.StandardLayers else [0])
    cells.append([param.name for param in project.parameters])
    cells.append([param.name for param in project.background_parameters])
    cells.append([param.name for param in project.scalefactors])
    cells.append([param.name for param in project.qz_shifts])
    cells.append([param.name for param in project.bulk_in])
    cells.append([param.name for param in project.bulk_out])
    cells.append([param.name for param in project.resolution_parameters])

    cells.append([file.filename for file in project.custom_files])  # Temp for files struct - need wrappers here

    cells.append([param.type for param in project.backgrounds])
    cells.append([param.type for param in project.resolutions])

    cells.append([[0, 0, 0]] * len(project.contrasts))  # Placeholder for oil chi data
    cells.append([[0, 1]] * len(project.domain_contrasts))  # This is marked as "to do" in RAT

    domain_contrast_models = [[project.layers.index(layer, 1) for layer in domain_contrast.model]
                              for domain_contrast in project.domain_contrasts]
    cells.append([domain_contrast_model if domain_contrast_model else 0
                  for domain_contrast_model in domain_contrast_models])

    cells.append([param.name for param in project.domain_ratios])

    return cells
