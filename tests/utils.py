import collections
import math
from typing import Any


class InputAttributes:
    """Set input arguments as class attributes."""
    def __init__(self, **kwargs) -> None:
        for key, value in kwargs.items():
            setattr(self, key, value)

    def __eq__(self, other: Any):
        if isinstance(other, InputAttributes):
            return self.__dict__ == other.__dict__
        return False


class SubInputAttributes(InputAttributes):
    """Trivial subclass of InputAttributes"""
    pass


class NaNList(collections.UserList):
    """Defines a list where NaNs are defined as equal."""

    def __eq__(self, other: Any) -> bool:
        return(self.data == other or
               ['NaN' if math.isnan(el) else el for el in self] == ['NaN' if math.isnan(el) else el for el in other]
               )
