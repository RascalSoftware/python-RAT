import collections
import collections.abc
import contextlib
from typing import Any, Dict, Iterable, Sequence, Union


class ClassList(collections.UserList):
    """Creates list of class objects from the class's input fields"""
    def __init__(self, initlist: Union[Sequence[object], object], emptyList: bool = False) -> None:
        """Initialise and set the class for this list"""

        # Set input as list if necessary, raising an error for empty input
        if not (isinstance(initlist, collections.abc.Sequence) and not isinstance(initlist, str)):
            initlist = [initlist]
        if not initlist:
            raise ValueError("Input value must not be empty")

        # Set model to be used for this instance of the ClassList
        # and check all elements are of the same type and have unique names
        self.classHandle = type(initlist[0])
        self._checkClasses(initlist)
        self._checkUniqueNames(initlist)
        super().__init__(initlist)
        if emptyList:
            self.data = []

    def __setitem__(self, index: int, setDict: Dict[str, Any]) -> None:
        """Assign the values of an existing class's fields using a dictionary"""
        self._validateName(setDict)
        for key, value in setDict.items():
            setattr(self.data[index], key, value)

    def __iadd__(self, other: Iterable[object]) -> 'ClassList':
        self._checkClasses(self + other)
        self._checkUniqueNames(self + other)
        super().__iadd__(other)
        return self

    def __mul__(self, n: int) -> None:
        raise TypeError(f"unsupported operand type(s) for *: '{self.__class__.__name__}' and '{n.__class__.__name__}'")

    def __rmul__(self, n: int) -> None:
        raise TypeError(f"unsupported operand type(s) for *: '{self.__class__.__name__}' and '{n.__class__.__name__}'")

    def __imul__(self, n: int) -> None:
        raise TypeError(f"unsupported operand type(s) for *=: '{self.__class__.__name__}' and '{n.__class__.__name__}'")

    def append(self, **kwargs) -> None:
        """Append a new class object to the list using key-value pairs to set field values"""
        self._validateName(kwargs)
        self.data.append(self.classHandle(**kwargs))

    def insert(self, index: int, **kwargs) -> None:
        """Insert a new class object into the list using key-value pairs to set field values"""
        self._validateName(kwargs)
        self.data.insert(index, self.classHandle(**kwargs))

    def remove(self, item: Union[object, str]) -> None:
        """Remove the given class object from the list"""
        if type(item) is not self.classHandle:
            item = self._getItemFromName(item)
        self.data.remove(item)

    def count(self, item: Union[object, str]) -> int:
        """Return the number of times an item appears in the list."""
        if type(item) is not self.classHandle:
            item = self._getItemFromName(item)
        return self.data.count(item)

    def index(self, item: Union[object, str], *args) -> int:
        """Get the index of a particular item in the list"""
        if type(item) is not self.classHandle:
            item = self._getItemFromName(item)
        return self.data.index(item, *args)

    def extend(self, other: Iterable[object]) -> None:
        """Extend the ClassList by adding another iterable"""
        self._checkClasses(self + other)
        self._checkUniqueNames(self + other)
        self.data.extend(other)

    def getNames(self) -> list[str]:
        """Get a list of the names of each class object in the list"""
        return [model.name for model in self.data if hasattr(model, 'name')]

    def _validateName(self, inputArgs: Dict[str, Any]) -> None:
        """If a name is passed as a object parameter, ensure it is unique"""
        names = self.getNames()
        with contextlib.suppress(KeyError):
            if inputArgs['name'] in names:
                raise ValueError(f"Input arguments contain the name '{inputArgs['name']}',\
                                which is already specified in the ClassList")

    def _checkUniqueNames(self, inputList: Iterable[object]) -> None:
        """Check all names in a list are unique"""
        names = [model.name for model in inputList if hasattr(model, 'name')]
        if len(set(names)) != len(names):
            raise ValueError(f"Input list contains objects with the same name")

    def _checkClasses(self, inputList: Iterable[object]) -> None:
        """Check all objects in a list are of the type specified by the class handle"""
        if not (all(type(element) == self.classHandle for element in inputList)):
            raise ValueError(f"Input list contains elements of type other than '{self.classHandle}'")

    def _getItemFromName(self, name: str) -> object:
        """Find the object with the given value of "name" in the list"""
        return next((model for model in self.data if model.name == name), name)
