import pathlib

import pytest

import RATapi as RAT
from RATapi.utils.plotting import plot_ref_sld

PROJECT_PATH = pathlib.Path(__file__).parents[0] / "test_data"


@pytest.fixture(scope="module")
def orso_project():
    """Load a project with all the ORSO validation data included."""
    return RAT.Project.load(pathlib.Path(PROJECT_PATH, "orso_validation.json"))


@pytest.mark.parametrize(
    ["test_index", "substrate_roughness", "layer_model"],
    [
        (0, 5.0, ["Test 0 Layer 1", "Test 0 Layer 2"]),
        (1, 0.0, ["Test 1 Layer 1", "Test 1 Layer 2"] * 10),
        (2, 3.0, ["Test 2 Layer 1"]),
        (3, 0.0, [f"Test 3 Layer {i}" for i in range(2001)]),
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
