"""Test the RAT examples."""

import importlib

import pytest

import RATapi.examples as examples


@pytest.mark.parametrize(
    "example_name",
    [
        "absorption",
        "domains_custom_layers",
        "domains_custom_XY",
        "domains_standard_layers",
        "DSPC_custom_layers",
        "DSPC_custom_XY",
        "DSPC_standard_layers",
        "DSPC_data_background",
    ],
)
def test_rat_examples(example_name):
    """Test that the RAT example projects run successfully."""
    p, r = getattr(examples, example_name)()
    assert p is not None
    assert r is not None


@pytest.mark.parametrize(
    "example_name",
    [
        "DSPC_function_background",
        # FIXME: https://github.com/RascalSoftware/python-RAT/issues/102
        # "convert_rascal",
    ],
)
@pytest.mark.skipif(importlib.util.find_spec("matlab") is None, reason="Matlab not installed")
def test_matlab_examples(example_name):
    """Test examples which rely on MATLAB engine being installed."""
    p, r = getattr(examples, example_name)()
    assert p is not None
    assert r is not None
