import unittest.mock as mock
import pytest
import RAT.events


def test_event_register() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()

    with pytest.raises(ValueError):
        RAT.events.register("Message", first_callback)

    RAT.events.register(RAT.events.EventTypes.Message, first_callback)
    result = RAT.events.get_event_callback(RAT.events.EventTypes.Message)
    assert result == [first_callback]

    RAT.events.register(RAT.events.EventTypes.Plot, second_callback)
    assert RAT.events.get_event_callback(RAT.events.EventTypes.Plot) == [second_callback]

    RAT.events.register(RAT.events.EventTypes.Message, second_callback)
    # the list is not guaranteed to be in the same order as inputted hence the set
    assert set(RAT.events.get_event_callback(RAT.events.EventTypes.Message)) == set([first_callback, second_callback])

    RAT.events.clear()
    assert RAT.events.get_event_callback(RAT.events.EventTypes.Plot) == []
    assert RAT.events.get_event_callback(RAT.events.EventTypes.Message) == []

def test_event_notify() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()
    RAT.events.register(RAT.events.EventTypes.Message, first_callback)
    RAT.events.register(RAT.events.EventTypes.Plot, second_callback)
    
    RAT.events.notify(RAT.events.EventTypes.Message, "Hello World")
    first_callback.assert_called_once_with("Hello World")
    second_callback.assert_not_called()

    data = RAT.events.PlotEventData()
    RAT.events.notify(RAT.events.EventTypes.Plot, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once_with(data)
    
    RAT.events.clear()
    RAT.events.notify(RAT.events.EventTypes.Message, "Hello World")
    RAT.events.notify(RAT.events.EventTypes.Plot, data)
    assert first_callback.call_count == 1
    assert second_callback.call_count == 1
