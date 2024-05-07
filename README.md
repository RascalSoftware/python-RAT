python-RAT
==========
Python-RAT is the Python interface for the [Reflectivity Algorithm Toolbox](https://github.com/RascalSoftware/RAT) (RAT).

Install
=======
To install in local directory:

    pip install -e .

matlabengine is an optional dependency only required for Matlab custom functions. The version of matlabengine should match the version of Matlab installed on the machine. This can be installed as shown below:

    pip install -e .[Matlab-2023a]

Development dependencies can be installed as shown below

    pip install -e .[Dev]    

To build wheel:

    pip install build
    python -m build --wheel
