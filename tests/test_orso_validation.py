import os
import pathlib
import tempfile

import numpy as np
import pytest

import ratapi as RAT

TEST_FUNC = """import pathlib
import numpy as np


def run_func(params, bulk_in, bulk_out, contrast):
    layers = np.loadtxt("{layers_file_path}")

    # Change the units to Å
    layers[:, 1:3] = layers[:, 1:3] * 1e-6

    # Returns layers only, bulk in and bulk out added to project
    return layers[1:-1, :], params[0]
"""


@pytest.mark.parametrize(
    "layer_index",
    [0, 1, 2, 3, 6, 7],
)
def test_orso_validation(layer_index):
    data_path = pathlib.Path(__file__).parent / "test_data" / "ORSO"

    problem = RAT.Project(name="test", model="custom layers", absorption=True)
    problem.scalefactors.set_fields(0, min=1, value=1, max=1)
    problem.background_parameters.set_fields(0, min=0, value=0, max=0)
    problem.resolution_parameters.set_fields(0, min=0, value=0, max=0)

    # Write a custom file that reads the ORSO layers.
    filename = data_path / f"test{layer_index}.layers"
    with tempfile.NamedTemporaryFile("w+", suffix=".py", delete=False) as f:
        f.write(TEST_FUNC.format(layers_file_path=filename.as_posix()))
        f.flush()

        layers = np.loadtxt(filename)
        sub_rough = layers[-1, -1]

        # Change the units to Å
        bulk_in = layers[0, 1] * 1e-6
        bulk_out = layers[-1, 1] * 1e-6

        problem.parameters.set_fields(0, min=sub_rough, value=sub_rough, max=sub_rough)
        problem.bulk_in.set_fields(0, name="Bulk In", min=bulk_in, value=bulk_in, max=bulk_in, fit=False)
        problem.bulk_out.set_fields(0, name="Bulk Out", min=bulk_out, value=bulk_out, max=bulk_out, fit=False)

        data = np.loadtxt(data_path / f"test_{layer_index}.dat")
        problem.data.append(name="Data", data=data)

        problem.custom_files.append(
            name="Model",
            filename=pathlib.Path(f.name).name,
            language="python",
            path=pathlib.Path(f.name).parent,
            function_name="run_func",
        )

        problem.contrasts.append(
            name="ORSO Contrast",
            background="Background 1",
            background_action="add",
            resolution="Resolution 1",
            scalefactor="Scalefactor 1",
            bulk_out="Bulk Out",
            bulk_in="Bulk In",
            data="Data",
            resample=False,
            model=["Model"],
        )

        controls = RAT.Controls()
        problem, results = RAT.run(problem, controls)
        total_error = sum((results.reflectivity[0][:, 1] - results.shiftedData[0][:, 1]) ** 2)

        assert total_error < 1e-10
        f.close()
        os.remove(f.name)
