"""Wrappers for the interface between RATapi and MATLAB custom files."""

import atexit
import os
import pathlib
import platform
import shutil
import subprocess
from contextlib import contextmanager, suppress
from typing import Callable

from numpy.typing import ArrayLike

import RATapi.rat_core

MATLAB_PATH_FILE = os.path.join(os.path.dirname(os.path.realpath(__file__)), "matlab.txt")
__MATLAB_ENGINE = None


def get_matlab_engine():
    """Return MATLAB engine object if available else None.

    Returns
    -------
    engine : Optional[RATapi.rat_core.MatlabEngine]
        A matlab engine object
    """
    return __MATLAB_ENGINE


@contextmanager
def cd(new_dir: str):
    """Context manager to change to a given directory and return to current directory on exit.

    Parameters
    ----------
    new_dir : str
        The path to change to
    """
    prev_dir = os.getcwd()
    os.chdir(os.path.expanduser(new_dir))
    try:
        yield
    finally:
        os.chdir(prev_dir)


def get_matlab_paths(matlab_exe_path: str) -> tuple[str, str]:
    """Return paths for loading MATLAB engine C interface dynamic libraries.

    Parameters
    ----------
    matlab_exe_path : str
        The full path of the MATLAB executable

    Returns
    -------
    paths : Tuple[str, str]
        The path of the MATLAB bin and the DLL location
    """
    if not matlab_exe_path:
        raise FileNotFoundError()

    bin_path = pathlib.Path(matlab_exe_path).parent
    if bin_path.stem != "bin":
        raise FileNotFoundError()

    if platform.system() == "Windows":
        arch = "win64"
    elif platform.system() == "Darwin":
        arch = "maca64" if platform.mac_ver()[-1] == "arm64" else "maci64"
    else:
        arch = "glnxa64"

    dll_path = bin_path / arch
    if not dll_path.exists():
        raise FileNotFoundError(f"The expected MATLAB folders were in found at the path: {dll_path}")

    return f"{bin_path.as_posix()}/", f"{dll_path.as_posix()}/"


def start_matlab(matlab_exe_path: str = ""):
    """Load MATLAB engine dynamic libraries and creates wrapper object.

    Parameters
    ----------
    matlab_exe_path : str, default ""
        The full path of the MATLAB executable

    Returns
    -------
    engine : Optional[RATapi.rat_core.MatlabEngine]
        A matlab engine object
    """
    matlab_exe_path = find_existing_matlab()

    if matlab_exe_path:
        bin_path, dll_path = get_matlab_paths(matlab_exe_path)
        os.environ["MATLAB_DLL_PATH"] = dll_path
        if platform.system() == "Windows":
            os.environ["PATH"] = dll_path + os.pathsep + os.environ["PATH"]

        with cd(bin_path):
            engine = RATapi.rat_core.MatlabEngine()
        # Ensure MATLAB is closed when Python shuts down.
        atexit.register(engine.close)

        return engine


def set_matlab_path(matlab_exe_path: str) -> None:
    """Set the path of MATLAB to use for custom functions.

    This will also register the MATLAB COM server on Windows OS which could be slow.

    Parameters
    ----------
    matlab_exe_path : str
        The full path of the MATLAB executable
    """
    if not matlab_exe_path:
        return

    global __MATLAB_ENGINE
    if __MATLAB_ENGINE is not None:
        __MATLAB_ENGINE.close()
        atexit.unregister(__MATLAB_ENGINE.close)
        __MATLAB_ENGINE = start_matlab(matlab_exe_path)

    if platform.system() == "Windows":
        process = subprocess.Popen(f'"{matlab_exe_path}" -batch "comserver(\'register\')"')
        process.wait()

    with open(MATLAB_PATH_FILE, "w") as path_file:
        path_file.write(matlab_exe_path)


class MatlabWrapper:
    """Creates a python callback for a MATLAB function.

    Parameters
    ----------
    filename : str
        The path of the file containing MATLAB function
    """

    def __init__(self, filename) -> None:
        engine = get_matlab_engine()
        if engine is None:
            raise ValueError(
                "MATLAB is not found. Please use `set_matlab_path` to set the location of your MATLAB installation"
            ) from None

        path = pathlib.Path(filename)
        engine.cd(str(path.parent))
        engine.setFunction(path.stem)

    def get_handle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]:
        """Return a wrapper for the custom dynamic library function.

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]
            The wrapper function for the dynamic library callback

        """

        def handle(*args):
            return get_matlab_engine().invoke(*args)

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

    def get_handle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]:
        """Return a wrapper for the custom dynamic library function.

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]
            The wrapper function for the dynamic library callback

        """

        def handle(*args):
            return self.engine.invoke(*args)

        return handle


def find_existing_matlab() -> str:
    """Find existing MATLAB from cache file or checking if the MATLAB command is available.

    Parameters
    ----------
    matlab_exe_path : str, default ""
        The full path of the MATLAB executable

    Returns
    -------
    engine : Optional[RATapi.rat_core.MatlabEngine]
        A matlab engine object
    """
    matlab_exe_path = ""

    with suppress(FileNotFoundError), open(MATLAB_PATH_FILE) as path_file:
        matlab_exe_path = path_file.read()

    if not matlab_exe_path:
        matlab_exe_path = shutil.which("matlab")
        if matlab_exe_path is None:
            matlab_exe_path = ""
        else:
            temp = pathlib.Path(matlab_exe_path)
            if temp.is_symlink():
                matlab_exe_path = temp.readlink().as_posix()
            set_matlab_path(matlab_exe_path)

    return matlab_exe_path


__MATLAB_ENGINE = start_matlab()
