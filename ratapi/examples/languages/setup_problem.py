"""A custom example problem for the languages benchmark."""

import pathlib

import numpy as np

import ratapi as RAT


def make_example_problem():
    """Generate a Custom Layers example for Supported DSPC layer.

    Example of using custom layers to model a DSPC supported bilayer.
    """
    problem = RAT.Project(name="Orso lipid example - custom layers", model="custom layers", geometry="substrate/liquid")

    # First we need to set up a parameters group. We will be using a pre-prepared custom model file, so we need to add
    # the relevant parameters we are going to need to define the model (note that Substrate Roughness always exists as
    # parameter 1).
    problem.parameters.append(name="Oxide Thickness", min=5.0, value=20.0, max=60.0, fit=True)
    problem.parameters.append(name="Oxide Hydration", min=0.0, value=0.2, max=0.5, fit=True)
    problem.parameters.append(name="Lipid APM", min=45.0, value=55.0, max=65.0, fit=True)
    problem.parameters.append(name="Head Hydration", min=00.0, value=0.2, max=0.5, fit=True)
    problem.parameters.append(name="Bilayer Hydration", min=0.0, value=0.1, max=0.2, fit=True)
    problem.parameters.append(name="Bilayer Roughness", min=2.0, value=4.0, max=8.0, fit=True)
    problem.parameters.append(name="Water Thickness", min=0.0, value=2.0, max=10.0, fit=True)

    problem.parameters.set_fields(0, min=1.0, max=10.0)

    # Need to add the relevant Bulk SLDs.
    # Change the bulk in from air to silicon, and add two additional water contrasts:
    problem.bulk_in.set_fields(0, name="Silicon", min=2.07e-6, value=2.073e-6, max=2.08e-6, fit=False)

    problem.bulk_out.append(name="SLD SMW", min=1.0e-6, value=2.073e-6, max=3.0e-6, fit=True)
    problem.bulk_out.append(name="SLD H2O", min=-0.6e-6, value=-0.56e-6, max=-0.3e-6, fit=True)

    problem.bulk_out.set_fields(0, min=5.0e-6, fit=True)

    # Now add the datafiles. We have three datasets we need to consider - the bilayer against D2O, Silicon Matched Water
    # and H2O. Load these datafiles in and put them in the data block

    # Read in the datafiles
    data_path = pathlib.Path(__file__).parents[1] / "data"
    D2O_data = np.loadtxt(data_path / "c_PLP0016596.dat", delimiter=",")
    SMW_data = np.loadtxt(data_path / "c_PLP0016601.dat", delimiter=",")
    H2O_data = np.loadtxt(data_path / "c_PLP0016607.dat", delimiter=",")

    # Add the data to the project - note this data has a resolution 4th column
    problem.data.append(name="Bilayer / D2O", data=D2O_data)
    problem.data.append(name="Bilayer / SMW", data=SMW_data)
    problem.data.append(name="Bilayer / H2O", data=H2O_data)

    # Add the custom file to the project
    problem.custom_files.append(
        name="DSPC Model",
        filename="custom_bilayer.py",
        language="python",
        path=pathlib.Path(__file__).parent,
    )

    # Also, add the relevant background parameters - one each for each contrast:
    problem.background_parameters.set_fields(
        0,
        name="Background parameter D2O",
        fit=True,
        min=1.0e-10,
        max=1.0e-5,
        value=1.0e-07,
    )

    problem.background_parameters.append(
        name="Background parameter SMW",
        min=1.0e-10,
        value=1.0e-7,
        max=1.0e-5,
        fit=True,
    )
    problem.background_parameters.append(
        name="Background parameter H2O",
        min=1.0e-10,
        value=1.0e-7,
        max=1.0e-5,
        fit=True,
    )

    # And add the two new constant backgrounds
    problem.backgrounds.append(name="Background SMW", type="constant", source="Background parameter SMW")
    problem.backgrounds.append(name="Background H2O", type="constant", source="Background parameter H2O")

    # And edit the other one
    problem.backgrounds.set_fields(0, name="Background D2O", source="Background parameter D2O")

    # Finally modify some of the other parameters to be more suitable values for a solid / liquid experiment
    problem.scalefactors.set_fields(0, value=1.0, min=0.5, max=2.0, fit=True)

    # Now add the three contrasts
    problem.contrasts.append(
        name="Bilayer / D2O",
        background="Background D2O",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        bulk_out="SLD D2O",
        bulk_in="Silicon",
        data="Bilayer / D2O",
        model=["DSPC Model"],
    )

    problem.contrasts.append(
        name="Bilayer / SMW",
        background="Background SMW",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        bulk_out="SLD SMW",
        bulk_in="Silicon",
        data="Bilayer / SMW",
        model=["DSPC Model"],
    )

    problem.contrasts.append(
        name="Bilayer / H2O",
        background="Background H2O",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        bulk_out="SLD H2O",
        bulk_in="Silicon",
        data="Bilayer / H2O",
        model=["DSPC Model"],
    )

    return problem
