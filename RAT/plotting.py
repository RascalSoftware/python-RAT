"""
Plots using the matplotlib library
"""
import matplotlib.pyplot as plt
import numpy as np
from RAT.rat_core import PlotEventData, makeSLDProfileXY


class RATPlots:
    """
    Creates the RAT reflectivity and Scattering
    Lenght Density (SLD) plots
    """

    def __init__(self, delay: bool = True):
        """
        Initializes the figure and the subplots
        """
        self._fig, (self._ref_plot, self._sld_plot) = \
            plt.subplots(1, 2, num="Reflectivity Algorithms Toolbox (RAT)")
        plt.get_current_fig_manager().set_icon("images/RAT-logo.png")
        plt.show(block=False)
        self._delay = delay
        self._esc_pressed = False
        self._close_clicked = False
        self._fig.canvas.mpl_connect("key_press_event",
                                     self._process_button_press)
        self._fig.canvas.mpl_connect('close_event',
                                     self._close)

    def _plot(self, data: PlotEventData):
        """
        Clears the previous plots and updates the ref and SLD plots.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        # Clears the previous plots
        self._ref_plot.cla()
        self._sld_plot.cla()

        for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                    data.shiftedData,
                                                    data.sldProfiles,
                                                    data.allLayers)):

            r, sd, sld, layer = map(lambda x: x[0], (r, sd, sld, layer))

            # Calculate the divisor
            div = 1 if i == 0 else 2**(4*(i+1))

            # Plot the reflectivity on plot (1,1)
            self._ref_plot.plot(r[:, 0],
                                r[:, 1]/div,
                                label=f'ref {i+1}',
                                linewidth=2)
            self.color = self._ref_plot.get_lines()[-1].get_color()
            if data.dataPresent[i]:
                self._plot_shifted_data(sd, div)

            # Plot the slds on plot (1,2)
            for j in range(1, sld.shape[1]):
                self._sld_plot.plot(sld[:, 0],
                                    sld[:, j],
                                    label=f'sld {i+1}',
                                    color=self.color,
                                    linewidth=2)

            if data.resample[i] == 1 or data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       data.ssubs[i],
                                       layer,
                                       len(layer),
                                       1.0)

                self._sld_plot.plot([row[0]-49 for row in new],
                                    [row[1] for row in new],
                                    color=self.color,
                                    linewidth=1)

        self._format_plots()

        if self._delay:
            plt.pause(0.005)

    def _plot_shifted_data(self, sd, div):
        """
        Plots the shifted data.

        Parameters
        ----------
        sd : nparray
            The shifted data containing the x, y, e data
        div : int
            The divisor for the data
        """
        sd_x = sd[:, 0]
        sd_y, sd_e = map(lambda x: x/div, (sd[:, 1], sd[:, 2]))

        # Plot the errorbars
        indices_removed = np.flip(np.nonzero(sd_y - sd_e < 0)[0])
        sd_x_r, sd_y_r, sd_e_r = map(lambda x:
                                     np.delete(x, indices_removed),
                                     (sd_x, sd_y, sd_e))
        self._plot_errorbars(sd_x_r, sd_y_r, sd_e_r, False)

        # Plot one sided errorbars
        indices_selected = [x for x in indices_removed
                            if x not in np.nonzero(sd_y < 0)[0]]
        sd_x_s, sd_y_s, sd_e_s = map(lambda x:
                                     [x[i] for i in indices_selected],
                                     (sd_x, sd_y, sd_e))
        self._plot_errorbars(sd_x_s, sd_y_s, sd_e_s, True)

    def _plot_errorbars(self, x, y, err, onesided):
        """
        Plots the error bars.

        Parameters
        ----------
        x : nparray
            The shifted data x axis data
        y : nparray
            The shifted data y axis data
        err : nparray
              The shifted data e data
        onesided : bool
            A boolean to indicate whether
            to draw one sided errorbars
        """
        y_error = [[0]*len(err), err] if onesided else err
        self._ref_plot.errorbar(x=x,
                                y=y,
                                yerr=y_error,
                                fmt='none',
                                ecolor=self.color,
                                elinewidth=1,
                                capsize=0)
        self._ref_plot.scatter(x=x,
                               y=y,
                               s=3,
                               marker="o",
                               color=self.color)

    def _format_plots(self):
        """
        Formats the ref and sld subplots.
        """
        self._ref_plot.set_yscale('log')
        self._ref_plot.set_xscale('log')
        self._ref_plot.set_xlabel('Qz')
        self._ref_plot.set_ylabel('Ref')
        self._ref_plot.legend()
        self._ref_plot.grid()

        self._sld_plot.set_xlabel('Z')
        self._sld_plot.set_ylabel('SLD')
        self._sld_plot.legend()
        self._sld_plot.grid()

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
