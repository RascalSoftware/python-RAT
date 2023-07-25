"""The classlist module. Contains the ClassList class, which defines a list containing instances of a particular class.
"""

import collections
import collections.abc
import contextlib
from typing import Any, Dict, Iterable, Sequence, Union


class ClassList(collections.UserList):
    """List of instances of a particular class.

    This class subclasses collections.UserList to construct a list intended to store ONLY instances of a particular
    class, given on initialisation. Any attempt to introduce an object of a different type will raise a ValueError.
    The class must be able to accept attribute values using keyword arguments. In addition, if the class has the
    attribute "name", the ClassList will ensure that all objects within the ClassList have different names. It is then
    possible to use the name of an object in the .remove(), .count(), and .index() routines in place of the full
    object. Due to the requirement of unique names, the multiplication operators __mul__, __rmul__, and __imul__ have
    been disabled, since they cannot allow for unique names by definition.

    We extend the UserList class to enable objects to be added and modified using just the keyword arguments, enable
    the object name to be used in place of the full object, and ensure all elements are of the specified type, with
    unique names defined.

    Parameters
    ----------
    init_list : Sequence [object] or object
        An instance, or list of instance(s), of the class to be used in this ClassList.
    empty_list : bool, optional
        If true, do not initialise the list with the contents of init_list, using an empty list instead
        (default is False).

    Raises
    ------
    ValueError
        Raised if the input list is empty.
    """

    def __init__(self, init_list: Union[Sequence[object], object], empty_list: bool = False) -> None:
        # Set input as list if necessary, raising a ValueError for empty input
        if not (isinstance(init_list, collections.abc.Sequence) and not isinstance(init_list, str)):
            init_list = [init_list]
        if not init_list:
            raise ValueError("Input value must not be empty")

        # Set class to be used for this instance of the ClassList
        # and check all elements are of the same type and have unique names
        self.class_handle = type(init_list[0])
        self._check_classes(init_list)
        self._check_unique_names(init_list)

        super().__init__(init_list)
        if empty_list:
            self.data = []

    def __setitem__(self, index: int, set_dict: Dict[str, Any]) -> None:
        """Assign the values of an existing object's attributes using a dictionary."""
        self._validate_name(set_dict)
        for key, value in set_dict.items():
            setattr(self.data[index], key, value)

    def __iadd__(self, other: Iterable[object]) -> 'ClassList':
        """Define in-place addition using the "+=" operator."""
        self._check_classes(self + other)
        self._check_unique_names(self + other)
        super().__iadd__(other)
        return self

    def __mul__(self, n: int) -> None:
        """Define multiplication using the "*" operator."""
        raise TypeError(f"unsupported operand type(s) for *: '{self.__class__.__name__}' and '{n.__class__.__name__}'")

    def __rmul__(self, n: int) -> None:
        """Define multiplication using the "*" operator."""
        raise TypeError(f"unsupported operand type(s) for *: '{n.__class__.__name__}' and '{self.__class__.__name__}'")

    def __imul__(self, n: int) -> None:
        """Define in-place multiplication using the "*=" operator."""
        raise TypeError(f"unsupported operand type(s) for *=: '{self.__class__.__name__}' and '{n.__class__.__name__}'")

    def append(self, **kwargs) -> None:
        """Append a new object to the ClassList using keyword arguments to set attribute values."""
        self._validate_name(kwargs)
        self.data.append(self.class_handle(**kwargs))

    def insert(self, index: int, **kwargs) -> None:
        """Insert a new object into the ClassList at a given index using keyword arguments to set attribute values."""
        self._validate_name(kwargs)
        self.data.insert(index, self.class_handle(**kwargs))

    def remove(self, item: Union[object, str]) -> None:
        """Remove an object from the ClassList using either the object itself or its name."""
        if not isinstance(item, self.class_handle):
            item = self._get_item_from_name(item)
        self.data.remove(item)

    def count(self, item: Union[object, str]) -> int:
        """Return the number of times an object appears in the ClassList using either the object itself or its name."""
        if not isinstance(item, self.class_handle):
            item = self._get_item_from_name(item)
        return self.data.count(item)

    def index(self, item: Union[object, str], *args) -> int:
        """Return the index of a particular object in the ClassList using either the object itself or its name."""
        if not isinstance(item, self.class_handle):
            item = self._get_item_from_name(item)
        return self.data.index(item, *args)

    def extend(self, other: Iterable[object]) -> None:
        """Extend the ClassList by adding another iterable."""
        self._check_classes(self + other)
        self._check_unique_names(self + other)
        self.data.extend(other)

    def get_names(self) -> list[str]:
        """Return a list of the names of each class object in the list.

        Returns
        -------
        names : list [str]
            The value of the name attribute of each object in the ClassList.
        """
        return [model.name for model in self.data if hasattr(model, 'name')]

    def _validate_name(self, input_args: Dict[str, Any]) -> None:
        """Raise a ValueError if a name is passed as an object parameter, and it is already used within the ClassList.

        Parameters
        ----------
        input_args : dict [str, Any]
            The input keyword arguments for a new object in the ClassList.

        Raises
        ------
        ValueError
            Raised if the input arguments contain a name already defined in the ClassList.
        """
        names = self.get_names()
        with contextlib.suppress(KeyError):
            if input_args['name'] in names:
                raise ValueError(f"Input arguments contain the name '{input_args['name']}',"
                                 f" which is already specified in the ClassList")

    @staticmethod
    def _check_unique_names(input_list: Iterable) -> None:
        """Raise a ValueError if any value of the "name" attribute is used more than once in a list of class objects.

        Parameters
        ----------
        input_list : iterable
            An iterable of instances of the class given in self.class_handle.

        Raises
        ------
        ValueError
            Raised if the input list defines more than one object with the same name.
        """
        names = [model.name for model in input_list if hasattr(model, 'name')]
        if len(set(names)) != len(names):
            raise ValueError("Input list contains objects with the same name")

    def _check_classes(self, input_list: Iterable[object]) -> None:
        """Raise a ValueError if any object in a list of objects is not of the type specified by self.class_handle.

        Parameters
        ----------
        input_list : iterable
            A list of instances of the class given in self.class_handle.

        Raises
        ------
        ValueError
            Raised if the input list defines objects of different types.
        """
        if not (all(isinstance(element, self.class_handle) for element in input_list)):
            raise ValueError(f"Input list contains elements of type other than '{self.class_handle}'")

    def _get_item_from_name(self, name: str) -> Union[object, str]:
        """Return the object with the given value of the attribute "name" in the ClassList.

        Parameters
        ----------
        name : str
            The name of an object in the ClassList.

        Returns
        -------
        instance : object or str
            Either the object with the value of the "name" attribute given by name, or the input name if an object with
            that value of the "name" attribute cannot be found.
        """
        return next((model for model in self.data if model.name == name), name)
