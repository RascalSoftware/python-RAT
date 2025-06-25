"""An example of how to convert a RasCAL-1 project to RAT."""

import pathlib
from pprint import pp

import ratapi as RAT


# convert R1 project to Project class
def convert_rascal(mat_filename="lipid_bilayer.mat"):
    """Convert a project from RasCAL-1 and a project to RasCAL-1.

    We convert a RasCAL-1 monolayer volume model to RAT, and the DSPC Standard Layers
    example to RasCAL-1.

    Parameters
    ----------
    mat_filename : str
        The filename of the output of the RAT project converted to RasCAL-2.

    Returns
    -------
    project, struct
        A RasCAL-1 monolayer volume model converted to a RAT project, and the
        struct of the DSPC standard layers example converted to a RasCAL-1 struct.

    """
    project_path = pathlib.Path(__file__).parent / "R1monolayerVolumeModel.mat"
    project = RAT.utils.convert.r1_to_project(project_path)

    # change values if you like, including ones not supported by R1
    project.parameters["Head Thickness"].prior_type = "gaussian"
    project.parameters["Theta"].mu = 2.0
    project.parameters["Area per molecule"].sigma = 50.0

    # convert DSPC standard layers example to a struct and save as file
    lipid_bilayer_project = RAT.examples.DSPC_standard_layers()[0]
    RAT.utils.convert.project_to_r1(lipid_bilayer_project, filename=mat_filename)

    # convert and return as a Python dictionary
    struct = RAT.utils.convert.project_to_r1(lipid_bilayer_project, return_struct=True)

    return project, struct


if __name__ == "__main__":
    project, struct = convert_rascal()
    print(project)
    pp(struct)
