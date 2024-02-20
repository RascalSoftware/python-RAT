from rat import rat_core
from rat.rat_core import EventTypes


def nofiy(event_type, data):
    callbacks = __event_callbacks[event_type]
    for callback in callbacks:
        callback(data)


def register(event_type, callback):
    if not isinstance(event_type, EventTypes):
        raise ValueError("event_type must be a events.EventTypes enum")
    
    if len(__event_callbacks[event_type]) == 0:
        __event_impl.register(event_type)
    __event_callbacks[event_type].add(callback)


def clear():
    __event_impl.clear()
    __event_callbacks.clear()


__event_impl = rat_core.EventBridge(nofiy)
__event_callbacks = {EventTypes.Message: set(), EventTypes.Plot: set()}
