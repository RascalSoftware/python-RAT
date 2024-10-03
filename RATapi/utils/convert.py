"""Utilities for converting input files to Python `Project`s."""

import json
from pathlib import Path
from typing import Iterable, Union

from numpy import array, empty
from scipy.io.matlab import MatlabOpaque, loadmat

from RATapi import Project, wrappers
from RATapi.classlist import ClassList
from RATapi.models import Background, Contrast, CustomFile, Data, Layer, Parameter, Resolution
from RATapi.utils.enums import Geometries, Languages, LayerModels


def r1_to_project_class(filename: str) -> Project:
    """Read a RasCAL1 project struct as a Python `Project`.

    Parameters
    ----------
    filename : str
        The path to a .mat file containing project data.

    Returns
    -------
    Project
        A RAT `Project` equivalent to the RasCAL1 project struct.

    """
    mat_project = loadmat(filename, simplify_cells=True)["problem"]

    mat_module = mat_project["module"]
    if mat_module["experiment_type"] == "Air / Liquid (or solid)":
        geom = Geometries.AirSubstrate
    else:
        geom = Geometries.SubstrateLiquid

    # R1 uses a different name for custom xy layer model
    layer_model = mat_module["type"]
    if layer_model == "custom XY profile":
        layer_model = LayerModels.CustomXY
    layer_model = LayerModels(layer_model)

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

    # we add these as new attributes of the mat project dict so we can feed them into read_param better
    if mat_project["numberOfBacks"] == 1:
        mat_project["back_param_names"] = "Background parameter 1"
    else:
        mat_project["back_param_names"] = [
            f"Background parameter {i}" for i in range(1, mat_project["numberOfBacks"] + 1)
        ]

    if mat_project["numberOfResolutions"] == 1:
        mat_project["res_param_names"] = "Resolution parameter 1"
    else:
        mat_project["res_param_names"] = [
            f"Resolution parameter {i}" for i in range(1, mat_project["numberOfBacks"] + 1)
        ]

    params = read_param("paramnames", "constr", "params", "fityesno")
    back_params = read_param("back_param_names", "backs_constr", "backs", "backgrounds_fityesno")
    res_params = read_param("res_param_names", "resolution_constr", "resolution", "resolution_fityesno")
    bulk_ins = read_param("nbaNames", "nbairs_constr", "nba", "nbairs_fityesno")
    bulk_outs = read_param("nbsNames", "nbsubs_constr", "nbs", "nbsubs_fityesno")
    scale_facs = read_param("scalesNames", "scale_constr", "scalefac", "scalefac_fityesno")

    # if just one background, backsNames and back_param_names are strings; fix that here
    # and mutatis mutandis for resolution
    if isinstance(mat_project["back_param_names"], str):
        mat_project["back_param_names"] = [mat_project["back_param_names"]]
    if isinstance(mat_project["backsNames"], str):
        mat_project["backsNames"] = [mat_project["backsNames"]]
    if isinstance(mat_project["res_param_names"], str):
        mat_project["res_param_names"] = [mat_project["res_param_names"]]
    if isinstance(mat_project["resolNames"], str):
        mat_project["resolNames"] = [mat_project["resolNames"]]

    # create backgrounds and resolutions from parameters
    backs = ClassList(
        [
            Background(name=back_name, value_1=mat_project["back_param_names"][i])
            for i, back_name in enumerate(mat_project["backsNames"])
        ]
    )
    res = ClassList(
        [
            Resolution(name=res_name, value_1=mat_project["res_param_names"][i])
            for i, res_name in enumerate(mat_project["resolNames"])
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
    # the java string is presented in Python as a MatlabOpaque object in a 1-item array (hence index [0])
    # which are given as a 1-item array with 4 entries. the fourth is the actual data (index [3]),
    # which is given as the byte data of a Java string; this consists of 7 metadata bytes (ignored)
    # and then the actual string characters (index [7:]) in ascii format (.decode("ascii"))
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
        layers = ClassList(
            [
                Layer(
                    name=name,
                    thickness=params[thickness - 1].name,
                    SLD=params[sld - 1].name,
                    roughness=params[roughness - 1].name,
                    hydration=params[hydration - 1].name,
                    hydrate_with=hydrate_with,
                )
                # R1 layers are 6-item arrays, unpack into a Layer object
                for thickness, sld, roughness, hydration, name, hydrate_with in mat_project["layersDetails"]
            ]
        )

        for i, contrast in enumerate(contrasts):
            # if there is only one layer, contrastsNumberOfLayers is an int; otherwise it is a list
            if (
                isinstance(mat_project["contrastsNumberOfLayers"], int)
                and mat_project["contrastsNumberOfLayers"] == 0
                or mat_project["contrastsNumberOfLayers"][i] == 0
            ):
                continue
            # contrastLayers is not an array, but rather a string with commas between entries
            contrast_layers = mat_project["contrastLayers"][i].split(",")
            contrast_layers = [
                int(x) - 1 for x in contrast_layers if x != ""
            ]  # remove empty string from hanging commas
            contrast.model = [layers[i].name for i in contrast_layers]

    else:
        custom_filepath = mat_module["name"]
        if Path(custom_filepath).suffix != ".m":
            custom_filepath += ".m"
        model_name = Path(custom_filepath).stem
        custom_file = ClassList([CustomFile(name=model_name, filename=custom_filepath, language=Languages.Matlab)])
        layers = ClassList()
        for contrast in contrasts:
            contrast.model = [model_name]

    project = Project(
        name=mat_project.get("name", ""),
        model=layer_model,
        geometry=geom,
        parameters=params,
        backgrounds=backs,
        background_parameters=back_params,
        resolutions=res,
        resolution_parameters=res_params,
        bulk_in=bulk_ins,
        bulk_out=bulk_outs,
        scalefactors=scale_facs,
        data=data,
        layers=layers,
        contrasts=contrasts,
        custom_files=custom_file,
    )

    return project


def project_class_to_r1(
    project: Project, filename: str = "RAT_project", return_struct: bool = False
) -> Union[dict, None]:
    """Convert a RAT Project to a RasCAL1 project struct.

    Parameters
    ----------
    project : Project
        The RAT Project to convert.
    filename : str, default "RAT_project"
        If given, saves as a .mat file with the given filename.
    return_struct : bool, default False
        If True, do not save and instead return the R1 struct.

    Returns
    -------
    dict or None
        If `return_struct` is True, return the r1 struct. Else, return nothing.
    """

    def convert_parameters(
        params: ClassList[Parameter], name: str, value: str, constr: str, fit: str, number: str = ""
    ):
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
            contrasts["contrastLayers"].append(",".join(str(project.layers.index(layer) + 1) for layer in model))
        else:
            contrasts["contrastsNumberOfLayers"].append(0)
            contrasts["contrastLayers"].append("")

    r1.update(contrasts)

    # some final processing:
    # .mat files just contain the item for any single-item list, so process and fix that
    # also, `savemat` will only write cells instead of matrices for object-type ndarrays. convert lists to those
    for key, value in r1.items():
        if isinstance(value, list):
            if len(value) == 1:
                r1[key] = value[0]
            else:
                if not all(isinstance(x, type(r1[key][0])) for x in r1[key]):
                    r1[key] = array(value, dtype="object")

    if return_struct:
        return r1

    if Path(filename).suffix != ".mat":
        filename += ".mat"
    # scipy.io.savemat doesn't do cells properly:
    # https://github.com/scipy/scipy/issues/3756
    # rather than fiddling we just use matlab
    eng = wrappers.start_matlab().result()
    if eng is None:
        raise ImportError("matlabengine is not installed.")
    eng.workspace["problem"] = r1
    eng.save(filename, "problem", nargout=0)
    eng.exit()
    return None


def project_to_json(project: Project) -> str:
    """Write a Project as a JSON file.

    Parameters
    ----------
    project : Project
        The input Project object to convert.

    Returns
    -------
    str
        A string representing the class in JSON format.
    """
    json_dict = {}
    for field in project.model_fields:
        attr = getattr(project, field)

        if field == "data":

            def make_data_dict(item):
                return {
                    "name": item.name,
                    "data": item.data.tolist(),
                    "data_range": item.data_range,
                    "simulation_range": item.simulation_range,
                }

            json_dict["data"] = [make_data_dict(data) for data in attr]

        elif field == "custom_files":

            def make_custom_file_dict(item):
                return {"name": item.name, "filename": item.filename, "language": item.language, "path": str(item.path)}

            json_dict["custom_files"] = [make_custom_file_dict(file) for file in attr]

        elif isinstance(attr, ClassList):
            json_dict[field] = [dict(item) for item in attr]
        else:
            json_dict[field] = attr

    return json.dumps(json_dict)


def project_from_json(input: str) -> Project:
    """Read a Project from a JSON string generated by `to_json`.

    Parameters
    ----------
    input : str
        The JSON input as a string.

    Returns
    -------
    Project
        The project corresponding to that JSON input.
    """
    model_dict = json.loads(input)
    for i in range(0, len(model_dict["data"])):
        if model_dict["data"][i]["name"] == "Simulation":
            model_dict["data"][i]["data"] = empty([0, 3])
            del model_dict["data"][i]["data_range"]
        else:
            data = model_dict["data"][i]["data"]
            model_dict["data"][i]["data"] = array(data)

    return Project.model_validate(model_dict)
