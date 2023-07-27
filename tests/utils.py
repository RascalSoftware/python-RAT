from typing import Any, Dict


class DictAttributes:
    """Set the items in an input dictionary to be class attributes."""
    def __init__(self, input_dict: Dict) -> None:
        for key, value in input_dict.items():
            setattr(self, key, value)


class DotDict(dict):
    """A dict type that can access keys using dot notation."""
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__

    def __getattr__(self, key: str) -> Any:
        try:
            return self.__getitem__(key)
        except KeyError as exc:
            raise AttributeError(key) from exc
