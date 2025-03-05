"""Test the ClassList."""

import importlib
import re
import warnings
from collections import deque
from collections.abc import Iterable, Sequence
from typing import Any, Union

import prettytable
import pytest

from RATapi.classlist import ClassList
from tests.utils import InputAttributes, SubInputAttributes


@pytest.fixture
def one_name_class_list():
    """A ClassList of InputAttributes, containing one element with a name defined."""
    return ClassList([InputAttributes(name="Alice")])


@pytest.fixture
def two_name_class_list():
    """A ClassList of InputAttributes, containing two elements with names defined."""
    return ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob")])


@pytest.fixture
def two_name_class_list_table():
    """The table representation of the ClassList defined in the "two_name_class_list" fixture."""
    return (
        "+-------+-------+\n"
        "| index |  name |\n"
        "+-------+-------+\n"
        "|   0   | Alice |\n"
        "|   1   |  Bob  |\n"
        "+-------+-------+"
    )


@pytest.fixture
def three_name_class_list():
    """A ClassList of InputAttributes, containing three elements with names defined."""
    return ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob"), InputAttributes(name="Eve")])


class DisplayFieldsClass:
    """A classlist with four attributes and a display_fields property."""

    def __init__(self, display_range):
        self.a = 1
        self.b = 2
        self.c = 3
        self.d = 4

        self.display_range = display_range

    @property
    def display_fields(self):
        fields = ["a", "b", "c", "d"][self.display_range[0] : self.display_range[1]]
        return {f: getattr(self, f) for f in fields}


class TestInitialisation:
    @pytest.mark.parametrize(
        "input_object",
        [
            (InputAttributes()),
            (InputAttributes(name="Alice")),
            (InputAttributes(surname="Morgan")),
            "Alice",
        ],
    )
    def test_input_object(self, input_object: Any) -> None:
        """For an input of an object, the ClassList should contain a one-element list containing the object and
        _class_handle should be set to the type of the object.
        """
        class_list = ClassList(input_object)
        assert class_list.data == [input_object]
        assert isinstance(input_object, class_list._class_handle)

    @pytest.mark.parametrize(
        "input_sequence",
        [
            ([InputAttributes()]),
            ([InputAttributes(name="Alice")]),
            ([InputAttributes(surname="Morgan")]),
            ([InputAttributes(name="Alice"), InputAttributes(name="Bob")]),
            (InputAttributes(),),
            (InputAttributes(name="Alice"),),
            (InputAttributes(surname="Morgan"),),
            (InputAttributes(name="Alice"), InputAttributes(name="Bob")),
        ],
    )
    def test_input_sequence(self, input_sequence: Sequence[object]) -> None:
        """For an input of a sequence, the ClassList should be a list equal to the input sequence, and _class_handle
        should be set to the type of the objects within.
        """
        class_list = ClassList(input_sequence)
        assert class_list.data == list(input_sequence)
        for element in input_sequence:
            assert isinstance(element, class_list._class_handle)

    @pytest.mark.parametrize(
        "input_sequence",
        [
            ([InputAttributes(name="Alice"), SubInputAttributes(name="Bob")]),
            ([SubInputAttributes(name="Alice"), InputAttributes(name="Bob")]),
        ],
    )
    def test_input_sequence_subclass(self, input_sequence: Sequence[object]) -> None:
        """For an input of a sequence containing objects of a class and its subclasses, the ClassList should be a list
        equal to the input sequence, and _class_handle should be set to the type of the parent class.
        """
        class_list = ClassList(input_sequence)
        assert class_list.data == list(input_sequence)
        for element in input_sequence:
            assert isinstance(element, class_list._class_handle)

    @pytest.mark.parametrize("empty_input", [([]), (())])
    def test_empty_input(self, empty_input: Sequence[object]) -> None:
        """If we initialise a ClassList with an empty input (list or tuple), the ClassList should be an empty list, and
        _class_handle should be unset.
        """
        class_list = ClassList(empty_input)
        assert class_list.data == []
        assert not hasattr(class_list, "_class_handle")

    @pytest.mark.parametrize(
        "input_list",
        [
            ([InputAttributes(name="Alice"), dict(name="Bob")]),
        ],
    )
    def test_different_classes(self, input_list: Sequence[object]) -> None:
        """If we initialise a ClassList with an input containing multiple classes, we should raise a ValueError."""
        with pytest.raises(
            ValueError,
            match=f"This ClassList only supports elements of type {type(input_list[0]).__name__}. In the input list:\n"
            f"    index 1 is of type {type(input_list[1]).__name__}\n",
        ):
            ClassList(input_list)

    @pytest.mark.parametrize(
        "input_list, name_field",
        [
            ([InputAttributes(name="Alice"), InputAttributes(name="Alice")], "name"),
            ([InputAttributes(id="Alice"), InputAttributes(id="Alice")], "id"),
        ],
    )
    def test_identical_name_fields(self, input_list: Sequence[object], name_field: str) -> None:
        """If we initialise a ClassList with input objects with identical values of the name_field,
        we should raise a ValueError.
        """
        with pytest.raises(
            ValueError,
            match=f"The value of the '{name_field}' attribute must be unique for each item in the "
            f"ClassList:\n    '{getattr(input_list[0], name_field).lower()}'"
            f" is shared between items 0 and 1 of the input list",
        ):
            ClassList(input_list, name_field=name_field)


def test_str_table(two_name_class_list: ClassList, two_name_class_list_table: str) -> None:
    """For classes with the __dict__ attribute, we should be able to print the ClassList like a table."""
    assert str(two_name_class_list) == two_name_class_list_table


def test_str_empty_table() -> None:
    """For empty classes with the __dict__ attribute, we should be able to print the contents of the ClassList as a
    list.
    """
    empty_attributes = InputAttributes()
    assert str(ClassList(empty_attributes)) == str([empty_attributes])


@pytest.mark.parametrize(
    "input_list",
    [
        (["Alice", "Bob"]),
    ],
)
def test_str_list(input_list: list[str]) -> None:
    """For classes without the __dict__ attribute, we should be able to print the ClassList as a list."""
    class_list = ClassList(input_list)
    assert str(class_list) == str(input_list)


def test_str_empty_classlist() -> None:
    """For empty ClassLists, we should be able to print the ClassList as an empty list."""
    assert str(ClassList()) == str([])


@pytest.mark.parametrize(
    "display_ranges, expected_header",
    (
        ([(1, 3), (1, 3), (1, 3)], ["b", "c"]),
        ([(1, 2), (0, 4), (2, 3)], ["a", "b", "c", "d"]),
        ([(0, 2), (0, 1), (2, 3)], ["a", "b", "c"]),
    ),
)
def test_str_display_fields(display_ranges, expected_header):
    """If a class has the `display_fields` property, the ClassList should print with the minimal required attributes."""
    class_list = ClassList([DisplayFieldsClass(dr) for dr in display_ranges])
    expected_table = prettytable.PrettyTable()
    expected_table.field_names = ["index"] + expected_header
    expected_vals = {"a": 1, "b": 2, "c": 3, "d": 4}
    row = [expected_vals[v] for v in expected_header]
    expected_table.add_rows([[0] + row, [1] + row, [2] + row])

    assert str(class_list) == expected_table.get_string()


@pytest.mark.parametrize(
    ["new_item", "expected_classlist"],
    [
        (InputAttributes(name="Eve"), ClassList([InputAttributes(name="Eve"), InputAttributes(name="Bob")])),
        (
            InputAttributes(name="John", surname="Luther"),
            ClassList([InputAttributes(name="John", surname="Luther"), InputAttributes(name="Bob")]),
        ),
    ],
)
def test_setitem(two_name_class_list: ClassList, new_item: InputAttributes, expected_classlist: ClassList) -> None:
    """We should be able to set values in an element of a ClassList using a new object."""
    class_list = two_name_class_list
    class_list[0] = new_item
    assert class_list == expected_classlist


@pytest.mark.parametrize(
    "new_item",
    [
        (InputAttributes(name="Bob")),
    ],
)
def test_setitem_same_name_field(two_name_class_list: ClassList, new_item: InputAttributes) -> None:
    """If we set the name_field of an object in the ClassList to one already defined, we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"The value of the '{two_name_class_list.name_field}' attribute must be unique for each item in the "
        f"ClassList:\n    '{new_item.name.lower()}' is shared between item 1 of the existing ClassList,"
        f" and item 0 of the input list",
    ):
        two_name_class_list[0] = new_item


@pytest.mark.parametrize(
    "new_values",
    [
        "Bob",
    ],
)
def test_setitem_different_classes(two_name_class_list: ClassList, new_values: dict[str, Any]) -> None:
    """If we set the name_field of an object in the ClassList to one already defined, we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"This ClassList only supports elements of type {two_name_class_list._class_handle.__name__}. "
        f"In the input list:\n    index 0 is of type {type(new_values).__name__}\n",
    ):
        two_name_class_list[0] = new_values


def test_delitem(two_name_class_list: ClassList, one_name_class_list: ClassList) -> None:
    """We should be able to delete elements from a ClassList with the del operator."""
    class_list = two_name_class_list
    del class_list[1]
    assert class_list == one_name_class_list


def test_delitem_not_present(two_name_class_list: ClassList) -> None:
    """If we use the del operator to delete an index out of range, we should raise an IndexError."""
    class_list = two_name_class_list
    with pytest.raises(IndexError, match=re.escape("list assignment index out of range")):
        del class_list[2]


@pytest.mark.parametrize(
    "added_list",
    [
        (ClassList(InputAttributes(name="Eve"))),
        ([InputAttributes(name="Eve")]),
        (InputAttributes(name="Eve"),),
        (InputAttributes(name="Eve")),
    ],
)
def test_iadd(two_name_class_list: ClassList, added_list: Iterable, three_name_class_list: ClassList) -> None:
    """We should be able to use the "+=" operator to add iterables to a ClassList. Individual objects should be wrapped
    in a list before being added.
    """
    class_list = two_name_class_list
    class_list += added_list
    assert class_list == three_name_class_list


@pytest.mark.parametrize(
    "added_list",
    [
        (ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob")])),
        ([InputAttributes(name="Alice"), InputAttributes(name="Bob")]),
        (InputAttributes(name="Alice"), InputAttributes(name="Bob")),
    ],
)
def test_iadd_empty_classlist(added_list: Sequence, two_name_class_list: ClassList) -> None:
    """We should be able to use the "+=" operator to add iterables to an empty ClassList, whilst also setting
    _class_handle.
    """
    class_list = ClassList()
    class_list += added_list
    assert class_list == two_name_class_list
    assert isinstance(added_list[-1], class_list._class_handle)


def test_mul(two_name_class_list: ClassList) -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(
        TypeError,
        match=re.escape(
            f"unsupported operand type(s) for *: "
            f"'{two_name_class_list.__class__.__name__}' and "
            f"'{n.__class__.__name__}'",
        ),
    ):
        two_name_class_list * n


def test_rmul(two_name_class_list: ClassList) -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(
        TypeError,
        match=re.escape(
            f"unsupported operand type(s) for *: "
            f"'{n.__class__.__name__}' and "
            f"'{two_name_class_list.__class__.__name__}'",
        ),
    ):
        n * two_name_class_list


def test_imul(two_name_class_list: ClassList) -> None:
    """If we use the "*=" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(
        TypeError,
        match=re.escape(
            f"unsupported operand type(s) for *=: "
            f"'{two_name_class_list.__class__.__name__}' and "
            f"'{n.__class__.__name__}'",
        ),
    ):
        two_name_class_list *= n


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Eve")),
    ],
)
def test_append_object(two_name_class_list: ClassList, new_object: object, three_name_class_list: ClassList) -> None:
    """We should be able to append to a ClassList using a new object."""
    class_list = two_name_class_list
    class_list.append(new_object)
    assert class_list == three_name_class_list


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Eve"}),
    ],
)
def test_append_kwargs(
    two_name_class_list: ClassList,
    new_values: dict[str, Any],
    three_name_class_list: ClassList,
) -> None:
    """We should be able to append to a ClassList using keyword arguments."""
    class_list = two_name_class_list
    class_list.append(**new_values)
    assert class_list == three_name_class_list


@pytest.mark.parametrize(
    ["new_object", "new_values"],
    [
        (InputAttributes(name="Eve"), {"name": "John"}),
    ],
)
def test_append_object_and_kwargs(
    two_name_class_list: ClassList,
    new_object: object,
    new_values: dict[str, Any],
    three_name_class_list: ClassList,
) -> None:
    """If we append to a ClassList using a new object and keyword arguments, we raise a warning, and append the object,
    discarding the keyword arguments.
    """
    class_list = two_name_class_list
    with pytest.warns(SyntaxWarning):
        warnings.warn(
            "ClassList.append() called with both an object and keyword arguments. "
            "The keyword arguments will be ignored.",
            SyntaxWarning,
            stacklevel=2,
        )
        class_list.append(new_object, **new_values)
        assert class_list == three_name_class_list


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Alice")),
    ],
)
def test_append_object_empty_classlist(new_object: object, one_name_class_list: ClassList) -> None:
    """We should be able to append to an empty ClassList using a new object, whilst also setting _class_handle."""
    class_list = ClassList()
    class_list.append(new_object)
    assert class_list == one_name_class_list
    assert isinstance(new_object, class_list._class_handle)


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Alice"}),
    ],
)
def test_append_kwargs_empty_classlist(new_values: dict[str, Any]) -> None:
    """If we append to an empty ClassList using keyword arguments we should raise a TypeError."""
    class_list = ClassList()
    with pytest.raises(
        TypeError,
        match=re.escape(
            "ClassList.append() called with keyword arguments for a ClassList "
            "without a class defined. Call ClassList.append() with an object to "
            "define the class.",
        ),
    ):
        class_list.append(**new_values)


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Alice")),
    ],
)
def test_append_object_same_name_field(two_name_class_list: ClassList, new_object: object) -> None:
    """If we append an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"The value of the '{two_name_class_list.name_field}' attribute must be unique for each item in the "
        f"ClassList:\n    '{new_object.name.lower()}' is shared between item 0 of the existing ClassList, and "
        f"item 0 of the input list",
    ):
        two_name_class_list.append(new_object)


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Alice"}),
    ],
)
def test_append_kwargs_same_name_field(two_name_class_list: ClassList, new_values: dict[str, Any]) -> None:
    """If we append an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"Input arguments contain the {two_name_class_list.name_field} "
        f"'{new_values[two_name_class_list.name_field]}', "
        f"which is already specified at index 0 of the ClassList",
    ):
        two_name_class_list.append(**new_values)


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Eve")),
    ],
)
def test_insert_object(two_name_class_list: ClassList, new_object: object) -> None:
    """We should be able to insert an object within a ClassList using a new object."""
    two_name_class_list.insert(1, new_object)
    assert two_name_class_list == ClassList(
        [InputAttributes(name="Alice"), InputAttributes(name="Eve"), InputAttributes(name="Bob")],
    )


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Eve"}),
    ],
)
def test_insert_kwargs(two_name_class_list: ClassList, new_values: dict[str, Any]) -> None:
    """We should be able to insert an object within a ClassList using keyword arguments."""
    two_name_class_list.insert(1, **new_values)
    assert two_name_class_list == ClassList(
        [InputAttributes(name="Alice"), InputAttributes(name="Eve"), InputAttributes(name="Bob")],
    )


@pytest.mark.parametrize(
    ["new_object", "new_values"],
    [
        (InputAttributes(name="Eve"), {"name": "John"}),
    ],
)
def test_insert_object_and_kwargs(
    two_name_class_list: ClassList,
    new_object: object,
    new_values: dict[str, Any],
    three_name_class_list: ClassList,
) -> None:
    """If call insert() on a ClassList using a new object and keyword arguments, we raise a warning, and append the
    object, discarding the keyword arguments.
    """
    class_list = two_name_class_list
    with pytest.warns(SyntaxWarning):
        warnings.warn(
            "ClassList.insert() called with both an object and keyword arguments. "
            "The keyword arguments will be ignored.",
            SyntaxWarning,
            stacklevel=2,
        )
        class_list.insert(1, new_object, **new_values)
        assert class_list == ClassList(
            [InputAttributes(name="Alice"), InputAttributes(name="Eve"), InputAttributes(name="Bob")],
        )


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Alice")),
    ],
)
def test_insert_object_empty_classlist(new_object: object, one_name_class_list: ClassList) -> None:
    """We should be able to insert a new object into an empty ClassList, whilst also setting _class_handle."""
    class_list = ClassList()
    class_list.insert(0, new_object)
    assert class_list == one_name_class_list
    assert isinstance(new_object, class_list._class_handle)


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Alice"}),
    ],
)
def test_insert_kwargs_empty_classlist(new_values: dict[str, Any]) -> None:
    """If we append to an empty ClassList using keyword arguments we should raise a TypeError."""
    class_list = ClassList()
    with pytest.raises(
        TypeError,
        match=re.escape(
            "ClassList.insert() called with keyword arguments for a ClassList "
            "without a class defined. Call ClassList.insert() with an object to "
            "define the class.",
        ),
    ):
        class_list.insert(0, **new_values)


@pytest.mark.parametrize(
    "new_object",
    [
        (InputAttributes(name="Alice")),
    ],
)
def test_insert_object_same_name(two_name_class_list: ClassList, new_object: object) -> None:
    """If we insert an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"The value of the '{two_name_class_list.name_field}' attribute must be unique for each item in the "
        f"ClassList:\n    '{new_object.name.lower()}' is shared between item 0 of the existing "
        f"ClassList, and item 0 of the input list",
    ):
        two_name_class_list.insert(1, new_object)


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Alice"}),
    ],
)
def test_insert_kwargs_same_name(two_name_class_list: ClassList, new_values: dict[str, Any]) -> None:
    """If we insert an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"Input arguments contain the {two_name_class_list.name_field} "
        f"'{new_values[two_name_class_list.name_field]}', "
        f"which is already specified at index 0 of the ClassList",
    ):
        two_name_class_list.insert(1, **new_values)


@pytest.mark.parametrize(
    "remove_value",
    [
        "Bob",
        (InputAttributes(name="Bob")),
    ],
)
def test_remove(two_name_class_list: ClassList, remove_value: Union[object, str]) -> None:
    """We should be able to remove an object either by the value of the name_field or by specifying the object
    itself.
    """
    two_name_class_list.remove(remove_value)
    assert two_name_class_list == ClassList([InputAttributes(name="Alice")])


@pytest.mark.parametrize(
    "remove_value",
    [
        "Eve",
        (InputAttributes(name="Eve")),
    ],
)
def test_remove_not_present(two_name_class_list: ClassList, remove_value: Union[object, str]) -> None:
    """If we remove an object not included in the ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=re.escape("list.remove(x): x not in list")):
        two_name_class_list.remove(remove_value)


@pytest.mark.parametrize(
    ["count_value", "expected_count"],
    [
        ("Bob", 1),
        (InputAttributes(name="Bob"), 1),
        ("Eve", 0),
        (InputAttributes(name="Eve"), 0),
    ],
)
def test_count(two_name_class_list: ClassList, count_value: Union[object, str], expected_count: int) -> None:
    """We should be able to determine the number of times an object is in the ClassList using either the object itself
    or its name_field value.
    """
    assert two_name_class_list.count(count_value) == expected_count


@pytest.mark.parametrize(
    ["index_value", "expected_index"],
    [
        ("Bob", 1),
        (InputAttributes(name="Bob"), 1),
    ],
)
def test_index(two_name_class_list: ClassList, index_value: Union[object, str], expected_index: int) -> None:
    """We should be able to find the index of an object in the ClassList either by its name_field value or by
    specifying the object itself.
    """
    assert two_name_class_list.index(index_value) == expected_index


@pytest.mark.parametrize(
    ["index_value", "offset", "expected_index"],
    [
        ("Bob", 1, 2),
        (InputAttributes(name="Bob"), -3, -2),
    ],
)
def test_index_offset(
    two_name_class_list: ClassList,
    index_value: Union[object, str],
    offset: int,
    expected_index: int,
) -> None:
    """We should be able to find the index of an object in the ClassList either by its name_field value or by
    specifying the object itself. When using an offset, the value of the index should be shifted accordingly.
    """
    assert two_name_class_list.index(index_value, offset) == expected_index


@pytest.mark.parametrize(
    "index_value",
    [
        "Eve",
        (InputAttributes(name="Eve")),
    ],
)
def test_index_not_present(two_name_class_list: ClassList, index_value: Union[object, str]) -> None:
    """If we try to find the index of an object not included in the ClassList we should raise a ValueError."""
    # with pytest.raises(ValueError, match=f"'{index_value}' is not in list") as e:
    with pytest.raises(ValueError):
        two_name_class_list.index(index_value)


@pytest.mark.parametrize(
    "extended_list",
    [
        (ClassList(InputAttributes(name="Eve"))),
        ([InputAttributes(name="Eve")]),
        (InputAttributes(name="Eve"),),
        (InputAttributes(name="Eve")),
    ],
)
def test_extend(two_name_class_list: ClassList, extended_list: Sequence, three_name_class_list: ClassList) -> None:
    """We should be able to extend a ClassList using another ClassList or a sequence. Individual objects should be
    wrapped in a list before being added.
    """
    class_list = two_name_class_list
    class_list.extend(extended_list)
    assert class_list == three_name_class_list


@pytest.mark.parametrize(
    "extended_list",
    [
        (ClassList(InputAttributes(name="Alice"))),
        ([InputAttributes(name="Alice")]),
        (InputAttributes(name="Alice"),),
    ],
)
def test_extend_empty_classlist(extended_list: Sequence, one_name_class_list: ClassList) -> None:
    """We should be able to extend a ClassList using another ClassList or a sequence"""
    class_list = ClassList()
    class_list.extend(extended_list)
    assert class_list == one_name_class_list
    assert isinstance(extended_list[-1], class_list._class_handle)


@pytest.mark.parametrize("index", [0, "Alice", InputAttributes(name="Alice")])
@pytest.mark.parametrize(
    ["new_values", "expected_classlist"],
    [
        ({"name": "Eve"}, ClassList([InputAttributes(name="Eve"), InputAttributes(name="Bob")])),
        (
            {"name": "John", "surname": "Luther"},
            ClassList([InputAttributes(name="John", surname="Luther"), InputAttributes(name="Bob")]),
        ),
    ],
)
def test_set_fields(
    two_name_class_list: ClassList, index: Union[int, str], new_values: dict[str, Any], expected_classlist: ClassList
) -> None:
    """We should be able to set field values in an element of a ClassList using keyword arguments."""
    class_list = two_name_class_list
    class_list.set_fields(index, **new_values)
    assert class_list == expected_classlist


@pytest.mark.parametrize(
    "new_values",
    [
        ({"name": "Bob"}),
    ],
)
def test_set_fields_same_name_field(two_name_class_list: ClassList, new_values: dict[str, Any]) -> None:
    """If we set the name_field of an object in the ClassList to one already defined, we should raise a ValueError."""
    with pytest.raises(
        ValueError,
        match=f"Input arguments contain the {two_name_class_list.name_field} "
        f"'{new_values[two_name_class_list.name_field]}', "
        f"which is already specified at index 1 of the ClassList",
    ):
        two_name_class_list.set_fields(0, **new_values)


@pytest.mark.parametrize(
    ["class_list", "expected_names"],
    [
        (ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob")]), ["Alice", "Bob"]),
        (ClassList([InputAttributes(id="Alice"), InputAttributes(id="Bob")], name_field="id"), ["Alice", "Bob"]),
        (ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob")], name_field="id"), []),
        (ClassList([InputAttributes(surname="Morgan"), InputAttributes(surname="Terwilliger")]), []),
        (
            ClassList([InputAttributes(name="Alice", surname="Morgan"), InputAttributes(surname="Terwilliger")]),
            ["Alice"],
        ),
        (
            ClassList(
                [InputAttributes(name="Alice", surname="Morgan"), InputAttributes(surname="Terwilliger")],
                name_field="surname",
            ),
            ["Morgan", "Terwilliger"],
        ),
        (ClassList(InputAttributes()), []),
    ],
)
def test_get_names(class_list: ClassList, expected_names: list[str]) -> None:
    """We should get a list of the values of the name_field attribute from each object with it defined in the
    ClassList.
    """
    assert class_list.get_names() == expected_names


@pytest.mark.parametrize(
    ["class_list", "expected_matches"],
    [
        (ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob")]), [(0, "name")]),
        (ClassList([InputAttributes(name="Alice"), InputAttributes(name="Bob", id="Alice")]), [(0, "name"), (1, "id")]),
        (ClassList([InputAttributes(surname="Morgan"), InputAttributes(surname="Terwilliger")]), []),
        (ClassList(InputAttributes()), []),
    ],
)
def test_get_all_matches(class_list: ClassList, expected_matches: list[tuple]) -> None:
    """We should get a list of (index, field) tuples matching the given value in the ClassList."""
    assert class_list.get_all_matches("Alice") == expected_matches


@pytest.mark.parametrize(
    "input_dict",
    [
        ({"name": "Eve"}),
        ({"surname": "Polastri"}),
    ],
)
def test__validate_name_field(two_name_class_list: ClassList, input_dict: dict[str, Any]) -> None:
    """We should not raise an error if the input values do not contain a name_field value defined in an object in the
    ClassList.
    """
    assert two_name_class_list._validate_name_field(input_dict) is None


@pytest.mark.parametrize(
    "input_dict",
    [
        ({"name": "Alice"}),
        ({"name": "BOB"}),
        ({"name": "alice"}),
    ],
)
def test__validate_name_field_not_unique(two_name_class_list: ClassList, input_dict: dict[str, Any]) -> None:
    """We should raise a ValueError if we input values containing a name_field defined in an object in the ClassList,
    accounting for case sensitivity.
    """
    with pytest.raises(
        ValueError,
        match=f"Input arguments contain the {two_name_class_list.name_field} "
        f"'{input_dict[two_name_class_list.name_field]}', which is already specified at index "
        f"{two_name_class_list.index(input_dict['name'].lower())} of the ClassList",
    ):
        two_name_class_list._validate_name_field(input_dict)


@pytest.mark.parametrize(
    "input_list",
    [
        ([InputAttributes(name="Eve"), InputAttributes(name="Gareth")]),
        ([InputAttributes(surname="Polastri"), InputAttributes(surname="Mallory")]),
        ([InputAttributes(name="Eve", surname="Polastri"), InputAttributes(surname="Mallory")]),
        ([InputAttributes()]),
        ([]),
    ],
)
def test__check_unique_name_fields(two_name_class_list: ClassList, input_list: Iterable) -> None:
    """We should not raise an error if an input list contains objects with different name_field values, or if the
    name_field is not defined.
    """
    assert two_name_class_list._check_unique_name_fields(input_list) is None


@pytest.mark.parametrize(
    ["input_list", "error_message"],
    [
        (
            [InputAttributes(name="Alice"), InputAttributes(name="Bob")],
            (
                "    'alice' is shared between item 0 of the existing ClassList, and item 0 of the input list\n"
                "    'bob' is shared between item 1 of the existing ClassList, and item 1 of the input list"
            ),
        ),
        (
            [InputAttributes(name="Alice"), InputAttributes(name="Alice")],
            "    'alice' is shared between item 0 of the existing ClassList, and items 0 and 1 of the input list",
        ),
        (
            [InputAttributes(name="Alice"), InputAttributes(name="ALICE")],
            "    'alice' is shared between item 0 of the existing ClassList, and items 0 and 1 of the input list",
        ),
        (
            [InputAttributes(name="Alice"), InputAttributes(name="alice")],
            "    'alice' is shared between item 0 of the existing ClassList, and items 0 and 1 of the input list",
        ),
        (
            [InputAttributes(name="Eve"), InputAttributes(name="Eve")],
            "    'eve' is shared between items 0 and 1 of the input list",
        ),
        (
            [
                InputAttributes(name="Bob"),
                InputAttributes(name="Alice"),
                InputAttributes(name="Eve"),
                InputAttributes(name="Alice"),
                InputAttributes(name="Eve"),
                InputAttributes(name="Alice"),
            ],
            (
                "    'bob' is shared between item 1 of the existing ClassList, and item 0 of the input list\n"
                "    'alice' is shared between item 0 of the existing ClassList,"
                " and items 1, 3 and 5 of the input list\n"
                "    'eve' is shared between items 2 and 4 of the input list"
            ),
        ),
    ],
)
def test__check_unique_name_fields_not_unique(
    two_name_class_list: ClassList, input_list: Sequence, error_message: str
) -> None:
    """We should raise a ValueError if an input list contains multiple objects with (case-insensitive) matching
    name_field values defined.
    """
    with pytest.raises(
        ValueError,
        match=f"The value of the '{two_name_class_list.name_field}' attribute must be unique for each item in the "
        f"ClassList:\n{error_message}",
    ):
        two_name_class_list._check_unique_name_fields(input_list)


@pytest.mark.parametrize(
    "input_list",
    [
        ([InputAttributes(name="Alice"), InputAttributes(name="Bob")]),
    ],
)
def test__check_classes(input_list: Iterable) -> None:
    """We should not raise an error all objects in the ClassList are of the same type."""
    class_list = ClassList([InputAttributes()])
    assert class_list._check_classes(input_list) is None


@pytest.mark.parametrize(
    "input_list",
    [
        ([InputAttributes(name="Alice"), dict(name="Bob")]),
    ],
)
def test__check_classes_different_classes(input_list: Sequence) -> None:
    """We should raise a ValueError if an input list contains objects of different types."""
    class_list = ClassList([InputAttributes()])
    with pytest.raises(
        ValueError,
        match=(
            f"This ClassList only supports elements of type {class_list._class_handle.__name__}. "
            f"In the input list:\n    index 1 is of type {type(input_list[1]).__name__}"
        ),
    ):
        class_list._check_classes(input_list)


@pytest.mark.parametrize(
    ["value", "expected_output"],
    [
        ("Alice", InputAttributes(name="Alice")),
        ("ALICE", InputAttributes(name="Alice")),
        ("alice", InputAttributes(name="Alice")),
        ("Eve", "Eve"),
        ("EVE", "EVE"),
        ("eve", "eve"),
    ],
)
def test__get_item_from_name_field(
    two_name_class_list: ClassList,
    value: str,
    expected_output: Union[object, str],
) -> None:
    """When we input the name_field value of an object defined in the ClassList, we should return the object.
    If the value is not the name_field of an object defined in the ClassList, we should return the value.
    """
    assert two_name_class_list._get_item_from_name_field(value) == expected_output


@pytest.mark.parametrize(
    ["input_list", "expected_type"],
    [
        ([InputAttributes(name="Alice")], InputAttributes),
        ([InputAttributes(name="Alice"), SubInputAttributes(name="Bob")], InputAttributes),
        ([SubInputAttributes(name="Alice"), InputAttributes(name="Bob")], InputAttributes),
        ([SubInputAttributes(name="Alice"), SubInputAttributes(name="Bob")], SubInputAttributes),
        (
            [SubInputAttributes(name="Alice"), SubInputAttributes(name="Bob"), InputAttributes(name="Eve")],
            InputAttributes,
        ),
        ([InputAttributes(name="Alice"), dict(name="Bob")], InputAttributes),
        ([dict(name="Alice"), InputAttributes(name="Bob")], dict),
    ],
)
def test_determine_class_handle(input_list: ClassList, expected_type: type) -> None:
    """The _class_handle for the ClassList should be the type that satisfies the condition "isinstance(element, type)"
    for all elements in the ClassList.
    """
    assert ClassList._determine_class_handle(input_list) == expected_type


def test_get_item(two_name_class_list):
    """Test item names can be gotten by name, index, object or slice."""
    assert two_name_class_list[0] == two_name_class_list["Alice"]
    assert two_name_class_list[1] == two_name_class_list["Bob"]
    assert two_name_class_list[:] == two_name_class_list
    alice = InputAttributes(name="Alice")
    assert two_name_class_list[alice] == two_name_class_list["Alice"]
    assert two_name_class_list[alice] == two_name_class_list[0]


@pytest.mark.skipif(importlib.util.find_spec("pydantic") is None, reason="Pydantic not installed")
class TestPydantic:
    """Tests for the Pydantic integration for ClassLists."""

    import pydantic

    class Model(pydantic.BaseModel):
        classlist: ClassList[str]

    model_class = Model

    @pytest.mark.parametrize(
        "sequence", [["a", "b", "c"], ("a", "b", "c", "d"), deque(["a", "b", "c"]), ClassList(["a", "b"])]
    )
    def test_sequence_coercion(self, sequence):
        """Test that sequences are coerced to ClassLists."""
        model = self.model_class(classlist=sequence)
        assert model.classlist.data == list(sequence)
        assert model.classlist._class_handle is type(sequence[0])

    @pytest.mark.parametrize("input", [[1, 2, "string"], deque(["a", "b", 5.3]), [3.0, 4]])
    def test_coerce_bad_inputs(self, input):
        """Test that Pydantic successfully checks the type for input sequences."""
        with pytest.raises(self.pydantic.ValidationError, match="This ClassList only supports elements of type str."):
            self.model_class(classlist=input)

    def test_coerce_models(self):
        """Test that a ClassList of pydantic Models is coerced from a list of dicts."""
        import pydantic

        class SubModel(pydantic.BaseModel):
            i: int
            s: str
            f: float

        class NestedModel(pydantic.BaseModel):
            submodels: ClassList[SubModel]

        submodels_list = [{"i": 3, "s": "hello", "f": 3.0}, {"i": 4, "s": "hi", "f": 3.14}]

        model = NestedModel(submodels=submodels_list)
        for submodel, exp_dict in zip(model.submodels, submodels_list):
            for key, value in exp_dict.items():
                assert getattr(submodel, key) == value

    def test_set_pydantic_fields(self):
        """Test that intermediate validation errors for pydantic models are suppressed when using "set_fields"."""
        from pydantic import BaseModel, model_validator

        class MinMaxModel(BaseModel):
            min: float
            value: float
            max: float

            @model_validator(mode="after")
            def check_value_in_range(self) -> "MinMaxModel":
                if self.value < self.min or self.value > self.max:
                    raise ValueError(
                        f"value {self.value} is not within the defined range: {self.min} <= value <= {self.max}"
                    )
                return self

        model_list = ClassList([MinMaxModel(min=1, value=2, max=5)])
        model_list.set_fields(0, min=3, value=4)

        assert model_list == ClassList([MinMaxModel(min=3.0, value=4.0, max=5.0)])
