"""An example custom layers domains project involving incoherent summing on a permalloy layer"""

import pathlib

import RAT
import RAT.utils.plotting

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
    path=pathlib.Path(__file__).parent.resolve(),
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

controls = RAT.set_controls()

problem, results = RAT.run(problem, controls)
RAT.utils.plotting.plot_ref_sld(problem, results, True)
