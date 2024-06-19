"""Test the miscellaneous utilities module."""
import pathlib

from RAT.utils.misc import get_python_handle
from tests.utils import dummy_function


def test_get_python_handle():
    path = pathlib.Path(__file__).parent.resolve()
    assert get_python_handle("utils.py", "dummy_function", path).__code__ == dummy_function.__code__
