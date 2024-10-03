"""The classlist module. Contains the ClassList class, which defines a list containing instances of a particular
class.
"""

import collections
import contextlib
import warnings
from collections.abc import Sequence
from typing import Any, Generic, TypeVar, Union

import numpy as np
import prettytable

T = TypeVar("T")


class ClassList(collections.UserList, Generic[T]):
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
    init_list : Sequence [T] or T, optional
        An instance, or list of instance(s), of the class to be used in this ClassList.
    name_field : str, optional
        The field used to define unique objects in the ClassList (default is "name").

    """

    def __init__(self, init_list: Union[Sequence[T], T] = None, name_field: str = "name") -> None:
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

    def __str__(self):
        try:
            [model.__dict__ for model in self.data]
        except AttributeError:
            output = str(self.data)
        else:
            if any(model.__dict__ for model in self.data):
                table = prettytable.PrettyTable()
                table.field_names = ["index"] + [key.replace("_", " ") for key in self.data[0].__dict__]
                table.add_rows(
                    [
                        [index]
                        + list(
                            f"{'Data array: ['+' x '.join(str(i) for i in v.shape) if v.size > 0 else '['}]"
                            if isinstance(v, np.ndarray)
                            else "\n".join(element for element in v)
                            if k == "model"
                            else str(v)
                            for k, v in model.__dict__.items()
                        )
                        for index, model in enumerate(self.data)
                    ]
                )
                output = table.get_string()
            else:
                output = str(self.data)
        return output

    def __getitem__(self, index: Union[int, slice, str, T]) -> T:
        """Get an item by its index, name, a slice, or the object itself."""
        if isinstance(index, (int, slice)):
            return self.data[index]
        elif isinstance(index, (str, self._class_handle)):
            return self.data[self.index(index)]
        else:
            raise IndexError("ClassLists can only be indexed by integers, slices, name strings, or objects.")

    def __setitem__(self, index: int, item: T) -> None:
        """Replace the object at an existing index of the ClassList."""
        self._setitem(index, item)

    def _setitem(self, index: int, item: T) -> None:
        """Auxiliary routine of "__setitem__" used to enable wrapping."""
        self._check_classes([item])
        self._check_unique_name_fields([item])
        self.data[index] = item

    def __delitem__(self, index: int) -> None:
        """Delete an object from the list by index."""
        self._delitem(index)

    def _delitem(self, index: int) -> None:
        """Auxiliary routine of "__delitem__" used to enable wrapping."""
        del self.data[index]

    def __iadd__(self, other: Sequence[T]) -> "ClassList":
        """Define in-place addition using the "+=" operator."""
        return self._iadd(other)

    def _iadd(self, other: Sequence[T]) -> "ClassList":
        """Auxiliary routine of "__iadd__" used to enable wrapping."""
        if other and not (isinstance(other, Sequence) and not isinstance(other, str)):
            other = [other]
        if not hasattr(self, "_class_handle"):
            self._class_handle = self._determine_class_handle(self + other)
        self._check_classes(other)
        self._check_unique_name_fields(other)
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

    def append(self, obj: T = None, **kwargs) -> None:
        """Append a new object to the ClassList using either the object itself, or keyword arguments to set attribute
        values.

        Parameters
        ----------
        obj : T, optional
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
            self._check_classes([obj])
            self._check_unique_name_fields([obj])
            self.data.append(obj)
        else:
            if not hasattr(self, "_class_handle"):
                raise TypeError(
                    "ClassList.append() called with keyword arguments for a ClassList without a class "
                    "defined. Call ClassList.append() with an object to define the class.",
                )
            self._validate_name_field(kwargs)
            self.data.append(self._class_handle(**kwargs))

    def insert(self, index: int, obj: T = None, **kwargs) -> None:
        """Insert a new object into the ClassList at a given index using either the object itself, or keyword arguments
        to set attribute values.

        Parameters
        ----------
        index: int
            The index at which to insert a new object in the ClassList.
        obj : T, optional
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
            self._check_classes([obj])
            self._check_unique_name_fields([obj])
            self.data.insert(index, obj)
        else:
            if not hasattr(self, "_class_handle"):
                raise TypeError(
                    "ClassList.insert() called with keyword arguments for a ClassList without a class "
                    "defined. Call ClassList.insert() with an object to define the class.",
                )
            self._validate_name_field(kwargs)
            self.data.insert(index, self._class_handle(**kwargs))

    def remove(self, item: Union[T, str]) -> None:
        """Remove an object from the ClassList using either the object itself or its name_field value."""
        item = self._get_item_from_name_field(item)
        self.data.remove(item)

    def count(self, item: Union[T, str]) -> int:
        """Return the number of times an object appears in the ClassList using either the object itself or its
        name_field value.
        """
        item = self._get_item_from_name_field(item)
        return self.data.count(item)

    def index(self, item: Union[T, str], offset: bool = False, *args) -> int:
        """Return the index of a particular object in the ClassList using either the object itself or its
        name_field value. If offset is specified, add one to the index. This is used to account for one-based indexing.
        """
        item = self._get_item_from_name_field(item)
        return self.data.index(item, *args) + int(offset)

    def extend(self, other: Sequence[T]) -> None:
        """Extend the ClassList by adding another sequence."""
        if other and not (isinstance(other, Sequence) and not isinstance(other, str)):
            other = [other]
        if not hasattr(self, "_class_handle"):
            self._class_handle = self._determine_class_handle(self + other)
        self._check_classes(other)
        self._check_unique_name_fields(other)
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
        names = [name.lower() for name in self.get_names()]
        with contextlib.suppress(KeyError):
            name = input_args[self.name_field].lower()
            if name in names:
                raise ValueError(
                    f"Input arguments contain the {self.name_field} '{input_args[self.name_field]}', "
                    f"which is already specified at index {names.index(name)} of the ClassList",
                )

    def _check_unique_name_fields(self, input_list: Sequence[T]) -> None:
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
        error_list = []
        try:
            existing_names = [name.lower() for name in self.get_names()]
        except AttributeError:
            existing_names = []

        new_names = [getattr(model, self.name_field).lower() for model in input_list if hasattr(model, self.name_field)]
        full_names = existing_names + new_names

        # There are duplicate names if this test fails
        if len(set(full_names)) != len(full_names):
            unique_names = [*dict.fromkeys(new_names)]

            for name in unique_names:
                existing_indices = [i for i, other_name in enumerate(existing_names) if other_name == name]
                new_indices = [i for i, other_name in enumerate(new_names) if other_name == name]
                if (len(existing_indices) + len(new_indices)) > 1:
                    existing_string = ""
                    new_string = ""
                    if existing_indices:
                        existing_list = ", ".join(str(i) for i in existing_indices[:-1])
                        existing_string = (
                            f" item{f's {existing_list} and ' if existing_list else ' '}"
                            f"{existing_indices[-1]} of the existing ClassList"
                        )
                    if new_indices:
                        new_list = ", ".join(str(i) for i in new_indices[:-1])
                        new_string = (
                            f" item{f's {new_list} and ' if new_list else ' '}" f"{new_indices[-1]} of the input list"
                        )
                    error_list.append(
                        f"    '{name}' is shared between{existing_string}"
                        f"{', and' if existing_string and new_string else ''}{new_string}"
                    )

            if error_list:
                newline = "\n"
                raise ValueError(
                    f"The value of the '{self.name_field}' attribute must be unique for each item in the ClassList:\n"
                    f"{newline.join(error for error in error_list)}"
                )

    def _check_classes(self, input_list: Sequence[T]) -> None:
        """Raise a ValueError if any object in a list of objects is not of the type specified by self._class_handle.

        Parameters
        ----------
        input_list : iterable
            A list of instances of the class given in self._class_handle.

        Raises
        ------
        ValueError
            Raised if the input list contains objects of any type other than that given in self._class_handle.

        """
        error_list = []
        for i, element in enumerate(input_list):
            if not isinstance(element, self._class_handle):
                error_list.append(f"    index {i} is of type {type(element).__name__}")
        if error_list:
            newline = "\n"
            raise ValueError(
                f"This ClassList only supports elements of type {self._class_handle.__name__}. "
                f"In the input list:\n{newline.join(error for error in error_list)}\n"
            )

    def _get_item_from_name_field(self, value: Union[T, str]) -> Union[T, str]:
        """Return the object with the given value of the name_field attribute in the ClassList.

        Parameters
        ----------
        value : T or str
            Either an object in the ClassList, or the value of the name_field attribute of an object in the ClassList.

        Returns
        -------
        instance : T or str
            Either the object with the value of the name_field attribute given by value, or the input value if an
            object with that value of the name_field attribute cannot be found.

        """
        try:
            lower_value = value.lower()
        except AttributeError:
            lower_value = value

        return next((model for model in self.data if getattr(model, self.name_field).lower() == lower_value), value)

    @staticmethod
    def _determine_class_handle(input_list: Sequence[T]):
        """When inputting a sequence of object to a ClassList, the _class_handle should be set as the type of the
        element which satisfies "issubclass" for all the other elements.

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

    # Pydantic core schema which allows ClassLists to be validated
    # in short: it validates that each ClassList is indeed a ClassList,
    # and then validates ClassList.data as though it were a typed list
    # e.g. ClassList[str] data is validated like list[str]
    @classmethod
    def __get_pydantic_core_schema__(cls, source: Any, handler):
        # import here so that the ClassList can be instantiated and used without Pydantic installed
        from pydantic import ValidatorFunctionWrapHandler
        from pydantic.types import (
            core_schema,  # import core_schema through here rather than making pydantic_core a dependency
        )
        from typing_extensions import get_args, get_origin

        # if annotated with a class, get the item type of that class
        origin = get_origin(source)
        item_tp = Any if origin is None else get_args(source)[0]

        list_schema = handler.generate_schema(list[item_tp])

        def coerce(v: Any, handler: ValidatorFunctionWrapHandler) -> ClassList[T]:
            """If a sequence is given, try to coerce it to a ClassList."""
            if isinstance(v, Sequence):
                classlist = ClassList()
                if len(v) > 0 and isinstance(v[0], dict):
                    # we want to be OK if the type is a model and is passed as a dict;
                    # pydantic will coerce it or fall over later
                    classlist._class_handle = dict
                elif item_tp is not Any:
                    classlist._class_handle = item_tp
                classlist.extend(v)
                v = classlist
            return v

        def validate_items(v: ClassList[T], handler: ValidatorFunctionWrapHandler) -> ClassList[T]:
            v.data = handler(v.data)
            return v

        schema = core_schema.chain_schema(
            [
                core_schema.no_info_wrap_validator_function(coerce, list_schema),
                core_schema.is_instance_schema(cls),
                core_schema.no_info_wrap_validator_function(validate_items, list_schema),
            ],
        )

        return schema
