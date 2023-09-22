"""Tests for control and procedure classes"""

import pytest
import pydantic
from typing import Union, Any
from RAT.controls import BaseProcedure, Calculate, Simplex, DE, NS, Dream, ControlsClass
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class TestBaseProcedure:
    """
    Tests the BaseProcedure class.
    """

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.base_procedure = BaseProcedure()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter)])
    def test_base_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of BaseProcedure class."""
        assert getattr(self.base_procedure, property) == value

    @pytest.mark.parametrize("property, var_type", [('parallel', ParallelOptions),
                                                    ('calcSldDuringFit', bool),
                                                    ('resamPars', list),
                                                    ('display', DisplayOptions)])
    def test_base_property_types(self, property: str, var_type: type) -> None:
        """Tests the types of the values in BaseProcedure class."""
        assert isinstance(getattr(self.base_procedure, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify)])
    def test_base_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters in BaseProcedure class."""
        setattr(self.base_procedure, property, value)
        assert getattr(self.base_procedure, property) == value

    @pytest.mark.parametrize("var1, var2", [('test', True), ('ALL', 1), ("Contrast", 3.0)])
    def test_base_parallel_validation(self, var1: str, var2: Any) -> None:
        """Tests the parallel setter validation in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'parallel', var1)
        assert exp.value.errors()[0]['msg'] == "Input should be 'single','points','contrasts' or 'all'"
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'parallel', var2)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid string"

    @pytest.mark.parametrize("value", [5.0, 12])
    def test_base_calcSldDuringFit_validation(self, value: Union[int, float]) -> None:
        """Tests the calcSldDuringFit setter validation in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'calcSldDuringFit', value)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid boolean, unable to interpret input"

    @pytest.mark.parametrize("var1, var2", [('test', True), ('iterate', 1), ("FINAL", 3.0)])
    def test_base_display_validation(self, var1: str, var2: Any) -> None:
        """Tests the display setter validation in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'display', var1)
        assert exp.value.errors()[0]['msg'] == "Input should be 'off','iter','notify' or 'final'"
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'display', var2)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid string"

    @pytest.mark.parametrize("value, msg", [([5.0], "List should have at least 2 items after validation, not 1"),
                                            ([12, 13, 14], "List should have at most 2 items after validation, not 3")])
    def test_base_resamPars_lenght_validation(self, value: list, msg: str) -> None:
        """Tests the resamPars setter lenght validation in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'resamPars', value)
        assert exp.value.errors()[0]['msg'] == msg

    @pytest.mark.parametrize("value, msg", [([1.0, 2], "Value error, resamPars[0] must be between 0 and 1"),
                                            ([0.5, -0.1], "Value error, resamPars[1] must be greater than or equal to 0")])
    def test_base_resamPars_value_validation(self, value: list, msg: str) -> None:
        """Tests the resamPars setter value validation in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'resamPars', value)
        assert exp.value.errors()[0]['msg'] == msg

    def test_base_extra_property_error(self) -> None:
        """Tests the extra property setter in BaseProcedure class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.base_procedure, 'test', 1)
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"


class TestCalculate:
    """
    Tests the Calculate class.
    """

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.calculate = Calculate()

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.Single),
                                                 ('calcSldDuringFit', False),
                                                 ('resamPars', [0.9, 50]),
                                                 ('display', DisplayOptions.Iter),
                                                 ('procedure', Procedures.Calculate)])
    def test_calculate_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of Calculate class."""
        assert getattr(self.calculate, property) == value

    def test_calculate_property_types(self) -> None:
        """Tests the types of default values of Calculate class."""
        assert isinstance(getattr(self.calculate, 'procedure'), Procedures)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify)])
    def test_calculate_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters of Calculate class."""
        setattr(self.calculate, property, value)
        assert getattr(self.calculate, property) == value

    def test_calculate_extra_property_error(self) -> None:
        """Tests the extra property setter in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'test', 1)
        assert exp.value.errors()[0]['msg'] == ("Object has no attribute 'test'")

    def test_calculate_procedure_error(self) -> None:
        """Tests the procedure property frozen in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Field is frozen"


class TestSimplex:
    """
    Tests the Simplex class.
    """

    @pytest.fixture(autouse=True)
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
    def test_simplex_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of Simplex class."""
        assert getattr(self.simplex, property) == value

    @pytest.mark.parametrize("property, var_type", [('procedure', Procedures),
                                                    ('tolX', float),
                                                    ('tolFun', float),
                                                    ('maxFunEvals', int),
                                                    ('maxIter', int),
                                                    ('updateFreq', int),
                                                    ('updatePlotFreq', int)])
    def test_simplex_property_types(self, property: str, var_type: type) -> None:
        """Tests the types of the values of Simplex class."""
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
    def test_simplex_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters of Simplex class."""
        setattr(self.simplex, property, value)
        assert getattr(self.simplex, property) == value

    @pytest.mark.parametrize("property, value", [('tolX', -4e-6),
                                                 ('tolFun', -3e-4),
                                                 ('maxFunEvals', -100),
                                                 ('maxIter', -50)])
    def test_simplex_property_errors(self, property: str,  value: Union[float, int]) -> None:
        """Tests the property errors of Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, property, value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than 0"

    def test_simplex_extra_property_error(self) -> None:
        """Tests the extra property setter in Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, 'test', 1)
        assert exp.value.errors()[0]['msg'] == ("Object has no attribute 'test'")

    def test_simplex_procedure_error(self) -> None:
        """Tests the procedure property frozen in Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Field is frozen"


class TestDE:
    """
    Tests the DE class.
    """

    @pytest.fixture(autouse=True)
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
                                                 ('strategy', StrategyOptions.RandomWithPerVectorDither),
                                                 ('targetValue', 1),
                                                 ('numGenerations', 500)])
    def test_de_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of DE class."""
        assert getattr(self.de, property) == value

    @pytest.mark.parametrize("property, var_type", [('procedure', Procedures),
                                                    ('populationSize', int),
                                                    ('fWeight', float),
                                                    ('crossoverProbability', float),
                                                    ('strategy', StrategyOptions),
                                                    ('targetValue', float),
                                                    ('numGenerations', int)])
    def test_de_property_types(self, property: str, var_type: type) -> None:
        """Tests the types of the values of DE class."""
        assert isinstance(getattr(self.de, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
                                                 ('populationSize', 20),
                                                 ('fWeight', 0.3),
                                                 ('crossoverProbability', 0.4),
                                                 ('strategy', StrategyOptions.BestWithJitter),
                                                 ('targetValue', 2.0),
                                                 ('numGenerations', 50)])
    def test_de_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters of DE class."""
        setattr(self.de, property, value)
        assert getattr(self.de, property) == value

    @pytest.mark.parametrize("value", [0, 2])
    def test_de_crossoverProbability_error(self,  value: int) -> None:
        """Tests the crossoverProbability setter error in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'crossoverProbability', value)
        assert exp.value.errors()[0]['msg'] in ["Input should be greater than 0",
                                                "Input should be less than 1"]

    @pytest.mark.parametrize("property, value", [('targetValue', 0),
                                                 ('targetValue',0.999),
                                                 ('numGenerations', -500),
                                                 ('numGenerations', 0),
                                                 ('populationSize', 0),
                                                 ('populationSize', -1)])
    def test_de_targetValue_numGenerations_populationSize_error(self,
                                                                property: str,
                                                                value: Union[int, float]) -> None:
        """Tests the targetValue, numGenerations, populationSize setter error in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, property, value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than or equal to 1"

    def test_de_extra_property_error(self) -> None:
        """Tests the extra property setter in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'test', 1)
        assert exp.value.errors()[0]['msg'] == ("Object has no attribute 'test'")

    def test_de_procedure_error(self) -> None:
        """Tests the procedure property frozen in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Field is frozen"


class TestNS:
    """
    Tests the NS class.
    """

    @pytest.fixture(autouse=True)
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
    def test_ns_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of NS class."""
        assert getattr(self.ns, property) == value

    @pytest.mark.parametrize("property, var_type", [('procedure', Procedures),
                                                    ('Nlive', int),
                                                    ('Nmcmc', float),
                                                    ('propScale', float),
                                                    ('nsTolerance', float)])
    def test_ns_property_types(self, property: str, var_type: type) -> None:
        """Tests the types of the values of NS class."""
        assert isinstance(getattr(self.ns, property), var_type)

    @pytest.mark.parametrize("property, value", [('parallel', ParallelOptions.All),
                                                 ('calcSldDuringFit', True),
                                                 ('resamPars', [0.2, 1]),
                                                 ('display', DisplayOptions.Notify),
                                                 ('Nlive', 1500),
                                                 ('Nmcmc', 1),
                                                 ('propScale', 0.5),
                                                 ('nsTolerance', 0.8)])
    def test_ns_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters of NS class."""
        setattr(self.ns, property, value)
        assert getattr(self.ns, property) == value

    @pytest.mark.parametrize("property, value, bound", [('Nmcmc', -0.6, 0),
                                                        ('nsTolerance', -500, 0),
                                                        ('Nlive', -500, 1)])
    def test_ns_Nmcmc_nsTolerance_Nlive_error(self, property: str, value: Union[int, float], bound: int) -> None:
        """Tests the Nmcmc, nsTolerance, Nlive setter error in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, property, value)
        assert exp.value.errors()[0]['msg'] == f"Input should be greater than or equal to {bound}"

    @pytest.mark.parametrize("value", [0, 2])
    def test_ns_propScale_error(self,  value: int) -> None:
        """Tests the propScale error in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'propScale', value)
        assert exp.value.errors()[0]['msg'] in ["Input should be greater than 0",
                                                "Input should be less than 1"]

    def test_ns_extra_property_error(self) -> None:
        """Tests the extra property setter in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'test', 1)
        assert exp.value.errors()[0]['msg'] == ("Object has no attribute 'test'")

    def test_ns_procedure_error(self) -> None:
        """Tests the procedure property frozen in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Field is frozen"


class TestDream:
    """
    Tests the Dream class.
    """

    @pytest.fixture(autouse=True)
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
    def test_dream_property_values(self, property: str, value: Any) -> None:
        """Tests the default values of Dream class."""
        assert getattr(self.dream, property) == value

    @pytest.mark.parametrize("property, var_type", [('procedure', Procedures),
                                                    ('nSamples', int),
                                                    ('nChains', int),
                                                    ('jumpProb', float),
                                                    ('pUnitGamma', float),
                                                    ('boundHandling', BoundHandlingOptions)])
    def test_dream_property_types(self, property: str, var_type: type) -> None:
        """Tests the type of values in Dream class."""
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
    def test_dream_property_setters(self, property: str,  value: Any) -> None:
        """Tests the setters in Dream class."""
        setattr(self.dream, property, value)
        assert getattr(self.dream, property) == value

    @pytest.mark.parametrize("property, value", [('jumpProb',0),
                                                 ('jumpProb', 2),
                                                 ('pUnitGamma',-5),
                                                 ('pUnitGamma', 20)])
    def test_dream_jumpprob_pUnitGamma_error(self, property:str, value: int) -> None:
        """Tests the jumpprob pUnitGamma setter errors in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, property, value)
        assert exp.value.errors()[0]['msg'] in ["Input should be greater than 0",
                                                "Input should be less than 1"]

    @pytest.mark.parametrize("value", [-80, -2])
    def test_dream_nSamples_error(self, value: int) -> None:
        """Tests the nSamples setter error in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, 'nSamples', value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than or equal to 0"

    @pytest.mark.parametrize("value", [-5, 0])
    def test_dream_nChains_error(self, value: int) -> None:
        """Tests the nChains setter error in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, 'nChains', value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than 0"

    def test_dream_extra_property_error(self) -> None:
        """Tests the extra property setter in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, 'test', 1)
        assert exp.value.errors()[0]['msg'] == ("Object has no attribute 'test'")

    def test_dream_procedure_error(self) -> None:
        """Tests the procedure property frozen in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Field is frozen"


class TestControlsClass:
    """
    Tests the Controls class.
    """

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.controls = ControlsClass()

    def test_controls_class_default_type(self) -> None:
        """Tests the procedure is Calculate in ControlsClass."""
        assert type(self.controls.controls).__name__ == "Calculate"

    def test_controls_class_properties(self) -> None:
        """Tests the ControlsClass has control property."""
        assert hasattr(self.controls, 'controls')

    @pytest.mark.parametrize("procedure, name", [(Procedures.Calculate, "Calculate"),
                                                 (Procedures.Simplex, "Simplex"),
                                                 (Procedures.DE, "DE"),
                                                 (Procedures.NS, "NS"),
                                                 (Procedures.Dream, "Dream")])
    def test_controls_class_return_type(self, procedure: Procedures, name: str) -> None:
        """Tests the ControlsClass is set to the correct procedure class."""
        controls = ControlsClass(procedure)
        assert type(controls.controls).__name__ == name

    def test_control_class_calculate_repr(self) -> None:
        """Tests the __repr__ of ControlsClass with Calculate procedure."""
        controls = ControlsClass()
        table = controls.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "procedure         calculate")
        assert table == table_str

    def test_control_class_simplex_repr(self) -> None:
        """Tests the __repr__ of ControlsClass with Simplex procedure."""
        controls = ControlsClass(procedure=Procedures.Simplex)
        table = controls.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "procedure         simplex\n"
                     "tolX              1e-06\n"
                     "tolFun            1e-06\n"
                     "maxFunEvals       10000\n"
                     "maxIter           1000\n"
                     "updateFreq        -1\n"
                     "updatePlotFreq    -1")
        assert table == table_str

    def test_control_class_de_repr(self) -> None:
        """Tests the __repr__ of ControlsClass with DE procedure."""
        controls = ControlsClass(procedure=Procedures.DE)
        table = controls.__repr__()
        table_str = ("Property              Value\n"
                     "--------------------  -----------------------------------------\n"
                     "parallel              single\n"
                     "calcSldDuringFit      False\n"
                     "resamPars             [0.9, 50]\n"
                     "display               iter\n"
                     "procedure             de\n"
                     "populationSize        20\n"
                     "fWeight               0.5\n"
                     "crossoverProbability  0.8\n"
                     "strategy              StrategyOptions.RandomWithPerVectorDither\n"
                     "targetValue           1.0\n"
                     "numGenerations        500")
        assert table == table_str

    def test_control_class_ns_repr(self) -> None:
        """Tests the __repr__ of ControlsClass with NS procedure."""
        controls = ControlsClass(procedure=Procedures.NS)
        table = controls.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "procedure         ns\n"
                     "Nlive             150\n"
                     "Nmcmc             0.0\n"
                     "propScale         0.1\n"
                     "nsTolerance       0.1")
        assert table == table_str

    def test_control_class_dream_repr(self) -> None:
        """Tests the __repr__ of ControlsClass with Dream procedure."""
        controls = ControlsClass(procedure=Procedures.Dream)
        table = controls.__repr__()
        table_str = ("Property          Value\n"
                     "----------------  ---------\n"
                     "parallel          single\n"
                     "calcSldDuringFit  False\n"
                     "resamPars         [0.9, 50]\n"
                     "display           iter\n"
                     "procedure         dream\n"
                     "nSamples          50000\n"
                     "nChains           10\n"
                     "jumpProb          0.5\n"
                     "pUnitGamma        0.2\n"
                     "boundHandling     fold")
        assert table == table_str
