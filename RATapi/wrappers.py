"""Wrappers for the interface between RATapi and MATLAB custom files."""
import os
import pathlib
import platform
import shutil
import subprocess
from typing import Callable

import numpy as np
from numpy.typing import ArrayLike

import RATapi.rat_core


MATLAB_PATH_FILE = os.path.join(os.path.dirname(os.path.realpath(__file__)), "matlab.txt")


def set_matlab_path(matlab_path):
    if not matlab_path:
        return 
    
    if platform.system() == "Windows": 
        process = subprocess.Popen(f'"{matlab_path}" -batch "comserver(\'register\')"')
        process.wait()
        

    with open(MATLAB_PATH_FILE, "w") as path_file:
        path_file.write(matlab_path)


def get_matlab_paths(exe_path):
    if not exe_path:
        raise FileNotFoundError()
    
    bin_path = pathlib.Path(exe_path).parent
    if bin_path.stem != 'bin':
        raise FileNotFoundError()
    
    if platform.system() == "Windows": 
        arch  = "win64"
    elif  platform.system() == "Darwin":
        arch = "maci64" if (bin_path / "maci64").exists() else "maca64"
    else: 
        arch = "glnxa64"
    
    dll_path = bin_path / arch
    if not dll_path.exists():
        raise FileNotFoundError(f"The expected MATLAB folders were in found at the path: {dll_path}")
    
    return f'{bin_path.as_posix()}/' , f'{dll_path.as_posix()}/'

def start_matlab():
    """Start MATLAB asynchronously and returns a future to retrieve the engine later.

    Returns
    -------
    future : RATapi.rat_core.MatlabEngine
        A custom matlab engine wrapper.

    """
    try:
        with open(MATLAB_PATH_FILE) as path_file:
            matlab_path = path_file.read()
    except FileNotFoundError:
        matlab_path = ""
    
    if not matlab_path:
        matlab_path = shutil.which("matlab")
        if matlab_path is None:
            matlab_path = ""
        else:
            print(matlab_path)
            temp = pathlib.Path(matlab_path)
            if temp.is_symlink():
                matlab_path = temp.readlink().as_posix()
            set_matlab_path(matlab_path)
    
    if matlab_path:
        bin_path, dll_path = get_matlab_paths(matlab_path)
        os.environ["MATLAB_DLL_PATH"] = dll_path
        os.environ["PATH"] = bin_path + os.pathsep + os.environ["PATH"]
        os.environ["PATH"] = dll_path + os.pathsep + os.environ["PATH"]
        engine = RATapi.rat_core.MatlabEngine()
    
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
        if self.engine is None:
            raise ValueError("MATLAB is not found. Please use `set_matlab_path` to set the location of your MATLAB installation") from None
        
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
