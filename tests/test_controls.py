"""Test the controls module."""

import pytest
import pydantic
from typing import Union, Any

from RAT.controls import Calculate, Simplex, DE, NS, Dream, set_controls
from RAT.utils.enums import ParallelOptions, Procedures, DisplayOptions, BoundHandlingOptions, StrategyOptions


class TestCalculate:
    """Tests the Calculate class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.calculate = Calculate()

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.Single),
        ('calcSldDuringFit', False),
        ('resamPars', [0.9, 50]),
        ('display', DisplayOptions.Iter),
        ('procedure', Procedures.Calculate)
    ])
    def test_calculate_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Calculate class."""
        assert getattr(self.calculate, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.All),
        ('calcSldDuringFit', True),
        ('resamPars', [0.2, 1]),
        ('display', DisplayOptions.Notify)
    ])
    def test_calculate_property_setters(self, control_property: str,  value: Any) -> None:
        """Tests the setters of Calculate class."""
        setattr(self.calculate, control_property, value)
        assert getattr(self.calculate, control_property) == value

    @pytest.mark.parametrize("var1, var2", [('test', True), ('ALL', 1), ("Contrast", 3.0)])
    def test_calculate_parallel_validation(self, var1: str, var2: Any) -> None:
        """Tests the parallel setter validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'parallel', var1)
        assert exp.value.errors()[0]['msg'] == "Input should be 'single', 'points', 'contrasts' or 'all'"
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'parallel', var2)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid string"

    @pytest.mark.parametrize("value", [5.0, 12])
    def test_calculate_calcSldDuringFit_validation(self, value: Union[int, float]) -> None:
        """Tests the calcSldDuringFit setter validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'calcSldDuringFit', value)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid boolean, unable to interpret input"

    @pytest.mark.parametrize("var1, var2", [('test', True), ('iterate', 1), ("FINAL", 3.0)])
    def test_calculate_display_validation(self, var1: str, var2: Any) -> None:
        """Tests the display setter validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'display', var1)
        assert exp.value.errors()[0]['msg'] == "Input should be 'off', 'iter', 'notify' or 'final'"
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'display', var2)
        assert exp.value.errors()[0]['msg'] == "Input should be a valid string"

    @pytest.mark.parametrize("value, msg", [
        ([5.0], "List should have at least 2 items after validation, not 1"),
        ([12, 13, 14], "List should have at most 2 items after validation, not 3")
    ])
    def test_calculate_resamPars_length_validation(self, value: list, msg: str) -> None:
        """Tests the resamPars setter length validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'resamPars', value)
        assert exp.value.errors()[0]['msg'] == msg

    @pytest.mark.parametrize("value, msg", [
        ([1.0, 2], "Value error, resamPars[0] must be between 0 and 1"),
        ([0.5, -0.1], "Value error, resamPars[1] must be greater than or equal to 0")
    ])
    def test_calculate_resamPars_value_validation(self, value: list, msg: str) -> None:
        """Tests the resamPars setter value validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'resamPars', value)
        assert exp.value.errors()[0]['msg'] == msg

    def test_calculate_extra_property_error(self) -> None:
        """Tests the extra property setter in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'test', 1)
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"

    def test_calculate_initialise_procedure_error(self) -> None:
        """Tests the procedure property can only be initialised as "calculate" in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            Calculate(procedure='test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Calculate: 'calculate'>"

    def test_calculate_set_procedure_error(self) -> None:
        """Tests the procedure property is frozen in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.calculate, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Calculate: 'calculate'>"

    def test_repr(self) -> None:
        """Tests the Calculate model __repr__."""
        table = self.calculate.__repr__()
        table_str = ("+------------------+-----------+\n"
                     "|     Property     |   Value   |\n"
                     "+------------------+-----------+\n"
                     "|    procedure     | calculate |\n"
                     "|     parallel     |   single  |\n"
                     "| calcSldDuringFit |   False   |\n"
                     "|    resamPars     | [0.9, 50] |\n"
                     "|     display      |    iter   |\n"
                     "+------------------+-----------+"
                     )

        assert table == table_str


class TestSimplex:
    """Tests the Simplex class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.simplex = Simplex()

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.Single),
        ('calcSldDuringFit', False),
        ('resamPars', [0.9, 50]),
        ('display', DisplayOptions.Iter),
        ('procedure', Procedures.Simplex),
        ('tolX', 1e-6),
        ('tolFun', 1e-6),
        ('maxFunEvals', 10000),
        ('maxIter', 1000),
        ('updateFreq', -1),
        ('updatePlotFreq', -1)
    ])
    def test_simplex_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Simplex class."""
        assert getattr(self.simplex, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.All),
        ('calcSldDuringFit', True),
        ('resamPars', [0.2, 1]),
        ('display', DisplayOptions.Notify),
        ('tolX', 4e-6),
        ('tolFun', 3e-4),
        ('maxFunEvals', 100),
        ('maxIter', 50),
        ('updateFreq', 4),
        ('updatePlotFreq', 3)
    ])
    def test_simplex_property_setters(self, control_property: str,  value: Any) -> None:
        """Tests the setters of Simplex class."""
        setattr(self.simplex, control_property, value)
        assert getattr(self.simplex, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('tolX', -4e-6),
        ('tolFun', -3e-4),
        ('maxFunEvals', -100),
        ('maxIter', -50)
    ])
    def test_simplex_property_errors(self, control_property: str,  value: Union[float, int]) -> None:
        """Tests the property errors of Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, control_property, value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than 0"

    def test_simplex_extra_property_error(self) -> None:
        """Tests the extra property setter in Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, 'test', 1)
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"

    def test_simplex_initialise_procedure_error(self) -> None:
        """Tests the procedure property can only be initialised as "simplex" in Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            Simplex(procedure='test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Simplex: 'simplex'>"

    def test_simplex_set_procedure_error(self) -> None:
        """Tests the procedure property is frozen in Simplex class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.simplex, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Simplex: 'simplex'>"

    def test_repr(self) -> None:
        """Tests the Simplex model __repr__."""
        table = self.simplex.__repr__()
        table_str = ("+------------------+-----------+\n"
                     "|     Property     |   Value   |\n"
                     "+------------------+-----------+\n"
                     "|    procedure     |  simplex  |\n"
                     "|     parallel     |   single  |\n"
                     "| calcSldDuringFit |   False   |\n"
                     "|    resamPars     | [0.9, 50] |\n"
                     "|     display      |    iter   |\n"                     
                     "|       tolX       |   1e-06   |\n"
                     "|      tolFun      |   1e-06   |\n"
                     "|   maxFunEvals    |   10000   |\n"
                     "|     maxIter      |    1000   |\n"
                     "|    updateFreq    |     -1    |\n"
                     "|  updatePlotFreq  |     -1    |\n"
                     "+------------------+-----------+"
                     )

        assert table == table_str


class TestDE:
    """Tests the DE class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.de = DE()

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.Single),
        ('calcSldDuringFit', False),
        ('resamPars', [0.9, 50]),
        ('display', DisplayOptions.Iter),
        ('procedure', Procedures.DE),
        ('populationSize', 20),
        ('fWeight', 0.5),
        ('crossoverProbability', 0.8),
        ('strategy', StrategyOptions.RandomWithPerVectorDither),
        ('targetValue', 1),
        ('numGenerations', 500)
    ])
    def test_de_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of DE class."""
        assert getattr(self.de, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.All),
        ('calcSldDuringFit', True),
        ('resamPars', [0.2, 1]),
        ('display', DisplayOptions.Notify),
        ('populationSize', 20),
        ('fWeight', 0.3),
        ('crossoverProbability', 0.4),
        ('strategy', StrategyOptions.BestWithJitter),
        ('targetValue', 2.0),
        ('numGenerations', 50)
    ])
    def test_de_property_setters(self, control_property: str,  value: Any) -> None:
        """Tests the setters of DE class."""
        setattr(self.de, control_property, value)
        assert getattr(self.de, control_property) == value

    @pytest.mark.parametrize("value, msg", [
        (0, "Input should be greater than 0"),
        (2, "Input should be less than 1")
    ])
    def test_de_crossoverProbability_error(self,  value: int, msg: str) -> None:
        """Tests the crossoverProbability setter error in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'crossoverProbability', value)
        assert exp.value.errors()[0]['msg'] == msg

    @pytest.mark.parametrize("control_property, value", [
        ('targetValue', 0),
        ('targetValue', 0.999),
        ('numGenerations', -500),
        ('numGenerations', 0),
        ('populationSize', 0),
        ('populationSize', -1)
    ])
    def test_de_targetValue_numGenerations_populationSize_error(self,
                                                                control_property: str,
                                                                value: Union[int, float]) -> None:
        """Tests the targetValue, numGenerations, populationSize setter error in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, control_property, value)
        assert exp.value.errors()[0]['msg'] == "Input should be greater than or equal to 1"

    def test_de_extra_property_error(self) -> None:
        """Tests the extra property setter in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'test', 1)
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"

    def test_de_initialise_procedure_error(self) -> None:
        """Tests the procedure property can only be initialised as "de" in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            DE(procedure='test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.DE: 'de'>"

    def test_de_set_procedure_error(self) -> None:
        """Tests the procedure property is frozen in DE class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.de, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.DE: 'de'>"

    def test_repr(self) -> None:
        """Tests the DE model __repr__."""
        table = self.de.__repr__()
        table_str = ("+----------------------+-------------------------------------------+\n"
                     "|       Property       |                   Value                   |\n"
                     "+----------------------+-------------------------------------------+\n"
                     "|      procedure       |                     de                    |\n"
                     "|       parallel       |                   single                  |\n"
                     "|   calcSldDuringFit   |                   False                   |\n"
                     "|      resamPars       |                 [0.9, 50]                 |\n"
                     "|       display        |                    iter                   |\n"
                     "|    populationSize    |                     20                    |\n"
                     "|       fWeight        |                    0.5                    |\n"
                     "| crossoverProbability |                    0.8                    |\n"
                     "|       strategy       | StrategyOptions.RandomWithPerVectorDither |\n"
                     "|     targetValue      |                    1.0                    |\n"
                     "|    numGenerations    |                    500                    |\n"
                     "+----------------------+-------------------------------------------+"
                     )

        assert table == table_str


class TestNS:
    """Tests the NS class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.ns = NS()

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.Single),
        ('calcSldDuringFit', False),
        ('resamPars', [0.9, 50]),
        ('display', DisplayOptions.Iter),
        ('procedure', Procedures.NS),
        ('Nlive', 150),
        ('Nmcmc', 0),
        ('propScale', 0.1),
        ('nsTolerance', 0.1)
    ])
    def test_ns_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of NS class."""
        assert getattr(self.ns, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.All),
        ('calcSldDuringFit', True),
        ('resamPars', [0.2, 1]),
        ('display', DisplayOptions.Notify),
        ('Nlive', 1500),
        ('Nmcmc', 1),
        ('propScale', 0.5),
        ('nsTolerance', 0.8)
    ])
    def test_ns_property_setters(self, control_property: str,  value: Any) -> None:
        """Tests the setters of NS class."""
        setattr(self.ns, control_property, value)
        assert getattr(self.ns, control_property) == value

    @pytest.mark.parametrize("control_property, value, bound", [
        ('Nmcmc', -0.6, 0),
        ('nsTolerance', -500, 0),
        ('Nlive', -500, 1)
    ])
    def test_ns_Nmcmc_nsTolerance_Nlive_error(self, control_property: str, value: Union[int, float], bound: int) -> None:
        """Tests the Nmcmc, nsTolerance, Nlive setter error in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, control_property, value)
        assert exp.value.errors()[0]['msg'] == f"Input should be greater than or equal to {bound}"

    @pytest.mark.parametrize("value, msg", [
        (0, "Input should be greater than 0"),
        (2, "Input should be less than 1")
    ])
    def test_ns_propScale_error(self,  value: int, msg: str) -> None:
        """Tests the propScale error in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'propScale', value)
        assert exp.value.errors()[0]['msg'] == msg

    def test_ns_extra_property_error(self) -> None:
        """Tests the extra property setter in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'test', 1)
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"

    def test_ns_initialise_procedure_error(self) -> None:
        """Tests the procedure property can only be initialised as "ns" in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            NS(procedure='test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.NS: 'ns'>"

    def test_ns_procedure_error(self) -> None:
        """Tests the procedure property is frozen in NS class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.ns, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.NS: 'ns'>"

    def test_control_class_ns_repr(self) -> None:
        """Tests the NS model __repr__."""
        table = self.ns.__repr__()
        table_str = ("+------------------+-----------+\n"
                     "|     Property     |   Value   |\n"
                     "+------------------+-----------+\n"
                     "|    procedure     |     ns    |\n"
                     "|     parallel     |   single  |\n"
                     "| calcSldDuringFit |   False   |\n"
                     "|    resamPars     | [0.9, 50] |\n"
                     "|     display      |    iter   |\n"                     
                     "|      Nlive       |    150    |\n"
                     "|      Nmcmc       |    0.0    |\n"
                     "|    propScale     |    0.1    |\n"
                     "|   nsTolerance    |    0.1    |\n"
                     "+------------------+-----------+"
                     )

        assert table == table_str


class TestDream:
    """Tests the Dream class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.dream = Dream()

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.Single),
        ('calcSldDuringFit', False),
        ('resamPars', [0.9, 50]),
        ('display', DisplayOptions.Iter),
        ('procedure', Procedures.Dream),
        ('nSamples', 50000),
        ('nChains', 10),
        ('jumpProb', 0.5),
        ('pUnitGamma', 0.2),
        ('boundHandling', BoundHandlingOptions.Fold)
    ])
    def test_dream_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Dream class."""
        assert getattr(self.dream, control_property) == value

    @pytest.mark.parametrize("control_property, value", [
        ('parallel', ParallelOptions.All),
        ('calcSldDuringFit', True),
        ('resamPars', [0.2, 1]),
        ('display', DisplayOptions.Notify),
        ('nSamples', 500),
        ('nChains', 1000),
        ('jumpProb', 0.7),
        ('pUnitGamma', 0.3),
        ('boundHandling', BoundHandlingOptions.Reflect)
    ])
    def test_dream_property_setters(self, control_property: str,  value: Any) -> None:
        """Tests the setters in Dream class."""
        setattr(self.dream, control_property, value)
        assert getattr(self.dream, control_property) == value

    @pytest.mark.parametrize("control_property, value, msg", [
        ('jumpProb', 0, "Input should be greater than 0"),
        ('jumpProb', 2, "Input should be less than 1"),
        ('pUnitGamma', -5, "Input should be greater than 0"),
        ('pUnitGamma', 20, "Input should be less than 1")
    ])
    def test_dream_jumpProb_pUnitGamma_error(self, control_property: str, value: int, msg: str) -> None:
        """Tests the jumpProb and pUnitGamma setter errors in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, control_property, value)
        assert exp.value.errors()[0]['msg'] == msg

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
        assert exp.value.errors()[0]['msg'] == "Object has no attribute 'test'"

    def test_dream_initialise_procedure_error(self) -> None:
        """Tests the procedure property can only be initialised as "dream" in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            Dream(procedure='test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Dream: 'dream'>"

    def test_dream_procedure_error(self) -> None:
        """Tests the procedure property is frozen in Dream class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            setattr(self.dream, 'procedure', 'test')
        assert exp.value.errors()[0]['msg'] == "Input should be <Procedures.Dream: 'dream'>"

    def test_control_class_dream_repr(self) -> None:
        """Tests the Dream model __repr__."""
        table = self.dream.__repr__()
        table_str = ("+------------------+-----------+\n"
                     "|     Property     |   Value   |\n"
                     "+------------------+-----------+\n"
                     "|    procedure     |   dream   |\n"
                     "|     parallel     |   single  |\n"
                     "| calcSldDuringFit |   False   |\n"
                     "|    resamPars     | [0.9, 50] |\n"
                     "|     display      |    iter   |\n"                     
                     "|     nSamples     |   50000   |\n"
                     "|     nChains      |     10    |\n"
                     "|     jumpProb     |    0.5    |\n"
                     "|    pUnitGamma    |    0.2    |\n"
                     "|  boundHandling   |    fold   |\n"
                     "+------------------+-----------+"
                     )

        assert table == table_str

@pytest.mark.parametrize(["procedure", "expected_model"], [
    ('calculate', Calculate),
    ('simplex', Simplex),
    ('de', DE),
    ('ns', NS),
    ('dream', Dream)
])
def test_set_controls(procedure: Procedures, expected_model: Union[Calculate, Simplex, DE, NS, Dream]) -> None:
    """We should return the correct model given the value of procedure."""
    controls_model = set_controls(procedure)
    assert type(controls_model) == expected_model


def test_set_controls_default_procedure() -> None:
    """We should return the default model when we call "set_controls" without specifying a procedure."""
    controls_model = set_controls()
    assert type(controls_model) == Calculate


def test_set_controls_invalid_procedure() -> None:
    """We should return the default model when we call "set_controls" without specifying a procedure."""
    with pytest.raises(ValueError, match="The controls procedure must be one of: 'calculate', 'simplex', 'de', 'ns' "
                                         "or 'dream'"):
        set_controls('invalid')


@pytest.mark.parametrize(["procedure", "expected_model"], [
    ('calculate', Calculate),
    ('simplex', Simplex),
    ('de', DE),
    ('ns', NS),
    ('dream', Dream)
])
def test_set_controls_extra_fields(procedure: Procedures, expected_model: Union[Calculate, Simplex, DE, NS, Dream])\
        -> None:
    """If we provide extra fields to a controls model through "set_controls", we should print a formatted
    ValidationError with a custom error message.
    """
    with pytest.raises(pydantic.ValidationError, match=f'1 validation error for {expected_model.__name__}\n'
                                                       f'extra_field\n  Extra inputs are not permitted. The fields for '
                                                       f'the {procedure} controls procedure are:\n    '
                                                       f'{", ".join(expected_model.model_fields.keys())}\n'):
        set_controls(procedure, extra_field='invalid')
