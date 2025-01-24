import pickle
from unittest import mock

import numpy as np
import pytest

import RATapi.events


def test_event_register() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()

    with pytest.raises(ValueError):
        RATapi.events.register("Message", first_callback)

    RATapi.events.register(RATapi.events.EventTypes.Message, first_callback)
    result = RATapi.events.get_event_callback(RATapi.events.EventTypes.Message)
    assert result == [first_callback]

    RATapi.events.register(RATapi.events.EventTypes.Plot, second_callback)
    assert RATapi.events.get_event_callback(RATapi.events.EventTypes.Plot) == [second_callback]

    RATapi.events.register(RATapi.events.EventTypes.Message, second_callback)
    # the list is not guaranteed to be in the same order as inputted hence the set
    assert set(RATapi.events.get_event_callback(RATapi.events.EventTypes.Message)) == {first_callback, second_callback}

    RATapi.events.clear(RATapi.events.EventTypes.Message, second_callback)
    result = RATapi.events.get_event_callback(RATapi.events.EventTypes.Message)
    assert result == [first_callback]
    result = RATapi.events.get_event_callback(RATapi.events.EventTypes.Plot)
    assert result == [second_callback]
    RATapi.events.clear()
    assert RATapi.events.get_event_callback(RATapi.events.EventTypes.Plot) == []
    assert RATapi.events.get_event_callback(RATapi.events.EventTypes.Message) == []


def test_event_notify() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()
    third_callback = mock.Mock()
    RATapi.events.register(RATapi.events.EventTypes.Message, first_callback)
    RATapi.events.register(RATapi.events.EventTypes.Plot, second_callback)
    RATapi.events.register(RATapi.events.EventTypes.Progress, third_callback)

    RATapi.events.notify(RATapi.events.EventTypes.Message, "Hello World")
    first_callback.assert_called_once_with("Hello World")
    second_callback.assert_not_called()

    data = RATapi.events.PlotEventData()
    RATapi.events.notify(RATapi.events.EventTypes.Plot, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once_with(data)

    data = RATapi.events.ProgressEventData()
    RATapi.events.notify(RATapi.events.EventTypes.Progress, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once()
    third_callback.assert_called_once_with(data)

    RATapi.events.clear()
    RATapi.events.notify(RATapi.events.EventTypes.Message, "Hello World")
    RATapi.events.notify(RATapi.events.EventTypes.Plot, data)
    assert first_callback.call_count == 1
    assert second_callback.call_count == 1
    assert third_callback.call_count == 1


def test_event_data_pickle():
    data = RATapi.events.ProgressEventData()
    data.message = "Hello"
    data.percent = 0.5
    pickled_data = pickle.loads(pickle.dumps(data))
    assert pickled_data.message == data.message
    assert pickled_data.percent == data.percent

    data = RATapi.events.PlotEventData()
    data.modelType = "custom layers"
    data.dataPresent = np.ones(2)
    data.subRoughs = np.ones((20, 2))
    data.resample = np.ones(2)
    data.resampledLayers = [np.ones((20, 2)), np.ones((20, 2))]
    data.reflectivity = [np.ones((20, 2)), np.ones((20, 2))]
    data.shiftedData = [np.ones((20, 2)), np.ones((20, 2))]
    data.sldProfiles = [np.ones((20, 2)), np.ones((20, 2))]
    data.contrastNames = ["D2O", "SMW"]

    pickled_data = pickle.loads(pickle.dumps(data))

    assert pickled_data.modelType == data.modelType
    assert (pickled_data.dataPresent == data.dataPresent).all()
    assert (pickled_data.subRoughs == data.subRoughs).all()
    assert (pickled_data.resample == data.resample).all()
    for i in range(2):
        assert (pickled_data.resampledLayers[i] == data.resampledLayers[i]).all()
        assert (pickled_data.reflectivity[i] == data.reflectivity[i]).all()
        assert (pickled_data.shiftedData[i] == data.shiftedData[i]).all()
        assert (pickled_data.sldProfiles[i] == data.sldProfiles[i]).all()
    assert pickled_data.contrastNames == data.contrastNames
