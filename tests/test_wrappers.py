import pathlib
import unittest.mock as mock
import pytest
import RAT.misc


def test_matlab_wrapper() -> None:
    with pytest.raises(ImportError):
        RAT.misc.MatlabWrapper('demo.m')
    mocked_matlab_module = mock.MagicMock()
    mocked_engine = mock.MagicMock()
    mocked_matlab_module.engine.start_matlab.return_value = mocked_engine
        
    # mocked_matlab_module.engine = mock.MagicMock()

    with mock.patch.dict('sys.modules', {'matlab': mocked_matlab_module,
                                         'matlab.engine': mocked_matlab_module.engine}):
        wrapper = RAT.misc.MatlabWrapper('demo.m')
        assert wrapper.function_name == 'demo'
        mocked_engine.cd.assert_called_once()
        assert pathlib.Path(mocked_engine.cd.call_args[0][0]).samefile('.')

        handle = wrapper.getHandle()
        
        mocked_engine.demo.return_value = ([2], 5)
        result = handle([1], [2], [3], 0)
        assert  result == ([2], 5)
        assert wrapper.engine.demo.call_args[0] == ([1], [2], [3], 1)
        mocked_engine.demo.assert_called_once()

        mocked_engine.demo.return_value = ([3, 1], 7)
        result = handle([4], [5], [6], 1, 1)
        assert result == ([3, 1], 7)
        assert wrapper.engine.demo.call_args[0] == ([4], [5], [6], 2, 2)
        assert mocked_engine.demo.call_count == 2
    

def test_dylib_wrapper() -> None:
    mocked_engine = mock.MagicMock()
    with mock.patch('RAT.misc.RAT.rat_core.DylibEngine', mocked_engine):
        wrapper = RAT.misc.DylibWrapper('demo.dylib', 'demo')
        mocked_engine.assert_called_once_with('demo.dylib', 'demo')

        wrapper.engine.invoke.return_value = ([2], 5)
        handle = wrapper.getHandle()
        result = handle([1], [2], [3], 0)
        assert  result == ([2], 5)
        assert wrapper.engine.invoke.call_args[0] == ([1], [2], [3], 0)
        wrapper.engine.invoke.assert_called_once()

        wrapper.engine.invoke.return_value = ([3, 1], 7)
        result = handle([4], [5], [6], 1, 1)
        assert result == ([3, 1], 7)
        assert wrapper.engine.invoke.call_args[0] == ([4], [5], [6], 1, 1)
        assert wrapper.engine.invoke.call_count == 2
