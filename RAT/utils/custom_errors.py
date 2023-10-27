"""Defines routines for custom error handling in RAT."""

from pydantic import ValidationError


def formatted_pydantic_error(error: ValidationError, custom_error_messages: dict[str, str] = None) -> str:
    """Write a custom string format for pydantic validation errors.

    Parameters
    ----------
    error : pydantic.ValidationError
        A ValidationError produced by a pydantic model.
    custom_error_messages: dict[str, str], optional
        A dict of custom error messages for given error types.

    Returns
    -------
    error_str : str
        A string giving details of the ValidationError in a custom format.
    """
    if custom_error_messages is None:
        custom_error_messages = {}
    num_errors = error.error_count()
    error_str = f'{num_errors} validation error{"s"[:num_errors!=1]} for {error.title}'

    for this_error in error.errors():
        error_type = this_error['type']
        error_msg = custom_error_messages[error_type] if error_type in custom_error_messages else this_error["msg"]

        error_str += '\n'
        if this_error['loc']:
            error_str += ' '.join(this_error['loc']) + '\n'
        error_str += f'  {error_msg}'

    return error_str
