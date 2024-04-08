"""
Plots using the pyqtgraph library
"""
from itertools import cycle
import pyqtgraph as pg
import numpy as np
from PyQt6.QtCore import pyqtSignal, QObject, pyqtSlot
from PyQt6.QtWidgets import QWidget, QMainWindow, QVBoxLayout
from RAT.rat_core import PlotEventData, makeSLDProfileXY


class PyQtPlots(QObject):
    """
    Creates the RAT reflectivity and SLD plots
    """

    _data_updated = pyqtSignal()

    def __init__(self):
        """
        Initializes the window and the subplots
        """
        super().__init__()

        self._win = QMainWindow()
        self._win.setWindowTitle("Reflectivity Algorithms Toolbox")
        self._win.resize(800, 800)

        widget = QWidget()
        self._win.setCentralWidget(widget)
        layout = QVBoxLayout()
        widget.setLayout(layout)
        widget.setStyleSheet("background: white")

        self._ref_plot = pg.PlotWidget(name="Reflectivity")
        layout.addWidget(self._ref_plot)
        self._ref_plot.setBackground("w")
        self._ref_plot.setLabel('left', "ref")
        self._ref_plot.setLabel('bottom', "Qz")
        self._ref_plot.addLegend()
        self._ref_plot.setLogMode(x=True, y=True)
        self._ref_plot.showGrid(x=True, y=True)

        self._sld_plot = pg.PlotWidget(name="Scattering Lenght Density")
        layout.addWidget(self._sld_plot)
        self._sld_plot.setBackground("w")
        self._sld_plot.setLabel('left', "sld")
        self._sld_plot.setLabel('bottom', "z")
        self._sld_plot.addLegend()
        self._sld_plot.showGrid(x=True, y=True)

        self._data_updated.connect(self._plot)

        self._win.show()

    def update_plots(self, data: PlotEventData):
        """
        A method that clears the previous plots and emits a
        signal when new data becomes avaliable.

        Parameters
        ----------
        data : PlotEventData
            The plot event data that contains all the information
            to generate the ref and sld plots
        """
        self._data = data
        self._ref_plot.clear()
        self._sld_plot.clear()
        self._data_updated.emit()

    @pyqtSlot()
    def _plot(self):
        """
        Slot function to plot from PlotEventData event. Updates
        the reflectivity and scattering lenght density plots.
        """

        self.colours = cycle(['r', 'g', 'b', 'c', 'm', 'y'])

        for i, (r, sd, sld, layer) in enumerate(zip(self._data.reflectivity,
                                                    self._data.shiftedData,
                                                    self._data.sldProfiles,
                                                    self._data.allLayers)):

            r, sd, sld, layer = map(lambda x: x[0], (r, sd, sld, layer))

            self.colour = next(self.colours)

            # Calculate the divisor
            div = 1 if i == 0 else 2**(4*(i+1))

            # Plot the reflectivity on plot (1,1)
            self._ref_plot.plot(y=r[:, 1]/div,
                                x=r[:, 0],
                                pen=pg.mkPen(self.colour,
                                             width=4),
                                name=f'ref {i+1}')

            # Plot the errors on plot (1,1)
            if self._data.dataPresent[i]:
                self._plot_shifted_data(sd, div)

            # Plot the scattering lenght densities (slds) on plot (1,2)
            for j in range(1, sld.shape[1]):
                self._sld_plot.plot(y=sld[:, j],
                                    x=sld[:, 0],
                                    pen=pg.mkPen(self.colour,
                                                 width=4),
                                    name=f'sld {i+1}')

            if self._data.resample[i] == 1 or \
                    self._data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       self._data.ssubs[i],
                                       layer,
                                       len(layer),
                                       1.0)

                self._sld_plot.plot(y=[row[1] for row in new],
                                    x=[row[0]-49 for row in new],
                                    pen=pg.mkPen(self.colour),
                                    name=f'resampled sld {i+1}')

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

        # Remove values where data - error will be negative
        indices_to_remove = np.flip(np.nonzero(0 > sd_y - sd_e)[0])
        sd_x_r, sd_y_r, sd_e_r = map(lambda x:
                                     np.delete(x, indices_to_remove),
                                     (sd_x, sd_y, sd_e))
        sd_x_s, sd_y_s, sd_e_s = map(lambda x:
                                     [x[i] for i in indices_to_remove],
                                     (sd_x, sd_y, sd_e))

        self._plot_error_bar(sd_x_r, sd_y_r, sd_e_r)
        self._plot_error_bar(sd_x_s, sd_y_s, sd_e_s, onesided=True)

    def _plot_error_bar(self, sd_x, sd_y, sd_e, onesided=False):
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
            A boolean to indicate whether to draw one sided errorbars
        """
        for x, y, e in zip(sd_x, sd_y, sd_e):
            y_error = [y+e, y] if onesided else [y+e, y-e]
            self._ref_plot.plot(y=y_error,
                                x=[x] * 2,
                                pen=pg.mkPen(self.colour))
        self._ref_plot.plot(y=sd_y,
                            x=sd_x,
                            pen=None,
                            symbolBrush=self.colour,
                            symbolPen='w',
                            symbol='o',
                            symbolSize=5)
