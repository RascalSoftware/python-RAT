import os
import pickle
from unittest.mock import MagicMock, patch

import matplotlib.pyplot as plt
import pytest

from RATapi.events import notify
from RATapi.rat_core import EventTypes, PlotEventData
from RATapi.utils.plotting import LivePlot, plot_ref_sld, plot_ref_sld_helper

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


def data() -> PlotEventData:
    """Creates the fixture for the tests."""
    data_path = os.path.join(TEST_DIR_PATH, "plotting_data.pickle")
    with open(data_path, "rb") as f:
        loaded_data = pickle.load(f)

    data = PlotEventData()
    data.modelType = loaded_data["modelType"]
    data.dataPresent = loaded_data["dataPresent"]
    data.subRoughs = loaded_data["subRoughs"]
    data.resample = loaded_data["resample"]
    data.resampledLayers = loaded_data["resampledLayers"]
    data.reflectivity = loaded_data["reflectivity"]
    data.shiftedData = loaded_data["shiftedData"]
    data.sldProfiles = loaded_data["sldProfiles"]
    return data


@pytest.fixture
def fig() -> plt.figure:
    """Creates the fixture for the tests."""
    plt.close("all")
    figure = plt.subplots(1, 3)[0]
    return plot_ref_sld_helper(fig=figure, data=data())


def test_figure_axis_formating(fig: plt.figure) -> None:
    """Tests the axis formating of the figure."""
    ref_plot = fig.axes[0]
    sld_plot = fig.axes[1]

    assert fig.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)
    assert len(fig.axes) == 2

    assert ref_plot.get_xlabel() == "$Q_{z} (\u00c5^{-1})$"
    assert ref_plot.get_xscale() == "log"
    assert ref_plot.get_ylabel() == "Reflectivity"
    assert ref_plot.get_yscale() == "log"
    assert [label._text for label in ref_plot.get_legend().texts] == ["ref 1", "ref 2", "ref 3"]

    assert sld_plot.get_xlabel() == "$Z (\u00c5)$"
    assert sld_plot.get_xscale() == "linear"
    assert sld_plot.get_ylabel() == "$SLD (\u00c5^{-2})$"
    assert sld_plot.get_yscale() == "linear"
    assert [label._text for label in sld_plot.get_legend().texts] == ["sld 1", "sld 2", "sld 3"]


def test_figure_color_formating(fig: plt.figure) -> None:
    """Tests the color formating of the figure."""
    ref_plot = fig.axes[0]
    sld_plot = fig.axes[1]

    assert len(ref_plot.get_lines()) == 3
    assert len(sld_plot.get_lines()) == 6

    for axis_ix in range(len(ref_plot.get_lines())):
        ax1 = axis_ix * 2
        ax2 = ax1 + 1

        # Tests whether the color of the line and the errorbars match on the ref_plot
        assert (
            ref_plot.containers[ax1][2][0]._original_edgecolor
            == ref_plot.containers[ax2][2][0]._original_edgecolor
            == ref_plot.get_lines()[axis_ix].get_color()
        )

        # Tests whether the color of the sld and resampled_sld match on the sld_plot
        assert sld_plot.get_lines()[ax1].get_color() == sld_plot.get_lines()[ax2].get_color()


@patch("RATapi.utils.plotting.makeSLDProfileXY")
def test_sld_profile_function_call(mock: MagicMock) -> None:
    """Tests the makeSLDProfileXY function called with
    correct args.
    """
    plot_ref_sld_helper(data())

    assert mock.call_count == 3
    assert mock.call_args_list[0].args[0] == 2.07e-06
    assert mock.call_args_list[0].args[1] == 6.28e-06
    assert mock.call_args_list[0].args[2] == 0.0
    assert mock.call_args_list[0].args[4] == 82
    assert mock.call_args_list[0].args[5] == 1.0

    assert mock.call_args_list[1].args[0] == 2.07e-06
    assert mock.call_args_list[1].args[1] == 1.83e-06
    assert mock.call_args_list[1].args[2] == 0.0
    assert mock.call_args_list[1].args[4] == 128
    assert mock.call_args_list[1].args[5] == 1.0

    assert mock.call_args_list[2].args[0] == 2.07e-06
    assert mock.call_args_list[2].args[1] == -5.87e-07
    assert mock.call_args_list[2].args[2] == 0.0
    assert mock.call_args_list[2].args[4] == 153
    assert mock.call_args_list[2].args[5] == 1.0


@patch("RATapi.utils.plotting.makeSLDProfileXY")
def test_live_plot(mock: MagicMock) -> None:
    plot_data = data()

    with LivePlot() as figure:
        assert len(figure.axes) == 2
        notify(EventTypes.Plot, plot_data)
        plt.close(figure)
        notify(EventTypes.Plot, plot_data)

    assert mock.call_count == 3
    assert mock.call_args_list[0].args[0] == 2.07e-06
    assert mock.call_args_list[0].args[1] == 6.28e-06
    assert mock.call_args_list[0].args[2] == 0.0
    assert mock.call_args_list[0].args[4] == 82
    assert mock.call_args_list[0].args[5] == 1.0

    assert mock.call_args_list[1].args[0] == 2.07e-06
    assert mock.call_args_list[1].args[1] == 1.83e-06
    assert mock.call_args_list[1].args[2] == 0.0
    assert mock.call_args_list[1].args[4] == 128
    assert mock.call_args_list[1].args[5] == 1.0

    assert mock.call_args_list[2].args[0] == 2.07e-06
    assert mock.call_args_list[2].args[1] == -5.87e-07
    assert mock.call_args_list[2].args[2] == 0.0
    assert mock.call_args_list[2].args[4] == 153
    assert mock.call_args_list[2].args[5] == 1.0


@patch("RATapi.utils.plotting.plot_ref_sld_helper")
def test_plot_ref_sld(mock: MagicMock, input_project, reflectivity_calculation_results) -> None:
    plot_ref_sld(input_project, reflectivity_calculation_results)
    mock.assert_called_once()
    data = mock.call_args[0][0]
    figure = mock.call_args[0][1]

    assert figure.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)
    assert len(figure.axes) == 2

    assert data.modelType == input_project.model
    assert data.reflectivity == reflectivity_calculation_results.reflectivity
    assert data.shiftedData == reflectivity_calculation_results.shiftedData
    assert data.sldProfiles == reflectivity_calculation_results.sldProfiles
    assert data.resampledLayers == reflectivity_calculation_results.resampledLayers
    assert data.dataPresent.size == 0
    assert (data.subRoughs == reflectivity_calculation_results.contrastParams.subRoughs).all()
    assert data.resample.size == 0
