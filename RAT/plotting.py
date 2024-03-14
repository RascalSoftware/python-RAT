from RAT.rat_core import PlotEventData, makeSLDProfileXY

import pyqtgraph as pg

from plotly.subplots import make_subplots
import plotly.graph_objects as go

import matplotlib.pyplot as plt
from matplotlib.pyplot import draw, show
import numpy as np


def plot_ref_SLD_helper_plotly(data: PlotEventData):
    """
    Helper function to make it easier to plot from event.
    Uses the plotly library to plot the reflectivity and the
    SLD profiles.

    Parameters
    ----------
    data : PlotEventData
        The plot event data that contains all the information
        to generate the ref and sld plots.
    """

    # Create the figure with 2 sub plots
    rat_plot = go.FigureWidget(
        make_subplots(
            rows=1,
            cols=2,
            subplot_titles=(
                "Reflectivity Plot",
                "Scattering Lenght Density Plot")))
 
    for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                data.shiftedData,
                                                data.sldProfiles,
                                                data.allLayers)):

        # Calculate the divisor 
        div = 1 if i == 0 else 2**(4*(i+1))

        # Plot the reflectivity on plot (1,1)
        rat_plot.add_scatter(y=r[1]/div,
                             x=r[0],
                             row=1,
                             col=1,
                             name=f'ref {i+1}',
                             legendgroup = '1')
        

        # Plot the errors on plot (1,1)
        if data.dataPresent[i]:

            sd_x = sd[0]
            sd_y, sd_e = map(lambda x: x/div, (sd[1], sd[2]))

            # Remove values where data - error will be negative
            indices_to_remove = np.flip(np.nonzero(0 > sd_y - sd_e)[0])
            sd_x, sd_y, sd_e = map(lambda x: np.delete(x, indices_to_remove),
                                   (sd_x, sd_y, sd_e))

            rat_plot.add_trace(go.Scatter(x=sd_x,
                                          y=sd_y,
                                          mode='markers',
                                          error_y=dict(
                                          type='data',
                                          array = sd_e),
                                          showlegend=False))

        # Plot the scattering lenght densities (slds) on plot (1,2)
        for j in range(1, sld.shape[0]):
            rat_plot.add_scatter(y=sld[j],
                                 x=sld[0],
                                 row=1,
                                 col=2,
                                 name=f'sld {i+1}',
                                 legendgroup = '2')
        
        if data.resample[i] == 1 or data.modelType == 'custom xy':
            new_layer = [[a, b, c] for a, b, c in zip(layer[0],
                                                      layer[1],
                                                      layer[2])]
            new = makeSLDProfileXY(layer[1][1],
                                   layer[1][-1],
                                   data.ssubs[i],
                                   new_layer,
                                   len(layer[0]),
                                   1.0)

            rat_plot.add_scatter(y=[row[1] for row in new],
                                 x=[row[0]-49 for row in new],
                                 row=1,
                                 col=2,
                                 showlegend=False)

    # Convert the axis to log
    rat_plot.update_yaxes(title_text="ref", type="log", row=1, col=1)
    rat_plot.update_xaxes(title_text="Qz", type="log", row=1, col=1)

    # Label the axis and disable legend
    rat_plot.update_yaxes(title_text="SLD", row=1, col=2)
    rat_plot.update_xaxes(title_text="Z", row=1, col=2)
    rat_plot.update_layout(showlegend=True)

    # Show plot
    rat_plot.show()


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
    _, (ref_plot, sld_plot) = plt.subplots(1, 2)
    draw()

    for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                data.shiftedData,
                                                data.sldProfiles,
                                                data.allLayers)):

        # Calculate the divisor 
        div = 1 if i == 0 else 2**(4*(i+1))

        # Plot the reflectivity on plot (1,1)
        ref_plot.plot(r[0],
                      r[1]/div,
                      label=f'ref {i+1}',
                      linewidth=2)       

        # Plot the errors on plot (1,1)
        if data.dataPresent[i]:

            sd_x = sd[0]
            sd_y, sd_e = map(lambda x: x/div, (sd[1], sd[2]))

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
        for j in range(1, sld.shape[0]):
            sld_plot.plot(sld[0],
                          sld[j],
                          label=f'sld {i+1}')
        
        if data.resample[i] == 1 or data.modelType == 'custom xy':
            new_layer = [[a, b, c] for a, b, c in zip(layer[0],
                                                      layer[1],
                                                      layer[2])]
            new = makeSLDProfileXY(layer[1][1],
                                   layer[1][-1],
                                   data.ssubs[i],
                                   new_layer,
                                   len(layer[0]),
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
    show()


def plot_ref_SLD_helper_pyqtgraph(data: PlotEventData):
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
 
    # Plot the reflectivity
 
    # App = QApplication(sys.argv)
    # widget = QWidget()
    # label = QLabel("Reflectivity Algorithms Toolbox (RAT) - plots")
    # label.setWordWrap(True)
    # pg.setConfigOptions(antialias=True)
    # layout = QGridLayout()
    # label.setMinimumWidth(130)
    # widget.setLayout(layout)
    # layout.addWidget(label, 1, 0)

    
    rat_plot = pg.tools.make_subplots(rows=1, cols=2)
 
    plotWidget = pg.plot(title="Reflectivity Algorithms Toolbox (RAT) - plots")
    plotWidget.setLogMode(True, True)
 
    for i, (r, s) in enumerate(zip(data.reflectivity, data.shiftedData)):
        div = 1 if i == 0 else 2**(4*(i+1))
 
        # refplot = plotWidget.AddPlot(r[0], list(np.divide(r[1], div)), symbol='o')

        # scatter = plotly.graph_objs.Scatter(
        #     y=r[1]/div, 
        #     x=r[0])
        # rat_plot.append_trace(scatter, 1, 1)
        
        
        # if data.dataPresent[i]:
        #     error = pg.ErrorBarItem(x=np.array(s[0]),
        #                             y=np.array(s[1])/div,
        #                             top=np.array(s[2])/div,
        #                             bottom=np.array(s[2])/div,
        #                             beam=0.01)
        #     refplot.addItem(error)
    
 
        # setting this widget as central widget of the main window
        # App.setCentralWidget(widget)
        
        
 
        # If there is data present
        # plot it - size of shiftedData
        # will be [n x 3] if so
        # if data_present[i]:
                # err = pg.ErrorBarItem(x=s[:,0], y=s[:,1]/mult, top=top, bottom=bottom, beam=0.5)
                # ref_plt.addItem(err)
        #     errorbar(thisData[:,0],thisData[:,1]/mult,thisData[:,2]/mult,'.','MarkerSize',2.5);
        # end
    
    rat_plot.update_xaxes(title_text="reflectivity", type="log", row=1, col=1)
    rat_plot.update_yaxes(title_text="y-axis in logarithmic scale", type="log", row=1, col=1)
    rat_plot.update_layout(showlegend=False)
    rat_plot.show()
    # pg.exec()
