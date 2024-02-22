RAT PyBind Demo
===============
This is a demonstration of using the generated C++ from MATLAB to create a python extension with PyBind11

Build
=====
This has been tested using python 3.9, a c++ compiler is required to run the code. Install the requirement then run the example, if the 
compiler can be found, the python extension (*.pyd) will be built and the example will run

Install
=======
To install in local directory:

    pip install -e .

To build wheel:
    pip install build
    python -m build --wheel

matlabengine is an optional dependency only required for Matlab custom functions. The version of matlabengine should match the version of Matlab installed on the machine. This can be installed as shown below:
    pip install -e .[Matlab-2023a]
