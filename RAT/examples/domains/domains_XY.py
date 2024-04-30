"""Simple example of a layer containing domains using a custom XY model"""
import RAT

problem = RAT.Project(calculation="domains", model="custom xy", geometry="substrate/liquid")

problem.parameters.append(name="Oxide Thickness", min=10.0, value=20.0, max=50.0, fit=True)
problem.parameters.append(name="Layer Thickness", min=1.0, value=30.0, max=500.0, fit=True)
problem.parameters.append(name="Layer SLD", min=-0.5e-6, value=-0.5e-6, max=0.0, fit=True)
problem.parameters.append(name="Layer Roughness", min=2.0, value=5.0, max=7.0, fit=True)
problem.parameters.append(name="Domain SLD", min=1.0e-6, value=1.0e-6, max=5.0e-6, fit=True)

problem.bulk_in.set_fields(0, name="Silicon", value=2.073e-6, max=1.0, fit=False)
problem.scalefactors.set_fields(0, min=0.8, value=1.0, max=1.1, fit=True)

problem.bulk_out.append(name="SLD SMW", min=2.0e-6, value=2.073e-6, max=2.1e-6)
problem.bulk_out.append(name="SLD H2O", min=-0.6e-6, value=-0.56e-6, max=-0.5e-6)

# Add the custom file
problem.custom_files.append(name="Domain Layer", filename="domainsXY", language="matlab", path="pwd")

# Make contrasts
problem.contrasts.append(name="D2O", background="Background 1", resolution="Resolution 1", scalefactor="Scalefactor 1",
                         bulk_in="Silicon", bulk_out="SLD D2O", domain_ratio="Domain Ratio 1", data="Simulation",
                         model=["Domain Layer"])

problem.contrasts.append(name="SMW", background="Background 1", resolution="Resolution 1", scalefactor="Scalefactor 1",
                         bulk_in="Silicon", bulk_out="SLD SMW", domain_ratio="Domain Ratio 1", data="Simulation",
                         model=["Domain Layer"])

problem.contrasts.append(name="H2O", background="Background 1", resolution="Resolution 1", scalefactor="Scalefactor 1",
                         bulk_in="Silicon", bulk_out="SLD H2O", domain_ratio="Domain Ratio 1", data="Simulation",
                         model=["Domain Layer"])
