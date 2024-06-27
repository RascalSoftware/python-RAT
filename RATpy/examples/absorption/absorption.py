"""Custom layers model including absorption"""

import os
import pathlib

import numpy as np

import RATpy as RAT

problem = RAT.Project(
    name="Absorption example",
    calculation="non polarised",
    model="custom layers",
    geometry="substrate/liquid",
    absorption=True,
)

# Add the required parameters (substrate roughness is already there by default)
problem.parameters.append(name="Alloy Thickness", min=100.0, value=135.6, max=200.0, fit=True)
problem.parameters.append(name="Alloy SLD up", min=6.0e-6, value=9.87e-6, max=1.2e-5, fit=True)
problem.parameters.append(name="Alloy SLD imaginary up", min=1.0e-9, value=4.87e-8, max=1.0e-7, fit=True)
problem.parameters.append(name="Alloy SLD down", min=6.0e-6, value=7.05e-6, max=1.3e-5, fit=True)
problem.parameters.append(name="Alloy SLD imaginary down", min=1.0e-9, value=4.87e-8, max=1.0e-7, fit=True)
problem.parameters.append(name="Alloy Roughness", min=2.0, value=5.71, max=10.0, fit=True)
problem.parameters.append(name="Gold Thickness", min=100.0, value=154.7, max=200.0, fit=True)
problem.parameters.append(name="Gold Roughness", min=0.1, value=5.42, max=10.0, fit=True)
problem.parameters.append(name="Gold SLD", min=4.0e-6, value=4.49e-6, max=5.0e-6, fit=True)
problem.parameters.append(name="Gold SLD imaginary", min=1.0e-9, value=4.20e-8, max=1.0e-7, fit=True)

problem.parameters.append(name="Thiol APM", min=40.0, value=56.27, max=100.0, fit=True)
problem.parameters.append(name="Thiol Head Hydration", min=20.0, value=30.0, max=50.0, fit=True)
problem.parameters.append(name="Thiol Coverage", min=0.5, value=0.9, max=1.0, fit=True)

problem.parameters.append(name="CW Thickness", min=1.0, value=12.87, max=25.0, fit=True)
problem.parameters.append(name="Bilayer APM", min=48.0, value=65.86, max=90.0, fit=True)
problem.parameters.append(name="Bilayer Head Hydration", min=20.0, value=30.0, max=50.0, fit=True)
problem.parameters.append(name="Bilayer Roughness", min=1.0, value=3.87, max=10.0, fit=True)
problem.parameters.append(name="Bilayer Coverage", min=0.5, value=0.94, max=1.0, fit=True)

# Change the existing Bulk In parameter to be Silicon
problem.bulk_in.set_fields(0, name="Silicon", min=2.0e-6, value=2.073e-6, max=2.1e-6)

# We need 2 bulk outs - D2O and H2O
problem.bulk_out.set_fields(0, name="D2O", min=5.8e-06, value=6.21e-06, max=6.35e-06, fit=True)
problem.bulk_out.append(name="H2O", min=-5.6e-07, value=-3.15e-07, max=0.0, fit=True)

# Use a different scalefactor for each dataset
del problem.scalefactors[0]
problem.scalefactors.append(name="Scalefactor 1", min=0.5, value=1, max=1.5, fit=True)
problem.scalefactors.append(name="Scalefactor 2", min=0.5, value=1, max=1.5, fit=True)
problem.scalefactors.append(name="Scalefactor 3", min=0.5, value=1, max=1.5, fit=True)
problem.scalefactors.append(name="Scalefactor 4", min=0.5, value=1, max=1.5, fit=True)

# Similarly, use an individual background for each dataset
del problem.backgrounds[0]
del problem.background_parameters[0]

problem.background_parameters.append(name="Background parameter 1", min=5.0e-08, value=7.88e-06, max=9.0e-05, fit=True)
problem.background_parameters.append(name="Background parameter 2", min=1.0e-08, value=5.46e-06, max=9.0e-05, fit=True)
problem.background_parameters.append(name="Background parameter 3", min=1.0e-06, value=9.01e-06, max=9.0e-05, fit=True)
problem.background_parameters.append(name="Background parameter 4", min=1.0e-06, value=5.61e-06, max=9.0e-05, fit=True)

problem.backgrounds.append(name="Background 1", type="constant", value_1="Background parameter 1")
problem.backgrounds.append(name="Background 2", type="constant", value_1="Background parameter 2")
problem.backgrounds.append(name="Background 3", type="constant", value_1="Background parameter 3")
problem.backgrounds.append(name="Background 4", type="constant", value_1="Background parameter 4")

# Make the resolution fittable
problem.resolution_parameters.set_fields(0, fit=True)

# Now add the data we need
data_path = os.path.join(pathlib.Path(__file__).parents[1].resolve(), "data")

data_1 = np.loadtxt(os.path.join(data_path, "D2O_spin_down.dat"))
problem.data.append(name="D2O_dn", data=data_1)

data_2 = np.loadtxt(os.path.join(data_path, "D2O_spin_up.dat"))
problem.data.append(name="D2O_up", data=data_2)

data_3 = np.loadtxt(os.path.join(data_path, "H2O_spin_down.dat"))
problem.data.append(name="H2O_dn", data=data_3)

data_4 = np.loadtxt(os.path.join(data_path, "H2O_spin_up.dat"))
problem.data.append(name="H2O_up", data=data_4)

# Add the custom file
problem.custom_files.append(
    name="DPPC absorption",
    filename="volume_thiol_bilayer.py",
    language="python",
    path=pathlib.Path(__file__).parent.resolve(),
)

# Finally add the contrasts
problem.contrasts.append(
    name="D2O Down",
    data="D2O_dn",
    background="Background 1",
    bulk_in="Silicon",
    bulk_out="D2O",
    scalefactor="Scalefactor 1",
    resolution="Resolution 1",
    resample=True,
    model=["DPPC absorption"],
)

problem.contrasts.append(
    name="D2O Up",
    data="D2O_up",
    background="Background 2",
    bulk_in="Silicon",
    bulk_out="D2O",
    scalefactor="Scalefactor 2",
    resolution="Resolution 1",
    resample=True,
    model=["DPPC absorption"],
)

problem.contrasts.append(
    name="H2O Down",
    data="H2O_dn",
    background="Background 3",
    bulk_in="Silicon",
    bulk_out="H2O",
    scalefactor="Scalefactor 3",
    resolution="Resolution 1",
    resample=True,
    model=["DPPC absorption"],
)

problem.contrasts.append(
    name="H2O Up",
    data="H2O_up",
    background="Background 4",
    bulk_in="Silicon",
    bulk_out="H2O",
    scalefactor="Scalefactor 4",
    resolution="Resolution 1",
    resample=True,
    model=["DPPC absorption"],
)

# Now make a controls block
controls = RAT.set_controls(parallel="contrasts", resampleParams=[0.9, 150.0])

# Run the code and plot the results
problem, results = RAT.run(problem, controls)
RAT.plotting.plot_ref_sld(problem, results, True)
