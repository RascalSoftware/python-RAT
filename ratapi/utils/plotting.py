"""Plot results using the matplotlib library."""

import copy
import types
from collections.abc import Callable
from functools import partial, wraps
from math import ceil, floor, sqrt
from statistics import stdev
from typing import Literal

import matplotlib
import matplotlib.figure
import matplotlib.pyplot as plt
import matplotlib.transforms as mtransforms
import numpy as np
from matplotlib.axes._axes import Axes
from scipy.ndimage import gaussian_filter1d
from scipy.stats import gaussian_kde, lognorm, norm

import ratapi
import ratapi.inputs
import ratapi.outputs
from ratapi.rat_core import PlotEventData, makeSLDProfile


def _extract_plot_data(event_data: PlotEventData, q4: bool, show_error_bar: bool, shift_value: float):
    """Extract the plot data for the sld, ref, error plot lines.

    Parameters
    ----------
    event_data : PlotEventData
        The plot event data that contains all the information
        to generate the ref and sld plots
    q4 : bool, default: False
        Controls whether Q^4 is plotted on the reflectivity plot
    show_error_bar : bool, default: True
        Controls whether the error bars are shown
    shift_value : float
        A value between 1 and 100 that controls the spacing between the reflectivity plots for each of the contrasts

    Returns
    -------
    plot_values : dict
        A dict containing the data for the sld, ref, error plot lines.

    """
    results = {"ref": [], "error": [], "sld": [], "sld_resample": []}

    if shift_value < 1 or shift_value > 100:
        raise ValueError("Parameter `shift_value` must be between 1 and 100")

    for i, (r, data, sld) in enumerate(
        zip(event_data.reflectivity, event_data.shiftedData, event_data.sldProfiles, strict=False)
    ):
        # Calculate the divisor
        div = 1 if i == 0 and not q4 else 10 ** ((i / 100) * shift_value)
        q4_data = 1 if not q4 or not event_data.dataPresent[i] else data[:, 0] ** 4
        mult = q4_data / div

        # Plot the reflectivity on plot (1,1)
        results["ref"].append([r[:, 0], r[:, 1] * mult])

        if event_data.dataPresent[i]:
            sd_x = data[:, 0]
            sd_y, sd_e = map(lambda x: x * mult, (data[:, 1], data[:, 2]))

            if show_error_bar:
                errors = np.zeros(len(sd_e))
                valid = sd_y - sd_e >= 0
                errors[valid] = sd_e[valid]
                valid |= sd_y < 0

                results["error"].append([sd_x[valid], sd_y[valid], sd_e[valid]])

        results["sld"].append([])
        for j in range(len(sld)):
            results["sld"][-1].append([sld[j][:, 0], sld[j][:, 1]])

        if event_data.resample[i] == 1 or event_data.modelType == "custom xy":
            layers = event_data.resampledLayers[i][0]
            results["sld_resample"].append([])
            for j in range(len(event_data.resampledLayers[i])):
                layer = event_data.resampledLayers[i][j]
                if layers.shape[1] == 4:
                    layer = np.delete(layer, 2, 1)
                new_profile = makeSLDProfile(
                    layers[0, 1],  # Bulk In
                    layers[-1, 1],  # Bulk Out
                    layer,
                    event_data.subRoughs[i],  # roughness
                    1,
                )

                results["sld_resample"][-1].append([new_profile[:, 0] - 49, new_profile[:, 1]])

    return results


def plot_ref_sld_helper(
    data: PlotEventData,
    fig: matplotlib.figure.Figure,
    delay: bool = True,
    confidence_intervals: dict | None = None,
    linear_x: bool = False,
    q4: bool = False,
    show_error_bar: bool = True,
    show_grid: bool = False,
    show_legend: bool = True,
    shift_value: float = 100,
    animated=False,
):
    """Clear the previous plots and updates the ref and SLD plots.

    Parameters
    ----------
    data : PlotEventData
        The plot event data that contains all the information
        to generate the ref and sld plots
    fig : matplotlib.figure.Figure
        The figure object that has two subplots
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
        Controls whether the legend is shown
    shift_value : float, default: 100
        A value between 1 and 100 that controls the spacing between the reflectivity plots for each of the contrasts
    animated : bool, default: False
        Controls whether the animated property of foreground plot elements should be set.

    """
    preserve_zoom = False

    if len(fig.axes) != 2:
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

    plot_data = _extract_plot_data(data, q4, show_error_bar, shift_value)
    for i, name in enumerate(data.contrastNames):
        ref_plot.plot(plot_data["ref"][i][0], plot_data["ref"][i][1], label=name, linewidth=1, animated=animated)
        color = ref_plot.get_lines()[-1].get_color()

        # Plot confidence intervals if required
        if confidence_intervals is not None:
            # Calculate the divisor
            div = 1 if i == 0 and not q4 else 10 ** ((i / 100) * shift_value)
            ref_min, ref_max = confidence_intervals["reflectivity"][i]
            mult = (1 if not q4 else plot_data["ref"][i][0] ** 4) / div
            ref_plot.fill_between(plot_data["ref"][i][0], ref_min * mult, ref_max * mult, alpha=0.6, color="grey")

        if data.dataPresent[i] and show_error_bar:
            # Plot the errorbars
            ref_plot.errorbar(
                x=plot_data["error"][i][0],
                y=plot_data["error"][i][1],
                yerr=plot_data["error"][i][2],
                elinewidth=1,
                ecolor=color,
                marker=".",
                markersize=3,
                linestyle="none",
                color=color,
                capsize=0,
                animated=animated,
            )

        # Plot the slds on plot (1,2)
        for j in range(len(plot_data["sld"][i])):
            label = name if len(plot_data["sld"][i]) == 1 else f"{name} Domain {j + 1}"
            sld_plot.plot(
                plot_data["sld"][i][j][0], plot_data["sld"][i][j][1], label=label, linewidth=1, animated=animated
            )

            # Plot confidence intervals if required
            if confidence_intervals is not None:
                sld_min, sld_max = confidence_intervals["sld"][i][j]
                sld_plot.fill_between(plot_data["sld"][i][j][0], sld_min, sld_max, alpha=0.6, color="grey")

        if plot_data["sld_resample"]:
            for j in range(len(plot_data["sld_resample"][i])):
                sld_plot.plot(
                    plot_data["sld_resample"][i][j][0],
                    plot_data["sld_resample"][i][j][1],
                    color=color,
                    linewidth=1,
                    animated=animated,
                )

    # Format the axis
    ref_plot.set_yscale("log")
    if not linear_x:
        ref_plot.set_xscale("log")
    ref_plot.set_xlabel("$Q_{z} (\u00c5^{-1})$")
    ref_plot.set_ylabel("Reflectivity")

    sld_plot.set_xlabel("$Z (\u00c5)$")
    sld_plot.set_ylabel("$SLD (\u00c5^{-2})$", labelpad=1)

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


def plot_ref_sld(
    project: ratapi.Project,
    results: ratapi.outputs.Results | ratapi.outputs.BayesResults,
    block: bool = False,
    fig: matplotlib.figure.Figure | None = None,
    return_fig: bool = False,
    bayes: Literal[65, 95, None] = None,
    linear_x: bool = False,
    q4: bool = False,
    show_error_bar: bool = True,
    show_grid: bool = False,
    show_legend: bool = True,
    shift_value: float = 100,
) -> plt.Figure | None:
    """Plot the reflectivity and SLD profiles.

    Parameters
    ----------
    project : Project
              An instance of the Project class
    results : Union[Results, BayesResults]
              The result from the calculation
    block : bool, default: False
            Indicates the plot should block until it is closed
    fig : matplotlib.figure.Figure, optional
        The figure object that has two subplots
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
    shift_value : float, default: 100
        A value between 1 and 100 that controls the spacing between the reflectivity plots for each of the contrasts

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
    data.dataPresent = ratapi.inputs.make_data_present(project)
    data.subRoughs = results.contrastParams.subRoughs
    data.resample = ratapi.inputs.make_resample(project)
    data.contrastNames = [contrast.name for contrast in project.contrasts]

    if bayes:
        if isinstance(results, ratapi.outputs.BayesResults):
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
            for reflectivity, mean_reflectivity in zip(
                data.reflectivity, results.predictionIntervals.reflectivity, strict=False
            ):
                reflectivity[:, 1] = mean_reflectivity[2]
            for sldProfile, mean_sld_profile in zip(data.sldProfiles, results.predictionIntervals.sld, strict=False):
                for sld, mean_sld in zip(sldProfile, mean_sld_profile, strict=False):
                    sld[:, 1] = mean_sld[2]
        else:
            raise ValueError(
                "Shaded confidence intervals are only available for the results of Bayesian analysis (NS or DREAM)"
            )
    else:
        confidence_intervals = None

    if fig is None:
        fig = plt.subplots(1, 2)[0]
    elif len(fig.axes) != 2:
        fig.clf()
        fig.subplots(1, 2)

    plot_ref_sld_helper(
        data,
        fig,
        confidence_intervals=confidence_intervals,
        linear_x=linear_x,
        q4=q4,
        show_error_bar=show_error_bar,
        show_grid=show_grid,
        show_legend=show_legend,
        shift_value=shift_value,
    )

    if return_fig:
        return fig

    plt.show(block=block)


class BlittingSupport:
    """Create a SLD plot that uses blitting to get faster draws.

    The blit plot stores the background from an
    initial draw then updates the foreground (lines and error bars) if the background is not changed.

    Parameters
    ----------
    data : PlotEventData
        The plot event data that contains all the information
        to generate the ref and sld plots
    fig : matplotlib.figure.Figure, optional
        The figure class that has two subplots
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
    shift_value : float, default: 100
        A value between 1 and 100 that controls the spacing between the reflectivity plots for each of the contrasts
    """

    def __init__(
        self,
        data,
        fig=None,
        linear_x: bool = False,
        q4: bool = False,
        show_error_bar: bool = True,
        show_grid: bool = False,
        show_legend: bool = True,
        shift_value: float = 100,
    ):
        self.figure = fig
        self.linear_x = linear_x
        self.q4 = q4
        self.show_error_bar = show_error_bar
        self.show_grid = show_grid
        self.show_legend = show_legend
        self.shift_value = shift_value
        self.update_plot(data)
        self.event_id = self.figure.canvas.mpl_connect("resize_event", self.resizeEvent)

    def __del__(self):
        self.figure.canvas.mpl_disconnect(self.event_id)

    def resizeEvent(self, _event):
        """Ensure the background is updated after a resize event."""
        self.__background_changed = True

    def update(self, data):
        """Update the foreground, if background has not changed otherwise it updates full plot.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        if self.__background_changed:
            self.update_plot(data)
        else:
            self.update_foreground(data)

    def __setattr__(self, name, value):
        old_value = getattr(self, name, None)
        if value == old_value:
            return

        super().__setattr__(name, value)
        if name in ["figure", "linear_x", "q4", "show_error_bar", "show_grid", "show_legend", "shift_value"]:
            self.__background_changed = True

    def set_animated(self, is_animated: bool):
        """Set the animated property of foreground plot elements.

        Parameters
        ----------
        is_animated : bool
            Indicates if the animated property should be set.
        """
        for line in self.figure.axes[0].lines:
            line.set_animated(is_animated)
        for line in self.figure.axes[1].lines:
            line.set_animated(is_animated)
        for container in self.figure.axes[0].containers:
            container[2][0].set_animated(is_animated)

    def adjust_error_bar(self, error_bar_container, x, y, y_error):
        """Adjust the error bar data.

        Parameters
        ----------
        error_bar_container : Tuple
            Tuple containing the artist of the errorbar i.e. (data line, cap lines, bar lines)
        x : np.ndarray
            The shifted data x axis data
        y : np.ndarray
            The shifted data y axis data
        y_error : np.ndarray
            The shifted data y axis error data
        """
        line, _, (bars_y,) = error_bar_container

        line.set_data(x, y)
        x_base = x
        y_base = y

        y_error_top = y_base + y_error
        y_error_bottom = y_base - y_error

        new_segments_y = [
            np.array([[x, yt], [x, yb]]) for x, yt, yb in zip(x_base, y_error_top, y_error_bottom, strict=False)
        ]
        bars_y.set_segments(new_segments_y)

    def update_plot(self, data):
        """Update the full plot.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        if self.figure is not None:
            self.figure.clf()
        plot_ref_sld_helper(
            data,
            self.figure,
            linear_x=self.linear_x,
            q4=self.q4,
            show_error_bar=self.show_error_bar,
            show_grid=self.show_grid,
            show_legend=self.show_legend,
            animated=True,
        )
        self.figure.tight_layout(pad=1)
        self.figure.canvas.draw()
        self.bg = self.figure.canvas.copy_from_bbox(self.figure.bbox)
        for line in self.figure.axes[0].lines:
            self.figure.axes[0].draw_artist(line)
        for line in self.figure.axes[1].lines:
            self.figure.axes[1].draw_artist(line)
        for container in self.figure.axes[0].containers:
            self.figure.axes[0].draw_artist(container[2][0])
        self.figure.canvas.blit(self.figure.bbox)
        self.set_animated(False)
        self.__background_changed = False

    def update_foreground(self, data):
        """Update the plot foreground only.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        self.set_animated(True)
        self.figure.canvas.restore_region(self.bg)
        plot_data = _extract_plot_data(data, self.q4, self.show_error_bar, self.shift_value)

        offset = 2 if self.show_error_bar else 1
        for i in range(
            0,
            len(self.figure.axes[0].lines),
        ):
            self.figure.axes[0].lines[i].set_data(plot_data["ref"][i // offset][0], plot_data["ref"][i // offset][1])
            self.figure.axes[0].draw_artist(self.figure.axes[0].lines[i])

        i = 0
        for j in range(len(plot_data["sld"])):
            for sld in plot_data["sld"][j]:
                self.figure.axes[1].lines[i].set_data(sld[0], sld[1])
                self.figure.axes[1].draw_artist(self.figure.axes[1].lines[i])
                i += 1

            if plot_data["sld_resample"]:
                for resampled in plot_data["sld_resample"][j]:
                    self.figure.axes[1].lines[i].set_data(resampled[0], resampled[1])
                    self.figure.axes[1].draw_artist(self.figure.axes[1].lines[i])
                    i += 1

        for i, container in enumerate(self.figure.axes[0].containers):
            self.adjust_error_bar(
                container, plot_data["error"][i][0], plot_data["error"][i][1], plot_data["error"][i][2]
            )
            self.figure.axes[0].draw_artist(container[2][0])
            self.figure.axes[0].draw_artist(container[0])

        self.figure.canvas.blit(self.figure.bbox)
        self.figure.canvas.flush_events()
        self.set_animated(False)


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
        self.blit_plot = None

    def __enter__(self):
        self.figure = plt.subplots(1, 2)[0]
        self.figure.canvas.mpl_connect("close_event", self._setCloseState)
        self.figure.show()
        ratapi.events.register(ratapi.events.EventTypes.Plot, self.plotEvent)

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
            if self.blit_plot is None:
                self.blit_plot = BlittingSupport(event, self.figure)
            else:
                self.blit_plot.update(event)

    def __exit__(self, _exc_type, _exc_val, _traceback):
        ratapi.events.clear(ratapi.events.EventTypes.Plot, self.plotEvent)
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
            if isinstance(results, ratapi.outputs.BayesResults):
                return func(results, *args, **kwargs)
            raise ValueError(f"{name} plots are only available for the results of Bayesian analysis (NS or DREAM)")

        return inner

    return decorator


def name_to_index(param: str | int, names: list[str]):
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
    results: ratapi.outputs.BayesResults,
    params: list[int | str] | None = None,
    smooth: bool = True,
    block: bool = False,
    fig: matplotlib.figure.Figure | None = None,
    return_fig: bool = False,
    hist_kwargs: dict | None = None,
    hist2d_kwargs: dict | None = None,
    progress_callback: Callable[[int, int], None] | None = None,
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
    fig : matplotlib.figure.Figure, optional
        The figure object to use for plot.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    hist_kwargs : dict
        Extra keyword arguments to pass to the 1d histograms.
        Default is {'density': True, 'bins': 25}
    hist2d_kwargs : dict
        Extra keyword arguments to pass to the 2d histograms.
        Default is {'density': True, 'bins': 25}
    progress_callback: Union[Callable[[int, int], None], None]
        Callback function for providing progress during plot creation
        First argument is current completed sub plot and second is total number of sub plots

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
    total_count = num_params + (num_params**2 - num_params) // 2

    if fig is None:
        fig, axes = plt.subplots(num_params, num_params, figsize=(11, 10), subplot_kw={"visible": False})
    else:
        fig.clf()
        axes = fig.subplots(num_params, num_params, subplot_kw={"visible": False})

    # i is row, j is column
    current_count = 0
    for i in range(num_params):
        for j in range(i + 1):
            row_param = params[i]
            col_param = params[j]
            current_axes: Axes = axes if isinstance(axes, matplotlib.axes.Axes) else axes[i][j]
            current_axes.tick_params(which="both", labelsize="medium")
            current_axes.xaxis.offsetText.set_fontsize("small")
            current_axes.yaxis.offsetText.set_fontsize("small")
            current_axes.set_visible(True)
            if i == j:  # diagonal: histograms
                plot_one_hist(results, param=row_param, smooth=smooth, axes=current_axes, **hist_kwargs)
            elif i > j:  # lower triangle: 2d histograms
                plot_contour(
                    results, x_param=col_param, y_param=row_param, smooth=smooth, axes=current_axes, **hist2d_kwargs
                )

            # remove label if on inside of corner plot
            if j != 0:
                current_axes.get_yaxis().set_visible(False)
            if i != len(params) - 1:
                current_axes.get_xaxis().set_visible(False)
            # make labels invisible as titles cover that
            current_axes.yaxis._update_offset_text_position = types.MethodType(
                _y_update_offset_text_position, current_axes.yaxis
            )
            current_axes.yaxis.offset_text_position = "center"
            current_axes.set_ylabel("")
            current_axes.set_xlabel("")
            if progress_callback is not None:
                current_count += 1
                progress_callback(current_count, total_count)
    if return_fig:
        return fig
    plt.show(block=block)


@assert_bayesian("Histogram")
def plot_one_hist(
    results: ratapi.outputs.BayesResults,
    param: int | str,
    smooth: bool = True,
    sigma: float | None = None,
    estimated_density: Literal["normal", "lognor", "kernel", None] = None,
    axes: Axes | None = None,
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

    axes.set_title(results.fitNames[param], loc="left", fontsize="medium")

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


def _y_update_offset_text_position(axis, _bboxes, bboxes2):
    """Update the position of the Y axis offset text using the provided bounding boxes.

    Adapted from https://github.com/matplotlib/matplotlib/issues/4476#issuecomment-105627334.

    Parameters
    ----------
    axis : matplotlib.axis.YAxis
        Y axis to update.
    _bboxes : List
        list of bounding boxes
    bboxes2 : List
        list of bounding boxes
    """
    x, y = axis.offsetText.get_position()

    if axis.offset_text_position == "left":
        # y in axes coords, x in display coords
        axis.offsetText.set_transform(
            mtransforms.blended_transform_factory(axis.axes.transAxes, mtransforms.IdentityTransform())
        )

        top = axis.axes.bbox.ymax
        y = top + axis.OFFSETTEXTPAD * axis.figure.dpi / 72.0

    else:
        # x & y in display coords
        axis.offsetText.set_transform(mtransforms.IdentityTransform())

        # Northwest of upper-right corner of right-hand extent of tick labels
        if bboxes2:
            bbox = mtransforms.Bbox.union(bboxes2)
        else:
            bbox = axis.axes.bbox
        center = bbox.ymin + (bbox.ymax - bbox.ymin) / 2
        x = bbox.xmin - axis.OFFSETTEXTPAD * axis.figure.dpi / 72.0
        y = center
    x_offset = 110
    axis.offsetText.set_position((x - x_offset, y))


@assert_bayesian("Contour")
def plot_contour(
    results: ratapi.outputs.BayesResults,
    x_param: int | str,
    y_param: int | str,
    smooth: bool = True,
    sigma: tuple[float] | None = None,
    axes: Axes | None = None,
    block: bool = False,
    return_fig: bool = False,
    **hist2d_settings,
):
    """Plot a 2D histogram of two indexed chain parameters, with contours.

    Parameters
    ----------
    results : ratapi.outputs.BayesResults
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


def panel_plot_helper(
    plot_func: Callable,
    indices: list[int],
    fig: matplotlib.figure.Figure | None = None,
    progress_callback: Callable[[int, int], None] | None = None,
) -> matplotlib.figure.Figure:
    """Generate a panel-based plot from a single plot function.

    Parameters
    ----------
    plot_func : Callable
        A function which plots one parameter on an Axes object, given its index.
    indices : list[int]
        The list of indices to pass into ``plot_func``.
    fig : matplotlib.figure.Figure, optional
        The figure object to use for plot.
    progress_callback: Union[Callable[[int, int], None], None]
        Callback function for providing progress during plot creation
        First argument is current completed sub plot and second is total number of sub plots

    Returns
    -------
    matplotlib.figure.Figure
        A figure containing a grid of plots over the indices in `indices`.

    """
    nplots = len(indices)
    nrows, ncols = ceil(sqrt(nplots)), round(sqrt(nplots))

    if fig is None:
        fig = plt.subplots(nrows, ncols, figsize=(11, 10), subplot_kw={"visible": False})[0]
    else:
        fig.clf()
        fig.subplots(nrows, ncols, subplot_kw={"visible": False})
    axs = fig.get_axes()
    for index, plot_num in enumerate(indices):
        axs[index].tick_params(which="both", labelsize="medium")
        axs[index].xaxis.offsetText.set_fontsize("small")
        axs[index].yaxis.offsetText.set_fontsize("small")
        axs[index].set_visible(True)
        plot_func(axs[index], plot_num)
        if progress_callback is not None:
            progress_callback(index, nplots)

    fig.tight_layout()
    return fig


@assert_bayesian("Histogram")
def plot_hists(
    results: ratapi.outputs.BayesResults,
    params: list[int | str] | None = None,
    smooth: bool = True,
    sigma: float | None = None,
    estimated_density: dict[Literal["normal", "lognor", "kernel", None]]
    | Literal["normal", "lognor", "kernel", None] = None,
    block: bool = False,
    fig: matplotlib.figure.Figure | None = None,
    return_fig: bool = False,
    progress_callback: Callable[[int, int], None] | None = None,
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
    fig : matplotlib.figure.Figure, optional
        The figure object to use for plot.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    progress_callback: Union[Callable[[int, int], None], None]
        Callback function for providing progress during plot creation
        First argument is current completed sub plot and second is total number of sub plots
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

        def validate_dens_type(dens_type: str | None, param: str):
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
            estimated_density=estimated_density.get(i),
            axes=ax,
            **hist_settings,
        ),
        params,
        fig,
        progress_callback,
    )
    if return_fig:
        return fig
    plt.show(block=block)


@assert_bayesian("Chain")
def plot_chain(
    results: ratapi.outputs.BayesResults,
    params: list[int | str] | None = None,
    maxpoints: int = 15000,
    block: bool = False,
    fig: matplotlib.figure.Figure | None = None,
    return_fig: bool = False,
    progress_callback: Callable[[int, int], None] | None = None,
):
    """Plot the MCMC chain for each parameter of a Bayesian analysis.

    Parameters
    ----------
    results : ratapi.outputs.BayesResults
        The results of a Bayesian analysis.
    params : list[int], default None
        The indices or names of a subset of parameters if required.
        If None, uses all indices.
    maxpoints : int
        The maximum number of points to plot for each parameter.
    block : bool, default False
        Whether Python should block until the plot is closed.
    fig : matplotlib.figure.Figure, optional
        The figure object to use for plot.
    return_fig: bool, default False
        If True, return the figure as an object instead of showing it.
    progress_callback: Union[Callable[[int, int], None], None]
        Callback function for providing progress during plot creation
        First argument is current completed sub plot and second is total number of sub plots

    Returns
    -------
    Figure or None
        If `return_fig` is True, return the figure - otherwise, return nothing.

    """
    chain = results.chain
    nsimulations, _ = chain.shape
    # skip is to evenly distribute points plotted
    # all points will be plotted if maxpoints < nsimulations
    skip = max(floor(nsimulations / maxpoints), 1)

    # convert names to indices if given
    fitname_to_index = partial(name_to_index, names=results.fitNames)

    if params is None:
        params = range(0, len(results.fitNames))
    else:
        params = list(map(fitname_to_index, params))

    def plot_one_chain(axes: Axes, i: int):
        axes.plot(range(0, nsimulations, skip), chain[:, i][0:nsimulations:skip])
        axes.set_title(results.fitNames[i], fontsize="small")

    fig = panel_plot_helper(plot_one_chain, params, fig, progress_callback)
    if return_fig:
        return fig
    plt.show(block=block)


def plot_bayes(project: ratapi.Project, results: ratapi.outputs.BayesResults):
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
    if isinstance(results, ratapi.outputs.BayesResults):
        plot_ref_sld(project, results)
        plot_ref_sld(project, results, bayes=95)
        plot_hists(results)
        plot_corner(results)
    else:
        raise ValueError("Bayes plots are only available for the results of Bayesian analysis (NS or DREAM)")
