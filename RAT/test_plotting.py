from RAT.rat_core import PlotEventData
from plotting import *
import csv
import numpy as np 
import pyqtgraph as pg
from PyQt6 import QtWidgets
import sys

 
def import_data(filename):
    all_data = []
    for num in ['1', '2', '3']:
        with open(f'RAT/test_data/{filename}_{num}.csv',
                  mode='r',
                  encoding='utf-8-sig') as csvfile:
            data_csv = list(csv.reader(csvfile,
                                       delimiter=',',
                                       quoting=csv.QUOTE_NONNUMERIC))
        all_data.append([np.array(data_csv)])
    return  all_data


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


    app = QApplication(sys.argv)
    app.setWindowIcon(QtGui.QIcon("RAT/RAT-logo.png"))
    window = RATPlot()
    window.update_data(data)
    sys.exit(app.exec())

    # app = QtWidgets.QApplication(sys.argv)
    # app.setWindowIcon(QtGui.QIcon("RAT/RAT-logo.png"))
    # rat = PyQtPlots()
    # rat.update_data(data)
    # sys.exit(app.exec())
    