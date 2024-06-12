"""Test custom function languages."""
import RAT.utils.plotting
import setup_problem
import time

project = setup_problem.make_example_problem()
controls = RAT.set_controls()

# Python
start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"Python time is: {end-start}s")

RAT.utils.plotting.plot_ref_sld(project, results)

# Matlab
project.custom_files.set_fields(0, filename='customBilayer.m', language='matlab')

start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"Matlab time is: {end-start}s")

RAT.utils.plotting.plot_ref_sld(project, results)

# C++
project.custom_files.set_fields(0, filename='customBilayer.dll', language='cpp')

start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"C++ time is: {end-start}s")

RAT.utils.plotting.plot_ref_sld(project, results)
