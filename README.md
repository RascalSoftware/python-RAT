RAT PyBind Demo
===============
This is a demonstration of using the generated C++ from MATLAB to create a python extension with PyBind11

Build
=====
This has been tested using python 3.8, a c++ compiler is required to run the code. Install the requirement then run the example, if the 
compiler can be found, the python extension (*.pyd) will be built and the example will run

Notes
=====
The following files need to be copied to the generated c++

1. tmwtypes.h from matlabroot\extern\include
2. eventHelper.hpp from compile/events
3. dylib.hpp, classHandle.hpp from compile/customWrapper