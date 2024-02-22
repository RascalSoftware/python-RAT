import unittest.mock as mock
import pytest
import rat.events


def test_event_register():
    first_callback = mock.Mock()
    second_callback = mock.Mock()

    with pytest.raises(ValueError):
        rat.events.register("Message", first_callback)

    rat.events.register(rat.events.EventTypes.Message, first_callback)
    result = rat.events.get_event_callback(rat.events.EventTypes.Message)
    assert result == [first_callback]

    rat.events.register(rat.events.EventTypes.Plot, second_callback)
    assert rat.events.get_event_callback(rat.events.EventTypes.Plot) == [second_callback]

    rat.events.register(rat.events.EventTypes.Message, second_callback)
    # the list is not guaranteed to be in the same order as inputted hence the set
    assert set(rat.events.get_event_callback(rat.events.EventTypes.Message)) == set([first_callback, second_callback])

    rat.events.clear()
    assert rat.events.get_event_callback(rat.events.EventTypes.Plot) == []
    assert rat.events.get_event_callback(rat.events.EventTypes.Message) == []

def test_event_notify():
    first_callback = mock.Mock()
    second_callback = mock.Mock()
    rat.events.register(rat.events.EventTypes.Message, first_callback)
    rat.events.register(rat.events.EventTypes.Plot, second_callback)
    
    rat.events.notify(rat.events.EventTypes.Message, "Hello World")
    first_callback.assert_called_once_with("Hello World")
    second_callback.assert_not_called()

    data = rat.events.PlotEventData()
    rat.events.notify(rat.events.EventTypes.Plot, data)
    first_callback.assert_called_once()
    second_callback.assert_called_once_with(data)
    
    rat.events.clear()
    rat.events.notify(rat.events.EventTypes.Message, "Hello World")
    rat.events.notify(rat.events.EventTypes.Plot, data)
    assert first_callback.call_count == 1
    assert second_callback.call_count == 1
    