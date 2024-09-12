import time

from tqdm.auto import tqdm

import RATapi.rat_core
from RATapi.inputs import make_input
from RATapi.outputs import make_results
from RATapi.utils.enums import Display


class ProgressBar:
    """Creates a progress bar that gets updates from the progress event during a
    calculation

    Parameters
    ----------
    display : bool, default: True
            Indicates if displaying is allowed
    """

    def __init__(self, display=True):
        self.pbar = None
        self.display = display
        self.tqdm_kwargs = {"total": 100, "desc": "", "bar_format": "{l_bar}{bar}", "disable": not self.display}
        # Determine if the auto tqdm is standard or notebook
        from tqdm.asyncio import tqdm as asyncio_tqdm

        if tqdm == asyncio_tqdm:
            self.tqdm_kwargs.update({"ncols": 90})

    def __enter__(self):
        if self.display:
            RATapi.events.register(RATapi.events.EventTypes.Progress, self.updateProgress)

        return self

    def updateProgress(self, event):
        """Callback for the progress event.

        Parameters
        ----------
        event: ProgressEventData
            The progress event data.
        """
        if self.pbar is None:
            self.pbar = tqdm(**self.tqdm_kwargs)
        value = event.percent * 100
        self.pbar.desc = event.message
        self.pbar.update(value - self.pbar.n)

    def __exit__(self, _exc_type, _exc_val, _traceback):
        if self.pbar is not None:
            self.pbar.close()
            print("")  # Print new line after bar

        if self.display:
            RATapi.events.clear(RATapi.events.EventTypes.Progress, self.updateProgress)


class TextOutput:
    """Pipes the message event to stdout

    Parameters
    ----------
    display : bool, default: True
            Indicates if displaying is allowed
    """

    def __init__(self, display=True):
        self.display = display

    def __enter__(self):
        if self.display:
            RATapi.events.register(RATapi.events.EventTypes.Message, self.printMessage)

        return self

    def printMessage(self, msg):
        """Callback for the message event.

        Parameters
        ----------
        msg: str
            The event message.
        """
        print(msg, end="")

    def __exit__(self, _exc_type, _exc_val, _traceback):
        if self.display:
            RATapi.events.clear(RATapi.events.EventTypes.Message, self.printMessage)


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
    display_on = controls.display != Display.Off
    problem_definition, cells, limits, priors, cpp_controls = make_input(project, controls)

    if display_on:
        print("Starting RAT " + horizontal_line)

    start = time.time()
    with ProgressBar(display=display_on), TextOutput(display=display_on):
        problem_definition, output_results, bayes_results = RATapi.rat_core.RATMain(
            problem_definition,
            cells,
            limits,
            cpp_controls,
            priors,
        )
    end = time.time()

    if display_on:
        print(f"Elapsed time is {end-start:.3f} seconds\n")

    results = make_results(controls.procedure, output_results, bayes_results)

    # Update parameter values in project
    for class_list in RATapi.project.parameter_class_lists:
        for index, value in enumerate(getattr(problem_definition, parameter_field[class_list])):
            getattr(project, class_list)[index].value = value

    controls.delete_IPC()

    if display_on:
        print("Finished RAT " + horizontal_line)

    return project, results
