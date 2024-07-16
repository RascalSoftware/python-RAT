import pathlib
from contextlib import suppress
from typing import Callable, Tuple

import numpy as np
from numpy.typing import ArrayLike

import RATapi.rat_core


def start_matlab():
    """Starts MATLAB asynchronously and returns a future to retrieve the engine later

    Returns
    -------
    future : matlab.engine.futureresult.FutureResult
        A future used to get the actual matlab engine

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

    def getHandle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], Tuple[ArrayLike, float]]:
        """Returns a wrapper for the custom MATLAB function

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], Tuple[ArrayLike, float]]
            The wrapper function for the MATLAB callback

        """

        def handle(params, bulk_in, bulk_out, contrast, domain=-1):
            if domain == -1:
                output, sub_rough = getattr(self.engine, self.function_name)(
                    np.array(params, "float"),
                    np.array(bulk_in, "float"),
                    np.array(bulk_out, "float"),
                    float(contrast + 1),
                    nargout=2,
                )
            else:
                output, sub_rough = getattr(self.engine, self.function_name)(
                    np.array(params, "float"),
                    np.array(bulk_in, "float"),
                    np.array(bulk_out, "float"),
                    float(contrast + 1),
                    float(domain + 1),
                    nargout=2,
                )
            return output, sub_rough

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

    def getHandle(self) -> Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], Tuple[ArrayLike, float]]:
        """Returns a wrapper for the custom dynamic library function

        Returns
        -------
        wrapper : Callable[[ArrayLike, ArrayLike, ArrayLike, int, int], Tuple[ArrayLike, float]]
            The wrapper function for the dynamic library callback

        """

        def handle(params, bulk_in, bulk_out, contrast, domain=-1):
            if domain == -1:
                output, sub_rough = self.engine.invoke(params, bulk_in, bulk_out, contrast)
            else:
                output, sub_rough = self.engine.invoke(params, bulk_in, bulk_out, contrast, domain)
            return output, sub_rough

        return handle
