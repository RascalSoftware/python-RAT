"""Tests the enums module."""

from typing import Callable

import pytest

from RATapi.utils.enums import (
    BackgroundActions,
    BoundHandling,
    Calculations,
    Display,
    Geometries,
    Hydration,
    Languages,
    LayerModels,
    Parallel,
    Priors,
    Procedures,
    RATEnum,
    Strategies,
    TypeOptions,
)


@pytest.mark.parametrize(
    ["test_enum", "test_str", "expected_value"],
    [
        (Procedures, "calculate", Procedures.Calculate),
        (Procedures, "simplex", Procedures.Simplex),
        (Procedures, "ns", Procedures.NS),
        (Procedures, "de", Procedures.DE),
        (Procedures, "dream", Procedures.DREAM),
        (Parallel, "single", Parallel.Single),
        (Parallel, "points", Parallel.Points),
        (Parallel, "contrasts", Parallel.Contrasts),
        (Display, "off", Display.Off),
        (Display, "iter", Display.Iter),
        (Display, "notify", Display.Notify),
        (Display, "final", Display.Final),
        (BoundHandling, "off", BoundHandling.Off),
        (BoundHandling, "reflect", BoundHandling.Reflect),
        (BoundHandling, "bound", BoundHandling.Bound),
        (BoundHandling, "fold", BoundHandling.Fold),
        (TypeOptions, "constant", TypeOptions.Constant),
        (TypeOptions, "data", TypeOptions.Data),
        (TypeOptions, "function", TypeOptions.Function),
        (BackgroundActions, "add", BackgroundActions.Add),
        (BackgroundActions, "subtract", BackgroundActions.Subtract),
        (Languages, "cpp", Languages.Cpp),
        (Languages, "python", Languages.Python),
        (Languages, "matlab", Languages.Matlab),
        (Hydration, "none", Hydration.None_),
        (Hydration, "bulk in", Hydration.BulkIn),
        (Hydration, "bulk out", Hydration.BulkOut),
        (Priors, "uniform", Priors.Uniform),
        (Priors, "gaussian", Priors.Gaussian),
        (Calculations, "normal", Calculations.Normal),
        (Calculations, "domains", Calculations.Domains),
        (LayerModels, "custom layers", LayerModels.CustomLayers),
        (LayerModels, "custom xy", LayerModels.CustomXY),
        (LayerModels, "standard layers", LayerModels.StandardLayers),
        (Geometries, "air/substrate", Geometries.AirSubstrate),
        (Geometries, "substrate/liquid", Geometries.SubstrateLiquid),
    ],
)
def test_case_insensitivity(test_enum: Callable, test_str: str, expected_value: RATEnum) -> None:
    """RAT Enums should accept string input regardless of case."""
    assert test_enum(test_str) == expected_value
    assert test_enum(test_str.upper()) == expected_value
    assert test_enum(test_str.title()) == expected_value


@pytest.mark.parametrize(
    ["test_enum", "test_str", "expected_value"],
    [
        (Hydration, "bulk in", Hydration.BulkIn),
        (Hydration, "bulk out", Hydration.BulkOut),
        (Calculations, "normal", Calculations.Normal),
        (LayerModels, "custom layers", LayerModels.CustomLayers),
        (LayerModels, "custom xy", LayerModels.CustomXY),
        (LayerModels, "standard layers", LayerModels.StandardLayers),
    ],
)
def test_dash_underscore(test_enum: Callable, test_str: str, expected_value: RATEnum) -> None:
    """RAT Enums should accept dashes or underscores between words with string input."""
    assert test_enum(test_str.replace(" ", "-")) == expected_value
    assert test_enum(test_str.replace(" ", "_")) == expected_value


@pytest.mark.parametrize(
    ["test_enum", "test_str", "expected_value"],
    [
        (Languages, "c++", Languages.Cpp),
    ],
)
def test_alternative_spellings(test_enum: Callable, test_str: str, expected_value: RATEnum) -> None:
    """RAT Enums should accept some alternative spellings."""
    assert test_enum(test_str) == expected_value
    assert test_enum(test_str.upper()) == expected_value
    assert test_enum(test_str.title()) == expected_value


def test_integer_strategies():
    """Test that strategies can be input as integers."""
    assert [Strategies(i) for i in range(1, len(Strategies) + 1)] == list(Strategies)
