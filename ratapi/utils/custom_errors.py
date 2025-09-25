"""Defines routines for custom error handling in RAT."""

import pydantic_core


def custom_pydantic_validation_error(
    error_list: list[pydantic_core.ErrorDetails],
    custom_error_msgs: dict[str, str] | None = None,
) -> list[pydantic_core.ErrorDetails]:
    """Give Pydantic errors a better custom message with extraneous information removed.

    We substitute the standard error for a PydanticCustomError for a given set of error types.
    For errors that do not have a custom error message defined,
    we redefine them using a PydanticCustomError to remove the url from the error message.

    Parameters
    ----------
    error_list : list[pydantic_core.ErrorDetails]
        A list of errors produced by pydantic.ValidationError.errors().
    custom_error_msgs: dict[str, str], optional
        A dict of custom error messages for given error types.

    Returns
    -------
    new_error : list[pydantic_core.ErrorDetails]
        A list of errors including PydanticCustomErrors in place of the error types in custom_errors.

    """
    if custom_error_msgs is None:
        custom_error_msgs = {}
    custom_error_list = []
    for error in error_list:
        if error["type"] in custom_error_msgs:
            custom_error = pydantic_core.PydanticCustomError(error["type"], custom_error_msgs[error["type"]])
        else:
            custom_error = pydantic_core.PydanticCustomError(error["type"], error["msg"])
        error["type"] = custom_error
        custom_error_list.append(error)

    return custom_error_list
