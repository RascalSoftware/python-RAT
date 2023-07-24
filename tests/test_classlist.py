import pytest
import re
from typing import Any, Dict, Iterable, Sequence, Union

from RAT.classlist import ClassList


class DotDict(dict):
    """Create a dict type that can access keys using dot notation to test the ClassList."""
    __setattr__ = dict.__setitem__
    __delattr__ = dict.__delitem__

    def __getattr__(self, key: str) -> Any:
        try:
            return self.__getitem__(key)
        except KeyError as exc:
            raise AttributeError(key) from exc


@pytest.mark.parametrize("input_object", [
    (DotDict(name='Alice')),
    (DotDict(name='Alice', surname='Morgan')),
    ('Alice'),
])
def test_classlist_input_object(input_object):
    """For an input of an object, the ClassList should contain a one-element list containing the object and
    class_handle should be set to the type of the object.
    """
    class_list = ClassList(input_object)
    assert class_list.data == [input_object]
    assert type(input_object) is class_list.class_handle


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice')]),
    ([DotDict(name='Alice', surname='Morgan')]),
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
])
def test_classlist_input_list(input_list):
    """For an input of a list, the ClassList should be equal to the input list, and class_handle should be set to the
    type of the objects within.
    """
    class_list = ClassList(input_list)
    assert class_list.data == input_list
    assert type(input_list[-1]) is class_list.class_handle


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice')]),
    ([DotDict(name='Alice', surname='Morgan')]),
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
 ])
def test_classlist_empty_list(input_list):
    """If we initialise a ClassList with empty_list=True, the list should always be empty."""
    class_list = ClassList(input_list, empty_list=True)
    assert class_list == []
    assert type(input_list[-1]) is class_list.class_handle


@pytest.mark.parametrize("input_list", [([]), (())])
def test_classlist_empty_input(input_list):
    """If we initialise a ClassList with an empty input (list or tuple), we should raise a ValueError."""
    with pytest.raises(ValueError, match="Input value must not be empty"):
        ClassList(input_list)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), dict(name='Bob')])
])
def test_classlist_different_classes(input_list):
    """If we initialise a ClassList with an input containing multiple classes, we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input list contains elements of type other than '{type(input_list[0])}'"):
        ClassList(input_list)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Alice')])
])
def test_classlist_identical_names(input_list):
    """If we initialise a ClassList with input objects with identical "name" fields, we should raise a ValueError."""
    with pytest.raises(ValueError, match="Input list contains objects with the same name"):
        ClassList(input_list)


@pytest.mark.parametrize(["input_class_list", "new_values"], [
    (ClassList(DotDict(name='Alice', surname='Morgan')), {'name': 'John', 'surname': 'Luther'})
])
def test_setitem(input_class_list, new_values):
    """We should be able to set values in an element of a ClassList using a dictionary."""
    input_class_list[0] = new_values
    assert input_class_list == ClassList(DotDict(**new_values))


@pytest.mark.parametrize("added_list", [
    (ClassList(DotDict(name='Bob'))),
    ([DotDict(name='Bob')]),
    ((DotDict(name='Bob'),)),
])
def test_iadd(added_list):
    """We should be able to use the "+=" operator to add iterables to a ClassList."""
    class_list = ClassList(DotDict(name='Alice'))
    class_list += added_list
    assert class_list == ClassList([DotDict(name='Alice'), DotDict(name='Bob')])


def test_mul():
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    class_list = ClassList(DotDict(name='Alice'))
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{class_list.__class__.__name__}' and '{n.__class__.__name__}'")):
        class_list * n


def test_rmul():
    """If we use the "*" operator on a ClassList, we should raise a TypeError."""
    class_list = ClassList(DotDict(name='Alice'))
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *: "
                                                  f"'{n.__class__.__name__}' and '{class_list.__class__.__name__}'")):
        n * class_list


def test_imul():
    """If we use the "*=" operator on a ClassList, we should raise a TypeError."""
    class_list = ClassList(DotDict(name='Alice'))
    n = 2
    with pytest.raises(TypeError, match=re.escape(f"unsupported operand type(s) for *=: "
                                                  f"'{class_list.__class__.__name__}' and '{n.__class__.__name__}'")):
        class_list *= n


@pytest.mark.parametrize(["input_class_list", "new_values"], [
    (ClassList(DotDict(name='Alice')), {'name': 'Bob'})
])
def test_append(input_class_list, new_values):
    """We should be able to append to a ClassList using keyword arguments."""
    input_class_list.append(**new_values)
    assert input_class_list == ClassList([DotDict(name='Alice'), DotDict(name='Bob')])


@pytest.mark.parametrize(["input_class_list", "new_values"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), {'name': 'Bob'})
])
def test_append_same_name(input_class_list, new_values):
    """If we append an object with an already-specified name to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=f"Input arguments contain the name '{new_values['name']}', "
                                         f"which is already specified in the ClassList"):
        input_class_list.append(**new_values)


@pytest.mark.parametrize(["input_class_list", "new_values"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), {'name': 'Eve'})
])
def test_insert(input_class_list, new_values):
    """We should be able to insert an object within a ClassList using keyword arguments."""
    input_class_list.insert(1, **new_values)
    assert input_class_list == ClassList([DotDict(name='Alice'), DotDict(name='Eve'), DotDict(name='Bob')])


@pytest.mark.parametrize(["input_class_list", "new_values"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), {'name': 'Bob'})
])
def test_insert_same_name(input_class_list, new_values):
    """If we insert an object with an already-specified name to a ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=(f"Input arguments contain the name '{new_values['name']}',"
                                          f" which is already specified in the ClassList")):
        input_class_list.insert(1, **new_values)


@pytest.mark.parametrize(["input_class_list", "remove_value"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), "Bob"),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Bob')),
])
def test_remove(input_class_list, remove_value):
    """We should be able to remove an object either by name or by specifying the object itself."""
    input_class_list.remove(remove_value)
    assert input_class_list == ClassList([DotDict(name='Alice')])


@pytest.mark.parametrize(["input_class_list", "remove_value"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), 'Eve'),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Eve')),
])
def test_remove_not_present(input_class_list, remove_value):
    """If we remove an object not included in the ClassList we should raise a ValueError."""
    with pytest.raises(ValueError, match=re.escape("list.remove(x): x not in list")):
        input_class_list.remove(remove_value)


@pytest.mark.parametrize(["input_class_list", "count_value", "expected_count"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), 'Bob', 1),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Bob'), 1),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), 'Eve', 0),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Eve'), 0),
])
def test_count(input_class_list, count_value, expected_count):
    """We should be able to determine the number of times an object is in the ClassList using either the object itself
    or its name.
    """
    assert input_class_list.count(count_value) == expected_count


@pytest.mark.parametrize(["input_class_list", "index_value", "expected_index"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), 'Bob', 1),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Bob'), 1),
])
def test_index(input_class_list, index_value, expected_index):
    """We should be able to find the index of an object in the ClassList either by name or by specifying the object
    itself.
    """
    assert input_class_list.index(index_value) == expected_index


@pytest.mark.parametrize(["input_class_list", "index_value"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), 'Eve'),
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), DotDict(name='Eve')),
])
def test_index_not_present(input_class_list, index_value):
    """If we try to find the index of an object not included in the ClassList we should raise a ValueError."""
    #with pytest.raises(ValueError, match=f"'{index_value}' is not in list") as e:
    with pytest.raises(ValueError):
        input_class_list.index(index_value)


@pytest.mark.parametrize("extended_list", [
    (ClassList(DotDict(name='Bob'))),
    ([DotDict(name='Bob')]),
    ((DotDict(name='Bob'),)),
])
def test_extend(extended_list):
    """We should be able to extend a ClassList using another ClassList or an iterable"""
    class_list = ClassList(DotDict(name='Alice'))
    class_list.extend(extended_list)
    assert class_list == ClassList([DotDict(name='Alice'), DotDict(name='Bob')])


@pytest.mark.parametrize(["class_list", "expected_names"], [
    (ClassList([DotDict(name='Alice'), DotDict(name='Bob')]), ["Alice", "Bob"]),
    (ClassList([DotDict(surname='Morgan'), DotDict(surname='Odenkirk')]), []),
    (ClassList([DotDict(name='Alice', surname='Morgan'), DotDict(surname='Odenkirk')]), ["Alice"]),
    (ClassList(DotDict(), empty_list=True), []),
])
def test_get_names(class_list, expected_names):
    """We should get a list of the values of the "name" attribute from each object with it defined in the ClassList."""
    assert class_list.get_names() == expected_names


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Eve'}),
    ({'surname': 'Polastri'}),
])
def test__validate_name(input_dict):
    """We should not raise an error if the input values do not contain a name defined in an object in the ClassList."""
    class_list = ClassList([DotDict(name='Alice'), DotDict(name='Bob')])
    assert class_list._validate_name(input_dict) is None


@pytest.mark.parametrize("input_dict", [
    ({'name': 'Alice'}),
    ({'name': 'Alice', 'surname': 'Morgan'}),
])
def test__validate_name_not_unique(input_dict):
    """We should raise a ValueError if we input values containing a name defined in an object in the ClassList."""
    class_list = ClassList([DotDict(name='Alice'), DotDict(name='Bob')])
    with pytest.raises(ValueError, match=(f"Input arguments contain the name '{input_dict['name']}',"
                                          f" which is already specified in the ClassList")):
        class_list._validate_name(input_dict)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
    ([DotDict(surname='Morgan'), DotDict(surname='Odenkirk')]),
    ([DotDict(name='Alice', surname='Morgan'), DotDict(surname='Odenkirk')]),
    ([DotDict()]),
])
def test__check_unique_names(input_list):
    """We should not raise an error if an input list contains objects with different names, or no names defined."""
    assert ClassList._check_unique_names(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Alice')]),
])
def test__check_unique_names_not_unique(input_list):
    """We should raise a ValueError if an input list contains multiple object with matching names defined"""
    class_list = ClassList([DotDict(name='Alice'), DotDict(name='Bob')])
    with pytest.raises(ValueError, match=("Input list contains objects with the same name")):
        ClassList._check_unique_names(input_list)


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), DotDict(name='Bob')]),
])
def test__check_classes(input_list):
    """We should not raise an error all objects in the ClassList are of the same type."""
    class_list = ClassList([DotDict()])
    assert class_list._check_classes(input_list) is None


@pytest.mark.parametrize("input_list", [
    ([DotDict(name='Alice'), {'name': 'Bob'}]),
])
def test__check_classes_different_classes(input_list):
    """We should raise a ValueError if an input list contains objects of different types."""
    class_list = ClassList([DotDict()])
    with pytest.raises(ValueError, match=(f"Input list contains elements of type other "
                                          f"than '{class_list.class_handle}'")):
        class_list._check_classes(input_list)


@pytest.mark.parametrize(["name", "expected_output"], [
    ("Alice", DotDict(name='Alice')),
    ("Eve", "Eve"),
])
def test__get_item_from_name(name, expected_output):
    """When we input the name of an object defined in the ClassList, we should return the object. If the name is not
    the name of an object defined in the ClassList, we should return the name.
    """
    class_list = ClassList([DotDict(name='Alice'), DotDict(name='Bob')])
    assert class_list._get_item_from_name(name) == expected_output
