"""Test the controls module."""

from typing import Any, Union

import pydantic
import pytest

from RATapi.controls import Controls, fields
from RATapi.utils.enums import BoundHandling, Display, Parallel, Procedures, Strategies


def test_initialise_procedure_error() -> None:
    """Tests for a ValidationError if the procedure property of the Controls class is initialised with an invalid
    value.
    """
    with pytest.raises(pydantic.ValidationError, match="Input should be 'calculate', 'simplex', 'DE', 'NS' or 'DREAM'"):
        Controls(procedure="test")


def test_set_procedure_error() -> None:
    """Tests for a ValidationError if the procedure property of the Controls class is set to an invalid value."""
    controls = Controls()
    with pytest.raises(pydantic.ValidationError, match="Input should be 'calculate', 'simplex', 'DE', 'NS' or 'DREAM'"):
        controls.procedure = "test"


def test_extra_property_error() -> None:
    """Tests the extra property setter in the Controls class."""
    controls = Controls()
    with pytest.raises(pydantic.ValidationError, match="Object has no attribute 'test'"):
        controls.test = 1


class TestCalculate:
    """Tests the Calculate class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.calculate = Controls()

    @pytest.fixture
    def table_str(self):
        table_str = (
            "+------------------+-----------+\n"
            "|     Property     |   Value   |\n"
            "+------------------+-----------+\n"
            "|    procedure     | calculate |\n"
            "|     parallel     |   single  |\n"
            "| calcSldDuringFit |   False   |\n"
            "|  resampleParams  | [0.9, 50] |\n"
            "|     display      |    iter   |\n"
            "+------------------+-----------+"
        )

        return table_str

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Single),
            ("calcSldDuringFit", False),
            ("resampleParams", [0.9, 50]),
            ("display", Display.Iter),
            ("procedure", Procedures.Calculate),
        ],
    )
    def test_calculate_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Calculate class."""
        assert getattr(self.calculate, control_property) == value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Points),
            ("calcSldDuringFit", True),
            ("resampleParams", [0.2, 1]),
            ("display", Display.Notify),
        ],
    )
    def test_calculate_property_setters(self, control_property: str, value: Any) -> None:
        """Tests the setters of Calculate class."""
        setattr(self.calculate, control_property, value)
        assert getattr(self.calculate, control_property) == value

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_initialise_non_calculate_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "calculate", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "calculate" controls procedure are:\n'
            f'    {", ".join(fields["calculate"])}\n',
        ):
            Controls(procedure=Procedures.Calculate, **{wrong_property: value})

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_set_non_calculate_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "calculate", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "calculate" controls procedure are:\n'
            f'    {", ".join(fields["calculate"])}\n',
        ):
            setattr(self.calculate, wrong_property, value)

    @pytest.mark.parametrize("value", ["test", "ALL", "Contrast", True, 1, 3.0])
    def test_calculate_parallel_validation(self, value: Any) -> None:
        """Tests the parallel setter validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be 'single', 'points' or 'contrasts'"):
            self.calculate.parallel = value

    @pytest.mark.parametrize("value", [5.0, 12])
    def test_calculate_calcSldDuringFit_validation(self, value: Union[int, float]) -> None:
        """Tests the calcSldDuringFit setter validation in Calculate class."""
        with pytest.raises(
            pydantic.ValidationError, match="Input should be a valid boolean, unable to interpret input"
        ):
            self.calculate.calcSldDuringFit = value

    @pytest.mark.parametrize("value", ["test", "iterate", True, 1, 3.0])
    def test_calculate_display_validation(self, value: Any) -> None:
        """Tests the display setter validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be 'off', 'iter', 'notify' or 'final'"):
            self.calculate.display = value

    @pytest.mark.parametrize(
        "value, msg",
        [
            ([5.0], "List should have at least 2 items after validation, not 1"),
            ([12, 13, 14], "List should have at most 2 items after validation, not 3"),
        ],
    )
    def test_calculate_resampleParams_length_validation(self, value: list, msg: str) -> None:
        """Tests the resampleParams setter length validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError, match=msg):
            self.calculate.resampleParams = value

    @pytest.mark.parametrize(
        "value, msg",
        [
            ([1.0, 2], "Value error, resampleParams[0] must be between 0 and 1"),
            ([0.5, -0.1], "Value error, resampleParams[1] must be greater than or equal to 0"),
        ],
    )
    def test_calculate_resampleParams_value_validation(self, value: list, msg: str) -> None:
        """Tests the resampleParams setter value validation in Calculate class."""
        with pytest.raises(pydantic.ValidationError) as exp:
            self.calculate.resampleParams = value
        assert exp.value.errors()[0]["msg"] == msg

    def test_str(self, table_str) -> None:
        """Tests the Calculate model __str__."""
        assert self.calculate.__str__() == table_str


class TestSimplex:
    """Tests the Simplex class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.simplex = Controls(procedure=Procedures.Simplex)

    @pytest.fixture
    def table_str(self):
        table_str = (
            "+------------------+-----------+\n"
            "|     Property     |   Value   |\n"
            "+------------------+-----------+\n"
            "|    procedure     |  simplex  |\n"
            "|     parallel     |   single  |\n"
            "| calcSldDuringFit |   False   |\n"
            "|  resampleParams  | [0.9, 50] |\n"
            "|     display      |    iter   |\n"
            "|    xTolerance    |   1e-06   |\n"
            "|  funcTolerance   |   1e-06   |\n"
            "|   maxFuncEvals   |   10000   |\n"
            "|  maxIterations   |    1000   |\n"
            "|    updateFreq    |     1     |\n"
            "|  updatePlotFreq  |     20    |\n"
            "+------------------+-----------+"
        )

        return table_str

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Single),
            ("calcSldDuringFit", False),
            ("resampleParams", [0.9, 50]),
            ("display", Display.Iter),
            ("procedure", Procedures.Simplex),
            ("xTolerance", 1e-6),
            ("funcTolerance", 1e-6),
            ("maxFuncEvals", 10000),
            ("maxIterations", 1000),
            ("updateFreq", 1),
            ("updatePlotFreq", 20),
        ],
    )
    def test_simplex_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Simplex class."""
        assert getattr(self.simplex, control_property) == value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Points),
            ("calcSldDuringFit", True),
            ("resampleParams", [0.2, 1]),
            ("display", Display.Notify),
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
        ],
    )
    def test_simplex_property_setters(self, control_property: str, value: Any) -> None:
        """Tests the setters of Simplex class."""
        setattr(self.simplex, control_property, value)
        assert getattr(self.simplex, control_property) == value

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_initialise_non_simplex_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "simplex", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "simplex" controls procedure are:\n'
            f'    {", ".join(fields["simplex"])}\n',
        ):
            Controls(procedure=Procedures.Simplex, **{wrong_property: value})

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_set_non_simplex_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "simplex", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "simplex" controls procedure are:\n'
            f'    {", ".join(fields["simplex"])}\n',
        ):
            setattr(self.simplex, wrong_property, value)

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("xTolerance", -4e-6),
            ("funcTolerance", -3e-4),
            ("maxFuncEvals", -100),
            ("maxIterations", -50),
        ],
    )
    def test_simplex_property_errors(self, control_property: str, value: Union[float, int]) -> None:
        """Tests the property errors of Simplex class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be greater than 0"):
            setattr(self.simplex, control_property, value)

    def test_str(self, table_str) -> None:
        """Tests the Simplex model __str__."""
        assert self.simplex.__str__() == table_str


class TestDE:
    """Tests the DE class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.de = Controls(procedure=Procedures.DE)

    @pytest.fixture
    def table_str(self):
        table_str = (
            "+----------------------+--------------------------------------+\n"
            "|       Property       |                Value                 |\n"
            "+----------------------+--------------------------------------+\n"
            "|      procedure       |                  DE                  |\n"
            "|       parallel       |                single                |\n"
            "|   calcSldDuringFit   |                False                 |\n"
            "|    resampleParams    |              [0.9, 50]               |\n"
            "|       display        |                 iter                 |\n"
            "|    populationSize    |                  20                  |\n"
            "|       fWeight        |                 0.5                  |\n"
            "| crossoverProbability |                 0.8                  |\n"
            "|       strategy       | Strategies.RandomWithPerVectorDither |\n"
            "|     targetValue      |                 1.0                  |\n"
            "|    numGenerations    |                 500                  |\n"
            "|      updateFreq      |                  1                   |\n"
            "|    updatePlotFreq    |                  20                  |\n"
            "+----------------------+--------------------------------------+"
        )

        return table_str

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Single),
            ("calcSldDuringFit", False),
            ("resampleParams", [0.9, 50]),
            ("display", Display.Iter),
            ("procedure", Procedures.DE),
            ("populationSize", 20),
            ("fWeight", 0.5),
            ("crossoverProbability", 0.8),
            ("strategy", Strategies.RandomWithPerVectorDither),
            ("targetValue", 1),
            ("numGenerations", 500),
        ],
    )
    def test_de_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of DE class."""
        assert getattr(self.de, control_property) == value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Points),
            ("calcSldDuringFit", True),
            ("resampleParams", [0.2, 1]),
            ("display", Display.Notify),
            ("populationSize", 20),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
        ],
    )
    def test_de_property_setters(self, control_property: str, value: Any) -> None:
        """Tests the setters of DE class."""
        setattr(self.de, control_property, value)
        assert getattr(self.de, control_property) == value

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_initialise_non_de_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "DE", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "DE" controls procedure are:\n'
            f'    {", ".join(fields["DE"])}\n',
        ):
            Controls(procedure=Procedures.DE, **{wrong_property: value})

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_set_non_de_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "DE", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "DE" controls procedure are:\n'
            f'    {", ".join(fields["DE"])}\n',
        ):
            setattr(self.de, wrong_property, value)

    @pytest.mark.parametrize(
        "value, msg",
        [
            (0, "Input should be greater than 0"),
            (2, "Input should be less than 1"),
        ],
    )
    def test_de_crossoverProbability_error(self, value: int, msg: str) -> None:
        """Tests the crossoverProbability setter error in DE class."""
        with pytest.raises(pydantic.ValidationError, match=msg):
            self.de.crossoverProbability = value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("targetValue", 0),
            ("targetValue", 0.999),
            ("numGenerations", -500),
            ("numGenerations", 0),
            ("populationSize", 0),
            ("populationSize", -1),
        ],
    )
    def test_de_targetValue_numGenerations_populationSize_error(
        self,
        control_property: str,
        value: Union[int, float],
    ) -> None:
        """Tests the targetValue, numGenerations, populationSize setter error in DE class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be greater than or equal to 1"):
            setattr(self.de, control_property, value)

    def test_str(self, table_str) -> None:
        """Tests the DE model __str__."""
        assert self.de.__str__() == table_str


class TestNS:
    """Tests the NS class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.ns = Controls(procedure=Procedures.NS)

    @pytest.fixture
    def table_str(self):
        table_str = (
            "+------------------+-----------+\n"
            "|     Property     |   Value   |\n"
            "+------------------+-----------+\n"
            "|    procedure     |     NS    |\n"
            "|     parallel     |   single  |\n"
            "| calcSldDuringFit |   False   |\n"
            "|  resampleParams  | [0.9, 50] |\n"
            "|     display      |    iter   |\n"
            "|      nLive       |    150    |\n"
            "|      nMCMC       |    0.0    |\n"
            "|    propScale     |    0.1    |\n"
            "|   nsTolerance    |    0.1    |\n"
            "+------------------+-----------+"
        )

        return table_str

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Single),
            ("calcSldDuringFit", False),
            ("resampleParams", [0.9, 50]),
            ("display", Display.Iter),
            ("procedure", Procedures.NS),
            ("nLive", 150),
            ("nMCMC", 0),
            ("propScale", 0.1),
            ("nsTolerance", 0.1),
        ],
    )
    def test_ns_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of NS class."""
        assert getattr(self.ns, control_property) == value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Points),
            ("calcSldDuringFit", True),
            ("resampleParams", [0.2, 1]),
            ("display", Display.Notify),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
        ],
    )
    def test_ns_property_setters(self, control_property: str, value: Any) -> None:
        """Tests the setters of NS class."""
        setattr(self.ns, control_property, value)
        assert getattr(self.ns, control_property) == value

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_initialise_non_ns_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "NS", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "NS" controls procedure are:\n'
            f'    {", ".join(fields["NS"])}\n',
        ):
            Controls(procedure=Procedures.NS, **{wrong_property: value})

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_set_non_ns_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "NS", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "NS" controls procedure are:\n'
            f'    {", ".join(fields["NS"])}\n',
        ):
            setattr(self.ns, wrong_property, value)

    @pytest.mark.parametrize(
        "control_property, value, bound",
        [
            ("nMCMC", -0.6, 0),
            ("nsTolerance", -500, 0),
            ("nLive", -500, 1),
        ],
    )
    def test_ns_setter_error(self, control_property: str, value: Union[int, float], bound: int) -> None:
        """Tests the nMCMC, nsTolerance, nLive setter error in NS class."""
        with pytest.raises(pydantic.ValidationError, match=f"Input should be greater than or equal to {bound}"):
            setattr(self.ns, control_property, value)

    @pytest.mark.parametrize(
        "value, msg",
        [
            (0, "Input should be greater than 0"),
            (2, "Input should be less than 1"),
        ],
    )
    def test_ns_propScale_error(self, value: int, msg: str) -> None:
        """Tests the propScale error in NS class."""
        with pytest.raises(pydantic.ValidationError, match=msg):
            self.ns.propScale = value

    def test_control_class_ns_str(self, table_str) -> None:
        """Tests the NS model __str__."""
        assert self.ns.__str__() == table_str


class TestDream:
    """Tests the Dream class."""

    @pytest.fixture(autouse=True)
    def setup_class(self):
        self.dream = Controls(procedure=Procedures.DREAM)

    @pytest.fixture
    def table_str(self):
        table_str = (
            "+------------------+-----------+\n"
            "|     Property     |   Value   |\n"
            "+------------------+-----------+\n"
            "|    procedure     |   DREAM   |\n"
            "|     parallel     |   single  |\n"
            "| calcSldDuringFit |   False   |\n"
            "|  resampleParams  | [0.9, 50] |\n"
            "|     display      |    iter   |\n"
            "|     nSamples     |   20000   |\n"
            "|     nChains      |     10    |\n"
            "| jumpProbability  |    0.5    |\n"
            "|    pUnitGamma    |    0.2    |\n"
            "|  boundHandling   |  reflect  |\n"
            "|     adaptPCR     |    True   |\n"
            "+------------------+-----------+"
        )

        return table_str

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Single),
            ("calcSldDuringFit", False),
            ("resampleParams", [0.9, 50]),
            ("display", Display.Iter),
            ("procedure", Procedures.DREAM),
            ("nSamples", 20000),
            ("nChains", 10),
            ("jumpProbability", 0.5),
            ("pUnitGamma", 0.2),
            ("boundHandling", BoundHandling.Reflect),
            ("adaptPCR", True),
        ],
    )
    def test_dream_property_values(self, control_property: str, value: Any) -> None:
        """Tests the default values of Dream class."""
        assert getattr(self.dream, control_property) == value

    @pytest.mark.parametrize(
        "control_property, value",
        [
            ("parallel", Parallel.Points),
            ("calcSldDuringFit", True),
            ("resampleParams", [0.2, 1]),
            ("display", Display.Notify),
            ("nSamples", 500),
            ("nChains", 1000),
            ("jumpProbability", 0.7),
            ("pUnitGamma", 0.3),
            ("boundHandling", BoundHandling.Fold),
            ("adaptPCR", False),
        ],
    )
    def test_dream_property_setters(self, control_property: str, value: Any) -> None:
        """Tests the setters in Dream class."""
        setattr(self.dream, control_property, value)
        assert getattr(self.dream, control_property) == value

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
        ],
    )
    def test_initialise_non_dream_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "DREAM", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "DREAM" controls procedure are:\n'
            f'    {", ".join(fields["DREAM"])}\n',
        ):
            Controls(procedure=Procedures.DREAM, **{wrong_property: value})

    @pytest.mark.parametrize(
        "wrong_property, value",
        [
            ("xTolerance", 4e-6),
            ("funcTolerance", 3e-4),
            ("maxFuncEvals", 100),
            ("maxIterations", 50),
            ("updateFreq", 4),
            ("updatePlotFreq", 3),
            ("populationSize", 200),
            ("fWeight", 0.3),
            ("crossoverProbability", 0.4),
            ("strategy", Strategies.BestWithJitter),
            ("targetValue", 2.0),
            ("numGenerations", 50),
            ("nLive", 1500),
            ("nMCMC", 1),
            ("propScale", 0.5),
            ("nsTolerance", 0.8),
        ],
    )
    def test_set_non_dream_properties(self, wrong_property: str, value: Any) -> None:
        incorrect_procedures = [key for (key, value) in fields.items() if wrong_property in value]
        with pytest.warns(
            UserWarning,
            match=f'\nThe current controls procedure is "DREAM", but the property'
            f' "{wrong_property}" applies instead to the'
            f' {", ".join(incorrect_procedures)} procedure.\n\n'
            f' The fields for the "DREAM" controls procedure are:\n'
            f'    {", ".join(fields["DREAM"])}\n',
        ):
            setattr(self.dream, wrong_property, value)

    @pytest.mark.parametrize(
        "control_property, value, msg",
        [
            ("jumpProbability", 0, "Input should be greater than 0"),
            ("jumpProbability", 2, "Input should be less than 1"),
            ("pUnitGamma", -5, "Input should be greater than 0"),
            ("pUnitGamma", 20, "Input should be less than 1"),
        ],
    )
    def test_dream_jumpProbability_pUnitGamma_error(self, control_property: str, value: int, msg: str) -> None:
        """Tests the jumpProbability and pUnitGamma setter errors in Dream class."""
        with pytest.raises(pydantic.ValidationError, match=msg):
            setattr(self.dream, control_property, value)

    @pytest.mark.parametrize("value", [-80, -2])
    def test_dream_nSamples_error(self, value: int) -> None:
        """Tests the nSamples setter error in Dream class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be greater than or equal to 0"):
            self.dream.nSamples = value

    @pytest.mark.parametrize("value", [-5, 0])
    def test_dream_nChains_error(self, value: int) -> None:
        """Tests the nChains setter error in Dream class."""
        with pytest.raises(pydantic.ValidationError, match="Input should be greater than 0"):
            self.dream.nChains = value

    def test_control_class_dream_str(self, table_str) -> None:
        """Tests the Dream model __str__."""
        assert self.dream.__str__() == table_str
