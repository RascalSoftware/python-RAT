"""Miscellaneous utility functions for RAT."""
import importlib.util
import os
import pathlib
from typing import Callable, Union


def get_python_handle(file_name: str, function_name: str, path: Union[str, pathlib.Path] = "") -> Callable:
    """Get the function handle from a function defined in a python module located anywhere within the filesystem.

    Parameters
    ----------
    file_name : str
        The name of the file containing the function of interest.
    function_name : str
        The name of the function we wish to obtain the handle for within the module.
    path : str
        The path to the file containing the function (default is "", which represent the working directory).

    Returns
    -------
    handle : Callable
        The handle of the function defined in the python module file.
    """
    spec = importlib.util.spec_from_file_location(pathlib.Path(file_name).stem, os.path.join(path, file_name))
    custom_module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(custom_module)
    handle = getattr(custom_module, function_name)
    return handle
