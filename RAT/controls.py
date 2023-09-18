import tabulate
from typing import Union, Any
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class BaseProcedure:
    """
    Defines the base class with properties used in 
    all five procedures.
    """

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value) -> None:

        self._parallel = parallel
        self._calcSldDuringFit = calcSldDuringFit
        self._resamPars = resamPars
        self._display = display

    def _validate_type(self,
                       name: str,
                       value: Any,
                       type: type) -> None:
        """
        Validates the value has the correct type.

        Parameters
        ----------
        name : str
            The name of the property.
        value : Any
            The value of the property.
        type : type
            The expected type of the property.

        Raises
        ------
        TypeError
            Raised if the property has value of the wrong type.
        """
        if not isinstance(value, type):
            raise TypeError(f"{name} must be of type {type.__name__}")
    
    def _validate_value(self,
                        name: str,
                        value: Union[str, int],
                        enum: Union[ParallelOptions,
                                    Procedures,
                                    DisplayOptions,
                                    BoundHandlingOptions,
                                    StrategyOptions],
                        type: Union[str, int] = str) -> None:
        """
        Validates the value is present in the enum.

        Parameters
        ----------
        name : str
            The name of the property.
        value : Union[str, int]
            The value of the property.
        enum : Union[ParallelOptions, Procedures,
                     DisplayOptions, BoundHandlingOptions,
                     StrategyOptions]
            The expected enum which contains the value.
        type : Union[str, int]
            The type of values in enum.

        Raises
        ------
        ValueError
            Raised if the value is not in enum.
        """
        allowed_options = [o.value for o in enum]
        if value not in allowed_options:
            raise ValueError((f"{name} must be a {enum.__name__} "
            f"enum or one of the following {type.__name__} " 
            f"{', '.join([str(o) for o in allowed_options])}"))
    
    def _validate_range(self,
                        name: str,
                        value: Union[float, int],
                        lower_limit: Union[float, int] = float('-inf'),
                        lower_exclusive: bool = True,
                        upper_limit: Union[float, int] = float('inf'),
                        upper_exclusive: bool = True) -> None:
        """
        Validates the value is between the lower and upper bounds.

        Parameters
        ----------
        name : str
            The name of the property.
        value : str
            The value of the property.
        lower_limit : Union[float, int]
            The lower limit of the value.
        lower_inclusive : bool
            Boolean to indicate if lower limit is exclusive.
        upper_limit : Union[float, int]
            The upper limit of the value.
        upper_inclusive : bool
            Boolean to indicate if upper limit is exclusive.

        Raises
        ------
        ValueError
            Raised if the value is not in range.
        """
        # Case 1 - lower and upper bounds are exclusive 
        if lower_exclusive and upper_exclusive:
            if not lower_limit < value < upper_limit:
                raise ValueError((f"{name} must be greater than "
                    f"{lower_limit} and less than {upper_limit}"))

        # Case 2 - upper bound is exclusive
        elif not lower_exclusive and upper_exclusive:
            if not lower_limit <= value < upper_limit:
                raise ValueError((f"{name} must be greater than or equal to "
                    f"{lower_limit} and less than {upper_limit}"))

        # Case 3 - lower bound is exclusive
        elif lower_exclusive and not upper_exclusive:
            if not lower_limit < value <= upper_limit:
                raise ValueError((f"{name} must be greater than "
                    f"{lower_limit} and less than or equal to {upper_limit}"))

        # Case 4 - lower and upper bounds are inclusive 
        elif not lower_exclusive and not upper_exclusive:
            if not lower_limit <= value <= upper_limit:
                raise ValueError((f"{name} must be greater than or equal to "
                    f"{lower_limit} and less than or equal to {upper_limit}"))

    @property
    def parallel(self) -> str:
        """
        Gets the parallel property.

        Returns
        -------
        parallel : str
            The value of the parallel property.
        """
        return self._parallel

    @parallel.setter
    def parallel(self, value: str) -> None:
        """
        Sets the parallel property after validation.

        Parameters
        ----------
        value : str
            The value to be set for the parallel property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the parallel property has
            input of the wrong type or value.
        """
        self._validate_type('parallel', value, str)
        self._validate_value('parallel', value, ParallelOptions)
        self._parallel = value
    
    @property
    def calcSldDuringFit(self) -> bool:
        """
        Gets the calcSldDuringFit property.

        Returns
        -------
        calcSldDuringFit : bool
            The value of the calcSldDuringFit property.
        """
        return self._calcSldDuringFit

    @calcSldDuringFit.setter
    def calcSldDuringFit(self, value: bool) -> None:
        """
        Sets the calcSldDuringFit property after validation.

        Parameters
        ----------
        value : bool
            The value to be set for the calcSldDuringFit property.

        Raises
        ------
        TypeError
            Raised if the calcSldDuringFit property has 
            input of the wrong type.
        """
        self._validate_type('calcSldDuringFit', value, bool)
        self._calcSldDuringFit = value

    @property
    def resamPars(self) -> list[float]:
        """
        Gets the resamPars property.

        Returns
        -------
        resamPars : list[float]
            The value of the resamPars property.
        """
        return self._resamPars

    @resamPars.setter
    def resamPars(self, value: list[Union[int, float]]) -> None:
        """
        Sets the resamPars property after validation.

        Parameters
        ----------
        value : list[Union[int, float]]
            The value to be set for the resamPars property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the resamPars property has
            input of the wrong type or value.
        """
        self._validate_type('resamPars', value, list)
        if len(value) != 2:
            raise ValueError("resamPars must have length of 2")
        if not all(isinstance(x, (float, int)) for x in value):
            raise TypeError("resamPars must be defined using floats or ints")
        self._validate_range(name = 'resamPars[0]',
                             value = value[0],
                             lower_limit = 0,
                             upper_limit = 1)
        self._validate_range(name = 'resamPars[1]',
                             value = value[1],
                             lower_limit = 0,
                             lower_exclusive = False)
        self._resamPars = [float(v) for v in value]

    @property
    def display(self) -> str:
        """
        Gets the display property.

        Returns
        -------
        display : str
            The value of the display property.
        """
        return self._display

    @display.setter
    def display(self, value: str) -> None:
        """
        Sets the display property after validation.

        Parameters
        ----------
        value : str
            The value to be set for the display property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the display property has
            input of the wrong type or value.
        """
        self._validate_type('display', value, str)
        self._validate_value('display', value, DisplayOptions)
        self._display = value

    def __repr__(self, procedure: str):
        """
        Defines the display method for procedure classes.

        Parameters
        ----------
        procedure : str
            The procedure for the controls classes.

        """
        properties = [["Property", "Value"]] +\
            [["procedure", procedure]] +\
            [[k.lstrip('_'), v] for k, v in self.__dict__.items()] 
        table = tabulate.tabulate(properties, headers="firstrow")
        return table


class Calculate(BaseProcedure):
    """Defines the class for the calculate procedure"""

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value) -> None:
        
        # call the constructor of the parent class
        super().__init__(parallel = parallel,
                         calcSldDuringFit = calcSldDuringFit,
                         resamPars = resamPars,
                         display = display)
    
    @property
    def procedure(self) -> str:
        """
        Gets the procedure property.

        Returns
        -------
        procedure : str
            The value of the procedure property.
        """
        return Procedures.Calculate.value

    def __repr__(self):
        """
        Defines the display method for Calculate class
        """
        table = super().__repr__(Procedures.Calculate.value)
        return table
    

class Simplex(BaseProcedure):
    """Defines the class for the simplex procedure"""

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value,
                 tolX: float = 1e-6,
                 tolFun: float = 1e-6,
                 maxFunEvals: int = 10000,
                 maxIter: int = 1000,
                 updateFreq: int = -1,
                 updatePlotFreq: int = -1) -> None:
        
        # call the constructor of the parent class
        super().__init__(parallel=parallel,
                         calcSldDuringFit=calcSldDuringFit,
                         resamPars=resamPars,
                         display=display)

        self._tolX = tolX
        self._tolFun = tolFun
        self._maxFunEvals = maxFunEvals
        self._maxIter = maxIter
        self._updateFreq = updateFreq
        self._updatePlotFreq = updatePlotFreq

    @property
    def procedure(self) -> str:
        """
        Gets the procedure property.

        Returns
        -------
        procedure : str
            The value of the procedure property.
        """
        return Procedures.Simplex.value

    @property
    def tolX(self) -> float:
        """
        Gets the tolX property.

        Returns
        -------
        tolX : float
            The value of the tolX property.
        """
        return self._tolX

    @tolX.setter
    def tolX(self, value: float) -> None:
        """
        Sets the tolX property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the tolX property.

        Raises
        ------
        TypeError
            Raised if the tolX property has
            input of the wrong type.
        """
        self._validate_type('tolX', value, float)
        self._tolX = value
    
    @property
    def tolFun(self) -> float:
        """
        Gets the tolFun property.

        Returns
        -------
        tolFun : float
            The value of the tolFun property.
        """
        return self._tolFun

    @tolFun.setter
    def tolFun(self, value: float) -> None:
        """
        Sets the tolFun property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the tolFun property.

        Raises
        ------
        TypeError
            Raised if the tolFun property has
            input of the wrong type.
        """
        self._validate_type('tolFun', value, float)
        self._tolFun = value
    
    @property
    def maxFunEvals(self) -> int:
        """
        Gets the maxFunEvals property.

        Returns
        -------
        maxFunEvals : int
            The value of the maxFunEvals property.
        """
        return self._maxFunEvals

    @maxFunEvals.setter
    def maxFunEvals(self, value: int) -> None:
        """
        Sets the maxFunEvals property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the maxFunEvals property.

        Raises
        ------
        TypeError
            Raised if the maxFunEvals property has
            input of the wrong type.
        """
        self._validate_type('maxFunEvals', value, int)
        self._maxFunEvals = value

    @property
    def maxIter(self) -> int:
        """
        Gets the maxIter property.

        Returns
        -------
        maxIter : int
            The value of the maxIter property.
        """
        return self._maxIter

    @maxIter.setter
    def maxIter(self, value: int) -> None:
        """
        Sets the maxIter property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the maxIter property.

        Raises
        ------
        TypeError
            Raised if the maxIter property has
            input of the wrong type.
        """
        self._validate_type('maxIter', value, int)
        self._maxIter = value
 
    @property
    def updateFreq(self) -> int:
        """
        Gets the updateFreq property.

        Returns
        -------
        updateFreq : int
            The value of the updateFreq property.
        """
        return self._updateFreq

    @updateFreq.setter
    def updateFreq(self, value: int) -> None:
        """
        Sets the updateFreq property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the updateFreq property.

        Raises
        ------
        TypeError
            Raised if the updateFreq property has
            input of the wrong type.
        """
        self._validate_type('updateFreq', value, int)
        self._updateFreq = value
    
    @property
    def updatePlotFreq(self) -> int:
        """
        Gets the updatePlotFreq property.

        Returns
        -------
        updatePlotFreq : int
            The value of the updatePlotFreq property.
        """
        return self._updatePlotFreq

    @updatePlotFreq.setter
    def updatePlotFreq(self, value: int) -> None:
        """
        Sets the updatePlotFreq property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the updatePlotFreq property.

        Raises
        ------
        TypeError
            Raised if the updatePlotFreq property has
            input of the wrong type.
        """
        self._validate_type('updatePlotFreq', value, int)
        self._updatePlotFreq = value

    def __repr__(self):
        """
        Defines the display method for Simplex class
        """
        table = super().__repr__(Procedures.Simplex.value)
        return table


class DE(BaseProcedure):
    """Defines the class for the Differential Evolution procedure"""

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value,
                 populationSize: int = 20,
                 fWeight: float = 0.5,
                 crossoverProbability: float = 0.8,
                 strategy: int = StrategyOptions.RandomWithPerVectorDither.value,
                 targetValue: Union[int, float] = 1,
                 numGenerations: int = 500) -> None:
        
        # call the constructor of the parent class
        super().__init__(parallel=parallel,
                         calcSldDuringFit=calcSldDuringFit,
                         resamPars=resamPars,
                         display=display)

        self._populationSize = populationSize
        self._fWeight = fWeight
        self._crossoverProbability = crossoverProbability
        self._strategy = strategy
        self._targetValue = targetValue
        self._numGenerations = numGenerations

    @property
    def procedure(self) -> str:
        """
        Gets the procedure property.

        Returns
        -------
        procedure : str
            The value of the procedure property.
        """
        return Procedures.DE.value
    
    @property
    def populationSize(self) -> int:
        """
        Gets the populationSize property.

        Returns
        -------
        populationSize : int
            The value of the populationSize property.
        """
        return self._populationSize

    @populationSize.setter
    def populationSize(self, value: int) -> None:
        """
        Sets the populationSize property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the populationSize property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the populationSize property has
            input of the wrong type or value.
        """
        self._validate_type('populationSize', value, int)
        self._validate_range(name = 'populationSize',
                             value = value,
                             lower_limit = 1,
                             lower_exclusive = False)
        self._populationSize = value
    
    @property
    def fWeight(self) -> float:
        """
        Gets the fWeight property.

        Returns
        -------
        fWeight : float
            The value of the fWeight property.
        """
        return self._fWeight

    @fWeight.setter
    def fWeight(self, value: float) -> None:
        """
        Sets the fWeight property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the fWeight property.

        Raises
        ------
        TypeError
            Raised if the fWeight property has
            input of the wrong type.
        """
        self._validate_type('fWeight', value, float)
        self._fWeight = value
    
    @property
    def crossoverProbability(self) -> float:
        """
        Gets the crossoverProbability property.

        Returns
        -------
        crossoverProbability : float
            The value of the crossoverProbability property.
        """
        return self._crossoverProbability
    
    @crossoverProbability.setter
    def crossoverProbability(self, value: float) -> None:
        """
        Sets the crossoverProbability property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the crossoverProbability property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the crossoverProbability property has
            input of the wrong type or value.
        """
        self._validate_type('crossoverProbability', value, float)
        self._validate_range(name = 'crossoverProbability',
                             value = value,
                             lower_limit = 0,
                             upper_limit = 1)
        self._crossoverProbability = value
    
    @property
    def strategy(self) -> int:
        """
        Gets the strategy property.

        Returns
        -------
        strategy : int
            The value of the strategy property.
        """
        return self._strategy

    @strategy.setter
    def strategy(self, value: int) -> None:
        """
        Sets the strategy property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the strategy property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the strategy property has
            input of the wrong type or value.
        """
        self._validate_type('strategy', value, int)
        self._validate_value(name = 'strategy',
                             value = value,
                             enum = StrategyOptions,
                             type = int)
        self._strategy = value
    
    @property
    def targetValue(self) -> float:
        """
        Gets the targetValue property.

        Returns
        -------
        targetValue : float
            The value of the targetValue property.
        """
        return self._targetValue

    @targetValue.setter
    def targetValue(self, value: Union[int, float]) -> None:
        """
        Sets the targetValue property after validation.

        Parameters
        ----------
        value : Union[int, float]
            The value to be set for the targetValue property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the targetValue property has
            input of the wrong type or value.
        """
        self._validate_type('targetValue', value, (int, float))
        self._validate_range(name = 'targetValue',
                             value = value,
                             lower_limit = 1,
                             lower_exclusive = False)
        self._targetValue = float(value)

    @property
    def numGenerations(self) -> int:
        """
        Gets the numGenerations property.

        Returns
        -------
        numGenerations : int
            The value of the numGenerations property.
        """
        return self._numGenerations
    
    @numGenerations.setter
    def numGenerations(self, value: int) -> None:
        """
        Sets the numGenerations property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the numGenerations property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the numGenerations property has
            input of the wrong type or value.
        """
        self._validate_type('numGenerations', value, int)
        self._validate_range(name = 'numGenerations',
                             value = value,
                             lower_limit = 1,
                             lower_exclusive = False)
        self._numGenerations = value

    def __repr__(self):
        """
        Defines the display method for DE class
        """
        table = super().__repr__(Procedures.DE.value)
        return table


class NS(BaseProcedure):
    """Defines the class for the  Nested Sampler procedure"""

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value,
                 Nlive: int = 150,
                 Nmcmc: Union[float, int] = 0,
                 propScale: float = 0.1,
                 nsTolerance: Union[float, int] = 0.1) -> None:

        # call the constructor of the parent class
        super().__init__(parallel=parallel,
                         calcSldDuringFit=calcSldDuringFit,
                         resamPars=resamPars,
                         display=display)

        self._Nlive = Nlive
        self._Nmcmc = Nmcmc
        self._propScale = propScale
        self._nsTolerance = nsTolerance

    @property
    def procedure(self) -> str:
        """
        Gets the procedure property.

        Returns
        -------
        procedure : str
            The value of the procedure property.
        """
        return Procedures.NS.value
    
    @property
    def Nlive(self) -> int:
        """
        Gets the Nlive property.

        Returns
        -------
        Nlive : int
            The value of the Nlive property.
        """
        return self._Nlive
    
    @Nlive.setter
    def Nlive(self, value: int) -> None:
        """
        Sets the Nlive property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the Nlive property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the Nlive property has
            input of the wrong type or value.
        """
        self._validate_type('Nlive', value, int)
        self._validate_range(name = 'Nlive',
                             value = value,
                             lower_limit = 1,
                             lower_exclusive = False)
        self._Nlive = value

    @property
    def Nmcmc(self) -> float:
        """
        Gets the Nmcmc property.

        Returns
        -------
        Nmcmc : float
            The value of the Nmcmc property.
        """
        return self._Nmcmc

    @Nmcmc.setter
    def Nmcmc(self, value: Union[float, int]) -> None:
        """
        Sets the Nmcmc property after validation.

        Parameters
        ----------
        value : Union[float, int]
            The value to be set for the Nmcmc property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the Nmcmc property has
            input of the wrong type or value.
        """
        self._validate_type('Nmcmc', value, (int, float))
        self._validate_range(name = 'Nmcmc',
                             value = value,
                             lower_limit = 1,
                             lower_exclusive = False)
        self._Nmcmc = float(value)

    @property
    def propScale(self) -> float:
        """
        Gets the propScale property.

        Returns
        -------
        propScale : float
            The value of the propScale property.
        """
        return self._propScale

    @propScale.setter
    def propScale(self, value: float) -> None:
        """
        Sets the propScale property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the propScale property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the propScale property has
            input of the wrong type or value.
        """
        self._validate_type('propScale', value, float)
        self._validate_range(name = 'propScale',
                             value = value,
                             lower_limit = 0,
                             upper_limit = 1)
        self._propScale = value

    @property
    def nsTolerance(self) -> float:
        """
        Gets the nsTolerance property.

        Returns
        -------
        nsTolerance : float
            The value of the nsTolerance property.
        """
        return self._nsTolerance
    
    @nsTolerance.setter
    def nsTolerance(self, value: Union[float, int]) -> None:
        """
        Sets the nsTolerance property after validation.

        Parameters
        ----------
        value : Union[float, int]
            The value to be set for the nsTolerance property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the nsTolerance property has
            input of the wrong type or value.
        """
        self._validate_type('nsTolerance', value, (int, float))
        self._validate_range(name = 'nsTolerance',
                             value = value,
                             lower_limit = 0,
                             lower_exclusive = False)
        self._nsTolerance = float(value)

    def __repr__(self):
        """
        Defines the display method for NS class
        """
        table = super().__repr__(Procedures.NS.value)
        return table


class Dream(BaseProcedure):
    """Defines the class for the Dream procedure"""

    def __init__(self,
                 parallel: str = ParallelOptions.Single.value,
                 calcSldDuringFit: bool = False,
                 resamPars: list[Union[int, float]] = [0.9, 50],
                 display: str = DisplayOptions.Iter.value,
                 nSamples: int = 50000,
                 nChains: int = 10,
                 jumpProb: float = 0.5,
                 pUnitGamma:float = 0.2,
                 boundHandling: str = BoundHandlingOptions.Fold.value) -> None:
        
        # call the constructor of the parent class
        super().__init__(parallel=parallel,
                         calcSldDuringFit=calcSldDuringFit,
                         resamPars=resamPars,
                         display=display)

        self._nSamples = nSamples
        self._nChains = nChains
        self._jumpProb = jumpProb  # lambda in MATLAB
        self._pUnitGamma = pUnitGamma
        self._boundHandling = boundHandling

    @property
    def procedure(self) -> str:
        """
        Gets the procedure property.

        Returns
        -------
        procedure : str
            The value of the procedure property.
        """
        return Procedures.Dream.value
    
    @property
    def nSamples(self) -> int:
        """
        Gets the nSamples property.

        Returns
        -------
        nSamples : int
            The value of the nSamples property.
        """
        return self._nSamples

    @nSamples.setter
    def nSamples(self, value: int) -> None:
        """
        Sets the nSamples property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the nSamples property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the nSamples property has
            input of the wrong type or value.
        """
        self._validate_type('nSamples', value, int)
        self._validate_range(name = 'nSamples',
                             value = value,
                             lower_limit = 0,
                             lower_exclusive = False)
        self._nSamples = value

    @property
    def nChains(self) -> int:
        """
        Gets the nChains property.

        Returns
        -------
        nChains : int
            The value of the nChains property.
        """
        return self._nChains

    @nChains.setter
    def nChains(self, value: int) -> None:
        """
        Sets the nChains property after validation.

        Parameters
        ----------
        value : int
            The value to be set for the nChains property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the nChains property has
            input of the wrong type or value.
        """
        self._validate_type('nChains', value, int)
        self._validate_range(name = 'nChains',
                             value = value,
                             lower_limit = 0,
                             lower_exclusive = False)
        self._nChains = value

    @property
    def jumpProb(self) -> float:
        """
        Gets the jumpProb property.

        Returns
        -------
        jumpProb : float
            The value of the jumpProb property.
        """
        return self._jumpProb

    @jumpProb.setter
    def jumpProb(self, value: float) -> None:
        """
        Sets the jumpProb property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the jumpProb property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the jumpProb property has
            input of the wrong type or value.
        """
        self._validate_type('jumpProb', value, float)
        self._validate_range(name = 'jumpProb',
                             value = value,
                             lower_limit = 0,
                             upper_limit = 1)
        self._jumpProb = value

    @property
    def pUnitGamma(self) -> float:
        """
        Gets the pUnitGamma property.

        Returns
        -------
        pUnitGamma : float
            The value of the pUnitGamma property.
        """
        return self._pUnitGamma

    @pUnitGamma.setter
    def pUnitGamma(self, value: float) -> None:
        """
        Sets the pUnitGamma property after validation.

        Parameters
        ----------
        value : float
            The value to be set for the pUnitGamma property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the pUnitGamma property has
            input of the wrong type or value.
        """
        self._validate_type('pUnitGamma', value, float)
        self._validate_range(name = 'pUnitGamma',
                             value = value,
                             lower_limit = 0,
                             upper_limit = 1)
        self._pUnitGamma = value

    @property
    def boundHandling(self) -> str:
        """
        Gets the boundHandling property.

        Returns
        -------
        boundHandling : str
            The value of the boundHandling property.
        """
        return self._boundHandling

    @boundHandling.setter
    def boundHandling(self, value: str) -> None:
        """
        Sets the boundHandling property after validation.

        Parameters
        ----------
        value : str
            The value to be set for the boundHandling property.

        Raises
        ------
        Union[TypeError, ValueError]
            Raised if the boundHandling property has
            input of the wrong type or value.
        """
        self._validate_type('boundHandling', value, str)
        self._validate_value('boundHandling', value, BoundHandlingOptions)
        self._boundHandling = value

    def __repr__(self):
        """
        Defines the display method for Dream class
        """
        table = super().__repr__(Procedures.Dream.value)
        return table


class ControlsClass:

    def __init__(self,
                 procedure: str = Procedures.Calculate.value,
                 **properties) -> None:
        
        self._procedure = procedure
        self._validate_properties(**properties)

        if self._procedure == Procedures.Calculate.value:
            self._controls = Calculate(**properties)

        elif self._procedure == Procedures.Simplex.value:
            self._controls = Simplex(**properties)
        
        elif self._procedure == Procedures.DE.value:
            self._controls = DE(**properties)
        
        elif self._procedure == Procedures.NS.value:
            self._controls = NS(**properties)
        
        elif self._procedure == Procedures.Dream.value:
            self._controls = Dream(**properties)
    
    @property
    def controls(self):
        """
        Gets the controls.

        Returns
        -------
        controls : Union[CalculateProcedure,
                         SimplexProcedure,
                         DEProcedure,
                         NSProcedure,
                         DreamProcedure]
            The class with control properties.
        """
        return self._controls

    def _validate_properties(self, **properties) -> bool:
        """
        Validates the inputs for the procedure.

        Parameters
        ----------
        properties : dict[str, 
                          Union[int,
                                float,
                                str,
                                bool,
                                list[Union[int, float]]]]
            The properties of the procedure.

        Raises
        ------
        ValueError
            Raised if properties are not validated.
        """
        property_names = {Procedures.Calculate.value: {'parallel',
                                                       'calcSLdDuringFit',
                                                       'resamPars',
                                                       'display'},
                          Procedures.Simplex.value: {'parallel',
                                                     'calcSLdDuringFit',
                                                     'resamPars',
                                                     'display',
                                                     'tolX',
                                                     'tolFun',
                                                     'maxFunEvals',
                                                     'maxIter',
                                                     'updateFreq',
                                                     'updatePlotFreq'},
                          Procedures.DE.value: {'parallel',
                                                'calcSLdDuringFit',
                                                'resamPars',
                                                'display',
                                                'populationSize',
                                                'fWeight',
                                                'crossoverProbability',
                                                'strategy',
                                                'targetValue',
                                                'numGenerations'},
                          Procedures.NS.value: {'parallel',
                                                'calcSLdDuringFit',
                                                'resamPars',
                                                'display',
                                                'Nlive',
                                                'Nmcmc',
                                                'propScale',
                                                'nsTolerance'},
                          Procedures.Dream.value: {'parallel',
                                                   'calcSLdDuringFit',
                                                   'resamPars',
                                                   'display',
                                                   'nSamples',
                                                   'nChains',
                                                   'jumpProb',
                                                   'pUnitGamma',
                                                   'boundHandling'}}
        expected_properties = property_names[self._procedure]
        input_properties = properties.keys()
        if not (expected_properties | input_properties == expected_properties):
            raise ValueError((f"Properties that can be set for {self._procedure} are "
                              f"{', '.join(sorted(expected_properties))}"))

    def __repr__(self):
        """
        Defines the display method for Controls class
        """
        table = self._controls.__repr__()
        return table
