"""Test the utils.custom_errors module."""
import pydantic
from pydantic import create_model, ValidationError
import pytest

import RAT.utils.custom_errors


@pytest.fixture
def TestModel():
    """Create a custom pydantic model for the tests."""
    TestModel = create_model('TestModel', int_field=(int, 1), str_field=(str, 'a'), __config__={'extra': 'forbid'})
    return TestModel


def test_formatted_pydantic_error(TestModel) -> None:
    """When a pytest ValidationError is raised we should be able to take it and construct a formatted string."""
    with pytest.raises(ValidationError) as exc_info:
        TestModel(int_field='string', str_field=5)

    error_str = RAT.utils.custom_errors.formatted_pydantic_error(exc_info.value)
    assert error_str == ('2 validation errors for TestModel\nint_field\n  Input should be a valid integer, unable to '
                         'parse string as an integer\nstr_field\n  Input should be a valid string')


def test_formatted_pydantic_error_custom_messages(TestModel) -> None:
    """When a pytest ValidationError is raised we should be able to take it and construct a formatted string,
    including the custom error messages provided."""
    with pytest.raises(ValidationError) as exc_info:
        TestModel(int_field='string', str_field=5)

    custom_messages = {'int_parsing': 'This is a custom error message',
                       'string_type': 'This is another custom error message'}
    error_str = RAT.utils.custom_errors.formatted_pydantic_error(exc_info.value, custom_messages)
    assert error_str == ('2 validation errors for TestModel\nint_field\n  This is a custom error message\n'
                         'str_field\n  This is another custom error message')


def test_formatted_traceback_type_error(TestModel) -> None:
    """The formatted_traceback routine should return the traceback string from "traceback.format_exc()", including the
    error message.
    """
    error_message = '__init__() takes 1 positional argument but 2 were given'
    traceback_string = ''

    try:
        TestModel('invalid')
    except TypeError:
        traceback_string = RAT.utils.custom_errors.formatted_traceback()

    assert 'TypeError' in traceback_string
    assert error_message in traceback_string


def test_formatted_traceback_validation_error(TestModel) -> None:
    """The formatted_traceback routine should return the traceback string from "traceback.format_exc()", with the error
    message removed for a pydantic ValidationError.
    """
    error_message = (f"pydantic_core._pydantic_core.ValidationError: 1 validation error for {TestModel.__name__}\n"
                     f"extra_field\n  Extra inputs are not permitted [type=extra_forbidden, input_value='invalid',"
                     f" input_type=str]\n")
    traceback_string = error_message

    try:
        TestModel(extra_field='invalid')
    except pydantic.ValidationError:
        traceback_string = RAT.utils.custom_errors.formatted_traceback()

    assert error_message not in traceback_string
