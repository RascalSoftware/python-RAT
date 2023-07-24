import cppimport
import numpy as np

rat = cppimport.imp("rat")

if __name__ == '__main__':
    #------------------------------------------------------------------------------------
    # Control
    control = rat.Control()
    problem = rat.ProblemDefinition()
    limits = rat.Limits()
    cells = rat.Cells()
    
    control.proc = 'calculate'
    control.para = 'points'
    control.display = 'iter'
    control.calcSld = False 
    control.resamPars = [0.9000, 50]
    
    control.checks.params_fitYesNo = [1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1]
    control.checks.backs_fitYesNo = np.ones((2))
    control.checks.shifts_fitYesNo = [0]
    control.checks.scales_fitYesNo = [0, 0]
    control.checks.nbairs_fitYesNo = [0]
    control.checks.nbsubs_fitYesNo = [0, 0]
    control.checks.resol_fitYesNo = [0]
    
    #------------------------------------------------------------------------------------
    # ProblemDef
    problem.contrastBacks = [1, 2]
    problem.contrastBacksType = [1, 1]
    problem.TF = 'non polarised'
    problem.resample = [0, 0]
    problem.dataPresent = [1, 1]
    problem.oilChiDataPresent = [0, 0]
    problem.numberOfContrasts = 2
    problem.geometry = 'substrate/liquid'
    problem.useImaginary = False
    problem.contrastShifts = [1, 1]
    problem.contrastScales = [1, 1]
    problem.contrastNbas = [1, 1]
    problem.contrastNbss = [1, 2]
    problem.contrastRes = [1, 1]
    problem.backs = [2.28525214157571e-06, 3.10246417982706e-06]
    problem.shifts = [0]
    problem.sf = [0.102860901766649, 0.151378164199993]
    problem.nba = [2.07399989449186e-06]
    problem.nbs = [5.98067802097389e-06, 2.21000000000000e-06]
    problem.res = [0.0300]
    problem.params = [5.08929897223891, 19.2830495317839, 3.39317612702561e-06, 21.5451088784623, -4.01001212540204e-07, 1.24963548580913, 8.09797576127018, 20.3854719276630, 
                      0, 5.79580504624961, 1.75686012656988e-06, 10.0916971312704, 11.7066921282983, 1.47000000000000e-06, 5.63694566866001, 17.8733566725932, -4.61089672942576e-07, 
                      17.8556570737753, 41.4142113901161, 100, 25.6893163532023]
    problem.numberOfLayers = 6
    problem.modelType = 'standard layers'
    problem.contrastCustomFiles = [np.NAN, np.NaN]
    problem.contrastDomainRatios = [0, 0]
    problem.domainRatio = []
    problem.numberOfDomainContrasts = 0
    problem.domainContrastCustomFiles = []
    problem.fitpars = [5.08929897223891, 19.2830495317839, 21.5451088784623, 1.24963548580913, 8.09797576127018, 
                       20.3854719276630, 5.79580504624961, 10.0916971312704, 11.7066921282983, 5.63694566866001,
                       17.8733566725932, 17.8556570737753, 41.4142113901161, 25.6893163532023, 2.28525214157571e-06,
                       3.10246417982706e-06]
    problem.otherpars = [3.39317612702561e-06, -4.01001212540204e-07, 0, 1.75686012656988e-06, 1.47e-06, -4.61089672942576e-07,
                         100, 0.102860901766649, 0.151378164199993,0, 2.07399989449186e-06, 5.98067802097389e-06, 2.21e-06, 0.030]
    problem.fitconstr = [[2, 8], [5, 60], [15, 35], [1, 50], [1, 15], [10, 28], [5, 17],
                         [10, 50], [7, 17], [2, 15], [14, 22], [10, 50], [10, 50], [0, 60], [5.0e-10, 7.0e-06], 
                         [1.0e-10, 4.99999987368938e-06]]
    problem.otherconstr = [[3.39e-06, 3.41e-06], [-5.0e-07, -3.0e-07], [0, 1.0e-09], [1.0e-07, 2.0e-06], [5.0e-07, 1.5e-06],
                           [-5.0e-07, 0], [99.9, 100], [0.050, 0.20], [0.050, 0.20], [-0.00010, 0.00010], [2.0e-06, 2.1e-06], 
                           [5.50000004295725e-06, 6.4e-06], [2.0e-06, 4.99999987368938e-06], [0.010, 0.050]]

    #------------------------------------------------------------------------------------
    # Limits
    limits.params = [[2, 8], [5, 60], [0, 0],  [15, 35], [-0, -0], [1, 50], [1, 15],
                     [10, 28], [0, 0], [5, 17], [0,  0], [10, 50], [7, 17], [0,  0], [2, 15],
                     [14, 22], [-0, 0], [10, 50], [10, 50], [99.9,100], [0, 60]]
    limits.backs = [[5.0000e-10, 7.0000e-06], [1.0000e-10, 5.0000e-06]]
    limits.scales = [[0.0500, 0.2000], [0.0500, 0.2000]]
    limits.shifts = [[-1.0000e-04, 1.0000e-04]]
    limits.nba = [[2.0000e-06, 2.1000e-06]]
    limits.nbs = [[5.5000e-06, 6.4000e-06], [2.0000e-06, 5.0000e-06]]
    limits.res = [[0.0100, 0.0500]]
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

    problem, output = rat.reflectivityCalculation(problem, cells, limits, control)
    
    print(problem.ssubs)
    print(problem.backgrounds)
    print(problem.qshifts)
    print(problem.scalefactors)
    print(problem.nbairs)
    print(problem.nbsubs)
    print(problem.resolutions)
    print(problem.calculations.all_chis)
    print(problem.calculations.sum_chi)
    print(problem.allSubRough)
    print(problem.resample)
    