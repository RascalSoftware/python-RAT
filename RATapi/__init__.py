"""RATapi is a Python package for modelling, fitting and optimising reflectivity problems."""

from contextlib import suppress

import RATapi.examples as examples
from RATapi import events, models
from RATapi.classlist import ClassList
from RATapi.controls import Controls
from RATapi.project import Project
from RATapi.run import run
from RATapi.utils import convert, plotting

with suppress(ImportError):  # orsopy is an optional dependency
    from RATapi.utils import orso as orso

__all__ = ["examples", "models", "events", "ClassList", "Controls", "Project", "run", "plotting", "convert"]
