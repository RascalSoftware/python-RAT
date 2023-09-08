"""Test the utils.custom_errors module."""

from pydantic import create_model, ValidationError
import pytest

import RAT.utils.custom_errors


def test_formatted_pydantic_error() -> None:
    """When a pytest ValidationError is raised we should be able to take it and construct a formatted string."""

    # Create a custom pydantic model for the test
    TestModel = create_model('TestModel', int_field=(int, 1), str_field=(str, 'a'))

    with pytest.raises(ValidationError) as exc_info:
        TestModel(int_field='string', str_field=5)

    error_str = RAT.utils.custom_errors.formatted_pydantic_error(exc_info.value)
    assert error_str == ('2 validation errors for TestModel\nint_field\n  Input should be a valid integer, unable to '
                         'parse string as an integer\nstr_field\n  Input should be a valid string')
