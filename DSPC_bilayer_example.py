import cppimport
import numpy as np

rat = cppimport.imp("rat")

if __name__ == '__main__':
    
    control = rat.Control()
    problem = rat.ProblemDefinition()
    limits = rat.Limits()
    cells = rat.Cells()
    priors = rat.Priors()

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

    control.checks.fitParam = [1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1]
    control.checks.fitBackgroundParam = np.ones((2))
    control.checks.fitQzshift = [0]
    control.checks.fitScalefactor = [0, 0]
    control.checks.fitBulkIn = [0]
    control.checks.fitBulkOut = [0, 0]
    control.checks.fitResolutionParam = [0]
    
    #------------------------------------------------------------------------------------
    # ProblemDef
    problem.contrastBackgrounds = [1, 2]
    problem.contrastBackgroundsType = [1, 1]
    problem.TF = 'non polarised'
    problem.resample = [0, 0]
    problem.dataPresent = [1, 1]
    problem.oilChiDataPresent = [0, 0]
    problem.numberOfContrasts = 2
    problem.geometry = 'substrate/liquid'
    problem.useImaginary = False
    problem.contrastQzshifts = [1, 1]
    problem.contrastScalefactors = [1, 1]
    problem.contrastBulkIns = [1, 1]
    problem.contrastBulkOuts = [1, 2]
    problem.contrastResolutions = [1, 1]
    problem.backgroundParams = [2.28525214157571e-06, 3.10246417982706e-06]
    problem.qzshifts = [0]
    problem.scalefactors = [0.102860901766649, 0.151378164199993]
    problem.bulkIn = [2.07399989449186e-06]
    problem.bulkOut = [5.98067802097389e-06, 2.21000000000000e-06]
    problem.resolutionParams = [0.0300]
    problem.params = [5.08929897223891, 19.2830495317839, 3.39317612702561e-06, 21.5451088784623, -4.01001212540204e-07, 1.24963548580913, 8.09797576127018, 20.3854719276630, 
                      0, 5.79580504624961, 1.75686012656988e-06, 10.0916971312704, 11.7066921282983, 1.47000000000000e-06, 5.63694566866001, 17.8733566725932, -4.61089672942576e-07, 
                      17.8556570737753, 41.4142113901161, 100, 25.6893163532023]
    problem.numberOfLayers = 6
    problem.modelType = 'standard layers'
    problem.contrastCustomFiles = [np.NAN, np.NaN]
    problem.contrastDomainRatios = [0, 0]
    problem.domainRatio = []
    problem.numberOfDomainContrasts = 0
    problem.fitParams = [5.08929897223891, 19.2830495317839, 21.5451088784623, 1.24963548580913, 8.09797576127018, 
                       20.3854719276630, 5.79580504624961, 10.0916971312704, 11.7066921282983, 5.63694566866001,
                       17.8733566725932, 17.8556570737753, 41.4142113901161, 25.6893163532023, 2.28525214157571e-06,
                       3.10246417982706e-06]
    problem.otherParams = [3.39317612702561e-06, -4.01001212540204e-07, 0, 1.75686012656988e-06, 1.47e-06, -4.61089672942576e-07,
                         100, 0.102860901766649, 0.151378164199993,0, 2.07399989449186e-06, 5.98067802097389e-06, 2.21e-06, 0.030]
    problem.fitLimits = [[2, 8], [5, 60], [15, 35], [1, 50], [1, 15], [10, 28], [5, 17],
                         [10, 50], [7, 17], [2, 15], [14, 22], [10, 50], [10, 50], [0, 60], [5.0e-10, 7.0e-06], 
                         [1.0e-10, 4.99999987368938e-06]]
    problem.otherLimits = [[3.39e-06, 3.41e-06], [-5.0e-07, -3.0e-07], [0, 1.0e-09], [1.0e-07, 2.0e-06], [5.0e-07, 1.5e-06],
                           [-5.0e-07, 0], [99.9, 100], [0.050, 0.20], [0.050, 0.20], [-0.00010, 0.00010], [2.0e-06, 2.1e-06], 
                           [5.50000004295725e-06, 6.4e-06], [2.0e-06, 4.99999987368938e-06], [0.010, 0.050]]

    #------------------------------------------------------------------------------------
    # Limits
    limits.param = [[2, 8], [5, 60], [0, 0],  [15, 35], [-0, -0], [1, 50], [1, 15],
                     [10, 28], [0, 0], [5, 17], [0,  0], [10, 50], [7, 17], [0,  0], [2, 15],
                     [14, 22], [-0, 0], [10, 50], [10, 50], [99.9,100], [0, 60]]
    limits.backgroundParam = [[5.0000e-10, 7.0000e-06], [1.0000e-10, 5.0000e-06]]
    limits.scalefactor = [[0.0500, 0.2000], [0.0500, 0.2000]]
    limits.qzshift = [[-1.0000e-04, 1.0000e-04]]
    limits.bulkIn = [[2.0000e-06, 2.1000e-06]]
    limits.bulkOut = [[5.5000e-06, 6.4000e-06], [2.0000e-06, 5.0000e-06]]
    limits.resolutionParam = [[0.0100, 0.0500]]
    limits.domainRatio = []

    #-------------------------------------------------------------------------------------
    # Cells
    cells.f1 = [[0, 1], [0, 1]]
    cells.f2 = [np.loadtxt('data/data1.csv', delimiter=','), 
                np.loadtxt('data/data2.csv', delimiter=',')]
    cells.f3 = [[0.011403, 0.59342], [0.011403, 0.59342]]
    cells.f4 = [[0.011403, 0.70956], [0.011403, 0.59342]]
    cells.f5 = [[1, 2, 3, 4, 5, 6, 6, 5], [1, 2, 3, 4, 5, 6, 6, 5]]
    cells.f6 = [[2, 3, 1, 21, 2], 
                [4, 5, 7, 6, 2], 
                [10, 11, 7, 12, 2],
                [8, 9, 15, 20, 2], 
                [13, 14, 15, 19, 2], 
                [16, 17, 15, 18, 2]]
    cells.f7 = ['Substrate Roughness', 'Oxide thick', 'Oxide SLD', 'Sam tails thick', 'Sam tails SLD',
                'Sam tails hydration', 'Sam rough', 'cw thick', 'cw SLD', 'SAM head thick', 'SAM head SLD',
                'SAM head hydration', 'Bilayer head thick', 'Bilayer head SLD', 'Bilayer rough', 
                'Bilayer tails thick', 'Bilayer tails SLD', 'Bilayer tails hydr', 'Bilayer heads hydr', 'cw hydration', 'Oxide Hydration']
    cells.f8 = ['Backs parameter 1', 'Backs parameter 2']
    cells.f9 = ['Scalefactor 1', 'Scalefactor 2']
    cells.f10 = ['Qz shift 1']
    cells.f11 = ['Air']
    cells.f12 = ['D2O', 'SMW']
    cells.f13 = ['Resolution par 1']
    # cells.f14 = [['', '', '']]
    cells.f15 = ['constant', 'constant']
    cells.f16 = ['constant']
    # cells.f17 = [[], []]
    # cells.f18 = []
    # cells.f19 = []
    # cells.f20 = []

    #------------------------------------------------------------------------------------
    # Priors
    priors.param = [['Substrate Roughness', 'uniform', 0, np.Inf],
                    ['Oxide thick', 'uniform', 0, np.Inf],
                    ['Oxide SLD', 'uniform', 0, np.Inf],
                    ['Sam tails thick', 'uniform', 0, np.Inf],
                    ['Sam tails SLD', 'uniform', 0, np.Inf],                    
                    ['Sam tails hydra…', 'uniform', 0, np.Inf],
                    ['Sam rough', 'uniform', 0, np.Inf],
                    ['cw thick', 'uniform', 0, np.Inf],
                    ['cw SLD', 'uniform', 0, np.Inf],
                    ['SAM head thick', 'uniform', 0, np.Inf],
                    ['SAM head SLD', 'uniform', 0, np.Inf],
                    ['SAM head hydrat…', 'uniform', 0, np.Inf],
                    ['Bilayer head th…', 'uniform', 0, np.Inf],
                    ['Bilayer head SLD', 'uniform', 0, np.Inf],
                    ['Bilayer rough', 'uniform', 0, np.Inf],
                    ['Bilayer tails t…', 'uniform', 0, np.Inf],
                    ['Bilayer tails SLD', 'uniform', 0, np.Inf],
                    ['Bilayer tails h…', 'uniform', 0, np.Inf],
                    ['Bilayer heads h…', 'uniform', 0, np.Inf],
                    ['cw hydration', 'uniform', 0, np.Inf],
                    ['Oxide Hydration', 'uniform', 0, np.Inf]]
    priors.backgroundParam = [['Backs parameter 1', 'uniform', 0, np.Inf], ['Backs parameter 2', 'uniform', 0, np.Inf]]
    priors.scalefactor = [['Scalefactor 1',	'uniform',	0,	np.Inf], ['Scalefactor 2',	'uniform',	0,	np.Inf]]
    priors.qzshift = [['Qz shift 1', 'uniform', 0, np.Inf]]
    priors.bulkIn = [['Air', 'uniform', 0, np.Inf]]
    priors.bulkOut = [['D2O', 'uniform', 0, np.Inf], ['SMW', 'uniform', 0, np.Inf]]
    priors.resolutionParam = [['Resolution par 1', 'uniform', 0, np.Inf]]
    priors.priorNames = ['Substrate Roughness', 'Oxide thick', 'Oxide SLD',
                          'Sam tails thick', 'Sam tails SLD', 'Sam tails hydration', 'Sam rough',
                          'cw thick', 'cw SLD', 'SAM head thick', 'SAM head SLD', 'SAM head hydration', 
                          'Bilayer head thick', 'Bilayer head SLD', 'Bilayer rough', 'Bilayer tails thick',
                          'Bilayer tails SLD', 'Bilayer tails hydr', 'Bilayer heads hydr', 'cw hydration',
                          'Oxide Hydration','Backs parameter 1', 'Backs parameter 2', 'Resolution par 1',
                          'Air', 'D2O', 'SMW', 'Qz shift 1', 'Scalefactor 1', 'Scalefactor 2']          
    priors.priorValues = [[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],
                          [1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf],[1, 0, np.Inf]]
    
    pdef, problem, output, bayes_result = rat.RATMain(problem, cells, limits, control, priors)
    
    print(problem.ssubs)
    print(problem.backgroundParams)
    print(problem.qzshifts)
    print(problem.scalefactors)
    print(problem.bulkIn)
    print(problem.bulkOut)
    print(problem.resolutionParams)
    print(problem.calculations.allChis)
    print(problem.calculations.sumChi)
    print(problem.allSubRough)
    print(problem.resample)
    #print(output)
    