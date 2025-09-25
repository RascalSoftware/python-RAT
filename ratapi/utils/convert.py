"""Utilities for converting input files to Python `Project`s."""

import warnings
from collections.abc import Iterable
from os import PathLike
from pathlib import Path

from numpy import array, empty
from scipy.io.matlab import MatlabOpaque, loadmat

from ratapi import Project, wrappers
from ratapi.classlist import ClassList
from ratapi.models import Background, Contrast, CustomFile, Data, Layer, Parameter, Resolution
from ratapi.utils.enums import Geometries, Languages, LayerModels


def r1_to_project(filename: str | PathLike) -> Project:
    """Read a RasCAL1 project struct as a Python `Project`.

    Parameters
    ----------
    filename : str, PathLike
        The path to a .mat file containing project data.

    Returns
    -------
    Project
        A RAT `Project` equivalent to the RasCAL1 project struct.

    """
    mat_project = loadmat(str(filename), simplify_cells=True)["problem"]

    mat_module = mat_project["module"]
    if mat_module["experiment_type"] == "Air / Liquid (or solid)":
        geom = Geometries.AirSubstrate
    else:
        geom = Geometries.SubstrateLiquid

    # R1 uses a different name for custom xy layer model
    layer_model = mat_module["type"]
    if layer_model.lower() == "custom xy profile":
        layer_model = LayerModels.CustomXY
    layer_model = LayerModels(layer_model)

    def zip_if_several(*params) -> tuple | list[tuple]:
        """Zips parameters if necessary, but can handle single-item parameters.

        Examples
        --------
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
            return zip(*params, strict=False)
        return [params]

    def read_param(names, constrs, values, fits):
        """Read in a parameter list from the relevant keys, and fix constraints for non-fit parameters.

        Parameters
        ----------
        names, constrs, values, fits : str
            The keys for names, constraints, values
            and whether to fit for a type of parameter.

        Returns
        -------
        ClassList
            A list of all relevant parameters.

        """

        def fix_invalid_constraints(name: str, constrs: tuple[float, float], value: float) -> tuple[float, float]:
            """Check that constraints are valid and fix them if they aren't.

            RasCAL-1 allowed the constraints of non-fit parameters to be invalid, which means
            we need to fix them here so that the project is valid.

            Parameters
            ----------
            name: str
                The name of the parameter.
            constrs : tuple[float, float]
                The constraints of the parameter (min and max, respectively)
            value : float
                The value of the parameter.

            Returns
            -------
            tuple[float, float]
                The adjusted constraints (identical to constrs if constraints were valid)

            """
            new_constrs = (min(constrs[0], value), max(constrs[1], value))
            if new_constrs[0] != constrs[0] or new_constrs[1] != constrs[1]:
                warnings.warn(
                    f"The parameter {name} has invalid constraints,"
                    " these have been adjusted to satisfy the current value of the parameter.",
                    stacklevel=2,
                )
            return new_constrs

        # adjust invalid constraints
        # if just one item in the classlist, these objects won't be in lists
        if not isinstance(fit := mat_project[fits], Iterable):
            if not fit:
                mat_project[constrs] = fix_invalid_constraints(
                    mat_project[names], mat_project[constrs], mat_project[values]
                )
        # else they will be iterable
        else:
            for i, fit in enumerate(mat_project[fits]):
                if not fit:
                    mat_project[constrs][i] = fix_invalid_constraints(
                        mat_project[names][i], mat_project[constrs][i], mat_project[values][i]
                    )

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

    # sometimes numberOfResolutions isn't defined: in all these cases there's only one resolution
    # maybe from before multiple resolutions were possible?
    if getattr(mat_project, "numberOfResolutions", 1) == 1:
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
            Background(name=back_name, type="constant", source=mat_project["back_param_names"][i])
            for i, back_name in enumerate(mat_project["backsNames"])
        ]
    )
    res = ClassList(
        [
            Resolution(name=res_name, type="constant", source=mat_project["res_param_names"][i])
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
                    thickness=params[int(thickness) - 1].name,
                    SLD=params[int(sld) - 1].name,
                    roughness=params[int(roughness) - 1].name,
                    # if hydration is not set, it is an empty numpy array, else it is a string representing a number
                    hydration=params[int(hydration) - 1].name if getattr(hydration, "size", 1) > 0 else "",
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
                or isinstance(mat_project["contrastsNumberOfLayers"], list)
                and mat_project["contrastsNumberOfLayers"][i] == 0
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


def project_to_r1(
    project: Project, filename: str | PathLike = "RAT_project", return_struct: bool = False
) -> dict | None:
    """Convert a RAT Project to a RasCAL1 project struct.

    Parameters
    ----------
    project : Project
        The RAT Project to convert.
    filename : str or PathLike, default "RAT_project"
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
        name, constr, value, fit : str
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
        "resolution": [project.resolution_parameters[r.source].value for r in project.resolutions],
        "resolution_constr": [
            [project.resolution_parameters[r.source].min, project.resolution_parameters[r.source].max]
            for r in project.resolutions
        ],
        "resolution_fityesno": [int(project.resolution_parameters[r.source].fit) for r in project.resolutions],
        "numberOfResolutions": len(project.resolutions),
    }
    r1.update(resolutions)

    backgrounds = {
        "backsNames": [b.name for b in project.backgrounds],
        "backs": [project.background_parameters[b.source].value for b in project.backgrounds],
        "backs_constr": [
            [project.background_parameters[b.source].min, project.background_parameters[b.source].max]
            for b in project.backgrounds
        ],
        "backgrounds_fityesno": [int(project.background_parameters[b.source].fit) for b in project.backgrounds],
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
                    project.parameters.index(layer.hydration, True) if layer.hydration else empty(shape=(0, 0)),
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
        contrasts["contrastResolutions"].append(project.resolution_parameters.index(resolution.source, True))

        background = project.backgrounds[contrast.background]
        contrasts["contrastBacks"].append(project.background_parameters.index(background.source, True))

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
    loader = wrappers.MatlabWrapper.loader
    if loader is None:
        raise ImportError("matlabengine is not installed.")
    eng = loader.result()
    eng.workspace["problem"] = r1
    eng.save(str(filename), "problem", nargout=0)
    return None
