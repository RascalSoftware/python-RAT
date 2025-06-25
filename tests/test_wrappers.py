from unittest import mock

import RATapi.wrappers

# def test_matlab_wrapper() -> None:
#     with (
#         mock.patch.object(RATapi.wrappers.MatlabWrapper, "engine", None),
#         pytest.raises(ValueError),
#     ):
#         RATapi.wrappers.MatlabWrapper("demo.m")

#     mocked_engine = mock.MagicMock()
#     with mock.patch.object(RATapi.wrappers.MatlabWrapper, "engine", mocked_engine):
#         wrapper = RATapi.wrappers.MatlabWrapper("demo.m")
#         mocked_engine.cd.assert_called_once()
#         assert pathlib.Path(mocked_engine.cd.call_args[0][0]).samefile(".")

#         wrapper.engine.invoke.return_value = ([2], 5)
#         handle = wrapper.get_handle()
#         result = handle([1], [2], [3], 0)
#         assert result == ([2], 5)
#         assert wrapper.engine.invoke.call_args[0] == ([1], [2], [3], 0)
#         wrapper.engine.invoke.assert_called_once()

#         wrapper.engine.invoke.return_value = ([3, 1], 7)
#         result = handle([4], [5], [6], 1, 1)
#         assert result == ([3, 1], 7)
#         assert wrapper.engine.invoke.call_args[0] == ([4], [5], [6], 1, 1)
#         assert wrapper.engine.invoke.call_count == 2


def test_dylib_wrapper() -> None:
    mocked_engine = mock.MagicMock()
    with mock.patch("RATapi.wrappers.RATapi.rat_core.DylibEngine", mocked_engine):
        wrapper = RATapi.wrappers.DylibWrapper("demo.dylib", "demo")
        mocked_engine.assert_called_once_with("demo.dylib", "demo")

        wrapper.engine.invoke.return_value = ([2], 5)
        handle = wrapper.get_handle()
        result = handle([1], [2], [3], 0)
        assert result == ([2], 5)
        assert wrapper.engine.invoke.call_args[0] == ([1], [2], [3], 0)
        wrapper.engine.invoke.assert_called_once()

        wrapper.engine.invoke.return_value = ([3, 1], 7)
        result = handle([4], [5], [6], 1, 1)
        assert result == ([3, 1], 7)
        assert wrapper.engine.invoke.call_args[0] == ([4], [5], [6], 1, 1)
        assert wrapper.engine.invoke.call_count == 2
