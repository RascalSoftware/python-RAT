"""Plot results using the matplotlib library."""

import copy
from functools import partial, wraps
from math import ceil, floor, sqrt
from statistics import stdev
from textwrap import fill
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
from RATapi.rat_core import PlotEventData, makeSLDProfile


def plot_errorbars(ax: Axes, x: np.ndarray, y: np.ndarray, err: np.ndarray, one_sided: bool, color: str):
    """Plot the error bars.

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
    linear_x: bool = False,
    q4: bool = False,
    show_error_bar: bool = True,
    show_grid: bool = False,
    show_legend: bool = True,
):
    """Clear the previous plots and updates the ref and SLD plots.

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
    linear_x : bool, default: False
            Controls whether the x-axis on reflectivity plot uses the linear scale
    q4 : bool, default: False
            Controls whether Q^4 is plotted on the reflectivity plot
    show_error_bar : bool, default: True
            Controls whether the error bars are shown
    show_grid : bool, default: False
            Controls whether the grid is shown
    show_legend : bool, default: True
            Controls whether the lengend is shown

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
        div = 1 if i == 0 and not q4 else 2 ** (4 * (i + 1))
        q4_data = 1 if not q4 or not data.dataPresent[i] else sd[:, 0] ** 4
        mult = q4_data / div

        # Plot the reflectivity on plot (1,1)
        ref_plot.plot(r[:, 0], r[:, 1] * mult, label=name, linewidth=2)
        color = ref_plot.get_lines()[-1].get_color()

        # Plot confidence intervals if required
        if confidence_intervals is not None:
            ref_min, ref_max = confidence_intervals["reflectivity"][i]
            mult = (1 if not q4 else r[:, 0] ** 4) / div
            ref_plot.fill_between(r[:, 0], ref_min * mult, ref_max * mult, alpha=0.6, color="grey")

        if data.dataPresent[i]:
            sd_x = sd[:, 0]
            sd_y, sd_e = map(lambda x: x * mult, (sd[:, 1], sd[:, 2]))

            if show_error_bar:
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
            label = name if len(sld) == 1 else f"{name} Domain {j + 1}"
            sld_plot.plot(sld[j][:, 0], sld[j][:, 1], label=label, linewidth=1)

        # Plot confidence intervals if required
        if confidence_intervals is not None:
            sld_min, sld_max = confidence_intervals["sld"][i][j]
            sld_plot.fill_between(sld[j][:, 0], sld_min, sld_max, alpha=0.6, color="grey")

        if data.resample[i] == 1 or data.modelType == "custom xy":
            layers = data.resampledLayers[i][0]
            for j in range(len(data.resampledLayers[i])):
                layer = data.resampledLayers[i][j]
                if layers.shape[1] == 4:
                    layer = np.delete(layer, 2, 1)
                new_profile = makeSLDProfile(
                    layers[0, 1],  # Bulk In
                    layers[-1, 1],  # Bulk Out
                    layer,
                    data.subRoughs[i],  # roughness
                    1,
                )

                sld_plot.plot(
                    [row[0] - 49 for row in new_profile],
                    [row[1] for row in new_profile],
                    color=color,
                    linewidth=1,
                )

    # Format the axis
    ref_plot.set_yscale("log")
    if not linear_x:
        ref_plot.set_xscale("log")
    ref_plot.set_xlabel("$Q_{z} (\u00c5^{-1})$")
    ref_plot.set_ylabel("Reflectivity")

    sld_plot.set_xlabel("$Z (\u00c5)$")
    sld_plot.set_ylabel("$SLD (\u00c5^{-2})$")

    if show_legend:
        ref_plot.legend()
        sld_plot.legend()

    if show_grid:
        ref_plot.grid()
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
    return_fig: bool = False,
    bayes: Literal[65, 95, None] = None,
    linear_x: bool = False,
    q4: bool = False,
    show_error_bar: bool = True,
    show_grid: bool = False,
    show_legend: bool = True,
) -> Union[plt.Figure, None]:
    """Plot the reflectivity and SLD profiles.

    Parameters
    ----------
    project : Project
              An instance of the Project class
    results : Union[Results, BayesResults]
              The result from the calculation
    block : bool, default: False
            Indicates the plot should block until it is closed
    return_fig : bool, default False
        If True, return the figure instead of displaying it.
    bayes : 65, 95 or None, default None
            Whether to shade Bayesian confidence intervals. Can be `None`
            (if no intervals), `65` to show 65% confidence intervals,
            and `95` to show 95% confidence intervals.
    linear_x : bool, default: False
            Controls whether the x-axis on reflectivity plot uses the linear scale
    q4 : bool, default: False
            Controls whether Q^4 is plotted on the reflectivity plot
    show_error_bar : bool, default: True
            Controls whether the error bars are shown
    show_grid : bool, default: False
            Controls whether the grid is shown
    show_legend : bool, default: True
            Controls whether the legend is shown

    Returns
    -------
    Figure or None
        Returns Figure if `return_fig` is True, else returns nothing.

    """
    data = PlotEventData()

    # We need to take a copy of reflectivity and SLD in case we are plotting a
    # shaded plot and will therefore change the plotted data to that from the
    # centre of the Bayesian distribution
    data.modelType = project.model
    data.reflectivity = copy.deepcopy(results.reflectivity)
    data.shiftedData = results.shiftedData
    data.sldProfiles = copy.deepcopy(results.sldProfiles)
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
            else:
                raise ValueError("Parameter `bayes` must be 95, 65 or None")
            confidence_intervals = {
                "reflectivity": [
                    (ref_inter[interval[0]], ref_inter[interval[1]])
                    for ref_inter in results.predictionIntervals.reflectivity
                ],
                "sld": [
                    [(sld_inter[interval[0]], sld_inter[interval[1]]) for sld_inter in sld]
                    for sld in results.predictionIntervals.sld
                ],
            }
            # For a shaded plot, use the mean values from predictionIntervals
            for reflectivity, mean_reflectivity in zip(data.reflectivity, results.predictionIntervals.reflectivity):
                reflectivity[:, 1] = mean_reflectivity[2]
            for sldProfile, mean_sld_profile in zip(data.sldProfiles, results.predictionIntervals.sld):
                for sld, mean_sld in zip(sldProfile, mean_sld_profile):
                    sld[:, 1] = mean_sld[2]
        else:
            raise ValueError(
                "Shaded confidence intervals are only available for the results of Bayesian analysis (NS or DREAM)"
            )
    else:
        confidence_intervals = None

    figure = plt.subplots(1, 2)[0]

    plot_ref_sld_helper(
        data,
        figure,
        confidence_intervals=confidence_intervals,
        linear_x=linear_x,
        q4=q4,
        show_error_bar=show_error_bar,
        show_grid=show_grid,
        show_legend=show_legend,
    )

    if return_fig:
        return figure

    plt.show(block=block)


class LivePlot:
    """Create a plot that gets updates from the plot event during a calculation.

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
        """Close event handler."""
        self.closed = True

    def plotEvent(self, event):
        """Plot the figure from plot event data.

        This is a callback for the plot event.

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


def assert_bayesian(name: str):
    """Ensure the results passed to a function are Bayesian.

    This is a decorator.

    Parameters
    ----------
    name : str
        The name of the plot for the error message.

    """

    def decorator(func: Callable):
        @wraps(func)
        def inner(results, *args, **kwargs):
            if isinstance(results, RATapi.outputs.BayesResults):
                return func(results, *args, **kwargs)
            raise ValueError(f"{name} plots are only available for the results of Bayesian analysis (NS or DREAM)")

        return inner

    return decorator


def name_to_index(param: Union[str, int], names: list[str]):
    """Convert parameter names to indices."""
    if isinstance(param, str):
        if param not in names:
            raise ValueError(f"Parameter {param} is not in this analysis.")
        param = names.index(param)
    elif isinstance(param, int):
        if param > len(names) or param < 0:
            raise IndexError(f"Index {param} has been given, but indices must be between zero and {len(names)}.")
    else:
        raise ValueError(f"Parameters must be given as indices or names, not {type(param)}.")
    return param


@assert_bayesian("Corner")
def plot_corner(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    smooth: bool = True,
    block: bool = False,
    return_fig: bool = False,
    hist_kwargs: Union[dict, None] = None,
    hist2d_kwargs: Union[dict, None] = None,
):
    """Create a corner plot from a Bayesian analysis.

    Parameters
    ----------
    results : BayesResults
        The results from a Bayesian calculation.
    params : list[int or str], default None
        The indices or names of a subset of parameters if required.
        If None, uses all indices.
    smooth : bool, default True
        Whether to apply Gaussian smoothing to the corner plot.
    block : bool, default False
        Whether Python should block until the plot is closed.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    hist_kwargs : dict
        Extra keyword arguments to pass to the 1d histograms.
        Default is {'density': True, 'bins': 25}
    hist2d_kwargs : dict
        Extra keyword arguments to pass to the 2d histograms.
        Default is {'density': True, 'bins': 25}

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    fitname_to_index = partial(name_to_index, names=results.fitNames)

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(fitname_to_index, params))

    # defaults are applied inside each function - just pass blank dicts for now
    if hist_kwargs is None:
        hist_kwargs = {}
    if hist2d_kwargs is None:
        hist2d_kwargs = {}

    num_params = len(params)

    fig, axes = plt.subplots(num_params, num_params, figsize=(2 * num_params, 2 * num_params))
    # i is row, j is column
    for i, row_param in enumerate(params):
        for j, col_param in enumerate(params):
            current_axes: Axes = axes[i][j]
            if i == j:  # diagonal: histograms
                plot_one_hist(results, param=row_param, smooth=smooth, axes=current_axes, **hist_kwargs)
            elif i > j:  # lower triangle: 2d histograms
                plot_contour(
                    results, x_param=col_param, y_param=row_param, smooth=smooth, axes=current_axes, **hist2d_kwargs
                )
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
    if return_fig:
        return fig
    plt.show(block=block)


@assert_bayesian("Histogram")
def plot_one_hist(
    results: RATapi.outputs.BayesResults,
    param: Union[int, str],
    smooth: bool = True,
    sigma: Union[float, None] = None,
    estimated_density: Literal["normal", "lognor", "kernel", None] = None,
    axes: Union[Axes, None] = None,
    block: bool = False,
    return_fig: bool = False,
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
        Whether to apply Gaussian smoothing to the histogram.
        Defaults to True.
    sigma: float or None, default None
        If given, is used as the sigma-parameter for the Gaussian smoothing.
        If None, the default (1/3rd of parameter chain standard deviation) is used.
    estimated_density : 'normal', 'lognor', 'kernel' or None, default None
        If None (default), ignore. Else, add an estimated density
        of the given form on top of the histogram by the following estimations:
        'normal': normal Gaussian.
        'lognor': Log-normal probability density.
        'kernel': kernel density estimation.
    axes: Axes or None, default None
        If provided, plot on the given Axes object.
    block : bool, default False
        Whether Python should block until the plot is closed.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    **hist_settings :
        Settings passed to `np.histogram`. By default, the settings
        passed are `bins = 25` and `density = True`.

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    chain = results.chain
    param = name_to_index(param, results.fitNames)

    if axes is None:
        fig, axes = plt.subplots(1, 1)
    else:
        fig = None

    # apply default settings if not set by user
    default_settings = {"bins": 25, "density": True}
    hist_settings = {**default_settings, **hist_settings}

    parameter_chain = chain[:, param]
    counts, bins = np.histogram(parameter_chain, **hist_settings)
    mean_y = np.mean(parameter_chain)
    sd_y = np.std(parameter_chain)

    if smooth:
        if sigma is None:
            sigma = sd_y / 2
        counts = gaussian_filter1d(counts, sigma)
    axes.hist(
        bins[:-1],
        bins,
        weights=counts,
        edgecolor="black",
        linewidth=1.2,
        color="white",
    )

    axes.set_title(fill(results.fitNames[param], 20))  # use `fill` to wrap long titles

    if estimated_density:
        dx = bins[1] - bins[0]
        if estimated_density == "normal":
            t = np.linspace(mean_y - 3.5 * sd_y, mean_y + 3.5 * sd_y)
            axes.plot(t, norm.pdf(t, loc=mean_y, scale=sd_y**2))
        elif estimated_density == "lognor":
            t = np.linspace(bins[0] - 0.5 * dx, bins[-1] + 2 * dx)
            axes.plot(t, lognorm.pdf(t, np.mean(np.log(parameter_chain)), np.std(np.log(parameter_chain))))
        elif estimated_density == "kernel":
            t = np.linspace(bins[0] - 2 * dx, bins[-1] + 2 * dx, 200)
            kde = gaussian_kde(parameter_chain)
            axes.plot(t, kde.evaluate(t))
        else:
            raise ValueError(
                f"{estimated_density} is not a supported estimated density function."
                " Supported functions are 'normal' 'lognor' or 'kernel'."
            )

    # adding the estimated density extends the figure range - reset it to histogram range
    x_range = hist_settings.get("range", (parameter_chain.min(), parameter_chain.max()))
    axes.set_xlim(x_range)

    if fig is not None:
        if return_fig:
            return fig
        plt.show(block=block)


@assert_bayesian("Contour")
def plot_contour(
    results: RATapi.outputs.BayesResults,
    x_param: Union[int, str],
    y_param: Union[int, str],
    smooth: bool = True,
    sigma: Union[tuple[float], None] = None,
    axes: Union[Axes, None] = None,
    block: bool = False,
    return_fig: bool = False,
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
    sigma : tuple[float] or None, default None
        If given, is used as parameters for Gaussian smoothing in x and y direction respectively.
        If None, defaults to the standard deviation of the parameter chain in either direction.
    axes: Axes or None, default None
        If provided, plot on the given Axes object.
    block : bool, default False
        Whether Python should block until the plot is closed.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    **hist2d_settings:
        Settings passed to `np.histogram2d`.
        Default settings are `bins = 25` and `density = True`.

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    if axes is None:
        fig, axes = plt.subplots(1, 1)
    else:
        fig = None
    x_param = name_to_index(x_param, results.fitNames)
    y_param = name_to_index(y_param, results.fitNames)

    default_settings = {"bins": 25, "density": True}
    hist2d_settings = {**default_settings, **hist2d_settings}

    counts, x_bins, y_bins = np.histogram2d(results.chain[:, x_param], results.chain[:, y_param], **hist2d_settings)
    if smooth:
        if sigma is None:
            sigma_x = stdev(results.chain[:, x_param]) / 2
            sigma_y = stdev(results.chain[:, y_param]) / 2
        else:
            sigma_x, sigma_y = sigma
        # perform a 1d smooth along both axes
        counts = gaussian_filter1d(counts, axis=0, sigma=sigma_x)
        counts = gaussian_filter1d(counts, axis=1, sigma=sigma_y)

    axes.pcolormesh(x_bins, y_bins, counts.max() - counts, cmap=matplotlib.colormaps["Greys"].reversed())
    axes.contour(x_bins[:-1], y_bins[:-1], counts.max() - counts, colors="black")
    axes.set_xlabel(results.fitNames[x_param])
    axes.set_ylabel(results.fitNames[y_param])

    if fig is not None:
        if return_fig:
            return fig
        plt.show(block=block)


def panel_plot_helper(plot_func: Callable, indices: list[int]) -> matplotlib.figure.Figure:
    """Generate a panel-based plot from a single plot function.

    Parameters
    ----------
    plot_func : Callable
        A function which plots one parameter on an Axes object, given its index.
    indices : list[int]
        The list of indices to pass into ``plot_func``.

    Returns
    -------
    matplotlib.figure.Figure
        A figure containing a grid of plots over the indices in `indices`.

    """
    nplots = len(indices)
    nrows, ncols = ceil(sqrt(nplots)), round(sqrt(nplots))
    fig = plt.subplots(nrows, ncols, figsize=(2.5 * ncols, 2 * nrows))[0]
    axs = fig.get_axes()

    for plot_num, index in enumerate(indices):
        plot_func(axs[plot_num], index)

    # blank unused plots
    for i in range(nplots, len(axs)):
        axs[i].set_visible(False)

    fig.tight_layout()
    return fig


@assert_bayesian("Histogram")
def plot_hists(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    smooth: bool = True,
    sigma: Union[float, None] = None,
    estimated_density: Union[
        dict[Literal["normal", "lognor", "kernel", None]], Literal["normal", "lognor", "kernel", None]
    ] = None,
    block: bool = False,
    return_fig: bool = False,
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
    smooth : bool, default True
        Whether to apply a Gaussian smoothing to the histogram.
        Defaults to True.
    sigma: float or None, default None
        If given, is used as the sigma-parameter for the Gaussian smoothing.
        If None, the default (1/3rd of parameter chain standard deviation) is used.
    estimated_density : dict, default None
        If None (default), ignore.
        Can also be a string 'normal', 'lognor' or 'kernel' to apply the same estimated density to all parameters.
        Else, a dictionary where the keys are
        indices or names of parameters, and values denote an estimated density
        of the given form on top of the histogram:
        None : do not plot estimated density for this parameter.
        'normal': normal Gaussian.
        'lognor': Log-normal probability density.
        'kernel': kernel density estimation.
        To provide a default estimated density function to all parameters that haven't been specifically set,
        pass the 'default' key,
        e.g. to apply 'normal' to all unset parameters, set `estimated_density = {'default': 'normal'}`.
    block : bool, default False
        Whether Python should block until the plot is closed.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    hist_settings :
        Settings passed to `np.histogram`. By default, the settings
        passed are `bins = 25` and `density = True`.

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    # first convert names to indices if given
    fitname_to_index = partial(name_to_index, names=results.fitNames)

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(fitname_to_index, params))

    if estimated_density is not None:

        def validate_dens_type(dens_type: Union[str, None], param: str):
            """Check estimated density is a supported type."""
            if dens_type not in [None, "normal", "lognor", "kernel"]:
                raise ValueError(
                    f"Parameter {param} has estimated density function {dens_type},"
                    " which is not supported. Supported estimated density functions"
                    " are 'normal', 'lognor', and 'kernel'."
                )
            return dens_type

        if isinstance(estimated_density, str):
            validate_dens_type(estimated_density, "default")
            estimated_density = {fitname_to_index(param): estimated_density for param in params}
        else:
            default = estimated_density.pop("default", None)
            validate_dens_type(default, "default")
            default_density = {fitname_to_index(param): default for param in params}
            # convert names to indices and ensure density types given are correct
            estimated_density = {
                name_to_index(k, results.fitNames): validate_dens_type(v, k) for k, v in estimated_density.items()
            }
            # merge other estimated densities into default dict
            estimated_density = {**default_density, **estimated_density}
    else:
        estimated_density = {}

    fig = panel_plot_helper(
        lambda ax, i: plot_one_hist(
            results,
            i,
            smooth=smooth,
            sigma=sigma,
            estimated_density=estimated_density.get(i, None),
            axes=ax,
            **hist_settings,
        ),
        params,
    )
    if return_fig:
        return fig
    plt.show(block=block)


@assert_bayesian("Chain")
def plot_chain(
    results: RATapi.outputs.BayesResults,
    params: Union[list[Union[int, str]], None] = None,
    maxpoints: int = 15000,
    block: bool = False,
    return_fig: bool = False,
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
    block : bool, default False
        Whether Python should block until the plot is closed.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    chain = results.chain
    nsimulations, nplots = chain.shape
    skip = floor(nsimulations / maxpoints)  # to evenly distribute points plotted

    # convert names to indices if given
    fitname_to_index = partial(name_to_index, names=results.fitNames)

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(fitname_to_index, params))

    def plot_one_chain(axes: Axes, i: int):
        axes.plot(range(0, nsimulations, skip), chain[:, i][0:nsimulations:skip])
        axes.set_title(results.fitNames[i])

    fig = panel_plot_helper(plot_one_chain, params)
    if return_fig:
        return fig
    plt.show(block=block)


def plot_bayes(project: RATapi.Project, results: RATapi.outputs.BayesResults):
    """Plot the results of a Bayesian analysis with confidence information.

    This produces an unshaded reflectivity/SLD plot, a reflectivity/SLD plot with shaded 95% confidence
    intervals, a grid of histograms giving probability density for each parameter, and a corner plot for
    all parameters.

    Parameters
    ----------
    project : Project
              An instance of the Project class
    results : Union[Results, BayesResults]
              The result from the calculation
    block : bool, default: False
            Indicates the plot should block until it is closed

    """
    if isinstance(results, RATapi.outputs.BayesResults):
        plot_ref_sld(project, results)
        plot_ref_sld(project, results, bayes=95)
        plot_hists(results)
        plot_corner(results)
    else:
        raise ValueError("Bayes plots are only available for the results of Bayesian analysis (NS or DREAM)")
