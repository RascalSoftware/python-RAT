"""Wrappers for the interface between RATapi and MATLAB custom files."""

import pathlib
from contextlib import suppress
from typing import Callable

import numpy as np
from numpy.typing import ArrayLike

import RATapi.rat_core


def start_matlab():
    """Start MATLAB asynchronously and returns a future to retrieve the engine later.

    Returns
    -------
    future : matlab.engine.futureresult.FutureResult
        A future used to get the actual matlab engine.

    """
    future = None
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

    def __init__(self, filename: str) -> None:
        if self.loader is None:
            raise ImportError("matlabengine is required to use MatlabWrapper") from None

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
