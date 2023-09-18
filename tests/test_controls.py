"""Tests for control and procedure classes"""

import pytest
from typing import Union, Any
from RAT.controls import BaseProcedure, Calculate, Simplex, DE, NS, Dream, ControlsClass
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class TestBaseProcedure:

    def setup_class(self):
        self.base_procedure = BaseProcedure()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value)])
    def test_base_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.base_procedure, property) == value
    
    @pytest.mark.parametrize("property", ['parallel', 'calcSldDuringFit', 'resamPars', 'display'])
    def test_base_procedure_properties(self, property: str) -> None:
        assert hasattr(self.base_procedure, property)
    
    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value)])
    def test_base_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.base_procedure, property, value)
        assert getattr(self.base_procedure, property) == value
    
    @pytest.mark.parametrize("var, exp_type", [('string', float),
                                               (True, str),
                                               (1, bool),
                                               (1.0, int)])
    def test_validate_type(self, var: Any, exp_type: Any) -> None:
        with pytest.raises(TypeError) as exc:
            self.base_procedure._validate_type('var', var, exp_type)
        assert f"var must be of type {exp_type.__name__}" in str(exc.value)

    @pytest.mark.parametrize("enum, enum_type", [(ParallelOptions, str),
                                                 (Procedures, str),
                                                 (DisplayOptions, str),
                                                 (BoundHandlingOptions, str),
                                                 (StrategyOptions, int)])
    def test_validate_value(self, enum: str, enum_type: Union[int, str]) -> None:
        allowed_options = [str(o.value) for o in enum]
        with pytest.raises(ValueError) as exc:
            self.base_procedure._validate_value('var', 'test_variable', enum, enum_type)
        assert (f"var must be a {enum.__name__} "
        f"enum or one of the following {enum_type.__name__} "
        f"{', '.join(allowed_options)}") == str(exc.value)

    @pytest.mark.parametrize("lower, upper, msg", 
                             [(True, True, "var must be greater than 0 and less than 1"),
                              (False, True, "var must be greater than or equal to 0 and less than 1"),
                              (True, False, "var must be greater than 0 and less than or equal to 1"),
                              (False, False, "var must be greater than or equal to 0 and less than or equal to 1")])
    def test_validate_range(self, lower: bool, upper: bool, msg: str) -> None:
        with pytest.raises(ValueError) as exc:
            self.base_procedure._validate_range(name='var',
                                                value=10,
                                                lower_limit=0,
                                                upper_limit=1,
                                                lower_exclusive=lower,
                                                upper_exclusive=upper)
        assert msg == str(exc.value)
    
    @pytest.mark.parametrize("property, value, msg", 
                             [('parallel', 1, 'parallel must be of type str'),
                              ('calcSldDuringFit', 1, 'calcSldDuringFit must be of type bool'),
                              ('resamPars', True, 'resamPars must be of type list'),
                              ('resamPars', ['a', 'b'], 'resamPars must be defined using floats or ints'),
                              ('display', True, 'display must be of type str')])
    def test_base_procedure_properties_type_exceptions(self, property: str,  value: Any, msg: str) -> None:
        with pytest.raises(TypeError) as exc:
            setattr(self.base_procedure, property, value)
        assert msg == str(exc.value)
    
    @pytest.mark.parametrize("property, value, msg", 
                             [('parallel',
                               'test_value',
                               'parallel must be a ParallelOptions enum or one of the following str single, points, contrasts, all'),
                              ('resamPars',
                               [1, 2, 3],
                               'resamPars must have length of 2'),
                              ('display',
                               'test_value',
                               'display must be a DisplayOptions enum or one of the following str off, iter, notify, final')])
    def test_base_procedure_properties_type_exceptions(self, property: str,  value: Any, msg: str) -> None:
        with pytest.raises(ValueError) as exc:
            setattr(self.base_procedure, property, value)
        assert msg == str(exc.value)


class TestCalculate:

    def setup_class(self):
        self.calulate = Calculate()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value),
                                                 ('procedure', Procedures.Calculate.value)])
    def test_calculate_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.calulate, property) == value
    
    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure'])
    def test_calulate_procedure_properties(self, property: str) -> None:
        assert hasattr(self.calulate, property)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value)])
    def test_calculate_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.calulate, property, value)
        assert getattr(self.calulate, property) == value
    
    def test_calculate_procedure_repr(self) -> None:
        calulate = Calculate()
        table = calulate.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "procedure         calculate\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter")
        assert table == table_str
          

class TestSimplex:

    def setup_class(self):
        self.simplex = Simplex()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value),
                                                 ('procedure', Procedures.Simplex.value),
                                                 ('tolX', 1e-6),
                                                 ('tolFun', 1e-6),
                                                 ('maxFunEvals', 10000),
                                                 ('maxIter', 1000),
                                                 ('updateFreq', -1),
                                                 ('updatePlotFreq', -1)])
    def test_simplex_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.simplex, property) == value

    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure',
                                          'tolX',
                                          'tolFun',
                                          'maxFunEvals',
                                          'maxIter',
                                          'updateFreq',
                                          'updatePlotFreq'])
    def test_simplex_procedure_properties(self, property: str) -> None:
        assert hasattr(self.simplex, property)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value),
                                                 ('tolX', 4e-6),
                                                 ('tolFun', 3e-4),
                                                 ('maxFunEvals', 100),
                                                 ('maxIter', 50),
                                                 ('updateFreq', 4),
                                                 ('updatePlotFreq', 3)])
    def test_simplex_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.simplex, property, value)
        assert getattr(self.simplex, property) == value
    
    def test_simplex_procedure_repr(self) -> None:
        simplex = Simplex()
        table = simplex.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "procedure         simplex\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "tolX              1e-06\n"
                     "tolFun            1e-06\n"
                     "maxFunEvals       10000\n"
                     "maxIter           1000\n"
                     "updateFreq        -1\n"
                     "updatePlotFreq    -1")
        assert table == table_str


class TestDE:

    def setup_class(self):
        self.de = DE()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value),
                                                 ('procedure', Procedures.DE.value),
                                                 ('populationSize', 20),
                                                 ('fWeight', 0.5),
                                                 ('crossoverProbability', 0.8),
                                                 ('strategy', 4),
                                                 ('targetValue', 1),
                                                 ('numGenerations', 500)])
    def test_de_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.de, property) == value

    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure',
                                          'populationSize',
                                          'fWeight',
                                          'crossoverProbability',
                                          'strategy',
                                          'targetValue',
                                          'numGenerations'])
    def test_de_procedure_properties(self, property: str) -> None:
        assert hasattr(self.de, property)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value),
                                                 ('populationSize', 20),
                                                 ('fWeight', 0.3),
                                                 ('crossoverProbability', 0.4),
                                                 ('strategy', 3),
                                                 ('targetValue', 2),
                                                 ('numGenerations', 50)])
    def test_de_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.de, property, value)
        assert getattr(self.de, property) == value

    def test_de_procedure_repr(self) -> None:
        de = DE()
        table = de.__repr__()
        table_str = ("Property              Value\n"
                     "--------------------  ---------\n"
                     "procedure             de\n"
                     "parallel              single\n"
                     "calcSldDuringFit      False\n"
                     "resamPars             [0.9, 50]\n"
                     "display               iter\n"
                     "populationSize        20\n"
                     "fWeight               0.5\n"
                     "crossoverProbability  0.8\n"
                     "strategy              4\n"
                     "targetValue           1\n"
                     "numGenerations        500")
        assert table == table_str


class TestNS:

    def setup_class(self):
        self.ns = NS()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value),
                                                 ('procedure', Procedures.NS.value),
                                                 ('Nlive', 150),
                                                 ('Nmcmc', 0),
                                                 ('propScale', 0.1),
                                                 ('nsTolerance', 0.1)])
    def test_ns_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.ns, property) == value

    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure',
                                          'Nlive',
                                          'Nmcmc',
                                          'propScale',
                                          'nsTolerance'])
    def test_ns_procedure_properties(self, property: str) -> None:
        assert hasattr(self.ns, property)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value),
                                                 ('Nlive', 1500),
                                                 ('Nmcmc', 1),
                                                 ('propScale', 0.5),
                                                 ('nsTolerance', 0.8)])
    def test_ns_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.ns, property, value)
        assert getattr(self.ns, property) == value

    def test_ns_procedure_repr(self) -> None:
        ns = NS()
        table = ns.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "procedure         ns\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "Nlive             150\n"
                     "Nmcmc             0\n"
                     "propScale         0.1\n"
                     "nsTolerance       0.1")
        assert table == table_str
        

class TestDream:

    def setup_class(self):
        self.dream = Dream()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single.value),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter.value),
                                                 ('procedure', Procedures.Dream.value),
                                                 ('nSamples', 50000),
                                                 ('nChains', 10),
                                                 ('jumpProb', 0.5),
                                                 ('pUnitGamma', 0.2),
                                                 ('boundHandling', BoundHandlingOptions.Fold.value)])
    def test_dream_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.dream, property) == value

    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure',
                                          'nSamples',
                                          'nChains',
                                          'jumpProb',
                                          'pUnitGamma',
                                          'boundHandling'])
    def test_dream_procedure_properties(self, property: str) -> None:
        assert hasattr(self.dream, property)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All.value),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify.value),
                                                 ('nSamples', 500),
                                                 ('nChains', 1000),
                                                 ('jumpProb', 0.7),
                                                 ('pUnitGamma', 0.3),
                                                 ('boundHandling', BoundHandlingOptions.Reflect.value)])
    def test_dream_procedure_setters(self, property: str,  value: Any) -> None:
        setattr(self.dream, property, value)
        assert getattr(self.dream, property) == value

    def test_dream_procedure_repr(self) -> None:
        dream = Dream()
        table = dream.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "procedure         dream\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "nSamples          50000\n"
                     "nChains           10\n"
                     "jumpProb          0.5\n"
                     "pUnitGamma        0.2\n"
                     "boundHandling     fold")
        assert table == table_str
