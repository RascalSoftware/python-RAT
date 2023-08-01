"""Test the ClassList."""

import pytest
import re
from typing import Any, Dict, Iterable, List, Sequence, Union

from RAT.classlist import ClassList
from tests.utils import InputAttributes


@pytest.fixture
def two_name_class_list():
    """A ClassList of InputAttributes, containing two elements with names defined."""
    return ClassList([InputAttributes(name='Alice'), InputAttributes(name='Bob')])


@pytest.fixture
def three_name_class_list():
    """A ClassList of InputAttributes, containing three elements with names defined."""
    return ClassList([InputAttributes(name='Alice'), InputAttributes(name='Bob'), InputAttributes(name='Eve')])


class TestInitialisation(object):
    @pytest.mark.parametrize("input_object", [
        (InputAttributes()),
        (InputAttributes(name='Alice')),
        (InputAttributes(surname='Morgan')),
        ('Alice'),
    ])
    def test_input_object(self, input_object: Any) -> None:
        """For an input of an object, the ClassList should contain a one-element list containing the object and
        _class_handle should be set to the type of the object.
        """
        class_list = ClassList(input_object)
        assert class_list.data == [input_object]
        assert isinstance(input_object, class_list._class_handle)

    @pytest.mark.parametrize("input_sequence", [
        ([InputAttributes()]),
        ([InputAttributes(name='Alice')]),
        ([InputAttributes(surname='Morgan')]),
        ([InputAttributes(name='Alice'), InputAttributes(name='Bob')]),
        ((InputAttributes(),)),
        ((InputAttributes(name='Alice'),)),
        ((InputAttributes(surname='Morgan'),)),
        ((InputAttributes(name='Alice'), InputAttributes(name='Bob'))),
    ])
    def test_input_sequence(self, input_sequence: Sequence) -> None:
        """For an input of a sequence, the ClassList should be a list equal to the input sequence, and _class_handle
        should be set to the type of the objects within.
        """
        class_list = ClassList(input_sequence)
        assert class_list.data == list(input_sequence)
        assert isinstance(input_sequence[-1], class_list._class_handle)

    @pytest.mark.parametrize("input_list", [
        ([InputAttributes()]),
        ([InputAttributes(name='Alice')]),
        ([InputAttributes(surname='Morgan')]),
        ([InputAttributes(name='Alice'), InputAttributes(name='Bob')]),
     ])
    def test_empty_list(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with empty_list=True, the list should always be empty."""
        class_list = ClassList(input_list, empty_list=True)
        assert class_list == []
        assert isinstance(input_list[-1], class_list._class_handle)

    @pytest.mark.parametrize("empty_input", [([]), (())])
    def test_empty_input(self, empty_input: Sequence) -> None:
        """If we initialise a ClassList with an empty input (list or tuple), we should raise a ValueError."""
        with pytest.raises(ValueError, match="Input value must not be empty"):
            ClassList(empty_input)

    @pytest.mark.parametrize("input_list", [
        ([InputAttributes(name='Alice'), dict(name='Bob')]),
    ])
    def test_different_classes(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with an input containing multiple classes, we should raise a ValueError."""
        with pytest.raises(ValueError,
                           match=f"Input list contains elements of type other than '{type(input_list[0])}'"):
            ClassList(input_list)

    @pytest.mark.parametrize("input_list", [
        ([InputAttributes(name='Alice'), InputAttributes(name='Alice')]),
    ])
    def test_identical_name_fields(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with input objects with identical values of the name_field,
        we should raise a ValueError."""
        with pytest.raises(ValueError, match=f"Input list contains objects with the same value of the name attribute"):
            ClassList(input_list)


@pytest.mark.parametrize("expected_string", [
    ('    name\n--  ------\n 0  Alice\n 1  Bob'),
])
def test_repr_table(two_name_class_list: 'ClassList', expected_string: str) -> None:
    """For classes with the __dict__ attribute, we should be able to print the ClassList like a table."""
    assert repr(two_name_class_list) == expected_string


@pytest.mark.parametrize("input_list", [
    (['Alice', 'Bob']),
])
def test_repr_list(input_list: List) -> None:
    """For classes without the __dict__ attribute, we should be able to print the ClassList as a list."""
    class_list = ClassList(input_list)
    assert repr(class_list) == str(input_list)


@pytest.mark.parametrize(["new_values", "expected_classlist"], [
    ({'name': 'Eve'}, ClassList([InputAttributes(name='Eve'), InputAttributes(name='Bob')])),
    ({'name': 'John', 'surname': 'Luther'},
     ClassList([InputAttributes(name='John', surname='Luther'), InputAttributes(name='Bob')])),
])
def test_setitem(two_name_class_list: 'ClassList', new_values: Dict, expected_classlist: 'ClassList') -> None:
    """We should be able to set values in an element of a ClassList using a dictionary."""
    class_list = two_name_class_list
    class_list[0] = new_values
    assert repr(class_list) == repr(expected_classlist)


@pytest.mark.parametrize("new_values", [
    ({'name': 'Bob'}),
])
def test_setitem_same_name_field(two_name_class_list: 'ClassList', new_values: Dict) -> None:
    """If we set the name_field of an object in the ClassList to one already defined, we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the {two_name_class_list.name_field} "
                                         f"'{new_values[two_name_class_list.name_field]}', "
                                         f"which is already specified in the ClassList"):
        two_name_class_list[0] = new_values


@pytest.mark.parametrize("added_list", [
    (ClassList(InputAttributes(name='Eve'))),
    ([InputAttributes(name='Eve')]),
    ((InputAttributes(name='Eve'),)),
])
def test_iadd(two_name_class_list: 'ClassList', added_list: Iterable, three_name_class_list: 'ClassList') -> None:
    """We should be able to use the "+=" operator to add iterables to a ClassList."""
    class_list = two_name_class_list
    class_list += added_list
    assert class_list == three_name_class_list


def test_mul(two_name_class_list: 'ClassList') -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{two_name_class_list.__class__.__name__}' and "
                                                  f"'{n.__class__.__name__}'")):
        two_name_class_list * n


def test_rmul(two_name_class_list: 'ClassList') -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{n.__class__.__name__}' and "
                                                  f"'{two_name_class_list.__class__.__name__}'")):
        n * two_name_class_list


def test_imul(two_name_class_list: 'ClassList') -> None:
    """If we use the "*=" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *=: "
                                                  f"'{two_name_class_list.__class__.__name__}' and "
                                                  f"'{n.__class__.__name__}'")):
        two_name_class_list *= n


@pytest.mark.parametrize("new_values", [
    ({'name': 'Eve'}),
])
def test_append(two_name_class_list: 'ClassList', new_values: Dict, three_name_class_list: 'ClassList') -> None:
    """We should be able to append to a ClassList using keyword arguments."""
    class_list = two_name_class_list
    class_list.append(**new_values)
    assert class_list == three_name_class_list


@pytest.mark.parametrize("new_values", [
    ({'name': 'Alice'}),
])
def test_append_same_name_field(two_name_class_list: 'ClassList', new_values: Dict) -> None:
    """If we append an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the {two_name_class_list.name_field} "
                                         f"'{new_values[two_name_class_list.name_field]}', "
                                         f"which is already specified in the ClassList"):
        two_name_class_list.append(**new_values)


@pytest.mark.parametrize("new_values", [
    ({'name': 'Eve'})
])
def test_insert(two_name_class_list: 'ClassList', new_values: Dict) -> None:
    """We should be able to insert an object within a ClassList using keyword arguments."""
    two_name_class_list.insert(1, **new_values)
    assert two_name_class_list == ClassList([InputAttributes(name='Alice'),
                                             InputAttributes(name='Eve'),
                                             InputAttributes(name='Bob')])


@pytest.mark.parametrize("new_values", [
    ({'name': 'Alice'})
])
def test_insert_same_name(two_name_class_list: 'ClassList', new_values: Dict) -> None:
    """If we insert an object with an already-specified name_field value to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the {two_name_class_list.name_field} "
                                         f"'{new_values[two_name_class_list.name_field]}', "
                                         f"which is already specified in the ClassList"):
        two_name_class_list.insert(1, **new_values)


@pytest.mark.parametrize("remove_value", [
    ("Bob"),
    (InputAttributes(name='Bob')),
])
def test_remove(two_name_class_list: 'ClassList', remove_value: Union[object, str]) -> None:
    """We should be able to remove an object either by the value of the name_field or by specifying the object
    itself."""
    two_name_class_list.remove(remove_value)
    assert two_name_class_list == ClassList([InputAttributes(name='Alice')])


@pytest.mark.parametrize("remove_value", [
    ('Eve'),
    (InputAttributes(name='Eve')),
])
def test_remove_not_present(two_name_class_list: 'ClassList', remove_value: Union[object, str]) -> None:
    """If we remove an object not included in the ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=re.escape("list.remove(x): x not in list")):
        two_name_class_list.remove(remove_value)


@pytest.mark.parametrize(["count_value", "expected_count"], [
    ('Bob', 1),
    (InputAttributes(name='Bob'), 1),
    ('Eve', 0),
    (InputAttributes(name='Eve'), 0),
])
def test_count(two_name_class_list: 'ClassList', count_value: Union[object, str], expected_count: int) -> None:
    """We should be able to determine the number of times an object is in the ClassList using either the object itself
    or its name_field value.
    """
    assert two_name_class_list.count(count_value) == expected_count


@pytest.mark.parametrize(["index_value", "expected_index"], [
    ('Bob', 1),
    (InputAttributes(name='Bob'), 1),
])
def test_index(two_name_class_list: 'ClassList', index_value: Union[object, str], expected_index: int) -> None:
    """We should be able to find the index of an object in the ClassList either by its name_field value or by
    specifying the object itself.
    """
    assert two_name_class_list.index(index_value) == expected_index


@pytest.mark.parametrize("index_value", [
    ('Eve'),
    (InputAttributes(name='Eve')),
])
def test_index_not_present(two_name_class_list: 'ClassList', index_value: Union[object, str]) -> None:
    """If we try to find the index of an object not included in the ClassList we should raise a ValueError."""
    # with pytest.raises(ValueError, match=f"'{index_value}' is not in list") as e:
    with pytest.raises(ValueError):
        two_name_class_list.index(index_value)


@pytest.mark.parametrize("extended_list", [
    (ClassList(InputAttributes(name='Eve'))),
    ([InputAttributes(name='Eve')]),
    ((InputAttributes(name='Eve'),)),
])
def test_extend(two_name_class_list: 'ClassList', extended_list: Iterable, three_name_class_list: 'ClassList') -> None:
    """We should be able to extend a ClassList using another ClassList or an iterable"""
    class_list = two_name_class_list
    class_list.extend(extended_list)
    assert class_list == three_name_class_list


@pytest.mark.parametrize(["class_list", "expected_names"], [
    (ClassList([InputAttributes(name='Alice'), InputAttributes(name='Bob')]), ["Alice", "Bob"]),
    (ClassList([InputAttributes(surname='Morgan'), InputAttributes(surname='Terwilliger')]), []),
    (ClassList([InputAttributes(name='Alice', surname='Morgan'), InputAttributes(surname='Terwilliger')]), ["Alice"]),
    (ClassList(InputAttributes(), empty_list=True), []),
])
def test_get_names(class_list: 'ClassList', expected_names: List[str]) -> None:
    """We should get a list of the values of the name_field attribute from each object with it defined in the
    ClassList."""
    assert class_list.get_names() == expected_names


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Eve'}),
    ({'surname': 'Polastri'}),
])
def test__validate_name_field(two_name_class_list: 'ClassList', input_dict: Dict) -> None:
    """We should not raise an error if the input values do not contain a name_field value defined in an object in the
    ClassList."""
    assert two_name_class_list._validate_name_field(input_dict) is None


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Alice'}),
])
def test__validate_name_field_not_unique(two_name_class_list: 'ClassList', input_dict: Dict) -> None:
    """We should raise a ValueError if we input values containing a name_field defined in an object in the ClassList."""
    with pytest.raises(ValueError, match=f"Input arguments contain the {two_name_class_list.name_field} "
                                         f"'{input_dict[two_name_class_list.name_field]}', "
                                         f"which is already specified in the ClassList"):
        two_name_class_list._validate_name_field(input_dict)


@pytest.mark.parametrize("input_list", [
    ([InputAttributes(name='Alice'), InputAttributes(name='Bob')]),
    ([InputAttributes(surname='Morgan'), InputAttributes(surname='Terwilliger')]),
    ([InputAttributes(name='Alice', surname='Morgan'), InputAttributes(surname='Terwilliger')]),
    ([InputAttributes()]),
    ([]),
])
def test__check_unique_name_fields(two_name_class_list: 'ClassList', input_list: Iterable) -> None:
    """We should not raise an error if an input list contains objects with different name_field values, or if the
    name_field is not defined."""
    assert two_name_class_list._check_unique_name_fields(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([InputAttributes(name='Alice'), InputAttributes(name='Alice')]),
])
def test__check_unique_name_fields_not_unique(two_name_class_list: 'ClassList', input_list: Iterable) -> None:
    """We should raise a ValueError if an input list contains multiple objects with matching name_field values
    defined."""
    with pytest.raises(ValueError, match=f"Input list contains objects with the same value of the "
                                         f"{two_name_class_list.name_field} attribute"):
        two_name_class_list._check_unique_name_fields(input_list)


@pytest.mark.parametrize("input_list", [
    ([InputAttributes(name='Alice'), InputAttributes(name='Bob')]),
])
def test__check_classes(input_list: Iterable) -> None:
    """We should not raise an error all objects in the ClassList are of the same type."""
    class_list = ClassList([InputAttributes()])
    assert class_list._check_classes(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([InputAttributes(name='Alice'), dict(name='Bob')]),
])
def test__check_classes_different_classes(input_list: Iterable) -> None:
    """We should raise a ValueError if an input list contains objects of different types."""
    class_list = ClassList([InputAttributes()])
    with pytest.raises(ValueError, match=(f"Input list contains elements of type other "
                                          f"than '{class_list._class_handle}'")):
        class_list._check_classes(input_list)


@pytest.mark.parametrize(["value", "expected_output"], [
    ("Alice", InputAttributes(name='Alice')),
    ("Eve", "Eve"),
])
def test__get_item_from_name_field(two_name_class_list: 'ClassList',
                                   value: str,
                                   expected_output: Union[object, str]) -> None:
    """When we input the name_field value of an object defined in the ClassList, we should return the object.
    If the value is not the name_field of an object defined in the ClassList, we should return the value.
    """
    assert two_name_class_list._get_item_from_name_field(value) == expected_output
