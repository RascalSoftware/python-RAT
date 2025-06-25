"""A standard layers example with a data background."""

import pathlib

import numpy as np

import ratapi as RAT


def DSPC_data_background():
    """Calculate a Standard Layers fit of a DSPC floating bilayer with a data background."""
    problem = RAT.Project(name="original_dspc_bilayer", model="standard layers", geometry="substrate/liquid")

    # Set up the relevant parameters
    problem.parameters.append(name="Oxide Thickness", min=5.0, value=19.54, max=60.0, fit=True)
    problem.parameters.append(name="Oxide SLD", min=3.39e-06, value=3.39e-06, max=3.41e-06, fit=False)
    problem.parameters.append(name="SAM Tails Thickness", min=15.0, value=22.66, max=35.0, fit=True)
    problem.parameters.append(name="SAM Tails SLD", min=-5e-07, value=-4.01e-07, max=-3e-07, fit=False)
    problem.parameters.append(name="SAM Tails Hydration", min=1.0, value=5.252, max=50.0, fit=True)
    problem.parameters.append(name="SAM Roughness", min=1.0, value=5.64, max=15.0, fit=True)
    problem.parameters.append(name="CW Thickness", min=10.0, value=17.12, max=28.0, fit=True)
    problem.parameters.append(name="CW SLD", min=0.0, value=0.0, max=1e-09, fit=False)

    problem.parameters.append(
        name="SAM Heads Thickness",
        min=5.0,
        value=8.56,
        max=17.0,
        fit=True,
        prior_type="gaussian",
        mu=10.0,
        sigma=2.0,
    )
    problem.parameters.append(name="SAM Heads SLD", min=1.0e-07, value=1.75e-06, max=2.0e-06, fit=False)
    problem.parameters.append(
        name="SAM Heads Hydration",
        min=10.0,
        value=45.45,
        max=50.0,
        fit=True,
        prior_type="gaussian",
        mu=30.0,
        sigma=3.0,
    )
    problem.parameters.append(
        name="Bilayer Heads Thickness",
        min=7.0,
        value=10.7,
        max=17.0,
        fit=True,
        prior_type="gaussian",
        mu=10.0,
        sigma=2.0,
    )
    problem.parameters.append(name="Bilayer Heads SLD", min=5.0e-07, value=1.47e-06, max=1.5e-06, fit=False)
    problem.parameters.append(name="Bilayer Roughness", min=2.0, value=6.014, max=15.0, fit=True)
    problem.parameters.append(name="Bilayer Tails Thickness", min=14.0, value=17.82, max=22.0, fit=True)
    problem.parameters.append(name="Bilayer Tails SLD", min=-5.0e-07, value=-4.61e-07, max=0.0, fit=False)
    problem.parameters.append(name="Bilayer Tails Hydration", min=10.0, value=17.64, max=50.0, fit=True)
    problem.parameters.append(name="Bilayer Heads Hydration", min=10.0, value=36.15, max=50.0, fit=True)
    problem.parameters.append(name="CW Hydration", min=99.9, value=100.0, max=100.0, fit=False)
    problem.parameters.append(name="Oxide Hydration", min=0.0, value=23.61, max=60.0, fit=True)

    problem.parameters.set_fields(0, max=10)

    # Group these into layers
    problem.layers.append(
        name="Oxide",
        thickness="Oxide Thickness",
        SLD="Oxide SLD",
        roughness="Substrate Roughness",
        hydration="Oxide Hydration",
        hydrate_with="bulk out",
    )

    problem.layers.append(
        name="SAM Tails",
        thickness="SAM Tails Thickness",
        SLD="SAM Tails SLD",
        roughness="SAM Roughness",
        hydration="SAM Tails Hydration",
        hydrate_with="bulk out",
    )

    problem.layers.append(
        name="SAM Heads",
        thickness="SAM Heads Thickness",
        SLD="SAM Heads SLD",
        roughness="SAM Roughness",
        hydration="SAM Heads Hydration",
        hydrate_with="bulk out",
    )

    problem.layers.append(
        name="Central Water",
        thickness="CW Thickness",
        SLD="CW SLD",
        roughness="Bilayer Roughness",
        hydration="CW Hydration",
        hydrate_with="bulk out",
    )

    problem.layers.append(
        name="Bilayer Heads",
        thickness="Bilayer Heads Thickness",
        SLD="Bilayer Heads SLD",
        roughness="Bilayer Roughness",
        hydration="Bilayer Heads Hydration",
        hydrate_with="bulk out",
    )

    problem.layers.append(
        name="Bilayer Tails",
        thickness="Bilayer Tails Thickness",
        SLD="Bilayer Tails SLD",
        roughness="Bilayer Roughness",
        hydration="Bilayer Tails Hydration",
        hydrate_with="bulk out",
    )

    # Make the bulk SLDs
    del problem.bulk_in[0]
    problem.bulk_in.append(name="Silicon", min=2.0e-06, value=2.073e-06, max=2.1e-06, fit=False)

    del problem.bulk_out[0]
    problem.bulk_out.append(name="D2O", min=5.50e-06, value=5.98e-06, max=6.4e-06, fit=True)
    problem.bulk_out.append(name="SMW", min=1.0e-06, value=2.21e-06, max=4.99e-06, fit=True)

    # Set the scalefactors - use one for each contrast
    del problem.scalefactors[0]
    problem.scalefactors.append(name="Scalefactor 1", min=0.05, value=0.10, max=0.2, fit=False)
    problem.scalefactors.append(name="Scalefactor 2", min=0.05, value=0.15, max=0.2, fit=False)

    # Now deal with the backgrounds
    # SMW has a constant background
    del problem.backgrounds[0]
    del problem.background_parameters[0]
    problem.background_parameters.append(
        name="Background parameter SMW",
        min=1.0e-10,
        value=3.38e-06,
        max=4.99e-06,
        fit=True,
    )
    problem.backgrounds.append(name="SMW Background", type="constant", source="Background parameter SMW")

    data_path = pathlib.Path(__file__).parents[1] / "data"

    # load in background data for D2O
    d2o_background = np.loadtxt(data_path / "d2o_background_data.dat")
    problem.data.append(name="D2O Background Data", data=d2o_background)

    # add background parameter for the offset
    problem.background_parameters.append(
        name="D2O Data Offset",
        min=-1e-8,
        value=0,
        max=1e-8,
        fit=True,
    )

    # add the background with data and offset
    problem.backgrounds.append(
        name="D2O Data Background",
        type="data",
        source="D2O Background Data",
        value_1="D2O Data Offset",
    )

    # Now add the data
    d2o_dat = np.loadtxt(data_path / "DSPC_D2O.dat", delimiter=",")
    problem.data.append(name="dspc_bil_D2O", data=d2o_dat)

    smw_dat = np.loadtxt(data_path / "DSPC_SMW.dat", delimiter=",")
    problem.data.append(name="dspc_bil_smw", data=smw_dat)

    # Set the model
    stack = [
        "Oxide",
        "SAM Tails",
        "SAM Heads",
        "Central Water",
        "Bilayer Heads",
        "Bilayer Tails",
        "Bilayer Tails",
        "Bilayer Heads",
    ]

    # Then make the two contrasts
    problem.contrasts.append(
        name="D2O",
        bulk_in="Silicon",
        bulk_out="D2O",
        background="D2O Data Background",
        resolution="Resolution 1",
        scalefactor="Scalefactor 1",
        data="dspc_bil_D2O",
        model=stack,
    )

    problem.contrasts.append(
        name="SMW",
        bulk_in="Silicon",
        bulk_out="SMW",
        background="SMW Background",
        resolution="Resolution 1",
        scalefactor="Scalefactor 2",
        data="dspc_bil_smw",
        model=stack,
    )

    controls = RAT.Controls()
    problem, results = RAT.run(problem, controls)

    return problem, results


if __name__ == "__main__":
    problem, results = DSPC_data_background()
    RAT.plotting.plot_ref_sld(problem, results, True)
