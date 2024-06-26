"""The classlist module. Contains the ClassList class, which defines a list containing instances of a particular
class.
"""

import collections
import contextlib
import warnings
from collections.abc import Iterable, Sequence
from typing import Any, Union

import prettytable


class ClassList(collections.UserList):
    """List of instances of a particular class.

    This class subclasses collections.UserList to construct a list intended to store ONLY instances of a particular
    class, given on initialisation. Any attempt to introduce an object of a different type will raise a ValueError.
    The class must be able to accept attribute values using keyword arguments. In addition, if the class has the
    attribute given in the ClassList's "name_field" attribute (the default is "name"), the ClassList will ensure that
    all objects within the ClassList have unique values for that attribute. It is then possible to use this attribute
    of an object in the .remove(), .count(), and .index() routines in place of the full object. Due to the requirement
    of unique values of the name_field attribute, the multiplication operators __mul__, __rmul__, and __imul__ have
    been disabled, since they cannot allow for unique attribute values by definition.

    We extend the UserList class to enable objects to be added and modified using just the keyword arguments, enable
    the object name_field attribute to be used in place of the full object, and ensure all elements are of the
    specified type, with unique name_field attributes defined.

    Parameters
    ----------
    init_list : Sequence [object] or object, optional
        An instance, or list of instance(s), of the class to be used in this ClassList.
    name_field : str, optional
        The field used to define unique objects in the ClassList (default is "name").

    """

    def __init__(self, init_list: Union[Sequence[object], object] = None, name_field: str = "name") -> None:
        self.name_field = name_field

        # Set input as list if necessary
        if init_list and not (isinstance(init_list, Sequence) and not isinstance(init_list, str)):
            init_list = [init_list]

        # Set class to be used for this instance of the ClassList, checking that all elements of the input list are of
        # the same type and have unique values of the specified name_field
        if init_list:
            self._class_handle = self._determine_class_handle(init_list)
            self._check_classes(init_list)
            self._check_unique_name_fields(init_list)

        super().__init__(init_list)

    def __repr__(self):
        try:
            [model.__dict__ for model in self.data]
        except AttributeError:
            output = repr(self.data)
        else:
            if any(model.__dict__ for model in self.data):
                table = prettytable.PrettyTable()
                table.field_names = ["index"] + [key.replace("_", " ") for key in self.data[0].__dict__]
                table.add_rows([[index] + list(model.__dict__.values()) for index, model in enumerate(self.data)])
                output = table.get_string()
            else:
                output = repr(self.data)
        return output

    def __setitem__(self, index: int, item: object) -> None:
        """Replace the object at an existing index of the ClassList."""
        self._setitem(index, item)

    def _setitem(self, index: int, item: object) -> None:
        """Auxiliary routine of "__setitem__" used to enable wrapping."""
        self._check_classes(self + [item])
        self._check_unique_name_fields(self + [item])
        self.data[index] = item

    def __delitem__(self, index: int) -> None:
        """Delete an object from the list by index."""
        self._delitem(index)

    def _delitem(self, index: int) -> None:
        """Auxiliary routine of "__delitem__" used to enable wrapping."""
        del self.data[index]

    def __iadd__(self, other: Sequence[object]) -> "ClassList":
        """Define in-place addition using the "+=" operator."""
        return self._iadd(other)

    def _iadd(self, other: Sequence[object]) -> "ClassList":
        """Auxiliary routine of "__iadd__" used to enable wrapping."""
        if other and not (isinstance(other, Sequence) and not isinstance(other, str)):
            other = [other]
        if not hasattr(self, "_class_handle"):
            self._class_handle = self._determine_class_handle(self + other)
        self._check_classes(self + other)
        self._check_unique_name_fields(self + other)
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

    def append(self, obj: object = None, **kwargs) -> None:
        """Append a new object to the ClassList using either the object itself, or keyword arguments to set attribute
        values.

        Parameters
        ----------
        obj : object, optional
            An instance of the class specified by self._class_handle.
        **kwargs : dict[str, Any], optional
            The input keyword arguments for a new object in the ClassList.

        Raises
        ------
        ValueError
            Raised if the input arguments contain a name_field value already defined in the ClassList.

        Warnings
        --------
        SyntaxWarning
            Raised if the input arguments contain BOTH an object and keyword arguments. In this situation the object is
            appended to the ClassList and the keyword arguments are discarded.

        """
        if obj and kwargs:
            warnings.warn(
                "ClassList.append() called with both an object and keyword arguments. "
                "The keyword arguments will be ignored.",
                SyntaxWarning,
                stacklevel=2,
            )
        if obj:
            if not hasattr(self, "_class_handle"):
                self._class_handle = type(obj)
            self._check_classes(self + [obj])
            self._check_unique_name_fields(self + [obj])
            self.data.append(obj)
        else:
            if not hasattr(self, "_class_handle"):
                raise TypeError(
                    "ClassList.append() called with keyword arguments for a ClassList without a class "
                    "defined. Call ClassList.append() with an object to define the class.",
                )
            self._validate_name_field(kwargs)
            self.data.append(self._class_handle(**kwargs))

    def insert(self, index: int, obj: object = None, **kwargs) -> None:
        """Insert a new object into the ClassList at a given index using either the object itself, or keyword arguments
        to set attribute values.

        Parameters
        ----------
        index: int
            The index at which to insert a new object in the ClassList.
        obj : object, optional
            An instance of the class specified by self._class_handle.
        **kwargs : dict[str, Any], optional
            The input keyword arguments for a new object in the ClassList.

        Raises
        ------
        ValueError
            Raised if the input arguments contain a name_field value already defined in the ClassList.

        Warnings
        --------
        SyntaxWarning
            Raised if the input arguments contain both an object and keyword arguments. In this situation the object is
            inserted into the ClassList and the keyword arguments are discarded.

        """
        if obj and kwargs:
            warnings.warn(
                "ClassList.insert() called with both an object and keyword arguments. "
                "The keyword arguments will be ignored.",
                SyntaxWarning,
                stacklevel=2,
            )
        if obj:
            if not hasattr(self, "_class_handle"):
                self._class_handle = type(obj)
            self._check_classes(self + [obj])
            self._check_unique_name_fields(self + [obj])
            self.data.insert(index, obj)
        else:
            if not hasattr(self, "_class_handle"):
                raise TypeError(
                    "ClassList.insert() called with keyword arguments for a ClassList without a class "
                    "defined. Call ClassList.insert() with an object to define the class.",
                )
            self._validate_name_field(kwargs)
            self.data.insert(index, self._class_handle(**kwargs))

    def remove(self, item: Union[object, str]) -> None:
        """Remove an object from the ClassList using either the object itself or its name_field value."""
        item = self._get_item_from_name_field(item)
        self.data.remove(item)

    def count(self, item: Union[object, str]) -> int:
        """Return the number of times an object appears in the ClassList using either the object itself or its
        name_field value.
        """
        item = self._get_item_from_name_field(item)
        return self.data.count(item)

    def index(self, item: Union[object, str], offset: bool = False, *args) -> int:
        """Return the index of a particular object in the ClassList using either the object itself or its
        name_field value. If offset is specified, add one to the index. This is used to account for one-based indexing.
        """
        item = self._get_item_from_name_field(item)
        return self.data.index(item, *args) + int(offset)

    def extend(self, other: Sequence[object]) -> None:
        """Extend the ClassList by adding another sequence."""
        if other and not (isinstance(other, Sequence) and not isinstance(other, str)):
            other = [other]
        if not hasattr(self, "_class_handle"):
            self._class_handle = self._determine_class_handle(self + other)
        self._check_classes(self + other)
        self._check_unique_name_fields(self + other)
        self.data.extend(other)

    def set_fields(self, index: int, **kwargs) -> None:
        """Assign the values of an existing object's attributes using keyword arguments."""
        self._validate_name_field(kwargs)
        class_handle = self.data[index].__class__
        new_fields = {**self.data[index].__dict__, **kwargs}
        self.data[index] = class_handle(**new_fields)

    def get_names(self) -> list[str]:
        """Return a list of the values of the name_field attribute of each class object in the list.

        Returns
        -------
        names : list [str]
            The value of the name_field attribute of each object in the ClassList.

        """
        return [getattr(model, self.name_field) for model in self.data if hasattr(model, self.name_field)]

    def get_all_matches(self, value: Any) -> list[tuple]:
        """Return a list of all (index, field) tuples where the value of the field is equal to the given value.

        Parameters
        ----------
        value : str
            The value we are searching for in the ClassList.

        Returns
        -------
         : list [tuple]
            A list of (index, field) tuples matching the given value.

        """
        return [
            (index, field)
            for index, element in enumerate(self.data)
            for field in vars(element)
            if getattr(element, field) == value
        ]

    def _validate_name_field(self, input_args: dict[str, Any]) -> None:
        """Raise a ValueError if the name_field attribute is passed as an object parameter, and its value is already
        used within the ClassList.

        Parameters
        ----------
        input_args : dict [str, Any]
            The input keyword arguments for a new object in the ClassList.

        Raises
        ------
        ValueError
            Raised if the input arguments contain a name_field value already defined in the ClassList.

        """
        names = self.get_names()
        with contextlib.suppress(KeyError):
            if input_args[self.name_field] in names:
                raise ValueError(
                    f"Input arguments contain the {self.name_field} '{input_args[self.name_field]}', "
                    f"which is already specified in the ClassList",
                )

    def _check_unique_name_fields(self, input_list: Iterable[object]) -> None:
        """Raise a ValueError if any value of the name_field attribute is used more than once in a list of class
        objects.

        Parameters
        ----------
        input_list : iterable
            An iterable of instances of the class given in self._class_handle.

        Raises
        ------
        ValueError
            Raised if the input list defines more than one object with the same value of name_field.

        """
        names = [getattr(model, self.name_field) for model in input_list if hasattr(model, self.name_field)]
        if len(set(names)) != len(names):
            raise ValueError(f"Input list contains objects with the same value of the {self.name_field} attribute")

    def _check_classes(self, input_list: Iterable[object]) -> None:
        """Raise a ValueError if any object in a list of objects is not of the type specified by self._class_handle.

        Parameters
        ----------
        input_list : iterable
            A list of instances of the class given in self._class_handle.

        Raises
        ------
        ValueError
            Raised if the input list defines objects of different types.

        """
        if not (all(isinstance(element, self._class_handle) for element in input_list)):
            raise ValueError(f"Input list contains elements of type other than '{self._class_handle.__name__}'")

    def _get_item_from_name_field(self, value: Union[object, str]) -> Union[object, str]:
        """Return the object with the given value of the name_field attribute in the ClassList.

        Parameters
        ----------
        value : object or str
            Either an object in the ClassList, or the value of the name_field attribute of an object in the ClassList.

        Returns
        -------
        instance : object or str
            Either the object with the value of the name_field attribute given by value, or the input value if an
            object with that value of the name_field attribute cannot be found.

        """
        return next((model for model in self.data if getattr(model, self.name_field) == value), value)

    @staticmethod
    def _determine_class_handle(input_list: Sequence[object]):
        """When inputting a sequence of object to a ClassList, the _class_handle should be set as the type of the
        element which satisfies "issubclass" for all of the other elements.

        Parameters
        ----------
        input_list : Sequence [object]
            A list of instances to populate the ClassList.

        Returns
        -------
        class_handle : type
            The type object of the element fulfilling the condition of satisfying "issubclass" for all of the other
            elements.

        """
        for this_element in input_list:
            if all([issubclass(type(instance), type(this_element)) for instance in input_list]):
                class_handle = type(this_element)
                break
        else:
            class_handle = type(input_list[0])

        return class_handle
