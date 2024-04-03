from RAT.rat_core import PlotEventData, makeSLDProfileXY

import pyqtgraph as pg
from itertools import cycle

import matplotlib.pyplot as plt
import numpy as np

def plot_ref_SLD_helper_matplotlib(data: PlotEventData):
    """
    Helper function to make it easier to plot from event.
    Uses the matplotlib library to plot the reflectivity and the
    SLD profiles.

    Parameters
    ----------
    data : PlotEventData
        The plot event data that contains all the information
        to generate the ref and sld plots
    """

    # Create the figure with 2 sub plots
    plt.close('all')
    fig, (ref_plot, sld_plot) = plt.subplots(1, 2)

    for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                data.shiftedData,
                                                data.sldProfiles,
                                                data.allLayers)):

        r, sd, sld, layer = map(lambda x: x[0], (r, sd, sld, layer))

        # Calculate the divisor 
        div = 1 if i == 0 else 2**(4*(i+1))

        # Plot the reflectivity on plot (1,1)
        ref_plot.plot(r[:, 0],
                      r[:, 1]/div,
                      label=f'ref {i+1}',
                      linewidth=2)       

        # Plot the errors on plot (1,1)
        if data.dataPresent[i]:

            sd_x = sd[:, 0]
            sd_y, sd_e = map(lambda x: x/div, (sd[:, 1], sd[:, 2]))

            # Remove values where data - error will be negative
            indices_to_remove = np.flip(np.nonzero(0 > sd_y - sd_e)[0])
            sd_x, sd_y, sd_e = map(lambda x: np.delete(x, indices_to_remove),
                                   (sd_x, sd_y, sd_e))

            ref_plot.errorbar(x=sd_x,
                              y=sd_y,
                              yerr=sd_e,
                              fmt='none',
                              color='red',
                              ecolor='red',
                              elinewidth=1,
                              capsize=2)

        # Plot the scattering lenght densities (slds) on plot (1,2)
        for j in range(1, sld.shape[1]):
            sld_plot.plot(sld[:, 0],
                          sld[:, j],
                          label=f'sld {i+1}')
        
        if data.resample[i] == 1 or data.modelType == 'custom xy':

            new = makeSLDProfileXY(layer[0, 1],
                                   layer[-1, 1],
                                   data.ssubs[i],
                                   layer,
                                   len(layer),
                                   1.0)

            sld_plot.plot([row[0]-49 for row in new],
                          [row[1] for row in new])

    # Convert the axis to log
    ref_plot.set_yscale('log')
    ref_plot.set_xscale('log')
    ref_plot.set_xlabel('Qz')
    ref_plot.set_ylabel('Ref')
    ref_plot.legend()
    ref_plot.grid()

    # Label the axis and disable legend
    sld_plot.set_xlabel('Z')
    sld_plot.set_ylabel('SLD')
    sld_plot.legend()
    sld_plot.grid()

    # Show plot
    fig.show()


class RATPlot:

    def __init__(self):
        """
        A method that sets up the window and the subplots
        """
        self.win = pg.GraphicsLayoutWidget(show=True)
        self.win.setWindowTitle('Reflectivity Algorithms Toolbox (RAT) plots')
        self.win.setBackground("w")

        self.ref_plot = self.win.addPlot(title="Reflectivity")
        self.ref_plot.setLabel('left', "ref")
        self.ref_plot.setLabel('bottom', "Qz")
        self.ref_plot.addLegend()
        self.ref_plot.setLogMode(x=True, y=True)
        self.ref_plot.showGrid(x=True, y=True)

        self.sld_plot = self.win.addPlot(title="Scattering Lenght Density")
        self.sld_plot.setLabel('left', "sld")
        self.sld_plot.setLabel('bottom', "z")
        self.sld_plot.addLegend()
        self.sld_plot.showGrid(x=True, y=True)
        self.colours = cycle(['r', 'g', 'b', 'c', 'm', 'y'])

    def plot(self, data: PlotEventData):
        """
        Helper function to make it easier to plot from event.
        Uses the pyqt library to plot the reflectivity and the
        SLD profiles.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """

        for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                    data.shiftedData,
                                                    data.sldProfiles,
                                                    data.allLayers)):

            r, sd, sld, layer = map(lambda x: x[0], (r, sd, sld, layer))

            self.colour = next(self.colours)

            # Calculate the divisor
            div = 1 if i == 0 else 2**(4*(i+1))

            # Plot the reflectivity on plot (1,1)
            self.ref_plot.plot(y=r[:, 1]/div,
                               x=r[:, 0],
                               pen=pg.mkPen(self.colour,
                                            width=4),
                               name=f'ref {i+1}')

            # Plot the errors on plot (1,1)
            if data.dataPresent[i]:
                self.plot_shifted_data(sd, div)

            # Plot the scattering lenght densities (slds) on plot (1,2)
            for j in range(1, sld.shape[1]):
                self.sld_plot.plot(y=sld[:, j],
                                   x=sld[:, 0],
                                   pen=pg.mkPen(self.colour,
                                                width=4),
                                   name=f'sld {i+1}')

            if data.resample[i] == 1 or data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       data.ssubs[i],
                                       layer,
                                       len(layer),
                                       1.0)

                self.sld_plot.plot(y=[row[1] for row in new],
                                   x=[row[0]-49 for row in new],
                                   pen=pg.mkPen(self.colour),
                                   name=f'resampled sld {i+1}')

    def plot_shifted_data(self, sd, div):
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

        # Remove values where data - error will be negative
        indices_to_remove = np.flip(np.nonzero(0 > sd_y - sd_e)[0])
        sd_x_r, sd_y_r, sd_e_r = map(lambda x: np.delete(x, indices_to_remove),
                                     (sd_x, sd_y, sd_e))
        sd_x_s, sd_y_s, sd_e_s = map(lambda x: [x[i] for i in indices_to_remove],
                                     (sd_x, sd_y, sd_e))

        self.plot_error_bar(sd_x_r, sd_y_r, sd_e_r)
        self.plot_error_bar(sd_x_s, sd_y_s, sd_e_s, onesided=True)

    def plot_error_bar(self, sd_x, sd_y, sd_e, onesided = False):
        """
        Plots the error bars.

        Parameters
        ----------
        sd_x : nparray
             The shifted data x axis data
        sd_y : nparray
             The shifted data y axis data
        sd_e : nparray
             The shifted data e data
        onesided : bool
            The bool to indicate drawing one sided errorbars
        """
        for x, y, e in zip(sd_x, sd_y, sd_e):
            y_error = [y+e, y] if onesided else [y+e, y-e]
            self.ref_plot.plot(y=y_error,
                               x=[x] * 2,
                               pen=pg.mkPen(self.colour))
        self.ref_plot.plot(y=sd_y,
                           x=sd_x,
                           pen=None,
                           symbolBrush=self.colour,
                           symbolPen='w',
                           symbol ='o',
                           symbolSize = 5)
