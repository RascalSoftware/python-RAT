from RAT.rat_core import PlotEventData
from RAT.utils.plotting.pyqt_plotting import *
from RAT.utils.plotting.matplotlib_plotting import *
import csv
import numpy as np 
from PyQt6 import QtGui
from PyQt6.QtWidgets import QApplication
import sys
import asyncio
from matplotlib.backends.qt_compat import QtWidgets

 
def import_data(filename):
    all_data = []
    for num in ['1', '2', '3']:
        with open(f'RAT/utils/plotting/test_data/{filename}_{num}.csv',
                  mode='r',
                  encoding='utf-8-sig') as csvfile:
            data_csv = list(csv.reader(csvfile,
                                       delimiter=',',
                                       quoting=csv.QUOTE_NONNUMERIC))
        all_data.append([np.array(data_csv)])
    return  all_data

async def run_app():
    app = QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon("images/RAT-logo.png"))
    window = PyQtPlots()
    window.update_plots(data)
    sys.exit(app.exec())

if __name__=="__main__":
 
    data = PlotEventData()
    data.modelType = 'custom xy'
    data.dataPresent = [1,1,1]
    data.ssubs = [0,0,0]
    data.resample = [0,0,0]
 
    data.allLayers = import_data('all_layers')
    data.reflectivity = import_data('reflectivity')
    data.shiftedData = import_data('shifted_data')
    data.sldProfiles = import_data('sld_profiles')

    # loop = asyncio.new_event_loop()
    # try:
    #     loop.run_until_complete(run_app())
    # finally:
    #     loop.close()
    

    qapp = QtWidgets.QApplication.instance()
    if not qapp:
        qapp = QtWidgets.QApplication(sys.argv)
    qapp.setWindowIcon(QtGui.QIcon("images/RAT-logo.png"))

    mapp = MatplotlibPlots()
    mapp._plot(data)
    mapp.show()
    mapp.activateWindow()
    mapp.raise_()
    qapp.exec()
