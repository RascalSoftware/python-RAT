from RAT.rat_core import PlotEventData, makeSLDProfileXY
import pyqtgraph as pg
from PyQt6.QtWidgets import *
from PyQt6.QtGui import *
from PyQt6.QtCore import *
import numpy as np
import plotly
from plotly.subplots import make_subplots

def plot_ref_SLD_helper_plotly(data: PlotEventData, noDelay: bool = True):
    """
    Helper function to make it easier to plot from event.
    Uses the plotly library to plot the reflectivity and the
    SLD profiles.
    """

    # Create the figure with 2 sub plots
    rat_plot = make_subplots(rows=1, cols=2)
 
    for i, (r, sd, sld, layer) in enumerate(zip(data.reflectivity,
                                                data.shiftedData,
                                                data.sldProfiles,
                                                data.allLayers)):

        # Calculate the divisor 
        div = 1 if i == 0 else 2**(4*(i+1))

        # Plot the reflectivity on plot (1,1)
        rat_plot.add_scatter(y=list(np.divide(r[1], div)),
                             x=r[0],
                             row=1,
                             col=1) 

        # Plot the errors on plot (1,1)
        if data.dataPresent[i]:
            rat_plot.add_scatter(x=sd[0],
                        y=np.array(sd[1])/div,
                        error_x=dict(type = 'data',
                                     array = np.array(sd[2])/div,
                                     visible = True),
                        error_y=dict(type = 'data',
                                     array = np.array(sd[2])/div,
                                     visible = True),
                        row=1,
                        col=1) 

        # Plot the scattering lenght densities (slds) on plot (1,2)
        for j in range(1, len(sld)):
            rat_plot.add_scatter(y=sld[j], x=sld[0], row=1, col=2)
        
        if data.resample[i] == 1 or data.modelType == 'custom xy':
            new = makeSLDProfileXY(layer[1][1],
                                   layer[1][-1],
                                   data.ssubs[i],
                                   layer,
                                   len(layer[0]),
                                   1.0)

            rat_plot.add_scatter(y=[row[1] for row in new],
                                 x=[row[0]-49 for row in new],
                                 row=1,
                                 col=2)

    rat_plot.update_yaxes(title_text="reflectivity", type="log", row=1, col=1)
    rat_plot.update_yaxes(title_text="y-axis in logarithmic scale", type="log", row=1, col=1)


    rat_plot.update_layout(showlegend=False)
    rat_plot.show()
 

def plot_ref_SLD_helper_pyqtgraph(data: PlotEventData, noDelay: bool = True):
    """
    Helper function to make it eaier to plot from event
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

    
    rat_plot = plotly.tools.make_subplots(rows=1, cols=2)
 
    plotWidget = pg.plot(title="Reflectivity Algorithms Toolbox (RAT) - plots")
    plotWidget.setLogMode(True, True)
 
    for i, (r, s) in enumerate(zip(data.reflectivity, data.shiftedData)):
        div = 1 if i == 0 else 2**(4*(i+1))
 
        # refplot = plotWidget.AddPlot(r[0], list(np.divide(r[1], div)), symbol='o')

        scatter = plotly.graph_objs.Scatter(
            y=list(np.divide(r[1], div)), 
            x=r[0])
        rat_plot.append_trace(scatter, 1, 1)
        
        
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
