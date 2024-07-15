from unittest import mock

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
    assert set(RATapi.events.get_event_callback(RATapi.events.EventTypes.Message)) == set(
        [first_callback, second_callback]
    )

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
