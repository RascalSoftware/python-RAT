import os
import pickle
from math import ceil, sqrt
from textwrap import fill
from unittest.mock import MagicMock, patch

import matplotlib.pyplot as plt
import numpy as np
import pytest
from matplotlib.collections import PolyCollection, QuadMesh
from matplotlib.patches import Rectangle

import RATapi.utils.plotting as RATplot
from RATapi.events import notify
from RATapi.rat_core import EventTypes, PlotEventData

TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)), "test_data")


def data() -> PlotEventData:
    """Creates the data for the tests."""
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
    data.contrastNames = ["D2O", "SMW", "H2O"]

    return data


def domains_data() -> PlotEventData:
    """Creates the fake domains data for the tests."""
    domains_data = data()
    for sld_list in domains_data.sldProfiles:
        sld_list.append(sld_list[0])

    return domains_data


@pytest.fixture(params=[False])
def fig(request) -> plt.figure:
    """Creates the fixture for the tests."""
    plt.close("all")
    figure = plt.subplots(1, 2)[0]
    return RATplot.plot_ref_sld_helper(fig=figure, data=domains_data() if request.param else data())


@pytest.fixture
def bayes_fig(request) -> plt.figure:
    plt.close("all")
    figure = plt.subplots(1, 2)[0]
    dat = data()

    confidence_intervals = {
        "reflectivity": [
            (curve[:, 1] - curve[:, 1] * 0.5, curve[:, 1] + curve[:, 1] * 0.5) for curve in dat.reflectivity
        ],
        "sld": [
            [(curve[:, 1] - curve[:, 1] * 0.1, curve[:, 1] + curve[:, 1] * 0.1) for curve in sld]
            for sld in dat.sldProfiles
        ],
    }
    return RATplot.plot_ref_sld_helper(data=dat, fig=figure, confidence_intervals=confidence_intervals)


@pytest.mark.parametrize("fig", [False, True], indirect=True)
def test_figure_axis_formatting(fig: plt.figure) -> None:
    """Tests the axis formatting of the figure."""
    ref_plot = fig.axes[0]
    sld_plot = fig.axes[1]

    assert fig.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)
    assert len(fig.axes) == 2

    assert ref_plot.get_xlabel() == "$Q_{z} (\u00c5^{-1})$"
    assert ref_plot.get_xscale() == "log"
    assert ref_plot.get_ylabel() == "Reflectivity"
    assert ref_plot.get_yscale() == "log"
    assert [label._text for label in ref_plot.get_legend().texts] == ["D2O", "SMW", "H2O"]

    assert sld_plot.get_xlabel() == "$Z (\u00c5)$"
    assert sld_plot.get_xscale() == "linear"
    assert sld_plot.get_ylabel() == "$SLD (\u00c5^{-2})$"
    assert sld_plot.get_yscale() == "linear"
    labels = [label._text for label in sld_plot.get_legend().texts]
    if len(labels) == 3:
        assert labels == ["D2O", "SMW", "H2O"]
    else:
        assert labels == [
            "D2O Domain 1",
            "D2O Domain 2",
            "SMW Domain 1",
            "SMW Domain 2",
            "H2O Domain 1",
            "H2O Domain 2",
        ]


def test_ref_sld_color_formating(fig: plt.figure) -> None:
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


@pytest.mark.parametrize("bayes", [65, 95])
def test_ref_sld_bayes(fig, bayes_fig, bayes):
    """Test that shading is correctly added to the figure when confidence intervals are supplied."""
    # the shading is of type PolyCollection
    for axes in fig.axes:
        components = axes.get_children()
        assert not any(isinstance(comp, PolyCollection) for comp in components)
    for axes in bayes_fig.axes:
        components = axes.get_children()
        assert any(isinstance(comp, PolyCollection) for comp in components)


@patch("RATapi.utils.plotting.makeSLDProfile")
def test_sld_profile_function_call(mock: MagicMock) -> None:
    """Tests the makeSLDProfile function called with
    correct args.
    """
    RATplot.plot_ref_sld_helper(data())

    assert mock.call_count == 3
    assert mock.call_args_list[0].args[0] == 2.07e-06
    assert mock.call_args_list[0].args[1] == 6.28e-06
    assert mock.call_args_list[0].args[3] == 0.0
    assert mock.call_args_list[0].args[4] == 1

    assert mock.call_args_list[1].args[0] == 2.07e-06
    assert mock.call_args_list[1].args[1] == 1.83e-06
    assert mock.call_args_list[1].args[3] == 0.0
    assert mock.call_args_list[1].args[4] == 1

    assert mock.call_args_list[2].args[0] == 2.07e-06
    assert mock.call_args_list[2].args[1] == -5.87e-07
    assert mock.call_args_list[2].args[3] == 0.0
    assert mock.call_args_list[2].args[4] == 1


@patch("RATapi.utils.plotting.makeSLDProfile")
def test_live_plot(mock: MagicMock) -> None:
    plot_data = data()

    with RATplot.LivePlot() as figure:
        assert len(figure.axes) == 2
        notify(EventTypes.Plot, plot_data)
        plt.close(figure)
        notify(EventTypes.Plot, plot_data)

    assert mock.call_count == 3
    assert mock.call_args_list[0].args[0] == 2.07e-06
    assert mock.call_args_list[0].args[1] == 6.28e-06
    assert mock.call_args_list[0].args[3] == 0.0
    assert mock.call_args_list[0].args[4] == 1

    assert mock.call_args_list[1].args[0] == 2.07e-06
    assert mock.call_args_list[1].args[1] == 1.83e-06
    assert mock.call_args_list[1].args[3] == 0.0
    assert mock.call_args_list[1].args[4] == 1

    assert mock.call_args_list[2].args[0] == 2.07e-06
    assert mock.call_args_list[2].args[1] == -5.87e-07
    assert mock.call_args_list[2].args[3] == 0.0
    assert mock.call_args_list[2].args[4] == 1


@patch("RATapi.utils.plotting.plot_ref_sld_helper")
def test_plot_ref_sld(mock: MagicMock, input_project, reflectivity_calculation_results) -> None:
    RATplot.plot_ref_sld(input_project, reflectivity_calculation_results)
    mock.assert_called_once()
    data = mock.call_args[0][0]
    figure = mock.call_args[0][1]

    assert figure.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)
    assert len(figure.axes) == 2

    for reflectivity, reflectivity_results in zip(data.reflectivity, reflectivity_calculation_results.reflectivity):
        assert (reflectivity == reflectivity_results).all()
    for sldProfile, result_sld_profile in zip(data.sldProfiles, reflectivity_calculation_results.sldProfiles):
        for sld, sld_results in zip(sldProfile, result_sld_profile):
            assert (sld == sld_results).all()

    assert data.modelType == input_project.model
    assert data.shiftedData == reflectivity_calculation_results.shiftedData
    assert data.resampledLayers == reflectivity_calculation_results.resampledLayers
    assert data.dataPresent.size == 0
    assert (data.subRoughs == reflectivity_calculation_results.contrastParams.subRoughs).all()
    assert data.resample.size == 0
    assert len(data.contrastNames) == 0


def test_ref_sld_subplot_correction():
    """Test that if an incorrect number of subplots is corrected in the figure helper."""
    fig = plt.subplots(1, 3)[0]
    ref_sld_fig = RATplot.plot_ref_sld_helper(data=data(), fig=fig)
    assert ref_sld_fig.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)
    assert len(ref_sld_fig.axes) == 2


@patch("RATapi.utils.plotting.plot_ref_sld_helper")
def test_plot_ref_sld_bayes_validation(mock, input_project, reflectivity_calculation_results, dream_results):
    """Test that plot_ref_sld correctly throws errors for bad Bayesian input."""
    RATplot.plot_ref_sld(input_project, dream_results)
    RATplot.plot_ref_sld(input_project, dream_results, bayes=65)
    RATplot.plot_ref_sld(input_project, dream_results, bayes=95)
    with pytest.raises(ValueError):
        RATplot.plot_ref_sld(input_project, reflectivity_calculation_results, bayes=65)
    with pytest.raises(ValueError):
        RATplot.plot_ref_sld(input_project, dream_results, bayes=15)


def test_assert_bayesian(dream_results, reflectivity_calculation_results):
    """Test that the `assert_bayesian` decorator validates correctly."""

    @RATplot.assert_bayesian("test")
    def test_plot(results):
        pass

    test_plot(dream_results)
    with pytest.raises(
        ValueError, match=r"test plots are only available for the results of Bayesian analysis \(NS or DREAM\)"
    ):
        test_plot(reflectivity_calculation_results)


@pytest.mark.parametrize("indices", [[0, 1, 2, 3, 4], [2, 5, 11], [8]])
def test_panel_helper(indices):
    """Test that the panel plot helper creates a panel with the expected subplots."""

    def plot_func(axes, k):
        """Plot k lines on an Axes."""
        for i in range(0, k):
            axes.plot([i], [i])

    nplots = len(indices)

    fig = RATplot.panel_plot_helper(plot_func, indices)
    # ensure correct number of axes were created
    expected_num_axes = ceil(sqrt(nplots)) * round(sqrt(nplots))
    assert len(fig.axes) == expected_num_axes

    # assert all required axes are visible and have the requested number of lines
    for i, index in enumerate(indices):
        assert len(fig.axes[i].get_lines()) == index
        assert fig.axes[i].get_visible()

    # assert remaining axes are not visible
    for i in range(nplots, expected_num_axes):
        assert fig.axes[i].get_visible() is False

    plt.close(fig)


@pytest.mark.parametrize("param", ["CW Thickness", "D2O", 5])
@pytest.mark.parametrize("hist_settings", [{}, {"bins": 18}, {"density": False, "range": (0, 5)}])
@pytest.mark.parametrize("est_dens", [None, "normal", "lognor", "kernel"])
def test_hist(dream_results, param, hist_settings, est_dens):
    """Tests the formatting of the histogram plot."""
    fig: plt.Figure = RATplot.plot_one_hist(
        dream_results, param, estimated_density=est_dens, **hist_settings, return_fig=True
    )
    ax = fig.axes[0]
    components = ax.get_children()

    # assert expected number of bins including default
    # this ensures default hist_settings are overwritten correctly
    # +1 rectangle because the bounds of the plot is a rectangle
    expected_bins = hist_settings.get("bins", 25) + 1
    assert len([c for c in components if isinstance(c, Rectangle)]) == expected_bins

    # assert line is only drawn if estimated density given
    assert len(ax.get_lines()) == (0 if est_dens is None else 1)

    # assert title is as expected
    # also tests string to index conversion
    assert ax.get_title() == fill(dream_results.fitNames[param] if isinstance(param, int) else param, 20)

    # assert range is default, unless given
    # this tests non-default hist_settings propagates correctly
    try:
        expected_range = hist_settings["range"]
    except KeyError:  # if no range given, compute the automatic range
        param_index = dream_results.fitNames.index(param) if isinstance(param, str) else param
        param_chain = dream_results.chain[:, param_index]
        expected_range = (param_chain.min(), param_chain.max())
    assert ax.get_xbound() == expected_range

    plt.close(fig)


@pytest.mark.parametrize(["x_param", "y_param"], [["CW Thickness", "D2O"], ["Bilayer Heads Thickness", 5], [2, 7]])
@pytest.mark.parametrize("hist2d_settings", [{}, {"bins": 15}, {"range": [(-50.0, 50.0), (-50.0, 200.0)]}])
def test_contour(dream_results, x_param, y_param, hist2d_settings):
    """Test the formatting of the contour plot."""
    fig: plt.Figure = RATplot.plot_contour(dream_results, x_param, y_param, return_fig=True, **hist2d_settings)
    ax = fig.axes[0]
    components = ax.get_children()

    # assert expected number of bins including default
    # this ensures default hist2d_settings are overwritten correctly
    # +1 as we are counting edges in this case
    expected_bins = hist2d_settings.get("bins", 25) + 1
    quad_mesh = [c for c in components if isinstance(c, QuadMesh)][0]
    assert quad_mesh._coordinates.shape == (expected_bins, expected_bins, 2)

    # assert correct axis labels
    # this ensures string to index conversion works
    assert ax.get_xlabel() == (dream_results.fitNames[x_param] if isinstance(x_param, int) else x_param)
    assert ax.get_ylabel() == (dream_results.fitNames[y_param] if isinstance(y_param, int) else y_param)

    # assert range is default, unless given
    # this tests non-default hist2d_settings propagates correctly
    try:
        x_expected_range, y_expected_range = hist2d_settings["range"]
    except KeyError:  # if no range given, compute the automatic range
        x_param_index = dream_results.fitNames.index(x_param) if isinstance(x_param, str) else x_param
        y_param_index = dream_results.fitNames.index(y_param) if isinstance(y_param, str) else y_param
        x_param_chain = dream_results.chain[:, x_param_index]
        y_param_chain = dream_results.chain[:, y_param_index]
        x_expected_range = (x_param_chain.min(), x_param_chain.max())
        y_expected_range = (y_param_chain.min(), y_param_chain.max())
    assert ax.get_xbound() == x_expected_range
    assert ax.get_ybound() == y_expected_range

    # plt.close(fig)


@pytest.mark.parametrize(
    "params",
    [
        None,
        ["Bilayer Heads Thickness", "Bilayer Heads Hydration", "D2O"],
        ["Bilayer Heads Thickness", 2, 3, "D2O", 5],
        [1, 2, 3, 4, 5],
    ],
)
def test_corner(dream_results, params):
    """Test that corner plots are formatted correctly."""
    # no use testing hist_settings and hist2d_settings here as they're tested above
    fig: plt.Figure = RATplot.plot_corner(dream_results, params, return_fig=True)
    axes = fig.axes
    if params is None:
        params = range(0, len(dream_results.fitNames))
    assert len(axes) == len(params) ** 2
    # annoyingly, fig.axes doesn't preserve the grid shape from plt.subplots... reconstruct grid
    axes = np.array([axes[i : i + len(params)] for i in range(0, len(axes), len(params))])

    for i in range(0, len(params)):
        for j in range(0, len(params)):
            current_axes = axes[i][j]
            # ensure upper triangle is invisible
            if i < j:
                assert current_axes.get_visible() is False
            elif i > j:
                # check axes are the same along each row and column for contours
                assert current_axes.get_ybound() == axes[i][0].get_ybound()
                assert current_axes.get_xbound() == axes[-1][j].get_xbound()
            elif i == j:
                # check title is correct
                assert current_axes.get_title() == fill(
                    dream_results.fitNames[params[i]] if isinstance(params[i], int) else params[i], 20
                )

    plt.close(fig)


@pytest.mark.parametrize(
    "params", [None, [2, 3], [1, 5, "D2O"], ["Bilayer Heads Thickness", "Bilayer Heads Hydration", "D2O"]]
)
@patch("RATapi.plotting.panel_plot_helper")
def test_hist_panel(mock_panel_helper: MagicMock, params, dream_results):
    """Test chain panel name-to-index (panel helper has already been tested)"""
    fig = RATplot.plot_hists(dream_results, params, return_fig=True)
    plt.close(fig)
    if params is None:
        params = range(0, len(dream_results.fitNames))

    passed_params = mock_panel_helper.call_args.args[1]
    assert len(passed_params) == len(params)
    for param in passed_params:
        assert param == (dream_results.fitNames.index(param) if isinstance(param, str) else param)


@pytest.mark.parametrize(
    ["input", "expected_dict"],
    [
        (None, "NONEDICT"),
        ({"D2O": "kernel"}, "D2O_DICT"),
        ({"default": "lognor"}, "DEFAULTDICT"),  # workaround as we need to access the fixture attrs
        ("lognor", "DEFAULTDICT"),
        ({"default": "normal", 1: "kernel"}, "DEFAULT_WITH_1CHANGE_DICT"),
    ],
)
@patch("RATapi.plotting.plot_one_hist")
def test_standardise_est_dens(mock_plot_hist: MagicMock, input, expected_dict, dream_results):
    """Test estimated density is correctly standardised."""
    _ = RATplot.plot_hists(dream_results, estimated_density=input, return_fig=True)

    expected_dict = {
        "NONEDICT": {i: None for i in range(0, len(dream_results.fitNames))},
        "D2O_DICT": {**{i: None for i in range(0, len(dream_results.fitNames))}, **{16: "kernel"}},
        "DEFAULTDICT": {i: "lognor" for i in range(0, len(dream_results.fitNames))},
        "DEFAULT_WITH_1CHANGE_DICT": {**{i: "normal" for i in range(0, len(dream_results.fitNames))}, **{1: "kernel"}},
    }[expected_dict]

    call_args = mock_plot_hist.call_args_list
    keys_called = [call[0][1] for call in call_args]
    est_density = [call[1]["estimated_density"] for call in call_args]
    est_dens_dict = {keys_called[i]: est_density[i] for i in range(0, len(keys_called))}

    assert expected_dict == est_dens_dict
    plt.close("all")


@pytest.mark.parametrize("input", [{250: "lognor"}, {"Oxide Quickness": "normal"}, {"D2O": "Rattian"}, {-5: "lognor"}])
def test_est_dens_error(dream_results, input):
    """Ensure a bad estimated density input raises an error."""
    # the error message contains the phrase "Parameter {key}" or "Index {key}", so use that
    # to ensure we're not getting some random ValueError
    with pytest.raises((ValueError, IndexError), match=f"Parameter|Index {(list(input.keys())[0])}"):
        RATplot.plot_hists(dream_results, estimated_density=input)


@pytest.mark.parametrize(
    "params", [None, [2, 3], [1, 5, "D2O"], ["Bilayer Heads Thickness", "Bilayer Heads Hydration", "D2O"]]
)
@patch("RATapi.plotting.panel_plot_helper")
def test_chain_panel(mock_panel_helper: MagicMock, params, dream_results):
    """Test chain panel name-to-index (panel helper has already been tested)"""
    # return fig just to avoid plt.show() being called
    fig = RATplot.plot_chain(dream_results, params, return_fig=True)
    plt.close(fig)
    if params is None:
        params = range(0, len(dream_results.fitNames))

    for param in mock_panel_helper.call_args()[1]:
        assert param == (dream_results.fitNames.index(param) if isinstance(param, str) else param)


@patch("RATapi.plotting.plot_ref_sld")
@patch("RATapi.plotting.plot_hists")
@patch("RATapi.plotting.plot_corner")
def test_bayes_calls(
    mock_corner: MagicMock, mock_hists: MagicMock, mock_ref_sld: MagicMock, input_project, dream_results
):
    """Test that the Bayes plot calls the required plotting subroutines."""
    RATplot.plot_bayes(input_project, dream_results)
    assert mock_ref_sld.call_count == 2
    mock_hists.assert_called_once()
    mock_corner.assert_called_once()


def test_bayes_validation(input_project, reflectivity_calculation_results):
    """Ensure that plot_bayes fails if given regular Results."""
    with pytest.raises(
        ValueError, match=r"Bayes plots are only available for the results of Bayesian analysis \(NS or DREAM\)"
    ):
        RATplot.plot_bayes(input_project, reflectivity_calculation_results)
