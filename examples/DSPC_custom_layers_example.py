import faulthandler
import numpy as np
from customBilayer import customBilayer
from rat import events, rat_core
from rat.misc import DylibWrapper, MatlabWrapper

faulthandler.enable()


if __name__ == '__main__':
    
    control = rat_core.Control()
    problem = rat_core.ProblemDefinition()
    limits = rat_core.Limits()
    cells = rat_core.Cells()
    priors = rat_core.Priors()

    #------------------------------------------------------------------------------------
    # Control
    control.procedure = 'calculate'
    control.parallel = 'single'
    control.display = 'iter'
    control.calcSldDuringFit = False 
    control.resamPars = [0.9000, 50]

    control.procedure = 'simplex'
    control.tolX = 1e-6    
    control.tolFun = 1e-6
    control.maxFunEvals = 10000
    control.maxIter = 1
    control.updateFreq = -1
    control.updatePlotFreq = 1

    # control.procedure = 'dream'
    # control.nSamples = 100     
    # control.nChains = 10      
    # control.jumpProbability = 0.5      
    # control.pUnitGamma = 0.2
    # control.boundHandling = 'fold'     
    # control.adaptPCR = False;
    
    # control.procedure = 'de'
    # control.populationSize = 10
    # control.fWeight = 0.5
    # control.crossoverProbability = 0.8
    # control.strategy = 4
    # control.targetValue = 1
    # control.numGenerations = 500

    # control.procedure = 'ns'
    # control.Nlive = 150
    # control.Nmcmc = 0
    # control.propScale = 0.1
    # control.nsTolerance = 0.1   

    control.checks.fitParam = [1, 1, 1, 1, 1, 1, 1, 1]
    control.checks.fitBackgroundParam = np.ones((3))
    control.checks.fitQzshift = [0]
    control.checks.fitScalefactor = [1]
    control.checks.fitBulkIn = [0]
    control.checks.fitBulkOut = [1, 1, 1]
    control.checks.fitResolutionParam = [0]
    
    #------------------------------------------------------------------------------------
    # ProblemDef
    problem.contrastBackgrounds = [1, 2, 3]
    problem.contrastBackgroundsType = [1, 1, 1]
    problem.TF = 'non polarised'
    problem.resample = [0, 0, 0]
    problem.dataPresent = [1, 1, 1]
    problem.oilChiDataPresent = [0, 0, 0]
    problem.numberOfContrasts = 3
    problem.geometry = 'substrate/liquid'
    problem.useImaginary = False
    problem.contrastQzshifts = [1, 1, 1]
    problem.contrastScalefactors = [1, 1, 1]
    problem.contrastBulkIns = [1, 1, 1]
    problem.contrastBulkOuts = [1, 2, 3]
    problem.contrastResolutions = [1, 1, 1]
    problem.backgroundParams = [1.0e-07, 1.0e-07, 1.0e-07]
    problem.qzshifts = [0]
    problem.scalefactors = [1]
    problem.bulkIn = [2.073e-06]
    problem.bulkOut = [6.35e-06, 2.073e-06, -5.6e-07]
    problem.resolutionParams = [0.0300]
    problem.params = [3, 20, 0.2, 55, 0.2, 0.1, 4, 2]
    problem.numberOfLayers = 0
    problem.modelType = 'custom layers'
    problem.contrastCustomFiles = [1, 1, 1]
    problem.contrastDomainRatios = [0, 0, 0]
    problem.domainRatio = []
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [3, 20, 0.2, 55, 0.2, 0.1, 4, 2, 1.0e-07, 1.0e-07, 1.0e-07, 1, 6.35e-06, 2.073e-06, -5.6e-07]
    problem.otherParams = [0, 2.073e-06, 0.03]
    problem.fitLimits = [[1, 10], [5, 60], [0, 0.5], [45, 65], [0, 0.5], [0, 0.2], [2, 8], [0, 10],
                         [1.0e-10, 1.0e-05], [1.0e-10, 1.0e-05], [1.0e-10, 1.0e-05], [0.5, 2], [5.0e-06, 6.35e-06], 
                         [1.0e-06, 3.0e-06], [-6.0e-07, -3.0e-07]]
    problem.otherLimits = [[-0.0001, 0.0001], [2.07e-06, 2.08e-06], [0.01, 0.05]]

    #------------------------------------------------------------------------------------
    # Limits
    limits.param = [[1, 10], [5, 60], [0, 0.5],  [45, 65], 
                    [0, 0.5], [0, 0.2], [2, 8], [0, 10]]
    limits.backgroundParam = [[1.0000e-10, 1.0000e-05], [1.0000e-10, 1.0000e-05], [1.0000e-10, 1.0000e-05]]
    limits.scalefactor = [[0.500, 2]]
    limits.qzshift = [[-0.0001,	0.0001]]
    limits.bulkIn = [[2.070e-06, 2.080e-06]]
    limits.bulkOut = [[5.0000e-06, 6.3500e-06], [1.0000e-06, 3.0000e-06], [-6.0000e-07, -3.0000e-07]]
    limits.resolutionParam = [[0.0100, 0.0500]]
    limits.domainRatio = []

    #-------------------------------------------------------------------------------------
    # Cells
    cells.f1 = [[0, 1], [0, 1], [0, 1]]
    cells.f2 = [np.loadtxt('data/dspc_custom/data1.csv', delimiter=','), 
                np.loadtxt('data/dspc_custom/data2.csv', delimiter=','), 
                np.loadtxt('data/dspc_custom/data3.csv', delimiter=',')]
    cells.f3 = [[0.0130, 0.370], [0.0130, 0.370], [0.0130, 0.370]]
    cells.f4 = [[0.00571180, 0.396060], [0.00760290, 0.329960],	[0.00633740, 0.330480]]
    cells.f5 = [[0, 0, 0]]
    cells.f6 = [[0]]
    cells.f7 = ['Substrate Roughness', 'Oxide thick', 'Oxide Hydration', 'Lipid APM', 
                'Head Hydration', 'Bilayer Hydration', 'Bilayer Roughness', 'Water Thickness']
    cells.f8 = ['Backs par D2O', 'Backs par SMW', 'Backs par H2O']
    cells.f9 = ['Scalefactor 1']
    cells.f10 = ['Qz shift 1']
    cells.f11 = ['Silicon']
    cells.f12 = ['SLD D2O', 'SLD SMW', 'SLD H2O']
    cells.f13 = ['Resolution par 1']

    # dylib_wrapper = DylibWrapper('examples/customBilayer.dll', 'customBilayer')
    # cells.f14 = [dylib_wrapper.getHandle()]  # C++ callback
    
    # matlab_wrapper = MatlabWrapper('examples/customBilayer.m')
    # cells.f14 = [matlab_wrapper.getHandle()]  # Matlab callback
    
    cells.f14 = [customBilayer]  # Python callback
    
    cells.f15 = ['constant', 'constant', 'constant']
    cells.f16 = ['constant']
    cells.f17 = [[], [], []]
    cells.f18 = []
    cells.f19 = []
    cells.f20 = []

    #------------------------------------------------------------------------------------
    # Priors
    priors.param = [['Substrate Roughness', 'uniform', 0, np.Inf],
                    ['Oxide thick', 'uniform', 0, np.Inf],
                    ['Oxide Hydration', 'uniform', 0, np.Inf],
                    ['Lipid APM', 'uniform', 0, np.Inf],
                    ['Head Hydration', 'uniform', 0, np.Inf],
                    ['Bilayer Hydration', 'uniform', 0, np.Inf],
                    ['Bilayer Roughness', 'uniform', 0, np.Inf],
                    ['Water Thickness', 'uniform', 0, np.Inf]]
    priors.backgroundParam = [['Backs par D2O', 'uniform', 0, np.Inf], ['Backs par SMW', 'uniform', 0, np.Inf], ['Backs par H20', 'uniform', 0, np.Inf]]
    priors.scalefactor = [['Scalefactor 1',	'uniform',	0,	np.Inf]]
    priors.qzshift = [['Qz shift 1', 'uniform', 0, np.Inf]]
    priors.bulkIn = [['Silicon', 'uniform', 0, np.Inf]]
    priors.bulkOut = [['SLD D2O', 'uniform', 0, np.Inf], ['SLD SMW', 'uniform', 0, np.Inf], ['SLD H20', 'uniform', 0, np.Inf]]
    priors.resolutionParam = [['Resolution par 1', 'uniform', 0, np.Inf]]
    priors.priorNames = ['Substrate Roughness', 'Oxide thick', 'Oxide Hydration',
                         'Lipid APM', 'Head Hydration', 'Bilayer Hydration',
                         'Bilayer Roughness', 'Water Thickness', 'Backs par D2O', 'Backs par SMW',
                         'Backs par H2O', 'Resolution par 1', 'Silicon', 'SLD D2O', 'SLD SMW',
                         'SLD H2O', 'Qz shift 1','Scalefactor 1']          
    priors.priorValues = [[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf]]
    

    def fakePlot(event):
        print('Hello plotting')

    import time    
    start = time.perf_counter()
    events.register(events.EventTypes.Plot, fakePlot)
    problem, contrast_params, result, bayes_results = rat_core.RATMain(problem, cells, limits, control, priors)
    events.clear()
    print(time.perf_counter() - start, 'sec')
    # print(contrast_params.ssubs)
    # print(contrast_params.backgroundParams)
    # print(contrast_params.qzshifts)
    # print(contrast_params.scalefactors)
    # print(contrast_params.bulkIn)
    # print(contrast_params.bulkOut)
    # print(contrast_params.resolutionParams)
    print(contrast_params.calculations.allChis)
    print(contrast_params.calculations.sumChi)
    # print(contrast_params.allSubRough)
    # print(contrast_params.resample)
    #print(output)
    
