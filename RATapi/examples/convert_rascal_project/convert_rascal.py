from pprint import pp

import RATapi as RAT


# convert R1 project to Project class
def convert_rascal():
    project = RAT.utils.convert.r1_to_project_class("R1monolayerVolumeModel.mat")

    # change values if you like, including ones not supported by R1
    project.parameters["Head Thickness"].prior_type = "gaussian"
    project.parameters["Theta"].mu = 2.0
    project.parameters["Area per molecule"].sigma = 50.0

    # convert DSPC standard layers example to a struct and save as file
    lipid_bilayer_project = RAT.examples.DSPC_standard_layers()[0]
    RAT.utils.convert.project_class_to_r1(lipid_bilayer_project, filename="lipid_bilayer.mat")

    # convert and return as a Python dictionary
    struct = RAT.utils.convert.project_class_to_r1(lipid_bilayer_project, return_struct=True)

    return project, struct


if __name__ == "__main__":
    project, struct = convert_rascal()
    print(project)
    pp(struct)
