"""Wrappers for the interface between ratapi and MATLAB custom files."""

import os
import pathlib
from collections.abc import Callable
from contextlib import suppress

import numpy as np
from numpy.typing import ArrayLike

import ratapi.rat_core


def start_matlab():
    """Start MATLAB asynchronously and returns a future to retrieve the engine later.

    Returns
    -------
    future : matlab.engine.futureresult.FutureResult
        A future used to get the actual matlab engine.

    """
    future = None
    if os.environ.get("DELAY_MATLAB_START", "0") == "0":
        with suppress(ImportError):
            import matlab.engine

            future = matlab.engine.start_matlab(background=True)

    return future


class MatlabWrapper:
    """Creates a python callback for a MATLAB function.

    Parameters
    ----------
    filename : string
        The path of the file containing MATLAB function

    """

    loader = start_matlab()
    loader_error_message = "matlabengine is required to use MatlabWrapper"

    def __init__(self, filename: str) -> None:
        if self.loader is None:
            raise ImportError(self.loader_error_message) from None

        self.engine = self.loader.result()
        path = pathlib.Path(filename)
        self.engine.cd(str(path.parent), nargout=0)
        self.function_name = path.stem

    def getHandle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]:
        """Return a wrapper for the custom MATLAB function.

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], tuple[ArrayLike, float]]
            The wrapper function for the MATLAB callback

        """

        def handle(*args):
            if len(args) == 2:
                output = getattr(self.engine, self.function_name)(
                    np.array(args[0], "float"),  # xdata
                    np.array(args[1], "float"),  # params
                    nargout=1,
                )
                return np.array(output, "float").tolist()
            else:
                matlab_args = [
                    np.array(args[0], "float"),  # params
                    np.array(args[1], "float"),  # bulk in
                    np.array(args[2], "float"),  # bulk out
                    float(args[3] + 1),  # contrast
                ]
                if len(args) > 4:
                    matlab_args.append(float(args[4] + 1))  # domain number

                output, sub_rough = getattr(self.engine, self.function_name)(
                    *matlab_args,
                    nargout=2,
                )
                return np.array(output, "float").tolist(), float(sub_rough)

        return handle


def use_shared_matlab(name, custom_error_message):
    """Connect asynchronously to shared MATLAB engine instance with the given name.

    Parameters
    ----------
    name : str
        The name of shared MATLAB engine instance
    custom_error_message : str
        The custom error message in case of failed connection

    Returns
    -------
    future : matlab.engine.futureresult.FutureResult
        A future used to get the actual matlab engine.

    """
    with suppress(ImportError):
        import matlab.engine

        MatlabWrapper.loader = matlab.engine.connect_matlab(name, background=True)
        MatlabWrapper.loader_error_message = custom_error_message
        return MatlabWrapper.loader


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
        self.engine = ratapi.rat_core.DylibEngine(filename, function_name)

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
