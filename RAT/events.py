from typing import Callable, Union, List
import RAT.rat_core
from RAT.rat_core import EventTypes, PlotEventData, ProgressEventData


def notify(event_type: EventTypes, data: Union[str, PlotEventData, ProgressEventData]) -> None:
    """Calls registered callbacks with the data when event type has 
    been triggered.

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

def get_event_callback(event_type: EventTypes) -> List[Callable[[Union[str, PlotEventData, ProgressEventData]], None]]:
    """Returns all callbacks registered for the given event type.

    Parameters
    ----------
    event_type : EventTypes
        The event type.

    Retuns
    ------
    callback : Callable[[Union[str, PlotEventData, ProgressEventData]], None]
        The callback for the event type.
    """
    return list(__event_callbacks[event_type])


def register(event_type: EventTypes, callback: Callable[[Union[str, PlotEventData, ProgressEventData]], None]) -> None:
    """Registers a new callback for the event type.

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


def clear() -> None:
    """Clears all event callbacks."""
    __event_impl.clear()
    for key in __event_callbacks.keys():
        __event_callbacks[key] = set()


__event_impl = RAT.rat_core.EventBridge(notify)
__event_callbacks = {EventTypes.Message: set(), EventTypes.Plot: set(), EventTypes.Progress: set()}
