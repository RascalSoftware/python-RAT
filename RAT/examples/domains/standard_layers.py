import RAT

problem = RAT.Project(calculation="domains")

# Define the parameters we need to define our two domains...
problem.parameters.append(name="L1 Thickness", min=5.0, value=20.0, max=60.0, fit=True)
problem.parameters.append(name="L1 SLD", min=3.0e-6, value=4.1e-6, max=5.0e-6, fit=False)
problem.parameters.append(name="L1 Roughness", min=2.0, value=5.0, max=20.0, fit=True)
problem.parameters.append(name="L1 Hydration", min=10.0, value=20.0, max=30.0, fit=True)

problem.parameters.append(name="L2 Thickness", min=5.0, value=60.0, max=100.0, fit=True)
problem.parameters.append(name="L2 SLD", min=2.1e-6, value=3.0e-6, max=5.0e-6, fit=False)
problem.parameters.append(name="L2 Roughness", min=2.0, value=5.0, max=20.0, fit=True)
problem.parameters.append(name="L2 Hydration", min=10.0, value=20.0, max=30.0, fit=True)

problem.parameters.append(name="L3 Thickness", min=5.0, value=200.0, max=300.0, fit=True)
problem.parameters.append(name="L3 SLD", min=3.0e-6, value=7.0e-6, max=8.0e-6, fit=False)
problem.parameters.append(name="L3 Roughness", min=2.0, value=5.0, max=20.0, fit=True)
problem.parameters.append(name="L3 Hydration", min=10.0, value=20.0, max=30.0, fit=True)


# Now group these parameters into layers
problem.layers.append(name="Layer 1", thickness="L1 Thickness", SLD="L1 SLD", roughness="L1 Roughness",
                      hydration="L1 Hydration", hydrate_with="bulk out")

problem.layers.append(name="Layer 2", thickness="L2 Thickness", SLD="L2 SLD", roughness="L2 Roughness",
                      hydration="L2 Hydration", hydrate_with="bulk out")

problem.layers.append(name="Layer 3", thickness="L3 Thickness", SLD="L3 SLD", roughness="L3 Roughness",
                      hydration="L3 Hydration", hydrate_with="bulk out")


# If we look at the project, there are two extra groups as compared to a normal standard layers calculation:
# Domain Contrasts and Domain Ratios
problem.domain_contrasts.append(name="Domain 1", model=["Layer 1"])
problem.domain_contrasts.append(name="Domain 2", model=["Layer 2", "Layer 3"])

# Now make a contrast as with standard models, but this time also including the default domain ratio ("Domain Ratio 1")
problem.contrasts.append(name="Domain Test", background="Background 1", resolution="Resolution 1",
                         scalefactor="Scalefactor 1", resample=False, bulk_in="SLD Air", bulk_out="SLD D2O",
                         domain_ratio="Domain Ratio 1", data="Simulation", model=["Domain 1", "Domain 2"])


# Now we can run our simulation as usual, and plot the results....

# Make a controls class....
controls = RAT.set_controls()

# Send everything to RAT....
#[problem,results] = RAT(problem,controls);

#plotRefSLD(problem,results);