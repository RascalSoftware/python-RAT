"""Hooks for connecting to run callback events."""

import os
from collections.abc import Callable

from ratapi.rat_core import EventBridge, EventTypes, PlotEventData, ProgressEventData


def notify(event_type: EventTypes, data: str | PlotEventData | ProgressEventData) -> None:
    """Call registered callbacks with data when event type has been triggered.

    Parameters
    ----------
    event_type : EventTypes
        The event type that was triggered.
    data : str or PlotEventData or ProgressEventData
        The data sent by the event. The message event data is a string.

    """
    callbacks = __event_callbacks[event_type]
    for callback in callbacks:
        callback(data)


def get_event_callback(event_type: EventTypes) -> list[Callable[[str | PlotEventData | ProgressEventData], None]]:
    """Return all callbacks registered for the given event type.

    Parameters
    ----------
    event_type : EventTypes
        The event type.

    Returns
    -------
    callback : Callable[[Union[str, PlotEventData, ProgressEventData]], None]
        The callback for the event type.

    """
    return list(__event_callbacks[event_type])


def register(event_type: EventTypes, callback: Callable[[str | PlotEventData | ProgressEventData], None]) -> None:
    """Register a new callback for the event type.

    Parameters
    ----------
    event_type : EventTypes
        The event type to register.
    callback : Callable[[Union[str, PlotEventData, ProgressEventData]], None]
        The callback for when the event is triggered.

    """
    if not isinstance(event_type, EventTypes):
        raise ValueError("event_type must be a events.EventTypes enum")

    if len(__event_callbacks[event_type]) == 0:
        __event_impl.register(event_type)
    __event_callbacks[event_type].add(callback)


def clear(key=None, callback=None) -> None:
    """Clear all event callbacks or specific callback.

    Parameters
    ----------
    key : EventTypes, optional
        The event type of the callback to clear if given.
    callback : Callable[[Union[str, PlotEventData, ProgressEventData]], None], optional
        A callback for an event which will be cleared if given.

    """
    if key is None and callback is None:
        for key in __event_callbacks:
            __event_callbacks[key] = set()
    elif key is not None and callback is not None:
        __event_callbacks[key].remove(callback)

    for value in __event_callbacks.values():
        if value:
            break
    else:
        __event_impl.clear()


dir_path = os.path.dirname(os.path.realpath(__file__))
os.environ["RAT_PATH"] = os.path.join(dir_path, "")
__event_impl = EventBridge(notify)
__event_callbacks = {EventTypes.Message: set(), EventTypes.Plot: set(), EventTypes.Progress: set()}
