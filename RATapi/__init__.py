"""RATapi is a Python package for modelling, fitting and optimising reflectivity problems."""

import RATapi.examples as examples
from RATapi import events, models
from RATapi.classlist import ClassList
from RATapi.controls import Controls
from RATapi.project import Project
from RATapi.run import run
from RATapi.utils import convert, plotting

__all__ = ["examples", "models", "events", "ClassList", "Controls", "Project", "run", "plotting", "convert"]
