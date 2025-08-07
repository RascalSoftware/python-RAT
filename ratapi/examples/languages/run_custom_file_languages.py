"""Test custom function languages."""

import pathlib
import time

import setup_problem

import ratapi as RAT

path = pathlib.Path(__file__).parent

project = setup_problem.make_example_problem()
controls = RAT.Controls()

# Python
start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"Python time is: {end - start}s\n")

RAT.plotting.plot_ref_sld(project, results)

# Matlab
project.custom_files.set_fields(0, filename="custom_bilayer.m", language="matlab", path=path)

start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"Matlab time is: {end - start}s\n")

RAT.plotting.plot_ref_sld(project, results)

# C++
project.custom_files.set_fields(0, filename="custom_bilayer.dll", language="cpp", path=path)

start = time.time()
project, results = RAT.run(project, controls)
end = time.time()
print(f"C++ time is: {end - start}s\n")

RAT.plotting.plot_ref_sld(project, results)
