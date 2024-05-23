import os
from RAT.classlist import ClassList
from RAT.project import Project
from RAT.controls import set_controls
from RAT.run import run
import RAT.models

dir_path = os.path.dirname(os.path.realpath(__file__))
os.environ["RAT_PATH"] = os.path.join(dir_path, '')
