"""Usage examples for the Python RAT API."""

from RATapi.examples.absorption.absorption import absorption
from RATapi.examples.convert_rascal_project.convert_rascal import convert_rascal
from RATapi.examples.domains.domains_custom_layers import domains_custom_layers
from RATapi.examples.domains.domains_custom_XY import domains_custom_XY
from RATapi.examples.domains.domains_standard_layers import domains_standard_layers
from RATapi.examples.normal_reflectivity.DSPC_custom_layers import DSPC_custom_layers
from RATapi.examples.normal_reflectivity.DSPC_custom_XY import DSPC_custom_XY
from RATapi.examples.normal_reflectivity.DSPC_data_background import DSPC_data_background
from RATapi.examples.normal_reflectivity.DSPC_function_background import DSPC_function_background
from RATapi.examples.normal_reflectivity.DSPC_standard_layers import DSPC_standard_layers

__all__ = [
    "absorption",
    "domains_custom_layers",
    "domains_custom_XY",
    "domains_standard_layers",
    "DSPC_custom_layers",
    "DSPC_custom_XY",
    "DSPC_standard_layers",
    "DSPC_data_background",
    "DSPC_function_background",
    "convert_rascal",
]
