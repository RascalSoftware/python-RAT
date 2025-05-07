import pathlib

import numpy as np
import pytest

import RATapi as RAT
from RATapi.utils.enums import BackgroundActions
from RATapi.utils.plotting import plot_ref_sld

data_path = pathlib.Path(__file__).parents[0] / "test_data/ORSO_data"
sld_values = np.loadtxt(data_path / "test_3_sld.dat")


@pytest.fixture(scope="module")
def orso_project():
    """Set up a project with all the ORSO validation data included."""
    orso_project = RAT.Project(absorption=True)

    orso_project.parameters.set_fields("Substrate Roughness", min=0.0)
    orso_project.background_parameters.set_fields(0, min=0.0, value=0.0)
    orso_project.resolution_parameters.set_fields(0, min=0.0, value=0.0)
    orso_project.scalefactors.set_fields(0, max=1.0, value=1.0)

    orso_project.bulk_in.extend(
        [
            RAT.models.Parameter(name="Bulk In 0", value=2.07e-6),
            RAT.models.Parameter(name="Bulk In 1", value=0.0),
            RAT.models.Parameter(name="Bulk In 2", value=0.0),
            RAT.models.Parameter(name="Bulk In 3", value=0.0),
            RAT.models.Parameter(name="Bulk In 6", value=2.07e-6),
            RAT.models.Parameter(name="Bulk In 7", value=0.0),
        ]
    )

    orso_project.bulk_out.extend(
        [
            RAT.models.Parameter(name="Bulk Out 0", value=6.0e-6),
            RAT.models.Parameter(name="Bulk Out 1", value=2.0704e-6),
            RAT.models.Parameter(name="Bulk Out 2", value=6.36e-6),
            RAT.models.Parameter(name="Bulk Out 3", value=6.36e-6),
            RAT.models.Parameter(name="Bulk Out 6", value=6.36e-6),
            RAT.models.Parameter(name="Bulk Out 7", value=6.36e-6),
        ]
    )

    orso_project.data.extend(
        [
            RAT.models.Data(name="Data 0", data=np.loadtxt(data_path / "test_0.dat")),
            RAT.models.Data(name="Data 1", data=np.loadtxt(data_path / "test_1.dat")),
            RAT.models.Data(name="Data 2", data=np.loadtxt(data_path / "test_2.dat")),
            RAT.models.Data(name="Data 3", data=np.loadtxt(data_path / "test_3.dat")),
            RAT.models.Data(name="Data 6", data=np.loadtxt(data_path / "test_6.dat")),
            RAT.models.Data(name="Data 7", data=np.loadtxt(data_path / "test_7.dat")),
        ]
    )

    orso_project.contrasts.append(
        name="ORSO Contrast",
        background="Background 1",
        background_action=BackgroundActions.Add,
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        resample=False,
    )

    # Now set up parameters and layers for each test

    # Test 0
    orso_project.parameters.append(name="Test 0 Layer 1 Thickness", value=100.0)
    orso_project.parameters.append(name="Test 0 Layer 1 SLD real", value=3.45e-6)
    orso_project.parameters.append(name="Test 0 Layer 1 SLD imaginary", value=1.0e-7)
    orso_project.parameters.append(name="Test 0 Layer 1 Roughness", value=3.0)

    orso_project.layers.append(
        name="Test 0 Layer 1",
        thickness="Test 0 Layer 1 Thickness",
        SLD_real="Test 0 Layer 1 SLD real",
        SLD_imaginary="Test 0 Layer 1 SLD imaginary",
        roughness="Test 0 Layer 1 Roughness",
    )

    orso_project.parameters.append(name="Test 0 Layer 2 Thickness", value=200.0)
    orso_project.parameters.append(name="Test 0 Layer 2 SLD real", value=5.0e-6)
    orso_project.parameters.append(name="Test 0 Layer 2 SLD imaginary", value=1.0e-8)
    orso_project.parameters.append(name="Test 0 Layer 2 Roughness", value=1.0)

    orso_project.layers.append(
        name="Test 0 Layer 2",
        thickness="Test 0 Layer 2 Thickness",
        SLD_real="Test 0 Layer 2 SLD real",
        SLD_imaginary="Test 0 Layer 2 SLD imaginary",
        roughness="Test 0 Layer 2 Roughness",
    )

    # Test 1
    orso_project.parameters.append(name="Test 1 Layer 1 Thickness", value=30.0)
    orso_project.parameters.append(name="Test 1 Layer 1 SLD real", value=-1.9493e-6)
    orso_project.parameters.append(name="Test 1 Layer 1 SLD imaginary", value=0.0)
    orso_project.parameters.append(name="Test 1 Layer 1 Roughness", value=0.0)

    orso_project.layers.append(
        name="Test 1 Layer 1",
        thickness="Test 1 Layer 1 Thickness",
        SLD_real="Test 1 Layer 1 SLD real",
        SLD_imaginary="Test 1 Layer 1 SLD imaginary",
        roughness="Test 1 Layer 1 Roughness",
    )

    orso_project.parameters.append(name="Test 1 Layer 2 Thickness", value=70.0)
    orso_project.parameters.append(name="Test 1 Layer 2 SLD real", value=9.4245e-6)
    orso_project.parameters.append(name="Test 1 Layer 2 SLD imaginary", value=0.0)
    orso_project.parameters.append(name="Test 1 Layer 2 Roughness", value=0.0)

    orso_project.layers.append(
        name="Test 1 Layer 2",
        thickness="Test 1 Layer 2 Thickness",
        SLD_real="Test 1 Layer 2 SLD real",
        SLD_imaginary="Test 1 Layer 2 SLD imaginary",
        roughness="Test 1 Layer 2 Roughness",
    )

    # Test 2
    orso_project.parameters.append(name="Test 2 Layer 1 Thickness", value=0.0)
    orso_project.parameters.append(name="Test 2 Layer 1 SLD real", value=0.0)
    orso_project.parameters.append(name="Test 2 Layer 1 SLD imaginary", value=0.0)
    orso_project.parameters.append(name="Test 2 Layer 1 Roughness", value=0.0)

    orso_project.layers.append(
        name="Test 2 Layer 1",
        thickness="Test 2 Layer 1 Thickness",
        SLD_real="Test 2 Layer 1 SLD real",
        SLD_imaginary="Test 2 Layer 1 SLD imaginary",
        roughness="Test 2 Layer 1 Roughness",
    )

    # Test 3
    orso_project.parameters.append(name="Test 3 Thickness", value=0.025)
    orso_project.parameters.append(name="Test 3 SLD imaginary", value=0.0)
    orso_project.parameters.append(name="Test 3 Roughness", value=0.0)

    for i, sld in enumerate(sld_values):
        orso_project.parameters.append(name=f"Test 3 Layer {i} SLD real", value=sld)
        orso_project.layers.append(
            name=f"Test 3 Layer {i}",
            thickness="Test 3 Thickness",
            SLD_real=f"Test 3 Layer {i} SLD real",
            SLD_imaginary="Test 3 SLD imaginary",
            roughness="Test 3 Roughness",
        )

    # Test 6
    orso_project.parameters.append(name="Test 6 Layer 1 Thickness", value=1200.0)
    orso_project.parameters.append(name="Test 6 Layer 1 SLD real", value=4.66e-6)
    orso_project.parameters.append(name="Test 6 Layer 1 SLD imaginary", value=1.6e-8)
    orso_project.parameters.append(name="Test 6 Layer 1 Roughness", value=10.0)

    orso_project.layers.append(
        name="Test 6 Layer 1",
        thickness="Test 6 Layer 1 Thickness",
        SLD_real="Test 6 Layer 1 SLD real",
        SLD_imaginary="Test 6 Layer 1 SLD imaginary",
        roughness="Test 6 Layer 1 Roughness",
    )

    # Test 7
    orso_project.parameters.append(name="Test 7 Layer 1 Thickness", value=1200.0)
    orso_project.parameters.append(name="Test 7 Layer 1 SLD real", value=4.66e-6)
    orso_project.parameters.append(name="Test 7 Layer 1 SLD imaginary", value=1.6e-8)
    orso_project.parameters.append(name="Test 7 Layer 1 Roughness", value=10.0)

    orso_project.layers.append(
        name="Test 7 Layer 1",
        thickness="Test 7 Layer 1 Thickness",
        SLD_real="Test 7 Layer 1 SLD real",
        SLD_imaginary="Test 7 Layer 1 SLD imaginary",
        roughness="Test 7 Layer 1 Roughness",
    )

    return orso_project


@pytest.mark.parametrize(
    ["test_index", "substrate_roughness", "layer_model"],
    [
        (0, 5.0, ["Test 0 Layer 1", "Test 0 Layer 2"]),
        (1, 0.0, ["Test 1 Layer 1", "Test 1 Layer 2"] * 10),
        (2, 3.0, ["Test 2 Layer 1"]),
        (3, 0.0, [f"Test 3 Layer {i}" for i in range(len(sld_values))]),
        (6, 3.0, ["Test 6 Layer 1"]),
        (7, 3.0, ["Test 7 Layer 1"]),
    ],
)
def test_orso_validation(orso_project, substrate_roughness, test_index, layer_model):
    orso_project.parameters.set_fields("Substrate Roughness", value=substrate_roughness)

    orso_project.contrasts.set_fields(
        "ORSO Contrast",
        data=f"Data {test_index}",
        bulk_in=f"Bulk In {test_index}",
        bulk_out=f"Bulk Out {test_index}",
        model=layer_model,
    )

    controls = RAT.Controls(procedure="calculate")
    output_project, results = RAT.run(orso_project, controls)

    plot_ref_sld(output_project, results)

    total_error = sum((results.reflectivity[0][:, 1] - results.shiftedData[0][:, 1]) ** 2)

    assert total_error < 1e-10
