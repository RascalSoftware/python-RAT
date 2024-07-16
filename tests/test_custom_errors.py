"""Test the utils.custom_errors module."""

import re

import pytest
from pydantic import ValidationError, create_model

import RATapi.utils.custom_errors


@pytest.fixture
def TestModel():
    """Create a custom pydantic model for the tests."""
    TestModel = create_model("TestModel", int_field=(int, 1), str_field=(str, "a"), __config__={"extra": "forbid"})
    return TestModel


@pytest.mark.parametrize(
    ["custom_errors", "expected_error_message"],
    [
        (
            {},
            "2 validation errors for TestModel\nint_field\n  Input should be a valid integer, unable to parse string "
            "as an integer [type=int_parsing, input_value='string', input_type=str]\nstr_field\n  Input should be a "
            "valid string [type=string_type, input_value=5, input_type=int]",
        ),
        (
            {"int_parsing": "This is a custom error message", "string_type": "This is another custom error message"},
            "2 validation errors for TestModel\nint_field\n  This is a custom error message [type=int_parsing, "
            "input_value='string', input_type=str]\nstr_field\n  This is another custom error message "
            "[type=string_type, input_value=5, input_type=int]",
        ),
    ],
)
def test_custom_pydantic_validation_error(
    TestModel,
    custom_errors: dict[str, str],
    expected_error_message: str,
) -> None:
    """When we call custom_pydantic_validation_error with custom errors, we should return an error list containing
    PydanticCustomErrors, otherwise we return the original set of errors.
    """
    try:
        TestModel(int_field="string", str_field=5)
    except ValidationError as exc:
        custom_error_list = RATapi.utils.custom_errors.custom_pydantic_validation_error(exc.errors(), custom_errors)

    with pytest.raises(ValidationError, match=re.escape(expected_error_message)):
        raise ValidationError.from_exception_data("TestModel", custom_error_list)
