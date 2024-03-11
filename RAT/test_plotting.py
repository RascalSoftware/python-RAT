from RAT.rat_core import PlotEventData
from plotting import plot_ref_SLD_helper_plotly
import csv

 
def import_data(filename):
    all_data = []
    for num in ['1', '2', '3']:
        with open(f'RAT/test_data/{filename}_{num}.csv',
                  mode='r',
                  encoding='utf-8-sig') as csvfile:
            data_csv = list(csv.reader(csvfile,
                                       delimiter=',',
                                       quoting=csv.QUOTE_NONNUMERIC))
            data = []
            for ix in range(len(data_csv[0])):
                data.append([row[ix] for row in data_csv])
        all_data.append(data)
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

    plot_ref_SLD_helper_plotly(data)