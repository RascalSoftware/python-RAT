"""Readers from file formats."""

from dataclasses import dataclass

import orsopy
from orsopy.fileio import load_orso

from RATapi import ClassList, Project
from RATapi.models import AbsorptionLayer, Background, Contrast, Data, Layer, Parameter, Resolution


def read_ort(filepath: str) -> Project:
    """Create a project from an .ort file.

    Parameters
    ----------
    filepath : str
        The path to the .ort file.

    Returns
    -------
    Project
        The project data from the .ort file.
    """
    project = Project()

    ort_data = load_orso(filepath)

    for dataset in ort_data:
        metadata = dataset.info
        sample = metadata.data_source.sample
        model_info = orso_model_to_rat(sample.model)

        # Add all parameters that aren't already defined
        project.parameters.union(model_info.parameters)
        project.layers.extend(model_info.layers)
        project.bulk_in.append(model_info.bulk_in)
        project.bulk_out.append(model_info.bulk_out)

        project.data.append(Data(name=sample.name, data=dataset.data))

        if dataset.data.shape[1] == 4:
            project.resolutions.append(
                Resolution(
                    name=f"{sample.name} Resolution",
                    type="data",
                )
            )
        else:
            project.resolution_parameters.append(Parameter(name=f"{sample.name} Resolution Parameter"))
            project.resolutions.append(
                Resolution(
                    name=f"{sample.name} Resolution",
                    type="constant",
                    source=f"{sample.name} Resolution Parameter",
                )
            )

        project.background_parameters.append(Parameter(name=f"{sample.name} Background Parameter"))
        project.backgrounds.append(
            Background(
                name=f"{sample.name} Background",
                type="constant",
                source=f"{sample.name} Background Parameter",
            )
        )

        project.scalefactors.append(Parameter(name=f"{sample.name} Scalefactor"))

        project.contrasts.append(
            Contrast(
                name=metadata.data_source.experiment.title,
                data=sample.name,
                background=f"{sample.name} Background",
                bulk_in=model_info.bulk_in.name,
                bulk_out=model_info.bulk_out.name,
                scalefactor=f"{sample.name} Scalefactor",
                resolution=f"{sample.name} Resolution",
                model=[layer.name for layer in model_info.layers],
            )
        )

    return project


@dataclass
class ORSOSample:
    """The stack data from an ORSO SampleModel, in RAT models."""

    bulk_in: Parameter
    bulk_out: Parameter
    parameters: ClassList[Parameter]
    layers: ClassList[Layer]


def orso_model_to_rat(model: orsopy.fileio.model_language.SampleModel | str) -> ORSOSample:
    """Get information from an ORSO SampleModel object.

    Parameters
    ----------
    model : orsopy.fileio.model_language.SampleModel or str
        The sample model to turn into a RAT data. If given as a string,
        the string is interpreted as a layer stack in ORSO model language.

    Returns
    -------
    ORSOSample
        A dataclass containing the sample data.

    """
    if isinstance(model, str):
        model = orsopy.fileio.model_language.SampleModel(stack=model)
    stack = model.resolve_to_layers()
    # if bulk in or out is air, it has SLD predefined
    # else we need to grab it from SLDDB
    if bulk_in_sld := stack[0].material.sld is None:
        bulk_in_sld = stack[0].material.get_sld()

    # orsopy SLDs are in 10^-6 inverse square Angstroms
    # but RAT uses just inverse square Angstroms
    bulk_in_sld *= 1e6

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

    bulk_out_sld *= 1e6

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

    for orso_layer in stack[1:-1]:
        layer_params, layer = orso_layer_to_rat_layer(orso_layer)
        parameters.union(layer_params)
        layers.append(layer)

    return ORSOSample(bulk_in=bulk_in, bulk_out=bulk_out, parameters=parameters, layers=layers)


def orso_layer_to_rat_layer(
    layer: orsopy.fileio.model_language.Layer, absorption: bool = False
) -> tuple[ClassList[Parameter], Layer]:
    """Convert an ``orsopy`` layer to a RAT layer.

    Parameters
    ----------
    layer : orsopy.fileio.model_language.Layer
        An ``orsopy`` Layer.
    absorption : bool, default True
        Whether absorption should be accounted for in the layer.

    Returns
    -------
    ClassList[Parameter], Layer
        The parameters required for the RAT layer and the layer itself.

    """
    name = layer.material.formula
    thickness = layer.thickness.as_unit("angstrom")
    roughness = layer.roughness.as_unit("angstrom")

    # orsopy SLDs are in 10^-6 inverse square Angstroms
    # but RAT uses just inverse square Angstroms
    sld = layer.material.get_sld() * 1e6

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
            name=f"{name}",
            thickness=f"{name} Thickness",
            roughness=f"{name} Roughness",
            SLD_real=f"{name} SLD",
            SLD_imag=f"{name} SLD imaginary",
        )
    else:
        layer = Layer(
            name=f"{name}",
            thickness=f"{name} Thickness",
            roughness=f"{name} Roughness",
            SLD=f"{name} SLD",
        )

    return params, layer
