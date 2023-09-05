"""Defines routines for custom error handling in RAT."""

from pydantic import ValidationError


def formatted_pydantic_error(error: ValidationError) -> str:
    """Write a custom string format for pydantic validation errors.

    Parameters
    ----------
    error : pydantic.ValidationError
        A ValidationError produced by a pydantic model

    Returns
    -------
    error_str : str
        A string giving details of the ValidationError in a custom format.
    """
    num_errors = error.error_count()
    error_str = f'{num_errors} validation error{"s"[:num_errors!=1]} for {error.title}'
    for this_error in error.errors():
        error_str += '\n'
        if this_error['loc']:
            error_str += ' '.join(this_error['loc']) + '\n'
        error_str += '  ' + this_error['msg']
    return error_str
