"""Utilities for converting input files to Python `Project`s."""

from pathlib import Path
from typing import Iterable, Union

from scipy.io.matlab import MatlabOpaque, loadmat

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
                background=backs[back - 1].name,
                resolution=mat_project["resolNames"][res - 1],
                scalefactor=scale_facs[scale - 1].name,
                bulk_in=bulk_ins[bulk_in - 1].name,
                bulk_out=bulk_outs[bulk_out - 1].name,
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
        custom_filepath = mat_module["name"]
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
