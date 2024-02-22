import faulthandler
import numpy as np
from alloyDomains import  alloyDomains
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

    # control.procedure = 'simplex'
    # control.tolX = 1e-6    
    # control.tolFun = 1e-6
    # control.maxFunEvals = 10000
    # control.maxIter = 1
    # control.updateFreq = -1
    # control.updatePlotFreq = 1

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
    control.checks.fitBackgroundParam = [0]
    control.checks.fitQzshift = [0]
    control.checks.fitScalefactor = [0]
    control.checks.fitBulkIn = [0]
    control.checks.fitBulkOut = [0]
    control.checks.fitResolutionParam = [0]
    
    #------------------------------------------------------------------------------------
    # ProblemDef
    problem.contrastBackgrounds = [1]
    problem.contrastBackgroundsType = [1]
    problem.TF = 'domains'
    problem.resample = [0]
    problem.dataPresent = [0]
    problem.oilChiDataPresent = [0]
    problem.numberOfContrasts = 1
    problem.geometry = 'substrate/liquid'
    problem.useImaginary = False
    problem.contrastQzshifts = [1]
    problem.contrastScalefactors = [1]
    problem.contrastBulkIns = [1]
    problem.contrastBulkOuts = [1]
    problem.contrastResolutions = [1]
    problem.backgroundParams = [1.0e-06]
    problem.qzshifts = [0]
    problem.scalefactors = [0.23]
    problem.bulkIn = [2.073e-06]
    problem.bulkOut = [6.35e-06]
    problem.resolutionParams = [0.0300]
    problem.params = [3, 150, 1.1e-05, 7e-06, 7, 150, 4.5e-06, 7]
    problem.numberOfLayers = 0
    problem.modelType = 'custom layers'
    problem.contrastCustomFiles = [1]
    problem.contrastDomainRatios = [1]
    problem.domainRatio = [0.5]
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [3, 150, 1.1e-05, 7.0e-06, 7, 150, 4.5e-06, 7]
    problem.otherParams = [1.0e-06, 0.23, 0, 2.073e-06, 6.35e-06, 0.03, 0.5]
    problem.fitLimits = [[1, 5], [100, 200], [9.0e-06, 1.3e-05], [5.0e-06, 1.0e-05], [5, 11], [100, 200], [4.0e-06, 5.0e-06], [5, 11]]
    problem.otherLimits = [[1.0e-07, 1.0e-05], [0.02, 0.25], [-0.0001,	0.0001], [0, 0], [6.2e-06, 6.35e-06], [0.01, 0.05], [0.4, 0.6]]

    #------------------------------------------------------------------------------------
    # Limits
    limits.param = [[1, 5], [100, 200], [9e-06, 1.3e-05],  [5e-06, 1e-05], 
                    [5, 11], [100, 200], [4e-06, 5e-06], [5, 11]]
    limits.backgroundParam = [[1.0000e-07, 1.0000e-05]]
    limits.scalefactor = [[0.02, 0.25]]
    limits.qzshift = [[-0.0001,	0.0001]]
    limits.bulkIn = [[0, 0]]
    limits.bulkOut = [[6.2000e-06, 6.3500e-06]]
    limits.resolutionParam = [[0.01, 0.05]]
    limits.domainRatio = [[0.4, 0.6]]

    #-------------------------------------------------------------------------------------
    # Cells
    cells.f1 = [[0, 1]]
    cells.f2 = [[]]
    cells.f3 = [[0, 0]]
    cells.f4 = [[0.005, 0.7]]
    cells.f5 = [[0]]
    cells.f6 = [[0]]
    cells.f7 = ['Substrate Roughness', 'Alloy thick', 'Alloy SLD up', 'Alloy SLD dn', 
                'Alloy rough', 'Gold thick', 'Gold SLD', 'Gold Rough']
    cells.f8 = ['Background Param 1']
    cells.f9 = ['Scalefactor 1']
    cells.f10 = ['Qz shift 1']
    cells.f11 = ['Silicon']
    cells.f12 = ['SLD D2O']
    cells.f13 = ['Resolution par 1']

    # dylib_wrapper = DylibWrapper('examples/alloyDomains.dll', 'alloyDomains')
    # cells.f14 = [dylib_wrapper.getHandle()]  # C++ callback
    
    # matlab_wrapper = MatlabWrapper('examples/alloyDomains.m')
    # cells.f14 = [matlab_wrapper.getHandle()]  # Matlab callback
    
    cells.f14 = [alloyDomains]  # Python callback
    
    cells.f15 = ['constant']
    cells.f16 = ['constant']
    cells.f17 = [[]]
    cells.f18 = []
    cells.f19 = []
    cells.f20 = ['Domain Ratio 1']

    #------------------------------------------------------------------------------------
    # Priors
    priors.param = [['Substrate Roughness', 'uniform', 0, np.Inf],
                    ['Alloy thick', 'uniform', 0, np.Inf],
                    ['Alloy SLD up', 'uniform', 0, np.Inf],
                    ['Alloy SLD dn', 'uniform', 0, np.Inf],
                    ['Alloy rough', 'uniform', 0, np.Inf],
                    ['Gold thick', 'uniform', 0, np.Inf],
                    ['Gold SLD', 'uniform', 0, np.Inf],
                    ['Gold Rough', 'uniform', 0, np.Inf]]
    priors.backgroundParam = [['Background Param 1', 'uniform', 0, np.Inf], ['Backs par SMW', 'uniform', 0, np.Inf], ['Backs par H20', 'uniform', 0, np.Inf]]
    priors.scalefactor = [['Scalefactor 1',	'uniform',	0,	np.Inf]]
    priors.qzshift = [['Qz shift 1', 'uniform', 0, np.Inf]]
    priors.bulkIn = [['Silicon', 'uniform', 0, np.Inf]]
    priors.bulkOut = [['SLD D2O', 'uniform', 0, np.Inf]]
    priors.domainRatio = [['Domain Ratio 1', 'uniform', 0, np.Inf]]
    priors.resolutionParam = [['Resolution par 1', 'uniform', 0, np.Inf]]
    priors.priorNames = ['Substrate Roughness', 'Alloy thick', 'Alloy SLD up', 
                         'Alloy SLD dn', 'Alloy rough', 'Gold thick', 
                         'Gold SLD', 'Gold Rough', 'Background Param 1', 
                         'Resolution par 1', 'Silicon', 'SLD D2O', 'Qz shift 1',
                         'Scalefactor 1', 'Domain Ratio 1']         
    priors.priorValues = [[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf]]
    

    import time    
    start = time.perf_counter()
    problem, contrast_params, result, bayes_results = rat_core.RATMain(problem, cells, limits, control, priors)
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
    
