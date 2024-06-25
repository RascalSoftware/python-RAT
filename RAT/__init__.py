import os

from RAT import models
from RAT.classlist import ClassList
from RAT.controls import set_controls
from RAT.project import Project
from RAT.run import run

__all__ = ["ClassList", "Project", "run", "set_controls", "models"]

dir_path = os.path.dirname(os.path.realpath(__file__))
os.environ["RAT_PATH"] = os.path.join(dir_path, "")
