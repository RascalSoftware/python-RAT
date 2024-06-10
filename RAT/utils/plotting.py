"""
Plots using the matplotlib library
"""
from typing import Optional, Union
import matplotlib.pyplot as plt
import numpy as np
from RAT.rat_core import PlotEventData, makeSLDProfileXY

import RAT
import RAT.inputs
import RAT.outputs


class Figure:
    """
    Creates a plotting figure.
    """

    def __init__(self, row: int = 1, col: int = 1):
        """
        Initializes the figure and the subplots.

        Parameters
        ----------
        row : int, default: 1
              The number of rows in subplot
        col : int, default: 1
              The number of columns in subplot
        """
        self._fig, self._ax = \
            plt.subplots(row, col, num="Reflectivity Algorithms Toolbox (RAT)")
        plt.show(block=False)
        self._esc_pressed = False
        self._close_clicked = False
        self._fig.canvas.mpl_connect("key_press_event",
                                     self._process_button_press)
        self._fig.canvas.mpl_connect('close_event',
                                     self._close)

    def wait_for_close(self):
        """
        Waits for the user to close the figure
        using the esc key.
        """
        while not (self._esc_pressed or self._close_clicked):
            plt.waitforbuttonpress(timeout=0.005)
        plt.close(self._fig)

    def _process_button_press(self, event):
        """
        Process the key_press_event.
        """
        if event.key == 'escape':
            self._esc_pressed = True

    def _close(self, _):
        """
        Process the close_event.
        """
        self._close_clicked = True


def plot_errorbars(ax: 'matplotlib.axes._axes.Axes', x: np.ndarray, y: np.ndarray, err: np.ndarray, 
                   one_sided: bool, color: str):
    """
    Plots the error bars.

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
    y_error = [[0]*len(err), err] if one_sided else err
    ax.errorbar(x=x,
                y=y,
                yerr=y_error,
                fmt='none',
                ecolor=color,
                elinewidth=1,
                capsize=0)
    ax.scatter(x=x, y=y, s=3, marker="o", color=color)


def plot_ref_sld_helper(data: PlotEventData, fig: Optional[Figure] = None, delay: bool = True):
    """
    Clears the previous plots and updates the ref and SLD plots.

    Parameters
    ----------
    data : PlotEventData
           The plot event data that contains all the information
           to generate the ref and sld plots
    fig : Figure, optional 
          The figure class that has two subplots
    delay : bool, default: True
            Controls whether to delay 0.005s after plot is created

    Returns
    -------
    fig : Figure
          The figure class that has two subplots
    """
    if fig is None:
        fig = Figure(1, 2)
    elif fig._ax.shape != (2,):
        fig._fig.clf()
        fig._ax = fig._fig.subplots(1, 2)

    ref_plot = fig._ax[0]
    sld_plot = fig._ax[1]

    # Clears the previous plots
    ref_plot.cla()
    sld_plot.cla()

    for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                data.shiftedData,
                                                data.sldProfiles,
                                                data.resampledLayers)):
        # Calculate the divisor
        div = 1 if i == 0 else 2**(4*(i+1))

        # Plot the reflectivity on plot (1,1)
        ref_plot.plot(r[:, 0],
                      r[:, 1]/div,
                      label=f'ref {i+1}',
                      linewidth=2)
        color = ref_plot.get_lines()[-1].get_color()

        if data.dataPresent[i]:
            sd_x = sd[:, 0]
            sd_y, sd_e = map(lambda x: x/div, (sd[:, 1], sd[:, 2]))

            # Plot the errorbars
            indices_removed = np.flip(np.nonzero(sd_y - sd_e < 0)[0])
            sd_x_r, sd_y_r, sd_e_r = map(lambda x:
                                         np.delete(x, indices_removed),
                                         (sd_x, sd_y, sd_e))
            plot_errorbars(ref_plot, sd_x_r, sd_y_r, sd_e_r, False, color)

            # Plot one sided errorbars
            indices_selected = [x for x in indices_removed
                                if x not in np.nonzero(sd_y < 0)[0]]
            sd_x_s, sd_y_s, sd_e_s = map(lambda x:
                                         [x[i] for i in indices_selected],
                                         (sd_x, sd_y, sd_e))
            plot_errorbars(ref_plot, sd_x_s, sd_y_s, sd_e_s, True, color)

        # Plot the slds on plot (1,2)
        for j in range(len(sld)):
            sld_plot.plot(sld[j][:, 0],
                          sld[j][:, 1],
                          label=f'sld {i+1}',
                          linewidth=1)

        if data.resample[i] == 1 or data.modelType == 'custom xy':
            layers = data.resampledLayers[i][0]
            for j in range(len(data.resampledLayers[i])):
                layer = data.resampledLayers[i][j]
                if layers.shape[1] == 4:
                    layer = np.delete(layer, 2, 1)
                new_profile = makeSLDProfileXY(layers[0, 1],      # Bulk In
                                               layers[-1, 1],     # Bulk Out
                                               data.subRoughs[i], # roughness
                                               layer,
                                               len(layer),
                                               1.0)

                sld_plot.plot([row[0]-49 for row in new_profile],
                              [row[1] for row in new_profile],
                              color=color,
                              linewidth=1)

    # Format the axis
    ref_plot.set_yscale('log')
    ref_plot.set_xscale('log')
    ref_plot.set_xlabel('Qz')
    ref_plot.set_ylabel('Ref')
    ref_plot.legend()
    ref_plot.grid()

    sld_plot.set_xlabel('Z')
    sld_plot.set_ylabel('SLD')
    sld_plot.legend()
    sld_plot.grid()

    if delay:
        plt.pause(0.005)
    
    return fig


def plot_ref_sld(project: RAT.Project, results: Union[RAT.outputs.Results, RAT.outputs.BayesResults], block: bool = False):
    """
    Plots the reflectivity and SLD profiles.

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

    data.reflectivity = results.reflectivity
    data.shiftedData = results.shiftedData
    data.sldProfiles = results.sldProfiles
    data.resampledLayers = results.resampledLayers
    data.dataPresent = RAT.inputs.make_data_present(project)
    data.subRoughs = results.contrastParams.subRoughs
    data.resample = RAT.inputs.make_resample(project)

    figure = Figure(1, 2)

    plot_ref_sld_helper(data, figure)
    if block:
        figure.wait_for_close()
