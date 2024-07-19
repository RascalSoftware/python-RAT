"""Plots using the matplotlib library"""

from math import ceil, floor, sqrt
from typing import Optional, Union

import corner
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.axes._axes import Axes

import RATapi
import RATapi.inputs
import RATapi.outputs
from RATapi.rat_core import PlotEventData, makeSLDProfileXY


def plot_errorbars(ax: Axes, x: np.ndarray, y: np.ndarray, err: np.ndarray, one_sided: bool, color: str):
    """Plots the error bars.

    Parameters
    ----------
    ax : matplotlib.axes._axes.Axes
            The axis on which to draw errorbars
    x : np.ndarray
        The shifted data x axis data
    y : np.ndarray
        The shifted data y axis data
    err : np.ndarray
            The shifted data e data
    one_sided : bool
        A boolean to indicate whether to draw one sided errorbars
    color : str
            The hex representing the color of the errorbars

    """
    y_error = [[0] * len(err), err] if one_sided else err
    ax.errorbar(x=x, y=y, yerr=y_error, fmt="none", ecolor=color, elinewidth=1, capsize=0)
    ax.scatter(x=x, y=y, s=3, marker="o", color=color)


def plot_ref_sld_helper(data: PlotEventData, fig: Optional[matplotlib.pyplot.figure] = None, delay: bool = True):
    """Clears the previous plots and updates the ref and SLD plots.

    Parameters
    ----------
    data : PlotEventData
           The plot event data that contains all the information
           to generate the ref and sld plots
    fig : matplotlib.pyplot.figure, optional
          The figure class that has two subplots
    delay : bool, default: True
            Controls whether to delay 0.005s after plot is created

    Returns
    -------
    fig : matplotlib.pyplot.figure
          The figure class that has two subplots

    """
    preserve_zoom = False

    if fig is None:
        fig = plt.subplots(1, 2)[0]
    elif len(fig.axes) != 2:
        fig.clf()
        fig.subplots(1, 2)

    ref_plot = fig.axes[0]
    sld_plot = fig.axes[1]
    if ref_plot.lines and fig.canvas.toolbar is not None:
        preserve_zoom = True
        fig.canvas.toolbar.push_current()

    # Clears the previous plots
    ref_plot.cla()
    sld_plot.cla()

    for i, (r, sd, sld, name) in enumerate(
        zip(data.reflectivity, data.shiftedData, data.sldProfiles, data.contrastNames),
    ):
        # Calculate the divisor
        div = 1 if i == 0 else 2 ** (4 * (i + 1))

        # Plot the reflectivity on plot (1,1)
        ref_plot.plot(r[:, 0], r[:, 1] / div, label=name, linewidth=2)
        color = ref_plot.get_lines()[-1].get_color()

        if data.dataPresent[i]:
            sd_x = sd[:, 0]
            sd_y, sd_e = map(lambda x: x / div, (sd[:, 1], sd[:, 2]))

            # Plot the errorbars
            indices_removed = np.flip(np.nonzero(sd_y - sd_e < 0)[0])
            sd_x_r, sd_y_r, sd_e_r = map(lambda x: np.delete(x, indices_removed), (sd_x, sd_y, sd_e))
            plot_errorbars(ref_plot, sd_x_r, sd_y_r, sd_e_r, False, color)

            # Plot one sided errorbars
            indices_selected = [x for x in indices_removed if x not in np.nonzero(sd_y < 0)[0]]
            sd_x_s, sd_y_s, sd_e_s = map(lambda x: [x[i] for i in indices_selected], (sd_x, sd_y, sd_e))
            plot_errorbars(ref_plot, sd_x_s, sd_y_s, sd_e_s, True, color)

        # Plot the slds on plot (1,2)
        for j in range(len(sld)):
            label = name if len(sld) == 1 else f"{name} Domain {j+1}"
            sld_plot.plot(sld[j][:, 0], sld[j][:, 1], label=label, linewidth=1)

        if data.resample[i] == 1 or data.modelType == "custom xy":
            layers = data.resampledLayers[i][0]
            for j in range(len(data.resampledLayers[i])):
                layer = data.resampledLayers[i][j]
                if layers.shape[1] == 4:
                    layer = np.delete(layer, 2, 1)
                new_profile = makeSLDProfileXY(
                    layers[0, 1],  # Bulk In
                    layers[-1, 1],  # Bulk Out
                    data.subRoughs[i],  # roughness
                    layer,
                    len(layer),
                    1.0,
                )

                sld_plot.plot(
                    [row[0] - 49 for row in new_profile],
                    [row[1] for row in new_profile],
                    color=color,
                    linewidth=1,
                )

    # Format the axis
    ref_plot.set_yscale("log")
    ref_plot.set_xscale("log")
    ref_plot.set_xlabel("$Q_{z} (\u00c5^{-1})$")
    ref_plot.set_ylabel("Reflectivity")
    ref_plot.legend()
    ref_plot.grid()

    sld_plot.set_xlabel("$Z (\u00c5)$")
    sld_plot.set_ylabel("$SLD (\u00c5^{-2})$")
    sld_plot.legend()
    sld_plot.grid()

    if preserve_zoom:
        fig.canvas.toolbar.back()
    if delay:
        plt.pause(0.005)

    return fig


def plot_ref_sld(
    project: RATapi.Project,
    results: Union[RATapi.outputs.Results, RATapi.outputs.BayesResults],
    block: bool = False,
):
    """Plots the reflectivity and SLD profiles.

    Parameters
    ----------
    project : Project
              An instance of the Project class
    results : Union[Results, BayesResults]
              The result from the calculation
    block : bool, default: False
            Indicates the plot should block until it is closed

    """
    data = PlotEventData()

    data.modelType = project.model
    data.reflectivity = results.reflectivity
    data.shiftedData = results.shiftedData
    data.sldProfiles = results.sldProfiles
    data.resampledLayers = results.resampledLayers
    data.dataPresent = RATapi.inputs.make_data_present(project)
    data.subRoughs = results.contrastParams.subRoughs
    data.resample = RATapi.inputs.make_resample(project)
    data.contrastNames = [contrast.name for contrast in project.contrasts]

    figure = plt.subplots(1, 2)[0]

    plot_ref_sld_helper(data, figure)

    plt.show(block=block)


class LivePlot:
    """Creates a plot that gets updates from the plot event during a
    calculation

    Parameters
    ----------
    block : bool, default: False
            Indicates the plot should block until it is closed

    """

    def __init__(self, block=False):
        self.block = block
        self.closed = False

    def __enter__(self):
        self.figure = plt.subplots(1, 2)[0]
        self.figure.canvas.mpl_connect("close_event", self._setCloseState)
        self.figure.show()
        RATapi.events.register(RATapi.events.EventTypes.Plot, self.plotEvent)

        return self.figure

    def _setCloseState(self, _):
        """Close event handler"""
        self.closed = True

    def plotEvent(self, event):
        """Callback for the plot event.

        Parameters
        ----------
        event: PlotEventData
            The plot event data.
        """
        if not self.closed and self.figure.number in plt.get_fignums():
            plot_ref_sld_helper(event, self.figure)

    def __exit__(self, _exc_type, _exc_val, _traceback):
        RATapi.events.clear(RATapi.events.EventTypes.Plot, self.plotEvent)
        if not self.closed and self.figure.number in plt.get_fignums():
            plt.show(block=self.block)


def plot_corner(results: RATapi.outputs.BayesResults, block: bool = False):
    """Create a corner plot from a Bayesian analysis.
    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    block : bool, default False
        Whether Python should block until the plot is closed.
    """
    try:
        chain = results.chain
        # wrap the names because otherwise long names poke into the neighbouring box
        fit_names = ["\n".join(fitname.split(" ")) for fitname in results.fitNames]
    except AttributeError as err:
        raise ValueError(
            "Corner plotting is only available for the results of Bayesian analysis (NS or DREAM)"
        ) from err

    fig = corner.corner(chain, titles=fit_names, show_titles=True, title_fmt=None)
    fig.show()
    if block:
        fig.wait_for_close()


def plot_hists(results: RATapi.outputs.BayesResults, block: bool = False):
    """Plot marginalised posteriors from a Bayesian analysis.
    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    block : bool, default False
        Whether Python should block until the plot is closed.
    """
    try:
        _, nplots = results.chain.shape
        fit_names = results.fitNames
    except AttributeError as err:
        raise ValueError(
            "Corner plotting is only available for the results of Bayesian analysis (NS or DREAM)"
        ) from err

    nrows, ncols = ceil(sqrt(nplots)), floor(sqrt(nplots))

    fig = plt.subplots(nrows, ncols, figsize=(2 * nrows, 2.5 * ncols))[0]
    axs = fig.get_axes()
    for i in range(0, nplots):
        counts, bins = np.histogram(results.chain[:, i])
        axs[i].hist(bins[:-1], bins, weights=counts, edgecolor="black", linewidth=1.2, color="white")
        axs[i].set_title(fit_names[i])
    for i in range(nplots, len(axs)):  # blank unused plots
        axs[i].set_visible(False)

    fig.tight_layout()
    fig.show()
    if block:
        fig.wait_for_close()
