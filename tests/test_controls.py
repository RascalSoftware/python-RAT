"""Tests for control and procedure classes"""

import pytest
from typing import Union, Any
from RAT.controls import BaseProcedure, Calculate, Simplex, DE, NS, Dream, ControlsClass
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class TestBaseProcedure:
    """Tests the BaseProcedure class."""

    def setup_class(self):
        self.base_procedure = BaseProcedure()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter)])
    def test_base_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.base_procedure, property) == value
    
    @pytest.mark.parametrize("property", ['parallel', 'calcSldDuringFit', 'resamPars', 'display'])
    def test_base_procedure_properties(self, property: str) -> None:
        assert hasattr(self.base_procedure, property)
    
    @pytest.mark.parametrize("property, var_type", [('parallel', str),
                                                    ('calcSldDuringFit', bool),
                                                    ('resamPars', list),
                                                    ('display', str)])
    def test_base_procedure_property_types(self, property: str, var_type) -> None:
        assert isinstance(getattr(self.base_procedure, property), var_type)
    
    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify)])
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
                              ('display', True, 'display must be of type str')])
    def test_base_procedure_properties_type_exceptions(self, property: str,  value: Any, msg: str) -> None:
        with pytest.raises(TypeError) as exc:
            setattr(self.base_procedure, property, value)
        assert msg == str(exc.value)
    
    def test_base_procedure_resamPars_type_exceptions(self) -> None:
        with pytest.raises(TypeError) as exc:
            self.base_procedure.resamPars = ['f', 'g']
        assert 'resamPars must be defined using floats or ints' == str(exc.value)
    
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
    """Tests the Calculate class."""

    def setup_class(self):
        self.calulate = Calculate()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.Calculate)])
    def test_calculate_procedure_values(self, property: str, value: Any) -> None:
        assert getattr(self.calulate, property) == value
    
    @pytest.mark.parametrize("property", ['parallel',
                                          'calcSldDuringFit',
                                          'resamPars',
                                          'display',
                                          'procedure'])
    def test_calulate_procedure_properties(self, property: str) -> None:
        assert hasattr(self.calulate, property)
    
    def test_calculate_procedure_property_types(self) -> None:
        assert isinstance(getattr(self.calulate, 'procedure'), str)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify)])
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
    """Tests the Simplex class."""

    def setup_class(self):
        self.simplex = Simplex()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.Simplex),
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
    
    @pytest.mark.parametrize("property, var_type", [('procedure', str),
                                                    ('tolX', float),
                                                    ('tolFun', float),
                                                    ('maxFunEvals', int),
                                                    ('maxIter', int),
                                                    ('updateFreq', int),
                                                    ('updatePlotFreq', int),])
    def test_simplex_procedure_property_types(self, property: str, var_type) -> None:
        assert isinstance(getattr(self.simplex, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
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
    """Tests the DE class."""

    def setup_class(self):
        self.de = DE()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.DE),
                                                 ('populationSize', 20),
                                                 ('fWeight', 0.5),
                                                 ('crossoverProbability', 0.8),
                                                 ('strategy', StrategyOptions.RandomWithPerVectorDither.value),
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

    @pytest.mark.parametrize("property, var_type", [('procedure', str),
                                                    ('populationSize', int),
                                                    ('fWeight', float),
                                                    ('crossoverProbability', float),
                                                    ('strategy', int),
                                                    ('targetValue', float),
                                                    ('numGenerations', int)])
    def test_de_procedure_property_types(self, property: str, var_type) -> None:
        assert isinstance(getattr(self.de, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
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
                     "targetValue           1.0\n"
                     "numGenerations        500")
        assert table == table_str


class TestNS:
    """Tests the NS class."""

    def setup_class(self):
        self.ns = NS()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.NS),
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

    @pytest.mark.parametrize("property, var_type", [('procedure', str),
                                                    ('Nlive', int),
                                                    ('Nmcmc', float),
                                                    ('propScale', float),
                                                    ('nsTolerance', float)])
    def test_ns_procedure_property_types(self, property: str, var_type) -> None:
        assert isinstance(getattr(self.ns, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
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
                     "Nmcmc             0.0\n"
                     "propScale         0.1\n"
                     "nsTolerance       0.1")
        assert table == table_str
        

class TestDream:
    """Tests the Dream class."""

    def setup_class(self):
        self.dream = Dream()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.Dream),
                                                 ('nSamples', 50000),
                                                 ('nChains', 10),
                                                 ('jumpProb', 0.5),
                                                 ('pUnitGamma', 0.2),
                                                 ('boundHandling', BoundHandlingOptions.Fold)])
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

    @pytest.mark.parametrize("property, var_type", [('procedure', str),
                                                    ('nSamples', int),
                                                    ('nChains', int),
                                                    ('jumpProb', float),
                                                    ('pUnitGamma', float),
                                                    ('boundHandling', str)])
    def test_dream_procedure_property_types(self, property: str, var_type) -> None:
        assert isinstance(getattr(self.dream, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
                                                 ('nSamples', 500),
                                                 ('nChains', 1000),
                                                 ('jumpProb', 0.7),
                                                 ('pUnitGamma', 0.3),
                                                 ('boundHandling', BoundHandlingOptions.Reflect)])
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

class TestControlsClass:
    """Tests the Controls class."""

    def setup_class(self):
        self.controls = ControlsClass()
    
    def test_controls_class_default_type(self) -> None:
        assert type(self.controls.controls).__name__ == "Calculate"
    
    def test_dream_procedure_properties(self) -> None:
        assert hasattr(self.controls, 'controls')
    
    @pytest.mark.parametrize("procedure, name", [(Procedures.Calculate, "Calculate"),
                                                 (Procedures.Simplex, "Simplex"),
                                                 (Procedures.DE, "DE"),
                                                 (Procedures.NS, "NS"),
                                                 (Procedures.Dream, "Dream")])
    def test_controls_class_return_type(self, procedure: str, name: str) -> None:
        controls = ControlsClass(procedure)
        assert type(controls.controls).__name__ == name

    @pytest.mark.parametrize("procedure, msg", [(Procedures.Calculate,
                                                 "Properties that can be set for calculate are calcSLdDuringFit, display, parallel, resamPars"),
                                                (Procedures.Simplex,
                                                 "Properties that can be set for simplex are calcSLdDuringFit, display, maxFunEvals, maxIter, parallel, resamPars, tolFun, tolX, updateFreq, updatePlotFreq"),
                                                (Procedures.DE,
                                                 "Properties that can be set for de are calcSLdDuringFit, crossoverProbability, display, fWeight, numGenerations, parallel, populationSize, resamPars, strategy, targetValue"),
                                                (Procedures.NS,
                                                 "Properties that can be set for ns are Nlive, Nmcmc, calcSLdDuringFit, display, nsTolerance, parallel, propScale, resamPars"),
                                                (Procedures.Dream,
                                                 "Properties that can be set for dream are boundHandling, calcSLdDuringFit, display, jumpProb, nChains, nSamples, pUnitGamma, parallel, resamPars")])
    def test_controls_class_validate_properties(self, procedure: str, msg: str) -> None:
        controls = ControlsClass(procedure)
        with pytest.raises(ValueError) as exc:
            controls._validate_properties(test_variable = 200)
        assert msg == str(exc.value)
    
    def test_control_class_repr(self) -> None:
        controls = ControlsClass()
        table = controls.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "procedure         calculate\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter")
        assert table == table_str