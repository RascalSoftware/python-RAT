from RAT.rat_core import PlotEventData, makeSLDProfileXY

import pyqtgraph as pg
from PyQt6 import QtWidgets, QtGui, QtCore
from PyQt6.QtCore import pyqtSignal, QObject
from PyQt6.QtWidgets import QApplication, QWidget, QMainWindow, QVBoxLayout
from itertools import cycle
import numpy as np
import sys


class PyQtPlots(QMainWindow):

    def __init__(self):
        """
        Initializes the windows 
        """
        super().__init__()
        self.initialize_ui()
        self.show()
    
    def initialize_ui(self):
        self.setWindowTitle('Reflectivity Algorithms Toolbox')
        self.setWindowIcon(QtGui.QIcon("RAT/RAT-logo.png"))
        self.resize(800,800)
        self.layout = QtWidgets.QVBoxLayout()
        self.setLayout(self.layout)
        self.setStyleSheet("background: white")

        self.ref_plot = pg.PlotWidget(name="Reflectivity")
        self.layout.addWidget(self.ref_plot)
        self.ref_plot.setBackground("w")
        self.ref_plot.setLabel('left', "ref")
        self.ref_plot.setLabel('bottom', "Qz")
        self.ref_plot.addLegend()
        self.ref_plot.setLogMode(x=True, y=True)
        self.ref_plot.showGrid(x=True, y=True)

        self.sld_plot = pg.PlotWidget(name="Scattering Lenght Density")
        self.layout.addWidget(self.sld_plot)
        self.sld_plot.setBackground("w")
        self.sld_plot.setLabel('left', "sld")
        self.sld_plot.setLabel('bottom', "z")
        self.sld_plot.addLegend()
        self.sld_plot.showGrid(x=True, y=True)

    def update_data(self, data: PlotEventData):
        self.data = data
        self.plot()

    def plot(self):
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
        self.ref_plot.clear()
        self.sld_plot.clear()

        self.colours = cycle(['r', 'g', 'b', 'c', 'm', 'y'])

        for i, (r, sd, sld, layer) in enumerate(zip(self.data.reflectivity,
                                                    self.data.shiftedData,
                                                    self.data.sldProfiles,
                                                    self.data.allLayers)):

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
            if self.data.dataPresent[i]:
                self.plot_shifted_data(sd, div)

            # Plot the scattering lenght densities (slds) on plot (1,2)
            for j in range(1, sld.shape[1]):
                self.sld_plot.plot(y=sld[:, j],
                                   x=sld[:, 0],
                                   pen=pg.mkPen(self.colour,
                                                width=4),
                                   name=f'sld {i+1}')

            if self.data.resample[i] == 1 or self.data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       self.data.ssubs[i],
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



class RATPlot(QObject):

    data_updated = pyqtSignal()

    def __init__(self):
        """
        A method that sets up the window and the subplots
        """
        super().__init__()

        self.win = QMainWindow()
        self.win.setWindowTitle('Reflectivity Algorithms Toolbox')
        self.win.resize(800,800)

        self.widget = QWidget()
        self.win.setCentralWidget(self.widget)

        self.layout = QVBoxLayout()
        self.widget.setLayout(self.layout)
        self.widget.setStyleSheet("background: white")

        self.ref_plot = pg.PlotWidget(name="Reflectivity")
        self.layout.addWidget(self.ref_plot)
        self.ref_plot.setBackground("w")
        self.ref_plot.setLabel('left', "ref")
        self.ref_plot.setLabel('bottom', "Qz")
        self.ref_plot.addLegend()
        self.ref_plot.setLogMode(x=True, y=True)
        self.ref_plot.showGrid(x=True, y=True)

        self.sld_plot = pg.PlotWidget(name="Scattering Lenght Density")
        self.layout.addWidget(self.sld_plot)
        self.sld_plot.setBackground("w")
        self.sld_plot.setLabel('left', "sld")
        self.sld_plot.setLabel('bottom', "z")
        self.sld_plot.addLegend()
        self.sld_plot.showGrid(x=True, y=True)

        self.data_updated.connect(self.plot)
    
    def update_data(self, data: PlotEventData):
        self.data = data
        self.data_updated.emit()

    def plot(self):
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
        self.ref_plot.clear()
        self.sld_plot.clear()

        self.colours = cycle(['r', 'g', 'b', 'c', 'm', 'y'])

        for i, (r, sd, sld, layer) in enumerate(zip(self.data.reflectivity,
                                                    self.data.shiftedData,
                                                    self.data.sldProfiles,
                                                    self.data.allLayers)):

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
            if self.data.dataPresent[i]:
                self.plot_shifted_data(sd, div)

            # Plot the scattering lenght densities (slds) on plot (1,2)
            for j in range(1, sld.shape[1]):
                self.sld_plot.plot(y=sld[:, j],
                                   x=sld[:, 0],
                                   pen=pg.mkPen(self.colour,
                                                width=4),
                                   name=f'sld {i+1}')

            if self.data.resample[i] == 1 or self.data.modelType == 'custom xy':
                new = makeSLDProfileXY(layer[0, 1],
                                       layer[-1, 1],
                                       self.data.ssubs[i],
                                       layer,
                                       len(layer),
                                       1.0)

                self.sld_plot.plot(y=[row[1] for row in new],
                                   x=[row[0]-49 for row in new],
                                   pen=pg.mkPen(self.colour),
                                   name=f'resampled sld {i+1}')

        self.win.show()


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
