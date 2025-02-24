import pathlib
from unittest import mock

import pytest

import RATapi.wrappers


def test_matlab_wrapper() -> None:
    with (
        mock.patch.object(RATapi.wrappers.MatlabWrapper, "loader", None),
        pytest.raises(ImportError),
    ):
        RATapi.wrappers.MatlabWrapper("demo.m")

    mocked_matlab_future = mock.MagicMock()
    mocked_engine = mock.MagicMock()
    mocked_matlab_future.result.return_value = mocked_engine
    with mock.patch.object(RATapi.wrappers.MatlabWrapper, "loader", mocked_matlab_future):
        wrapper = RATapi.wrappers.MatlabWrapper("demo.m")
        assert wrapper.function_name == "demo"
        mocked_engine.cd.assert_called_once()
        assert pathlib.Path(mocked_engine.cd.call_args[0][0]).samefile(".")

        handle = wrapper.getHandle()

        mocked_engine.demo.return_value = ([2], 5)
        result = handle([1], [2], [3], 0)
        assert result == ([2], 5)
        assert wrapper.engine.demo.call_args[0] == ([1], [2], [3], 1)
        mocked_engine.demo.assert_called_once()

        mocked_engine.demo.return_value = ([3, 1], 7)
        result = handle([4], [5], [6], 1, 1)
        assert result == ([3, 1], 7)
        assert wrapper.engine.demo.call_args[0] == ([4], [5], [6], 2, 2)
        assert mocked_engine.demo.call_count == 2

        mocked_engine.demo.return_value = [4, 7]
        result = handle([3], [9])
        assert result == [4, 7]
        assert wrapper.engine.demo.call_args[0] == ([3], [9])
        assert mocked_engine.demo.call_count == 3


def test_dylib_wrapper() -> None:
    mocked_engine = mock.MagicMock()
    with mock.patch("RATapi.wrappers.RATapi.rat_core.DylibEngine", mocked_engine):
        wrapper = RATapi.wrappers.DylibWrapper("demo.dylib", "demo")
        mocked_engine.assert_called_once_with("demo.dylib", "demo")

        wrapper.engine.invoke.return_value = ([2], 5)
        handle = wrapper.getHandle()
        result = handle([1], [2], [3], 0)
        assert result == ([2], 5)
        assert wrapper.engine.invoke.call_args[0] == ([1], [2], [3], 0)
        wrapper.engine.invoke.assert_called_once()

        wrapper.engine.invoke.return_value = ([3, 1], 7)
        result = handle([4], [5], [6], 1, 1)
        assert result == ([3, 1], 7)
        assert wrapper.engine.invoke.call_args[0] == ([4], [5], [6], 1, 1)
        assert wrapper.engine.invoke.call_count == 2
