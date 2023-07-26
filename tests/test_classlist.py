"""Test the ClassList."""

import pytest
import re
from typing import Any, Dict, Iterable, List, Sequence, Union

from RAT.classlist import ClassList


class DotDict(dict):
    """A dict type that can access keys using dot notation."""
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__

    def __getattr__(self, key: str) -> Any:
        try:
            return self.__getitem__(key)
        except KeyError as exc:
            raise AttributeError(key) from exc


@pytest.fixture
def two_name_classlist():
    """A ClassList of DotDicts, containing two elements with names defined."""
    return ClassList([DotDict(name='Alice'), DotDict(name='Bob')])


@pytest.fixture
def three_name_classlist():
    """A ClassList of DotDicts, containing three elements with names defined."""
    return ClassList([DotDict(name='Alice'), DotDict(name='Bob'), DotDict(name='Eve')])


class TestInitialisation(object):
    @pytest.mark.parametrize("input_object", [
        (DotDict()),
        (DotDict(name='Alice')),
        (DotDict(surname='Morgan')),
        ('Alice'),
    ])
    def test_input_object(self, input_object: Any) -> None:
        """For an input of an object, the ClassList should contain a one-element list containing the object and
        class_handle should be set to the type of the object.
        """
        class_list = ClassList(input_object)
        assert class_list.data == [input_object]
        assert isinstance(input_object, class_list.class_handle)

    @pytest.mark.parametrize("input_sequence", [
        ([DotDict()]),
        ([DotDict(name='Alice')]),
        ([DotDict(surname='Morgan')]),
        ([DotDict(name='Alice'), DotDict(name='Bob')]),
        ((DotDict(),)),
        ((DotDict(name='Alice'),)),
        ((DotDict(surname='Morgan'),)),
        ((DotDict(name='Alice'), DotDict(name='Bob'))),
    ])
    def test_input_sequence(self, input_sequence: Sequence) -> None:
        """For an input of a sequence, the ClassList should be a list equal to the input sequence, and class_handle
        should be set to the type of the objects within.
        """
        class_list = ClassList(input_sequence)
        assert class_list.data == list(input_sequence)
        assert isinstance(input_sequence[-1], class_list.class_handle)

    @pytest.mark.parametrize("input_list", [
        ([DotDict()]),
        ([DotDict(name='Alice')]),
        ([DotDict(surname='Morgan')]),
        ([DotDict(name='Alice'), DotDict(name='Bob')]),
     ])
    def test_empty_list(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with empty_list=True, the list should always be empty."""
        class_list = ClassList(input_list, empty_list=True)
        assert class_list == []
        assert isinstance(input_list[-1], class_list.class_handle)

    @pytest.mark.parametrize("empty_input", [([]), (())])
    def test_empty_input(self, empty_input: Sequence) -> None:
        """If we initialise a ClassList with an empty input (list or tuple), we should raise a ValueError."""
        with pytest.raises(ValueError, match="Input value must not be empty"):
            ClassList(empty_input)

    @pytest.mark.parametrize("input_list", [
        ([DotDict(name='Alice'), dict(name='Bob')]),
    ])
    def test_different_classes(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with an input containing multiple classes, we should raise a ValueError."""
        with pytest.raises(ValueError, match=f"Input list contains elements of type other than '{type(input_list[0])}'"):
            ClassList(input_list)

    @pytest.mark.parametrize("input_list", [
        ([DotDict(name='Alice'), DotDict(name='Alice')]),
    ])
    def test_identical_names(self, input_list: Sequence) -> None:
        """If we initialise a ClassList with input objects with identical "name" fields, we should raise a ValueError."""
        with pytest.raises(ValueError, match="Input list contains objects with the same name"):
            ClassList(input_list)


@pytest.mark.parametrize(["new_values", "expected_classlist"], [
    ({'name': 'Eve'}, ClassList([DotDict(name='Eve'), DotDict(name='Bob')])),
    ({'name': 'John', 'surname': 'Luther'}, ClassList([DotDict(name='John', surname='Luther'), DotDict(name='Bob')])),
])
def test_setitem(two_name_classlist: 'ClassList', new_values: Dict, expected_classlist: 'ClassList') -> None:
    """We should be able to set values in an element of a ClassList using a dictionary."""
    class_list = two_name_classlist
    class_list[0] = new_values
    assert class_list == expected_classlist


@pytest.mark.parametrize("new_values", [
    ({'name': 'Bob'}),
])
def test_setitem_same_name(two_name_classlist: 'ClassList', new_values: Dict) -> None:
    """If we set the name of an object in the ClassList to one already defined, we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the name '{new_values['name']}', "
                                         f"which is already specified in the ClassList"):
        two_name_classlist[0] = new_values


@pytest.mark.parametrize("added_list", [
    (ClassList(DotDict(name='Eve'))),
    ([DotDict(name='Eve')]),
    ((DotDict(name='Eve'),)),
])
def test_iadd(two_name_classlist: 'ClassList', added_list: Iterable, three_name_classlist: 'ClassList') -> None:
    """We should be able to use the "+=" operator to add iterables to a ClassList."""
    class_list = two_name_classlist
    class_list += added_list
    assert class_list == three_name_classlist


def test_mul(two_name_classlist: 'ClassList') -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{two_name_classlist.__class__.__name__}' and "
                                                  f"'{n.__class__.__name__}'")):
        two_name_classlist * n


def test_rmul(two_name_classlist: 'ClassList') -> None:
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{n.__class__.__name__}' and "
                                                  f"'{two_name_classlist.__class__.__name__}'")):
        n * two_name_classlist


def test_imul(two_name_classlist: 'ClassList') -> None:
    """If we use the "*=" operator on a ClassList, we should raise a TypeError."""
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *=: "
                                                  f"'{two_name_classlist.__class__.__name__}' and "
                                                  f"'{n.__class__.__name__}'")):
        two_name_classlist *= n


@pytest.mark.parametrize("new_values", [
    ({'name': 'Eve'}),
])
def test_append(two_name_classlist: 'ClassList', new_values: Dict, three_name_classlist: 'ClassList') -> None:
    """We should be able to append to a ClassList using keyword arguments."""
    class_list = two_name_classlist
    class_list.append(**new_values)
    assert class_list == three_name_classlist


@pytest.mark.parametrize("new_values", [
    ({'name': 'Alice'}),
])
def test_append_same_name(two_name_classlist: 'ClassList', new_values: Dict) -> None:
    """If we append an object with an already-specified name to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the name '{new_values['name']}', "
                                         f"which is already specified in the ClassList"):
        two_name_classlist.append(**new_values)


@pytest.mark.parametrize("new_values", [
    ({'name': 'Eve'})
])
def test_insert(two_name_classlist: 'ClassList', new_values: Dict) -> None:
    """We should be able to insert an object within a ClassList using keyword arguments."""
    two_name_classlist.insert(1, **new_values)
    assert two_name_classlist == ClassList([DotDict(name='Alice'), DotDict(name='Eve'), DotDict(name='Bob')])


@pytest.mark.parametrize("new_values", [
    ({'name': 'Alice'})
])
def test_insert_same_name(two_name_classlist: 'ClassList', new_values: Dict) -> None:
    """If we insert an object with an already-specified name to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=(f"Input arguments contain the name '{new_values['name']}',"
                                          f" which is already specified in the ClassList")):
        two_name_classlist.insert(1, **new_values)


@pytest.mark.parametrize("remove_value", [
    ("Bob"),
    (DotDict(name='Bob')),
])
def test_remove(two_name_classlist: 'ClassList', remove_value: Union[object, str]) -> None:
    """We should be able to remove an object either by name or by specifying the object itself."""
    two_name_classlist.remove(remove_value)
    assert two_name_classlist == ClassList([DotDict(name='Alice')])


@pytest.mark.parametrize("remove_value", [
    ('Eve'),
    (DotDict(name='Eve')),
])
def test_remove_not_present(two_name_classlist: 'ClassList', remove_value: Union[object, str]) -> None:
    """If we remove an object not included in the ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=re.escape("list.remove(x): x not in list")):
        two_name_classlist.remove(remove_value)


@pytest.mark.parametrize(["count_value", "expected_count"], [
    ('Bob', 1),
    (DotDict(name='Bob'), 1),
    ('Eve', 0),
    (DotDict(name='Eve'), 0),
])
def test_count(two_name_classlist: 'ClassList', count_value: Union[object, str], expected_count: int) -> None:
    """We should be able to determine the number of times an object is in the ClassList using either the object itself
    or its name.
    """
    assert two_name_classlist.count(count_value) == expected_count


@pytest.mark.parametrize(["index_value", "expected_index"], [
    ('Bob', 1),
    (DotDict(name='Bob'), 1),
])
def test_index(two_name_classlist: 'ClassList', index_value: Union[object, str], expected_index: int) -> None:
    """We should be able to find the index of an object in the ClassList either by name or by specifying the object
    itself.
    """
    assert two_name_classlist.index(index_value) == expected_index


@pytest.mark.parametrize("index_value", [
    ('Eve'),
    (DotDict(name='Eve')),
])
def test_index_not_present(two_name_classlist: 'ClassList', index_value: Union[object, str]) -> None:
    """If we try to find the index of an object not included in the ClassList we should raise a ValueError."""
    # with pytest.raises(ValueError, match=f"'{index_value}' is not in list") as e:
    with pytest.raises(ValueError):
        two_name_classlist.index(index_value)


@pytest.mark.parametrize("extended_list", [
    (ClassList(DotDict(name='Eve'))),
    ([DotDict(name='Eve')]),
    ((DotDict(name='Eve'),)),
])
def test_extend(two_name_classlist: 'ClassList', extended_list: Iterable, three_name_classlist: 'ClassList') -> None:
    """We should be able to extend a ClassList using another ClassList or an iterable"""
    class_list = two_name_classlist
    class_list.extend(extended_list)
    assert class_list == three_name_classlist


@pytest.mark.parametrize(["class_list", "expected_names"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), ["Alice", "Bob"]),
    (ClassList([DotDict(surname='Morgan'), DotDict(surname='Terwilliger')]), []),
    (ClassList([DotDict(name='Alice', surname='Morgan'), DotDict(surname='Terwilliger')]), ["Alice"]),
    (ClassList(DotDict(), empty_list=True), []),
])
def test_get_names(class_list: 'ClassList', expected_names: List[str]) -> None:
    """We should get a list of the values of the "name" attribute from each object with it defined in the ClassList."""
    assert class_list.get_names() == expected_names


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Eve'}),
    ({'surname': 'Polastri'}),
])
def test__validate_name(two_name_classlist: 'ClassList', input_dict: Dict) -> None:
    """We should not raise an error if the input values do not contain a name defined in an object in the ClassList."""
    assert two_name_classlist._validate_name(input_dict) is None


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Alice'}),
])
def test__validate_name_not_unique(two_name_classlist: 'ClassList', input_dict: Dict) -> None:
    """We should raise a ValueError if we input values containing a name defined in an object in the ClassList."""
    with pytest.raises(ValueError, match=(f"Input arguments contain the name '{input_dict['name']}',"
                                          f" which is already specified in the ClassList")):
        two_name_classlist._validate_name(input_dict)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
    ([DotDict(surname='Morgan'), DotDict(surname='Terwilliger')]),
    ([DotDict(name='Alice', surname='Morgan'), DotDict(surname='Terwilliger')]),
    ([DotDict()]),
    ([]),
])
def test__check_unique_names(input_list: Iterable) -> None:
    """We should not raise an error if an input list contains objects with different names, or no names defined."""
    assert ClassList._check_unique_names(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Alice')]),
])
def test__check_unique_names_not_unique(input_list: Iterable) -> None:
    """We should raise a ValueError if an input list contains multiple object with matching names defined."""
    with pytest.raises(ValueError, match="Input list contains objects with the same name"):
        ClassList._check_unique_names(input_list)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
])
def test__check_classes(input_list: Iterable) -> None:
    """We should not raise an error all objects in the ClassList are of the same type."""
    class_list = ClassList([DotDict()])
    assert class_list._check_classes(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), dict(name='Bob')]),
])
def test__check_classes_different_classes(input_list: Iterable) -> None:
    """We should raise a ValueError if an input list contains objects of different types."""
    class_list = ClassList([DotDict()])
    with pytest.raises(ValueError, match=(f"Input list contains elements of type other "
                                          f"than '{class_list.class_handle}'")):
        class_list._check_classes(input_list)


@pytest.mark.parametrize(["name", "expected_output"], [
    ("Alice", DotDict(name='Alice')),
    ("Eve", "Eve"),
])
def test__get_item_from_name(two_name_classlist: 'ClassList', name: str, expected_output: Union[object, str]) -> None:
    """When we input the name of an object defined in the ClassList, we should return the object. If the name is not
    the name of an object defined in the ClassList, we should return the name.
    """
    assert two_name_classlist._get_item_from_name(name) == expected_output
