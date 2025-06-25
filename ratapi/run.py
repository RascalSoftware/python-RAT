"""The function used to run a RAT algorithm for a given project and controls."""

import time

from tqdm.auto import tqdm

import ratapi.rat_core
from ratapi.inputs import make_input
from ratapi.outputs import make_results
from ratapi.utils.enums import Display


class ProgressBar:
    """Create a progress bar that gets updates from the progress event during a calculation.

    Parameters
    ----------
    display : bool, default True
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
            ratapi.events.register(ratapi.events.EventTypes.Progress, self.updateProgress)

        return self

    def updateProgress(self, event):
        """Update the progress bar with progress event data.

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
            ratapi.events.clear(ratapi.events.EventTypes.Progress, self.updateProgress)


class TextOutput:
    """Context manager to pipe message events to stdout.

    Parameters
    ----------
    display : bool, default: True
            Indicates if displaying is allowed

    """

    def __init__(self, display=True):
        self.display = display

    def __enter__(self):
        if self.display:
            ratapi.events.register(ratapi.events.EventTypes.Message, self.printMessage)

        return self

    def printMessage(self, msg):
        """Print an event message.

        Parameters
        ----------
        msg: str
            The event message.

        """
        print(msg, end="")

    def __exit__(self, _exc_type, _exc_val, _traceback):
        if self.display:
            ratapi.events.clear(ratapi.events.EventTypes.Message, self.printMessage)


def run(project, controls):
    """Run RAT for the given project and controls inputs."""
    parameter_field = {
        "parameters": "params",
        "bulk_in": "bulkIns",
        "bulk_out": "bulkOuts",
        "scalefactors": "scalefactors",
        "domain_ratios": "domainRatios",
        "background_parameters": "backgroundParams",
        "resolution_parameters": "resolutionParams",
    }

    horizontal_line = "\u2500" * 107 + "\n"
    display_on = controls.display != Display.Off
    problem_definition, cpp_controls = make_input(project, controls)

    if display_on:
        print("Starting RAT " + horizontal_line)

    start = time.time()
    with ProgressBar(display=display_on), TextOutput(display=display_on):
        problem_definition, output_results, bayes_results = ratapi.rat_core.RATMain(
            problem_definition,
            cpp_controls,
        )
    end = time.time()

    if display_on:
        print(f"Elapsed time is {end - start:.3f} seconds\n")

    results = make_results(controls.procedure, output_results, bayes_results)

    # Update parameter values in project
    for class_list in ratapi.project.parameter_class_lists:
        for index, value in enumerate(getattr(problem_definition, parameter_field[class_list])):
            getattr(project, class_list)[index].value = value

    controls.delete_IPC()

    if display_on:
        print("Finished RAT " + horizontal_line)

    return project, results
