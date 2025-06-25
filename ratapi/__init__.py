"""ratapi is a Python package for modelling, fitting and optimising reflectivity problems."""

from contextlib import suppress

import ratapi.examples as examples
from ratapi import events, models
from ratapi.classlist import ClassList
from ratapi.controls import Controls
from ratapi.outputs import BayesResults, Results
from ratapi.project import Project
from ratapi.run import run
from ratapi.utils import convert, plotting

with suppress(ImportError):  # orsopy is an optional dependency
    from ratapi.utils import orso as orso

__all__ = [
    "examples",
    "models",
    "events",
    "ClassList",
    "Controls",
    "BayesResults",
    "Results",
    "Project",
    "run",
    "plotting",
    "convert",
]
