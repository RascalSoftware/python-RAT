from unittest import mock

import pytest

import RATpy.events


def test_event_register() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()

    with pytest.raises(ValueError):
        RATpy.events.register("Message", first_callback)

    RATpy.events.register(RATpy.events.EventTypes.Message, first_callback)
    result = RATpy.events.get_event_callback(RATpy.events.EventTypes.Message)
    assert result == [first_callback]

    RATpy.events.register(RATpy.events.EventTypes.Plot, second_callback)
    assert RATpy.events.get_event_callback(RATpy.events.EventTypes.Plot) == [second_callback]

    RATpy.events.register(RATpy.events.EventTypes.Message, second_callback)
    # the list is not guaranteed to be in the same order as inputted hence the set
    assert set(RATpy.events.get_event_callback(RATpy.events.EventTypes.Message)) == set(
        [first_callback, second_callback]
    )

    RATpy.events.clear()
    assert RATpy.events.get_event_callback(RATpy.events.EventTypes.Plot) == []
    assert RATpy.events.get_event_callback(RATpy.events.EventTypes.Message) == []


def test_event_notify() -> None:
    first_callback = mock.Mock()
    second_callback = mock.Mock()
    third_callback = mock.Mock()
    RATpy.events.register(RATpy.events.EventTypes.Message, first_callback)
    RATpy.events.register(RATpy.events.EventTypes.Plot, second_callback)
    RATpy.events.register(RATpy.events.EventTypes.Progress, third_callback)

    RATpy.events.notify(RATpy.events.EventTypes.Message, "Hello World")
    first_callback.assert_called_once_with("Hello World")
    second_callback.assert_not_called()

    data = RATpy.events.PlotEventData()
    RATpy.events.notify(RATpy.events.EventTypes.Plot, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once_with(data)

    data = RATpy.events.ProgressEventData()
    RATpy.events.notify(RATpy.events.EventTypes.Progress, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once()
    third_callback.assert_called_once_with(data)

    RATpy.events.clear()
    RATpy.events.notify(RATpy.events.EventTypes.Message, "Hello World")
    RATpy.events.notify(RATpy.events.EventTypes.Plot, data)
    assert first_callback.call_count == 1
    assert second_callback.call_count == 1
    assert third_callback.call_count == 1
