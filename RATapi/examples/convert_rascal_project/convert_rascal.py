from pprint import pp

from RATapi.examples import DSPC_standard_layers
from RATapi.utils.convert import project_class_to_r1, r1_to_project_class


# convert R1 project to Project class
def convert_rascal():
    project = r1_to_project_class("R1monolayerVolumeModel.mat")
    print(project)

    # change values if you like, including ones not supported by R1
    project.parameters["Head Thickness"].prior_type = "gaussian"
    project.parameters["Theta"].mu = 2.0
    project.parameters["Area per molecule"].sigma = 50.0

    # convert DSPC standard layers example to a struct and save as file
    lipid_bilayer_project = DSPC_standard_layers()[0]
    project_class_to_r1(lipid_bilayer_project, filename="lipid_bilayer.mat")

    # convert and return as a Python dictionary
    struct = project_class_to_r1(lipid_bilayer_project, return_struct=True)
    pp(struct)

    return project, struct


if __name__ == "__main__":
    convert_rascal()
