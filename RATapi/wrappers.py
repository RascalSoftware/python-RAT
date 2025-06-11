"""Wrappers for the interface between RATapi and MATLAB custom files."""
import os
import pathlib
from typing import Callable

import numpy as np
from numpy.typing import ArrayLike

import RATapi.rat_core


def find_matlab():
    pass




def start_matlab():
    """Start MATLAB asynchronously and returns a future to retrieve the engine later.

    Returns
    -------
    future : RATapi.rat_core.MatlabEngine
        A custom matlab engine wrapper.

    """

    path = os.path.join(os.path.dirname(os.path.realpath(__file__)), "matlab.txt")
    # if pathlib(matlab_path).is_file()
    with open(path) as path_file:
        matlab_path = path_file.read()
    
    os.environ["RAT_PATH"] = dir_path, "")
    os.environ["MATLAB_INSTALL_DIR"] += os.pathsep + "C:\\Program Files\\MATLAB\\R2023a\\bin\\win64"
    engine = RATapi.rat_core.MatlabEngine()
    engine.start()
    
    return engine



class MatlabWrapper:
    """Creates a python callback for a MATLAB function.

    Parameters
    ----------
    filename : string
        The path of the file containing MATLAB function
    """
    engine = start_matlab()
    
    def __init__(self, filename) -> None:
        path = pathlib.Path(filename)
        self.engine.cd(str(path.parent))
        self.engine.setFunction(path.stem)

    def getHandle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]:
        """Return a wrapper for the custom dynamic library function.

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]
            The wrapper function for the dynamic library callback

        """

        def handle(*args):
            return self.engine.invoke(*args)

        return handle



class DylibWrapper:
    """Creates a python callback for a function in dynamic library.

    Parameters
    ----------
    filename : str
        The path of the dynamic library
    function_name : str
        The name of the function to call

    """

    def __init__(self, filename, function_name) -> None:
        self.engine = RATapi.rat_core.DylibEngine(filename, function_name)

    def getHandle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]:
        """Return a wrapper for the custom dynamic library function.

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]
            The wrapper function for the dynamic library callback

        """

        def handle(*args):
            return self.engine.invoke(*args)

        return handle
