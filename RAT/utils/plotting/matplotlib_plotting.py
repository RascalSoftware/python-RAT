"""
Plots using the matplotlib library
"""
import numpy as np
from matplotlib.backends.backend_qtagg import FigureCanvas
from matplotlib.backends.backend_qtagg import \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.backends.qt_compat import QtWidgets
from matplotlib.figure import Figure
from RAT.rat_core import PlotEventData, makeSLDProfileXY


class MatplotlibPlots(QtWidgets.QMainWindow):
    """
    Creates the RAT reflectivity and SLD plots
    """

    def __init__(self):
        """
        Initializes the window and the subplots
        """
        super().__init__()

        main = QtWidgets.QWidget()
        self.setCentralWidget(main)
        layout = QtWidgets.QVBoxLayout(main)

        ref_plot = FigureCanvas(Figure())
        layout.addWidget(NavigationToolbar(ref_plot, self))
        layout.addWidget(ref_plot)

        sld_plot = FigureCanvas(Figure())
        layout.addWidget(sld_plot)
        layout.addWidget(NavigationToolbar(sld_plot, self))

        self._ref_plot_ax = ref_plot.figure.subplots()
        self._sld_plot_ax = sld_plot.figure.subplots()

    def _plot(self, data: PlotEventData):
        """
        A method that clears the previous plots and Updates
        the reflectivity and scattering lenght density plots.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        self._ref_plot_ax.cla()
        self._ref_plot_ax.set_yscale('log')
        self._ref_plot_ax.set_xscale('log')
        self._ref_plot_ax.set_xlabel('Qz')
        self._ref_plot_ax.set_ylabel('Ref')
        self._ref_plot_ax.legend()
        self._ref_plot_ax.grid()

        self._sld_plot_ax.cla()
        self._sld_plot_ax.set_xlabel('Z')
        self._sld_plot_ax.set_ylabel('SLD')
        self._sld_plot_ax.legend()
        self._sld_plot_ax.grid()

        for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                    data.shiftedData,
                                                    data.sldProfiles,
                                                    data.allLayers)):

            r, sd, sld, layer = map(lambda x: x[0], (r, sd, sld, layer))

            # Calculate the divisor
            div = 1 if i == 0 else 2**(4*(i+1))

            # Plot the reflectivity on plot (1,1)
            self._ref_plot_ax.plot(r[:, 0],
                                   r[:, 1]/div,
                                   label=f'ref {i+1}',
                                   linewidth=2)

            # Plot the errors on plot (1,1)
            if data.dataPresent[i]:

                sd_x = sd[:, 0]
                sd_y, sd_e = map(lambda x: x/div, (sd[:, 1], sd[:, 2]))

                # Remove values where data - error will be negative
                indices_to_remove = np.flip(np.nonzero(0 > sd_y - sd_e)[0])
                sd_x, sd_y, sd_e = map(lambda x:
                                       np.delete(x, indices_to_remove),
                                       (sd_x, sd_y, sd_e))

                self._ref_plot_ax.errorbar(x=sd_x,
                                           y=sd_y,
                                           yerr=sd_e,
                                           fmt='none',
                                           color='red',
                                           ecolor='red',
                                           elinewidth=1,
                                           capsize=2)
            # Plot the scattering lenght densities (slds) on plot (1,2)
            for j in range(1, sld.shape[1]):
                self._sld_plot_ax.plot(sld[:, 0],
                                       sld[:, j],
                                       label=f'sld {i+1}')

            if data.resample[i] == 1 or data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       data.ssubs[i],
                                       layer,
                                       len(layer),
                                       1.0)

                self._sld_plot_ax.plot([row[0]-49 for row in new],
                                       [row[1] for row in new])
