import time

import RATapi.rat_core
from RATapi.inputs import make_input
from RATapi.outputs import make_results
from RATapi.utils.enums import Display


def run(project, controls):
    """Run RAT for the given project and controls inputs."""
    parameter_field = {
        "parameters": "params",
        "bulk_in": "bulkIn",
        "bulk_out": "bulkOut",
        "scalefactors": "scalefactors",
        "domain_ratios": "domainRatio",
        "background_parameters": "backgroundParams",
        "resolution_parameters": "resolutionParams",
    }

    horizontal_line = "\u2500" * 107 + "\n"

    problem_definition, cells, limits, priors, cpp_controls = make_input(project, controls)

    if controls.display != Display.Off:
        print("Starting RAT " + horizontal_line)

    start = time.time()
    problem_definition, output_results, bayes_results = RATapi.rat_core.RATMain(
        problem_definition,
        cells,
        limits,
        cpp_controls,
        priors,
    )
    end = time.time()

    if controls.display != Display.Off:
        print(f"Elapsed time is {end-start:.3f} seconds\n")

    results = make_results(controls.procedure, output_results, bayes_results)

    # Update parameter values in project
    for class_list in RATapi.project.parameter_class_lists:
        for index, value in enumerate(getattr(problem_definition, parameter_field[class_list])):
            getattr(project, class_list)[index].value = value

    if controls.display != Display.Off:
        print("Finished RAT " + horizontal_line)

    return project, results


def get_time_string(total_time: float) -> str:
    """Return a string of the time elapsed in an appropriate format."""
    hours = int(total_time // 3600)
    minutes = int((total_time % 3600) // 60)
    seconds = (total_time % 3600) % 60

    if hours > 0:
        time_string = (
            f"{hours} hour{'s' if hours != 1 else ''}, {minutes} minute{'s' if minutes != 1 else ''},"
            f" {seconds:.3f} seconds"
        )
    elif minutes > 0:
        time_string = f"{minutes} minute{'s' if minutes != 1 else ''}, {seconds:.3f} seconds"
    else:
        time_string = f"{seconds:.3f} seconds"

    return time_string
