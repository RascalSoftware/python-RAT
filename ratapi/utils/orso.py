"""Readers from file formats."""

from dataclasses import dataclass
from itertools import count
from pathlib import Path
from textwrap import shorten

import orsopy
import prettytable
from orsopy.fileio import load_orso

from ratapi import ClassList
from ratapi.models import AbsorptionLayer, Data, Layer, Parameter


class ORSOProject:
    """A class to encapsulate model information and data from an .ort file.

    Parameters
    ----------
    filepath : str or Path
        The path to the .ort file.
    absorption : bool, default None
        Whether to account for absorption in the model data.

    """

    def __init__(self, filepath: str | Path, absorption: bool = False):
        ort_data = load_orso(filepath)
        datasets = [Data(name=dataset.info.data_source.sample.name, data=dataset.data) for dataset in ort_data]
        # orso datasets in the same file can have repeated names!
        # but classlists do not allow this
        # use this dict to keep track of counts for repeated names
        name_counts = {d.name: count(1) for d in datasets}
        names = [d.name for d in datasets]
        if len(names) > len(list(set(names))):
            for i, data in enumerate(datasets):
                if data.name in names[:i]:
                    data.name += f"-{next(name_counts[data.name])}"
        self.data = ClassList(datasets)
        self.samples = [
            orso_model_to_rat(dataset.info.data_source.sample.model, absorption=absorption) for dataset in ort_data
        ]

    def __str__(self):
        data_str = f"Data:\n{str(self.data)}\n\n"
        if len(self.samples) == 1:
            samples_str = f"Sample:\n{str(self.samples[0])}"
        else:
            table = prettytable.PrettyTable()
            table.field_names = ["index", "bulk in", "bulk out", "parameters", "layers", "model"]
            for index, sample in enumerate(self.samples):
                if sample is None:
                    row = [index, "", "", "", "", ""]
                else:
                    row = [
                        index,
                        sample.bulk_in.name,
                        sample.bulk_out.name,
                        shorten(", ".join([p.name for p in sample.parameters]), width=20, placeholder="..."),
                        shorten(", ".join([layer.name for layer in sample.layers]), width=20, placeholder="..."),
                        shorten(str(sample.model), width=20, placeholder="..."),
                    ]
                table.add_row(row)
            samples_str = table.get_string()

        return data_str + samples_str


@dataclass
class ORSOSample:
    """The stack data from an ORSO SampleModel, in RAT models."""

    bulk_in: Parameter
    bulk_out: Parameter
    parameters: ClassList[Parameter]
    layers: ClassList[Layer] | ClassList[AbsorptionLayer]
    model: list[str]

    def __str__(self):
        return (
            "Bulk in:\n"
            f"{str(self.bulk_in)}\n\n"
            "Bulk out:\n"
            f"{str(self.bulk_out)}\n\n"
            "Parameters:\n"
            f"{str(self.parameters)}\n\n"
            "Layers:\n"
            f"{str(self.layers)}\n\n"
            "Model:\n"
            f"{self.model}"
        )


def orso_model_to_rat(
    model: orsopy.fileio.model_language.SampleModel | str, absorption: bool = False
) -> ORSOSample | None:
    """Get information from an ORSO SampleModel object.

    Parameters
    ----------
    model : orsopy.fileio.model_language.SampleModel or str
        The sample model to turn into a RAT data. If given as a string,
        the string is interpreted as a layer stack in ORSO model language.
    absorption : bool, default False
        Whether to account for absorption in the model.

    Returns
    -------
    ORSOSample
        A dataclass containing the sample data, or None if the model is None.

    """
    if model is None:
        return None

    if isinstance(model, str):
        model = orsopy.fileio.model_language.SampleModel(stack=model)

    stack = model.resolve_to_layers()
    # if bulk in or out is air, it has SLD predefined
    # else we need to grab it from SLDDB
    if bulk_in_sld := stack[0].material.sld is None:
        bulk_in_sld = stack[0].material.get_sld()

    # resolve_to_layers loses the name of bulk in and out
    bulk_in_name = model.stack.split("|")[0].strip()
    bulk_in = Parameter(
        name=f"{bulk_in_name} SLD",
        min=bulk_in_sld.real,
        value=bulk_in_sld.real,
        max=bulk_in_sld.real,
        fit=False,
    )

    if bulk_out_sld := stack[-1].material.sld is None:
        bulk_out_sld = stack[-1].material.get_sld()

    bulk_out_name = model.stack.split("|")[-1].strip()
    bulk_out = Parameter(
        name=f"{bulk_out_name} SLD",
        min=bulk_out_sld.real,
        value=bulk_out_sld.real,
        max=bulk_out_sld.real,
        fit=False,
    )

    parameters = ClassList()
    layers = ClassList()
    contrast_model = []

    for orso_layer in stack[1:-1]:
        name = get_material_name(orso_layer.material, model)
        contrast_model.append(name)
        layer_params, layer = orso_layer_to_rat_layer(orso_layer, name, absorption)
        parameters.union(layer_params)
        layers.union(layer)

    return ORSOSample(
        bulk_in=bulk_in,
        bulk_out=bulk_out,
        parameters=parameters,
        layers=layers,
        model=contrast_model,
    )


def orso_layer_to_rat_layer(
    layer: orsopy.fileio.model_language.Layer, name: str, absorption: bool = False
) -> tuple[ClassList[Parameter], Layer]:
    """Convert an ``orsopy`` layer to a RAT layer.

    Parameters
    ----------
    layer : orsopy.fileio.model_language.Layer
        An ``orsopy`` Layer.
    name : str
        The name of the material in the layer.
    absorption : bool, default True
        Whether absorption should be accounted for in the layer.

    Returns
    -------
    ClassList[Parameter], Layer
        The parameters required for the RAT layer and the layer itself.

    """
    thickness = layer.thickness.as_unit("angstrom")
    roughness = layer.roughness.as_unit("angstrom")
    sld = layer.material.get_sld()

    params = ClassList(
        [
            Parameter(name=f"{name} Thickness", min=thickness, value=thickness, max=thickness, fit=False),
            Parameter(name=f"{name} Roughness", min=roughness, value=roughness, max=roughness, fit=False),
            Parameter(name=f"{name} SLD", min=sld.real, value=sld.real, max=sld.real, fit=False),
        ]
    )
    if absorption:
        params.append(Parameter(name=f"{name} SLD imaginary", min=sld.imag, value=sld.imag, max=sld.imag, fit=False))
        layer = AbsorptionLayer(
            name=name,
            thickness=f"{name} Thickness",
            roughness=f"{name} Roughness",
            SLD_real=f"{name} SLD",
            SLD_imaginary=f"{name} SLD imaginary",
        )
    else:
        layer = Layer(
            name=name,
            thickness=f"{name} Thickness",
            roughness=f"{name} Roughness",
            SLD=f"{name} SLD",
        )

    return params, layer


def get_material_name(
    material: orsopy.fileio.model_language.Material, model: orsopy.fileio.model_language.SampleModel
) -> str:
    """Get the name of a material in the model.

    Layers with custom property definitions may not have a formula, so this adjusts the name for that.

    Parameters
    ----------
    material : Material
        The material to get the name of.
    model : SampleModel
        The sample model from which the material came.

    Returns
    -------
    str
        The name of the material.

    """
    if material.formula is not None:
        return material.formula
    else:
        matching_materials = [k for k, v in model.materials.items() if v == material]
        if matching_materials:
            return matching_materials[0]
        else:
            # orsopy should catch that this is the case before we get here, but just in case...
            raise ValueError("ORSO model contains layers with undefined materials!")
