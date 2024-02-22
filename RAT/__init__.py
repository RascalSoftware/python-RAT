import os
from RAT.classlist import ClassList
from RAT.project import Project
import RAT.controls
import RAT.models


dir_path = os.path.dirname(os.path.realpath(__file__))
os.environ["RAT_PATH"] = os.path.join(dir_path, '')
