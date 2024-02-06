import pathlib
import cppimport
import numpy as np
import matlab.engine

rat = cppimport.imp("rat")


def nofiy(event_type, data):
    callbacks = __event_callbacks[event_type]
    for callback in callbacks:
        callback(data)


def register(event_type, callback):
    if not isinstance(event_type, rat.EventTypes):
        raise ValueError("event_type must be a rat.EventTypes enum")
    
    if len(__event_callbacks[event_type]) == 0:
        __event_impl.register(event_type)
    __event_callbacks[event_type].add(callback)


def clear():
    __event_impl.clear()
    __event_callbacks.clear()


__event_impl = rat.EventStuff(nofiy)
__event_callbacks = {rat.EventTypes.Message: set(), rat.EventTypes.Plot: set()}
