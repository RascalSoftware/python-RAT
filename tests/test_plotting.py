import os
import re
import csv
import pytest
import pickle
from unittest.mock import patch
from unittest.mock import MagicMock
import numpy as np
import matplotlib.pyplot as plt
from RAT.rat_core import PlotEventData
from RAT.utils.plotting import RATPlots


TEST_DIR_PATH = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                             'test_data')


def data() -> PlotEventData:
    """
    Creates the fixture for the tests.
    """
    data_path = os.path.join(TEST_DIR_PATH, 'plotting_data.pickle')    
    with open(data_path, 'rb') as f:
        loaded_data = pickle.load(f)
    
    data = PlotEventData()
    data.modelType = loaded_data['modelType']
    data.dataPresent = loaded_data['dataPresent']
    data.subRoughs = loaded_data['subRoughs']
    data.resample = loaded_data['resample']
    data.resampledLayers = loaded_data['resampledLayers']
    data.reflectivity = loaded_data['reflectivity']
    data.shiftedData = loaded_data['shiftedData']
    data.sldProfiles = loaded_data['sldProfiles']
    return data


@pytest.fixture
def rat_plots() -> RATPlots:
    """
    Creates the fixture for the tests.
    """
    rat_plots = RATPlots()
    rat_plots._plot(data())
    return rat_plots


def test_figure_axis_formating(rat_plots: RATPlots) -> None:
    """
    Tests the axis formating of the figure.
    """
    assert rat_plots._fig.axes[0].get_subplotspec().get_gridspec().get_geometry() == (1, 2)

    assert rat_plots._ref_plot.get_xlabel() == "Qz"
    assert rat_plots._ref_plot.get_xscale() == "log"
    assert rat_plots._ref_plot.get_ylabel() == "Ref"
    assert rat_plots._ref_plot.get_yscale() == "log"
    assert [label._text for label in rat_plots._ref_plot.get_legend().texts] == ['ref 1', 'ref 2', 'ref 3']

    assert rat_plots._sld_plot.get_xlabel() == "Z"
    assert rat_plots._sld_plot.get_xscale() == "linear"
    assert rat_plots._sld_plot.get_ylabel() == "SLD"
    assert rat_plots._sld_plot.get_yscale() == "linear"
    assert [label._text for label in rat_plots._sld_plot.get_legend().texts] == ['sld 1', 'sld 2', 'sld 3']


def test_figure_color_formating(rat_plots: RATPlots) -> None:
    """
    Tests the color formating of the figure.
    """
    ref_plot_line_lenght = len(rat_plots._ref_plot.get_lines())
    assert ref_plot_line_lenght == 3
    assert len(rat_plots._sld_plot.get_lines()) == 6

    for axis_ix in range(ref_plot_line_lenght):
        ax1 = axis_ix*2
        ax2 = ax1 + 1

        # Tests whether the color of the line and the errorbars match on the ref_plot
        assert (rat_plots._ref_plot.containers[ax1][2][0]._original_edgecolor ==
                rat_plots._ref_plot.containers[ax2][2][0]._original_edgecolor ==
                rat_plots._ref_plot.get_lines()[axis_ix].get_color())

        # Tests whether the color of the sld and resampled_sld match on the sld_plot
        assert (rat_plots._sld_plot.get_lines()[ax1].get_color() ==
                rat_plots._sld_plot.get_lines()[ax2].get_color())


def test_eventhandlers_linked_to_figure(rat_plots: RATPlots) -> None:
    """
    Tests whether the eventhandlers for close_event
    and key_press_event in the figure are linked to the
    class methods.
    """
    pattern = r'\(([^\]]+)\)'

    for ix, val in rat_plots._fig.canvas.callbacks.callbacks['close_event'].items():
        if str(type(val)) == "<class 'weakref.WeakMethod'>":
            break
    canvas_close_event_callback = rat_plots._fig.canvas.callbacks.callbacks['close_event'][ix]._func_ref.__repr__()
    close_event_callback = re.findall(pattern,
                                      canvas_close_event_callback)[0]
    assert close_event_callback == "_close"
    assert hasattr(RATPlots, "_close")

    for ix, val in rat_plots._fig.canvas.callbacks.callbacks['key_press_event'].items():
        if str(type(val)) == "<class 'weakref.WeakMethod'>":
            break
    canvas_key_press_event_callback = rat_plots._fig.canvas.callbacks.callbacks['key_press_event'][ix]._func_ref.__repr__()
    key_press_event_callback = re.findall(pattern,
                                          canvas_key_press_event_callback)[0]
    assert key_press_event_callback == "_process_button_press"
    assert hasattr(RATPlots, "_process_button_press")

def test_eventhandler_variable_update(rat_plots: RATPlots) -> None:
    """
    Tests whether the eventhandlers for close_event
    and key_press_event update variables that stop
    while loop in wait_for_close.
    """
    assert not rat_plots._esc_pressed
    on_key_mock_event = type('MockEvent', (object,), {'key': 'escape'})
    rat_plots._process_button_press(on_key_mock_event)
    assert rat_plots._esc_pressed

    assert not rat_plots._close_clicked
    plt.close(rat_plots._fig)
    assert rat_plots._close_clicked


@patch("RAT.utils.plotting.plt.waitforbuttonpress")  
def test_wait_for_close(mock: MagicMock, rat_plots: RATPlots) -> None:
    """
    Tests the _wait_for_close method stops the
    while loop when _esc_pressed is True.
    """
    def mock_wait_for_button_press(timeout):
        rat_plots._esc_pressed = True

    mock.side_effect = mock_wait_for_button_press
    assert not rat_plots._esc_pressed
    rat_plots.wait_for_close()
    assert rat_plots._esc_pressed


@patch("RAT.utils.plotting.makeSLDProfileXY")   
def test_sld_profile_function_call(mock: MagicMock) -> None:
    """
    Tests the makeSLDProfileXY function called with
    correct args.
    """
    rat_plots = RATPlots()
    rat_plots._plot(data())

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
