"""An example of using domains with custom layer models."""

import pathlib

import ratapi as RAT


def domains_custom_layers():
    """Calculate an example custom layers domains project involving incoherent summing on a permalloy layer.

    For a custom layers model, rather than being forced to define our layers as [Thick SLD Rough…. etc],
    we can parameterise however we like and then use a function to calculate the parameters for each layer.
    So for example, if the volume of lipid tails are known (from the literature),
    then all we need is the Area per molecule to calculate the layers.
    """
    problem = RAT.Project(calculation="domains", model="custom layers", geometry="substrate/liquid")

    # Make some parameters...
    problem.parameters.append(name="Alloy Thickness", min=100.0, value=150.0, max=200.0, fit=True)
    problem.parameters.append(name="Alloy SLD up", min=9.0e-6, value=11.0e-6, max=13.0e-6, fit=True)
    problem.parameters.append(name="Alloy SLD down", min=5.0e-6, value=7.0e-6, max=10.0e-6, fit=True)
    problem.parameters.append(name="Alloy Roughness", min=5.0, value=7.0, max=11.0, fit=True)
    problem.parameters.append(name="Gold Thickness", min=100.0, value=150.0, max=200.0, fit=True)
    problem.parameters.append(name="Gold SLD", min=4.0e-6, value=4.5e-6, max=5.0e-6, fit=True)
    problem.parameters.append(name="Gold Roughness", min=5.0, value=7.0, max=11.0, fit=True)

    # Set the bulk SLD
    problem.bulk_in.set_fields(0, name="Silicon", value=2.073e-6, max=1.0)

    # Add the custom file
    problem.custom_files.append(
        name="Alloy domains",
        filename="alloy_domains.py",
        language="python",
        path=pathlib.Path(__file__).parent,
    )

    # Make a contrast
    problem.contrasts.append(
        name="D2O Contrast",
        data="Simulation",
        background="Background 1",
        bulk_in="Silicon",
        bulk_out="SLD D2O",
        scalefactor="Scalefactor 1",
        resolution="Resolution 1",
        resample=False,
        domain_ratio="Domain Ratio 1",
        model=["Alloy domains"],
    )

    controls = RAT.Controls()
    problem, results = RAT.run(problem, controls)

    return problem, results


if __name__ == "__main__":
    problem, results = domains_custom_layers()
    RAT.plotting.plot_ref_sld(problem, results, True)
