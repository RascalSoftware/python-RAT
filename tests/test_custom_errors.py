"""Test the utils.custom_errors module."""

from pydantic import create_model, ValidationError
import pytest

import RAT.utils.custom_errors


@pytest.fixture
def TestModel():
    """Create a custom pydantic model for the tests."""
    TestModel = create_model('TestModel', int_field=(int, 1), str_field=(str, 'a'))
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
