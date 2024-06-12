from RAT.inputs import make_input
from RAT.outputs import make_results
import RAT.rat_core


def run(project, controls):
    """Run RAT for the given project and controls inputs."""

    parameter_field = {'parameters': 'params',
                       'bulk_in': 'bulkIn',
                       'bulk_out': 'bulkOut',
                       'scalefactors': 'scalefactors',
                       'domain_ratios': 'domainRatio',
                       'background_parameters': 'backgroundParams',
                       'resolution_parameters': 'resolutionParams',
                       }

    problem_definition, cells, limits, priors, cpp_controls = make_input(project, controls)

    problem_definition, output_results, bayes_results = RAT.rat_core.RATMain(problem_definition, cells, limits,
                                                                             cpp_controls, priors)

    results = make_results(controls.procedure, output_results, bayes_results)

    # Update parameter values in project
    for class_list in RAT.project.parameter_class_lists:
        for (index, value) in enumerate(getattr(problem_definition, parameter_field[class_list])):
            setattr(getattr(project, class_list)[index], 'value', value)

    return project, results
