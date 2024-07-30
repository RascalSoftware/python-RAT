"""Utilities for converting input files to Python `Project`s."""

from pathlib import Path
from typing import Iterable, Union

from scipy.io.matlab import MatlabOpaque, loadmat, savemat

from RATapi import Project
from RATapi.classlist import ClassList
from RATapi.models import Background, Contrast, CustomFile, Data, Parameter
from RATapi.utils.enums import Geometries, Languages, LayerModels


def r1_to_project_class(filename: str) -> Project:
    """Read a RasCAL1 project struct as a Python `Project`.

    Parameters
    ----------
    input_file : str
        The path to a .mat file containing project data.

    Returns
    -------
    Project
        A RAT `Project` equivalent to the RasCAL1 project struct.

    """
    mat_project = loadmat(filename, simplify_cells=True)["problem"]

    mat_module = mat_project["module"]
    layer_model = mat_module["type"]
    if mat_module["experiment_type"] == "Air / Liquid (or solid)":
        geom = Geometries.AirSubstrate
    else:
        geom = Geometries.SubstrateLiquid

    # R1 uses a different name for custom xy layer model
    if layer_model == "custom XY profile":
        layer_model = "custom xy"
    # FIXME: when case sensitivity is fixed, the .lower can be removed
    layer_model = LayerModels(layer_model.lower())

    def zip_if_several(*params) -> Union[tuple, list[tuple]]:
        """Zips parameters if necessary, but can handle single-item parameters.

        Examples:
        zip_if_several([1, 2], [3, 4]) = [(1, 3), (2, 4)]
        zip_if_several(1, 2, 3) = [(1, 2, 3)]

        Parameters
        ----------
        *params
            Any number of parameters.

        Returns
        -------
        tuple or list of tuple
            If any parameter is a single item, returns a list just containing the tuple of parameters.
            Otherwise, returns the same as zip(*params).

        """
        if all(isinstance(param, Iterable) and not isinstance(param, str) for param in params):
            return zip(*params)
        return [params]

    def read_param(names, constrs, values, fits):
        """Read in a parameter list from the relevant keys.

        Parameters
        ----------
        names, constrs, values, fits : str
            The keys for names, constraints, values
            and whether to fit for a type of parameter.

        Returns
        -------
        list
            A list of all relevant parameters.
        """

        return ClassList(
            [
                Parameter(
                    name=name,
                    min=constr[0],
                    value=val,
                    max=constr[1],
                    fit=fitbool,
                )
                for name, constr, val, fitbool in zip_if_several(
                    mat_project[names],
                    mat_project[constrs],
                    mat_project[values],
                    mat_project[fits],
                )
            ]
        )

    if mat_project["numberOfBacks"] == 1:
        mat_project["back_param_names"] = "Background parameter 1"
    else:
        mat_project["back_param_names"] = [
            f"Background parameter {i}" for i in range(1, mat_project["numberOfBacks"] + 1)
        ]

    params = read_param("paramnames", "constr", "params", "fityesno")
    back_params = read_param("back_param_names", "backs_constr", "backs", "backgrounds_fityesno")
    bulk_ins = read_param("nbaNames", "nbairs_constr", "nba", "nbairs_fityesno")
    bulk_outs = read_param("nbsNames", "nbsubs_constr", "nbs", "nbsubs_fityesno")
    scale_facs = read_param("scalesNames", "scale_constr", "scalefac", "scalefac_fityesno")

    # if just one background, backsNames and back_param_names are strings; fix that here
    if isinstance(mat_project["back_param_names"], str):
        mat_project["back_param_names"] = [mat_project["back_param_names"]]
    if isinstance(mat_project["backsNames"], str):
        mat_project["backsNames"] = [mat_project["backsNames"]]

    # create backgrounds from background parameters
    backs = ClassList(
        [
            Background(name=back_name, value_1=mat_project["back_param_names"][i])
            for i, back_name in enumerate(mat_project["backsNames"])
        ]
    )

    data = ClassList(
        [
            Data(
                name=Path(name).stem,
                data=data,
                data_range=data_range,
                simulation_range=sim_range,
            )
            for name, data, data_range, sim_range in zip_if_several(
                mat_project["contrastFiles"],
                mat_project["data"],
                mat_project["dataLimits"],
                mat_project["simLimits"],
            )
        ]
    )

    # contrast names may be java strings (unsure why, maybe GUI input?): convert to Python str
    # indexing gets the byte data out of the MatlabOpaque object
    if len(mat_project["contrastNames"]) == 1 and isinstance(mat_project["contrastNames"], MatlabOpaque):
        mat_project["contrastNames"] = bytes(mat_project["contrastNames"][0][3][7:]).decode("ascii")
    else:
        for i, contrast_name in enumerate(mat_project["contrastNames"]):
            if isinstance(contrast_name, MatlabOpaque):
                mat_project["contrastNames"][i] = bytes(contrast_name[0][3][7:]).decode("ascii")

    # if just one contrast, resolNames is a string; fix that here
    if isinstance(mat_project["resolNames"], str):
        mat_project["resolNames"] = [mat_project["resolNames"]]

    contrasts = ClassList(
        [
            Contrast(
                name=name,
                background=backs[int(back) - 1].name,
                resolution=mat_project["resolNames"][int(res) - 1],
                scalefactor=scale_facs[int(scale) - 1].name,
                bulk_in=bulk_ins[int(bulk_in) - 1].name,
                bulk_out=bulk_outs[int(bulk_out) - 1].name,
                data=data[i].name,
            )
            for i, (name, back, res, scale, bulk_in, bulk_out) in enumerate(
                zip_if_several(
                    mat_project["contrastNames"],
                    mat_project["contrastBacks"],
                    mat_project["contrastResolutions"],
                    mat_project["contrastScales"],
                    mat_project["contrastNbas"],
                    mat_project["contrastNbss"],
                )
            )
        ]
    )

    # set model for each contrast and add custom files
    if layer_model == LayerModels.StandardLayers:
        custom_file = ClassList()
        for i, contrast in enumerate(contrasts):
            # if there is only one layer, contrastsNumberOfLayers is an int; otherwise it is a list
            if mat_project["contrastsNumberOfLayers"] == 0 or mat_project["contrastsNumberOfLayers"][i] == 0:
                continue
            contrast_layers = mat_project["contrastLayers"][i]
            layers_list = [mat_project.layersDetails[layer[4]] for layer in contrast_layers]
            contrast.model = layers_list

    else:
        custom_filepath = mat_module["name"] + ".m"
        model_name = Path(custom_filepath).stem
        custom_file = ClassList([CustomFile(name=model_name, filename=custom_filepath, language=Languages.Matlab)])
        for contrast in contrasts:
            contrast.model = [model_name]

    project = Project(
        name=mat_project.get("name", ""),
        model=layer_model,
        geometry=geom,
        parameters=params,
        backgrounds=backs,
        background_parameters=back_params,
        bulk_in=bulk_ins,
        bulk_out=bulk_outs,
        scalefactors=scale_facs,
        data=data,
        contrasts=contrasts,
        custom_files=custom_file,
    )

    return project


def project_class_to_r1(project: Project, save: str = "RAT_project") -> Union[dict, None]:
    """Convert a RAT Project to a RasCAL1 project struct.

    Parameters
    ----------
    project : Project
        The RAT Project to convert.
    save : str, default "RAT_project"
        If given, saves as a .mat file with the given filename.
        If empty, return the R1 struct dict.

    Returns
    -------
    dict or None
        If `save` is False, return the r1 struct. Else, return nothing.
    """

    def convert_parameters(params: ClassList, name: str, value: str, constr: str, fit: str, number: str = ""):
        """Convert a list of parameters to r1 data fields.

        Parameters
        ----------
        params: ClassList
            A list of parameter type from the Project.
        names, constrs, values, fits : str
            The keys for names, constraints, values
            and whether to fit for a type of parameter.
        number : str, optional, default ""
            Key for the length of the parameter list if not blank.

        Returns
        -------
        dict
            A dict of the relevant struct fields.
        """
        output = {
            name: [p.name for p in params],
            value: [p.value for p in params],
            constr: [[p.min, p.max] for p in params],
            fit: [int(p.fit) for p in params],
        }
        if number:
            output[number] = len(params)

        return output

    # translate RAT geometries into R1 experiment types
    r1_geoms_dict = {Geometries.AirSubstrate: "Air / Liquid (or solid)", Geometries.SubstrateLiquid: "Solid / Liquid"}

    r1 = {
        # set name, type, experiment type
        "name": project.name,
        "module": {
            "type": str(project.model),
            "experiment_type": r1_geoms_dict[project.geometry],
        },
    }
    # parameter names, values, constraints, fits
    r1.update(
        convert_parameters(
            project.parameters,
            "paramnames",
            "params",
            "constr",
            "fityesno",
        )
    )
    # scalefactors names, values, constraints, fit, and number
    r1.update(
        convert_parameters(
            project.scalefactors,
            "scalesNames",
            "scalefac",
            "scale_constr",
            "scalefac_fityesno",
            "numberOfScales",
        )
    )
    # bulk in names, values, constraints, fit, and number
    r1.update(
        convert_parameters(
            project.bulk_in,
            "nbaNames",
            "nba",
            "nbairs_constr",
            "nbairs_fityesno",
            "numberOfNbas",
        )
    )
    # bulk out names, values, constraints, fit, and number
    r1.update(
        convert_parameters(
            project.bulk_out,
            "nbsNames",
            "nbs",
            "nbsubs_constr",
            "nbsubs_fityesno",
            "numberOfNbss",
        )
    )

    shifts = {
        "numberOfShifts": 1,
        "shiftsNames": "Shift 1",
        "shifts_horisontal": 0,
        "shifts_fityesno": 0,
        "shifts_constr": [-1e-4, 1e-4],
    }
    r1.update(shifts)

    resolutions = {
        "resolNames": [r.name for r in project.resolutions],
        "resolution": [project.resolution_parameters[r.value_1].value for r in project.resolutions],
        "resolution_constr": [
            [project.resolution_parameters[r.value_1].min, project.resolution_parameters[r.value_1].max]
            for r in project.resolutions
        ],
        "resolution_fityesno": [int(project.resolution_parameters[r.value_1].fit) for r in project.resolutions],
        "numberOfResolutions": len(project.resolutions),
    }
    r1.update(resolutions)

    backgrounds = {
        "backsNames": [b.name for b in project.backgrounds],
        "backs": [project.background_parameters[b.value_1].value for b in project.backgrounds],
        "backs_constr": [
            [project.background_parameters[b.value_1].min, project.background_parameters[b.value_1].max]
            for b in project.backgrounds
        ],
        "backgrounds_fityesno": [int(project.background_parameters[b.value_1].fit) for b in project.backgrounds],
        "numberOfBacks": len(project.backgrounds),
    }
    r1.update(backgrounds)

    if project.model == LayerModels.StandardLayers:
        layer_info = {
            "numberOfLayers": len(project.layers),
            "layersDetails": [
                [
                    project.parameters.index(layer.thickness, True),
                    project.parameters.index(layer.SLD, True),
                    project.parameters.index(layer.roughness, True),
                    project.parameters.index(layer.hydration, True),
                    layer.name,
                    str(layer.hydrate_with),
                ]
                for layer in project.layers
            ],
        }
        r1["module"]["name"] = ""
    else:
        layer_info = {"numberOfLayers": 0, "layersDetails": []}
        # note R1 only supports one custom file!
        r1["module"]["name"] = project.custom_files[0].name
    r1.update(layer_info)

    contrasts = {
        "contrastNames": [c.name for c in project.contrasts],
        "contrastNbas": [project.bulk_in.index(c.bulk_in, True) for c in project.contrasts],
        "contrastNbss": [project.bulk_out.index(c.bulk_out, True) for c in project.contrasts],
        "contrastScales": [project.scalefactors.index(c.scalefactor, True) for c in project.contrasts],
        "data": [project.data[c.data].data for c in project.contrasts],
        "simLimits": [project.data[c.data].simulation_range for c in project.contrasts],
        "dataLimits": [project.data[c.data].data_range for c in project.contrasts],
        "fitlowrange": [project.data[c.data].data_range[0] for c in project.contrasts],
        "fithirange": [project.data[c.data].data_range[1] for c in project.contrasts],
        "contrastRepeatSLDs": [[0, 1] for _ in project.contrasts],
        "contrastFiles": [project.data[c.data].name for c in project.contrasts],
        # some of the data is a bit complex for a (readable) list comp; create empty lists and append below
        "contrastResolutions": [],
        "contrastBacks": [],
        "dataPresent": [],
        "include_data": [],
        "dataTypes": [],
        "contrastLayers": [],
        "contrastsNumberOfLayers": [],
    }
    for contrast in project.contrasts:
        # R1 stores contrast resolutions and background by the index of the relevant parameter
        resolution = project.resolutions[contrast.resolution]
        contrasts["contrastResolutions"].append(project.resolution_parameters.index(resolution.value_1, True))

        background = project.backgrounds[contrast.background]
        contrasts["contrastBacks"].append(project.background_parameters.index(background.value_1, True))

        data = project.data[contrast.data]
        if "simulation" in data.name:
            contrasts["dataPresent"].append(0)
            contrasts["include_data"].append(0)
            contrasts["dataTypes"].append("Simulation")
        else:
            contrasts["dataPresent"].append(1)
            contrasts["include_data"].append(1)
            contrasts["dataTypes"].append("Ascii File")

        if project.model == LayerModels.StandardLayers:
            model = contrast.model
            contrasts["contrastsNumberOfLayers"].append(len(model))
            contrasts["contrastLayers"].append(",".join(str(project.layers.index(layer)) for layer in model))
        else:
            contrasts["contrastsNumberOfLayers"].append(0)
            contrasts["contrastLayers"].append("")

    r1.update(contrasts)

    # finally, .mat files just contain a string for any single-item list, so process and fix that
    for key, value in r1.items():
        if isinstance(value, list) and len(value) == 1:
            r1[key] = value[0]

    if save:
        if save[-4:] != ".mat":
            save += ".mat"

        savemat(save, {"problem": r1})
        return None
    return r1
