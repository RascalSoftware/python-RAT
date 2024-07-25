"""Plots using the matplotlib library"""

from functools import wraps
from math import ceil, floor, sqrt
from statistics import mean, stdev
from typing import Callable, Literal, Optional, Union

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.axes._axes import Axes
from scipy.ndimage import gaussian_filter1d
from scipy.stats import gaussian_kde, lognorm, norm

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


def plot_ref_sld_helper(
    data: PlotEventData,
    fig: Optional[matplotlib.pyplot.figure] = None,
    delay: bool = True,
    confidence_intervals: Union[dict, None] = None,
):
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
    confidence_intervals : dict or None, default None
        The Bayesian confidence intervals for reflectivity and SLD.
        Only relevant if the procedure used is Bayesian (NS or DREAM)

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
    fig.subplots_adjust(wspace=0.3)

    ref_plot: plt.Axes = fig.axes[0]
    sld_plot: plt.Axes = fig.axes[1]
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

        # Plot confidence intervals if required
        if confidence_intervals is not None:
            ref_min, ref_max = confidence_intervals["reflectivity"][i]
            ref_x_data = confidence_intervals["reflectivity-x-data"][i][0]
            ref_plot.fill_between(ref_x_data, ref_min / div, ref_max / div, alpha=0.6, color="grey")

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

        # Plot confidence intervals if required
        if confidence_intervals is not None:
            sld_min, sld_max = confidence_intervals["sld"][i][j]
            sld_x_data = confidence_intervals["sld-x-data"][i][j][0]
            sld_plot.fill_between(sld_x_data, sld_min, sld_max, alpha=0.6, color="grey")

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
    bayes: Literal[65, 95, None] = None,
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
    bayes : 65, 95 or None, default None
            Whether to shade Bayesian confidence intervals. Can be `None`
            (if no intervals), `65` to show 65% confidence intervals,
            and `95` to show 95% confidence intervals.

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

    if bayes:
        if isinstance(results, RATapi.outputs.BayesResults):
            # the predictionIntervals data consists of 5 rows:
            # row 0: min with 95% confidence
            # row 1: min with 65% confidence
            # row 2: mean
            # row 3: max with 65% confidence
            # row 4: max with 95% confidence
            if bayes == 95:
                interval = [0, 4]
            elif bayes == 65:
                interval = [1, 3]
            confidence_intervals = {
                "reflectivity": [
                    (ref_inter[interval[0]], ref_inter[interval[1]])
                    for ref_inter in results.predictionIntervals.reflectivity
                ],
                "sld": [
                    [(sld_inter[interval[0]], sld_inter[interval[1]]) for sld_inter in sld]
                    for sld in results.predictionIntervals.sld
                ],
                "reflectivity-x-data": results.predictionIntervals.reflectivityXData,
                "sld-x-data": results.predictionIntervals.sldXData,
            }
        else:
            raise ValueError(
                "Shaded confidence intervals are only available for the results of Bayesian analysis (NS or DREAM)"
            )
    else:
        confidence_intervals = None

    figure = plt.subplots(1, 2)[0]

    plot_ref_sld_helper(data, figure, confidence_intervals=confidence_intervals)

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


def assert_bayesian(name: str, takes_project: bool = False):
    """Decorator to ensure the results passed to a function are Bayesian.

    Parameters
    ----------
    name : str
        The name of the plot for the error message.
    takes_project : bool
        Whether the first variable of the plot is 'project'
        (as in plot_bayes) or 'results' (as in most plots)
    """

    def decorator(func: Callable):
        if takes_project:

            @wraps(func)
            def inner(project, results, *args, **kwargs):
                if isinstance(results, RATapi.outputs.BayesResults):
                    return func(project, results, *args, **kwargs)
                raise ValueError(f"{name} plots are only available for the results of Bayesian analysis (NS or DREAM)")

        @wraps(func)
        def inner(results, *args, **kwargs):
            if isinstance(results, RATapi.outputs.BayesResults):
                return func(results, *args, **kwargs)
            raise ValueError(f"{name} plots are only available for the results of Bayesian analysis (NS or DREAM)")

        return inner

    return decorator


@assert_bayesian("Corner")
def plot_corner(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    smooth: bool = True,
    block: bool = False,
    hist_kwargs: Union[dict, None] = None,
    hist2d_kwargs: Union[dict, None] = None,
):
    """Create a corner plot from a Bayesian analysis.

    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    block : bool, default False
        Whether Python should block until the plot is closed.
    params : list[int or str], default None
        The indices or names of a subset of parameters if required.
        If None, uses all indices.
    smooth : bool, default True
        Whether to apply Gaussian smoothing to the corner plot.
    hist_kwargs : dict
        Extra keyword arguments to pass to the 1d histograms.
        Default is {'density': True, 'bins': 25}
    hist2d_kwargs : dict
        Extra keyword arguments to pass to the 2d histograms.
        Default is {'density': True, 'bins': 25}
    """

    # first convert names to indices if given
    def name_to_index(param: Union[str, int]):
        """Convert parameter names to indices."""
        if isinstance(param, str):
            return results.fitNames.index(param)
        return param

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(name_to_index, params))

    # defaults are applied inside each function - just pass blank dicts for now
    if hist_kwargs is None:
        hist_kwargs = {}
    if hist2d_kwargs is None:
        hist2d_kwargs = {}

    num_params = len(params)

    fig, axes = plt.subplots(num_params, num_params, figsize=(2 * num_params, 2 * num_params))
    # i is row, j is column
    for i in range(0, num_params):
        for j in range(0, num_params):
            current_axes: Axes = axes[i][j]
            if i == j:  # diagonal: histograms
                plot_hist(results, param=j, smooth=smooth, axes=current_axes, **hist_kwargs)
            elif i > j:  # lower triangle: 2d histograms
                plot_contour(results, x_param=i, y_param=j, smooth=smooth, axes=current_axes, **hist2d_kwargs)
            elif i < j:  # upper triangle: no plot
                current_axes.set_visible(False)
            # remove label if on inside of corner plot
            if j != 0:
                current_axes.get_yaxis().set_visible(False)
            if i != len(params) - 1:
                current_axes.get_xaxis().set_visible(False)
            # make labels invisible as titles cover that
            current_axes.set_ylabel("")
            current_axes.set_xlabel("")

    fig.tight_layout()
    fig.show()
    if block:
        fig.wait_for_close()


@assert_bayesian("Histogram")
def plot_hist(
    results: RATapi.outputs.BayesResults,
    param: Union[int, str],
    block: bool = False,
    smooth: bool = True,
    estimated_density: Literal["normal", "lognor", "kernel", None] = None,
    axes: Union[Axes, None] = None,
    **hist_settings,
):
    """Plot the marginalised posterior for a parameter of a Bayesian analysis.

    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    param : Union[int, str]
        Either the index or name of a parameter.
    block : bool, default False
        Whether Python should block until the plot is closed.
    smooth : bool, default True
        Whether to apply a [TODO] smoothing to the histogram.
        Defaults to True.
    estimated_density : 'normal', 'lognor', 'kernel' or None, default None
        If None (default), ignore. Else, add an estimated density
        of the given form on top of the histogram by the following estimations:
        'normal': normal Gaussian.
        'lognor': Log-normal probability density.
        'kernel': kernel density estimation.
    axes: Axes or None, default None
        If provided, plot on the given Axes object.
    **hist_settings :
        Settings passed to `np.histogram`. By default, the settings
        passed are `bins = 25` and `density = True`.
    """
    chain = results.chain
    if isinstance(param, str):
        param = results.fitNames.index(param)

    if axes is None:
        fig, axes = plt.subplots(1, 1)
    else:
        fig = None

    # apply default settings if not set by user
    default_settings = {"bins": 25, "density": True}
    for setting, value in default_settings.items():
        if setting not in hist_settings:
            hist_settings[setting] = value

    parameter_chain = chain[:, param]
    counts, bins = np.histogram(parameter_chain, **hist_settings)
    mean_y = mean(parameter_chain)
    sd_y = stdev(parameter_chain)

    if smooth:
        counts = gaussian_filter1d(counts, sd_y / 6)
    axes.hist(
        bins[:-1],
        bins,
        weights=counts,
        edgecolor="black",
        linewidth=1.2,
        color="white",
    )
    axes.set_title(results.fitNames[param])

    if estimated_density:
        dx = bins[1] - bins[0]
        if estimated_density == "normal":
            t = np.linspace(mean_y - 3.5 * sd_y, mean_y + 3.5 * sd_y)
            axes.plot(t, norm.pdf(t, loc=mean_y, scale=sd_y**2))
        elif estimated_density == "lognor":
            t = np.linspace(bins[0] - 0.5 * dx, bins[-1] + 2 * dx)
            axes.plot(t, lognorm.pdf(t, mean(np.log(parameter_chain)), stdev(np.log(parameter_chain))))
        elif estimated_density == "kernel":
            t = np.linspace(bins[0] - 2 * dx, bins[-1] + 2 * dx, 200)
            kde = gaussian_kde(parameter_chain)
            axes.plot(t, kde.evaluate(t))

    if fig is not None:
        fig.show()
        if block:
            fig.wait_for_close()


@assert_bayesian("Contour")
def plot_contour(
    results: RATapi.outputs.BayesResults,
    x_param: Union[int, str],
    y_param: Union[int, str],
    smooth: bool = True,
    block: bool = False,
    axes: Union[Axes, None] = None,
    **hist2d_settings,
):
    """Plot a 2D histogram of two indexed chain parameters, with contours.

    Parameters
    ----------
    results : RATapi.outputs.BayesResults
        The results of a Bayesian analysis.
    x_param : int
        The index or name of the parameter on the x-axis.
    y_param : int
        The index or name ofthe parameter on the y-axis.
    smooth : bool, default True
        If True, apply Gaussian smoothing to the histogram.
    axes: Axes or None, default None
        If provided, plot on the given Axes object.
    **hist2d_settings:
        Settings passed to `np.histogram2d`.
        Default settings are `bins = 25`.

    """
    if axes is None:
        fig, axes = plt.subplots(1, 1)
    else:
        fig = None
    if isinstance(x_param, str):
        x_param = results.fitNames.index(x_param)
    if isinstance(y_param, str):
        y_param = results.fitNames.index(y_param)

    default_settings = {"bins": 25}
    for setting, value in default_settings.items():
        if setting not in hist2d_settings:
            hist2d_settings[setting] = value

    counts, y_bins, x_bins = np.histogram2d(results.chain[:, x_param], results.chain[:, y_param], **hist2d_settings)
    counts = counts.T  # for some reason the counts given by numpy are sideways
    if smooth:
        # perform a 1d smooth along both axes
        sd_x = stdev(results.chain[:, x_param])
        sd_y = stdev(results.chain[:, y_param])
        counts = gaussian_filter1d(counts, axis=0, sigma=sd_x / 6)
        counts = gaussian_filter1d(counts, axis=1, sigma=sd_y / 6)

    bin_area = (x_bins[1] - x_bins[0]) * (y_bins[1] - y_bins[0])
    counts *= (bin_area * hist2d_settings["bins"]) / np.sum(counts)

    axes.pcolormesh(x_bins, y_bins, counts.max() - counts.T, cmap=matplotlib.colormaps["Greys"].reversed())
    axes.contour(x_bins[:-1], y_bins[:-1], counts.max() - counts.T, colors="black")
    axes.set_xlabel(results.fitNames[x_param])
    axes.set_ylabel(results.fitNames[y_param])

    if fig is not None:
        fig.show()
        if block:
            fig.wait_for_close()


def panel_plot_helper(plot_func: Callable, indices: list[int]) -> matplotlib.figure.Figure:
    """Helper function for panel-based plots.

    Parameters
    ----------
    plot_func : Callable
        A function which plots one parameter on an Axes object, given its index.

    Returns
    -------
    matplotlib.figure.Figure
        A figure containing a grid of plots over the indices in `indices`.
    """
    nplots = len(indices)
    nrows, ncols = ceil(sqrt(nplots)), floor(sqrt(nplots))
    fig = plt.subplots(nrows, ncols, figsize=(2 * nrows, 2.5 * ncols))[0]
    axs = fig.get_axes()

    for plot_num, index in enumerate(indices):
        plot_func(axs[plot_num], index)

    # blank unused plots
    for i in range(nplots, len(axs)):
        axs[i].set_visible(False)

    fig.tight_layout()
    return fig


@assert_bayesian("Histogram")
def plot_hist_panel(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    block: bool = False,
    smooth: bool = True,
    estimated_density: dict[Literal["normal", "lognor", "kernel", None]] = None,
    **hist_settings,
):
    """Plot marginalised posteriors for several parameters from a Bayesian analysis.

    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    params : list[int], default None
        The indices or names of a subset of parameters if required.
        If None, uses all indices.
    block : bool, default False
        Whether Python should block until the plot is closed.
    smooth : bool, default True
        Whether to apply a [TODO] smoothing to the histogram.
        Defaults to True.
    estimated_density : dict, default None
        If None (default), ignore. Else, a dictionary where the keys are
        indices or names of parameters, and values denote an estimated density
        of the given form on top of the histogram:
        None : do not plot estimated density for this parameter.
        'normal': normal Gaussian.
        'lognor': Log-normal probability density.
        'kernel': kernel density estimation.
    hist_settings :
        Settings passed to `np.histogram`. By default, the settings
        passed are `bins = 25` and `density = True`.

    """

    # first convert names to indices if given
    def name_to_index(param: Union[str, int]):
        """Convert parameter names to indices."""
        if isinstance(param, str):
            return results.fitNames.index(param)
        return param

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(name_to_index, params))

    if estimated_density is not None:
        for key, value in estimated_density.items():
            temp = {}  # as dictionary cannot change size during iteration
            temp[name_to_index(key)] = value
        estimated_density = temp
    else:
        estimated_density = {}

    fig = panel_plot_helper(
        lambda ax, i: plot_hist(
            results, i, smooth=smooth, estimated_density=estimated_density.get(i, None), axes=ax, **hist_settings
        ),
        params,
    )
    fig.show()
    if block:
        fig.wait_for_close()


@assert_bayesian("Chain")
def plot_chain_panel(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    maxpoints: int = 15000,
    block: bool = False,
):
    """Plot the MCMC chain for each parameter of a Bayesian analysis.

    Parameters
    ----------
    results : RATapi.outputs.BayesResults
        The results of a Bayesian analysis.
    params : list[int], default None
        The indices or names of a subset of parameters if required.
        If None, uses all indices.
    maxpoints : int
        The maximum number of points to plot for each parameter.

    """
    chain = results.chain
    nsimulations, nplots = chain.shape
    skip = floor(nsimulations / maxpoints)  # to evenly distribute points plotted

    # convert names to indices if given
    def name_to_index(param: Union[str, int]):
        """Convert parameter names to indices."""
        if isinstance(param, str):
            return results.fitNames.index(param)
        return param

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(name_to_index, params))

    def plot_one_chain(axes: Axes, i: int):
        axes.plot(range(0, nsimulations, skip), chain[:, i][0:nsimulations:skip])
        axes.set_title(results.fitNames[i])

    fig = panel_plot_helper(plot_one_chain, params)
    fig.show()
    if block:
        fig.wait_for_close()


@assert_bayesian(name="Bayes", takes_project=True)
def plot_bayes(project: RATapi.Project, results: RATapi.outputs.BayesResults):
    """Plot the results of a Bayesian analysis with confidence information.

    This produces an unshaded reflectivity/SLD plot, a reflectivity/SLD plot with shaded 95% confidence
    intervals, a grid of histograms giving probability density for each parameter, and a corner plot for
    all parameters.

    Parameters
    project : Project
              An instance of the Project class
    results : Union[Results, BayesResults]
              The result from the calculation
    block : bool, default: False
            Indicates the plot should block until it is closed

    """
    plot_ref_sld(project, results)
    plot_ref_sld(project, results, bayes=95)
    plot_hist_panel(results)
    plot_corner(results)
