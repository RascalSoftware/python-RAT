"""An example script to compare different methods of Bayesian fitting.

This example compares three Bayesian posteriors for a low-dimensional
example: a posterior generated by DREAM, one generated by NS, and
one calculated directly.

The likelihood of the parameters being equal to a certain value is proportional
to exp(-chi^2 / 2) [1], so for a low-dimensional example we can calculate this directly
for a sample of parameter values.

Citation:
[1] D. S. Sivia, J. R. P. Webster,
    "The Bayesian approach to reflectivity data",
    Physica B: Condensed Matter,
    Volume 248, June 1998, pages 327-337
    DOI: 10.1016/S0921-4526(98)00259-2
    URL: https://bayes.wustl.edu/sivia/98_20feb03.pdf

"""

from dataclasses import dataclass
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np

import RATapi as RAT
import RATapi.utils.plotting as RATplot
from RATapi.models import Background, Contrast, Data, Parameter, Resolution

PWD = Path(__file__).parents[0]


# function to get our starting project
# this is the RasCAL-1 default project
# it is a bare D2O substrate
def get_project() -> RAT.Project:
    """Create the project used as our example."""
    return RAT.Project(
        name="Bare D2O Substrate",
        calculation="normal",
        model="standard layers",
        geometry="air/substrate",
        absorption="False",
        parameters=[Parameter(name="Substrate Roughness", min=3.0, value=4.844363132849221, max=8.0, fit=True)],
        background_parameters=[
            Parameter(name="Background parameter 1", min=5e-08, value=3.069003361230152e-06, max=7e-06, fit=True)
        ],
        scalefactors=[Parameter(name="Scalefactor 1", min=0.07, value=0.10141560336360426, max=0.13, fit=False)],
        bulk_in=[Parameter(name="Air", min=0.0, value=0.0, max=0.0, fit=False)],
        bulk_out=[Parameter(name="D2O", min=6.3e-06, value=6.35e-06, max=6.4e-06, fit=False)],
        resolution_parameters=[Parameter(name="Resolution parameter 1", min=0.01, value=0.03, max=0.05, fit=False)],
        backgrounds=[Background(name="Background 1", type="constant", source="Background parameter 1")],
        resolutions=[Resolution(name="Resolution 1", type="constant", source="Resolution parameter 1")],
        data=[
            Data(name="Simulation", data=np.empty([0, 3]), simulation_range=[0.005, 0.7]),
            Data(
                name="f82395c",
                data=np.array(
                    [
                        [4.8866e-02, 1.2343e-04, 1.3213e-06],
                        [5.1309e-02, 1.0063e-04, 1.0803e-06],
                        [5.3874e-02, 8.2165e-05, 8.8779e-07],
                        [5.6568e-02, 6.4993e-05, 7.2018e-07],
                        [5.9396e-02, 5.3958e-05, 6.0015e-07],
                        [6.2366e-02, 4.3590e-05, 5.0129e-07],
                        [6.5485e-02, 3.5780e-05, 4.1957e-07],
                        [6.8759e-02, 2.9130e-05, 3.5171e-07],
                        [7.2197e-02, 2.3481e-05, 3.0586e-07],
                        [7.5807e-02, 1.8906e-05, 2.6344e-07],
                        [7.9597e-02, 1.4642e-05, 2.2314e-07],
                        [8.3577e-02, 1.1589e-05, 1.8938e-07],
                        [8.7756e-02, 9.5418e-06, 1.6220e-07],
                        [9.2143e-02, 7.5694e-06, 1.3809e-07],
                        [9.6751e-02, 6.3831e-06, 1.2097e-07],
                        [1.0159e-01, 5.0708e-06, 1.0333e-07],
                        [1.0667e-01, 4.1041e-06, 8.9548e-08],
                        [1.1200e-01, 3.4253e-06, 7.9830e-08],
                        [1.1760e-01, 2.8116e-06, 7.1554e-08],
                        [1.2348e-01, 2.3767e-06, 6.3738e-08],
                        [1.2966e-01, 1.9241e-06, 5.6586e-08],
                        [1.3614e-01, 1.5642e-06, 5.2778e-08],
                        [1.4294e-01, 1.2922e-06, 4.9730e-08],
                        [1.5009e-01, 1.1694e-06, 5.1175e-08],
                        [1.5760e-01, 9.7837e-07, 5.0755e-08],
                        [1.6548e-01, 8.9138e-07, 5.3542e-08],
                        [1.7375e-01, 7.9420e-07, 5.4857e-08],
                        [1.8244e-01, 7.9131e-07, 5.8067e-08],
                        [1.9156e-01, 6.5358e-07, 5.7717e-08],
                        [2.0114e-01, 6.2970e-07, 5.7951e-08],
                        [2.1119e-01, 5.0130e-07, 5.5262e-08],
                        [2.2175e-01, 5.0218e-07, 5.6461e-08],
                        [2.3284e-01, 3.9299e-07, 5.0685e-08],
                        [2.4448e-01, 3.5324e-07, 5.0194e-08],
                        [2.5671e-01, 4.4475e-07, 5.6485e-08],
                        [2.6954e-01, 5.1338e-07, 6.2247e-08],
                        [2.8302e-01, 3.4918e-07, 4.9745e-08],
                        [2.9717e-01, 4.3037e-07, 5.5488e-08],
                        [3.1203e-01, 4.0099e-07, 5.3591e-08],
                        [3.2763e-01, 3.8397e-07, 5.1303e-08],
                        [3.4401e-01, 3.0995e-07, 4.5965e-08],
                        [3.6121e-01, 3.9357e-07, 5.0135e-08],
                        [3.7927e-01, 3.0997e-07, 4.3680e-08],
                        [3.9824e-01, 2.9656e-07, 4.2432e-08],
                        [4.1815e-01, 2.1909e-07, 3.6117e-08],
                        [4.3906e-01, 2.3153e-07, 3.6307e-08],
                        [4.6101e-01, 3.3428e-07, 4.3874e-08],
                        [4.8406e-01, 2.3441e-07, 3.7488e-08],
                        [5.0826e-01, 1.5496e-07, 3.0585e-08],
                        [5.3368e-01, 2.4708e-07, 3.9376e-08],
                        [5.6036e-01, 2.2157e-07, 3.8258e-08],
                        [5.8838e-01, 2.2798e-07, 4.6976e-08],
                        [6.1169e-01, 6.0272e-07, 2.3239e-07],
                    ]
                ),
                data_range=[0.048866, 0.61169],
                simulation_range=[0.048866, 0.61169],
            ),
        ],
        contrasts=[
            Contrast(
                name="Chain-d, acmw",
                data="f82395c",
                background="Background 1",
                background_action="add",
                bulk_in="Air",
                bulk_out="D2O",
                scalefactor="Scalefactor 1",
                resolution="Resolution 1",
                resample=False,
                model=[],
            )
        ],
    )


@dataclass
class CalculationResults:
    """Data class for results from a direct calculation."""

    x_data: list[np.array]
    distribution: np.array


def bayes_benchmark_2d(grid_size: int) -> (RAT.outputs.BayesResults, CalculationResults):
    """Bayes benchmark for a 2-dimensional example.

    Parameters
    ----------
    grid_size : int
        The number of points to sample for each fit parameter.

    Here we estimate the substrate roughness and background using two different methods:
    nested sampling (the 'ns' procedure in RAT) and through a direct calculation of chi-squared
    over a range of parameter values.

    Returns
    -------
    RAT.BayesResults
        The BayesResults object from a nested sampler calculation.
    CalculationResults
        Results from the direct calculation.

    """
    problem = get_project()

    ns_controls = RAT.Controls(procedure="ns", nsTolerance=1, nLive=500, display="final")
    _, ns_results = RAT.run(problem, ns_controls)

    dream_controls = RAT.Controls(procedure="dream", display="final")
    _, dream_results = RAT.run(problem, dream_controls)

    # now we get the parameters and use them to do a direct calculation
    rough_param = problem.parameters[0]
    roughness = np.linspace(rough_param.min, rough_param.max, grid_size)

    back_param = problem.background_parameters[0]
    background = np.linspace(back_param.min, back_param.max, grid_size)

    controls = RAT.Controls(procedure="calculate", display="off")

    def calculate_posterior(roughness_index: int, background_index: int) -> float:
        """Calculate the posterior for an item in the roughness and background vectors.

        Parameters
        ----------
        roughness_index : int
            The index of the roughness vector to use as the roughness parameter value.
        background_index : int
            The index of the background vector to use as the background parameter value.

        Returns
        -------
        float
            The value of exp(-chi^2 / 2) for the given roughness and background values.

        """
        problem.parameters[0].value = roughness[roughness_index]
        problem.background_parameters[0].value = background[background_index]

        _, results = RAT.run(problem, controls)
        chi_squared = results.calculationResults.sumChi

        return np.exp(-chi_squared / 2)

    vectorized_calc_posterior = np.vectorize(calculate_posterior)

    print("Calculating posterior directly...")
    probability_array = vectorized_calc_posterior(*np.indices((grid_size, grid_size), dtype=int))

    return ns_results, dream_results, CalculationResults(x_data=[roughness, background], distribution=probability_array)


def bayes_benchmark_3d(grid_size: int) -> (RAT.outputs.BayesResults, CalculationResults):
    """Bayes benchmark for a 3-dimensional example.

    Here we estimate the substrate roughness and background using two different methods:
    nested sampling (the 'ns' procedure in RAT) and through a direct calculation of chi-squared
    over a range of parameter values.

    Parameters
    ----------
    grid_size : int
        The number of points to sample for each fit parameter.

    Returns
    -------
    RAT.BayesResults
        The BayesResults object from a nested sampler calculation.
    CalculationResults
        Results from the direct calculation.

    """
    problem = get_project()
    problem.scalefactors[0].fit = True

    ns_controls = RAT.Controls(procedure="ns", nsTolerance=1, nLive=500, display="final")
    _, ns_results = RAT.run(problem, ns_controls)

    dream_controls = RAT.Controls(procedure="dream", display="final")
    _, dream_results = RAT.run(problem, dream_controls)

    # now we get the parameters and use them to do a direct calculation
    rough_param = problem.parameters[0]
    roughness = np.linspace(rough_param.min, rough_param.max, grid_size)

    back_param = problem.background_parameters[0]
    background = np.linspace(back_param.min, back_param.max, grid_size)

    scale_param = problem.scalefactors[0]
    scalefactor = np.linspace(scale_param.min, scale_param.max, grid_size)

    controls = RAT.Controls(procedure="calculate", calcSldDuringFit=True, display="off")

    def calculate_posterior(roughness_index: int, background_index: int, scalefactor_index: int) -> float:
        """Calculate the posterior for an item in the roughness, background, and scalefactor vectors.

        Parameters
        ----------
        roughness_index : int
            The index of the roughness vector to use as the roughness parameter value.
        background_index : int
            The index of the background vector to use as the background parameter value.
        scalefactor_index : int
            The index of the scalefactor vector to use as the scalefactor parameter.

        Returns
        -------
        float
            The value of exp(-chi^2 / 2) for the given roughness and background values.

        """
        problem.parameters[0].value = roughness[roughness_index]
        problem.background_parameters[0].value = background[background_index]
        problem.scalefactors[0].value = scalefactor[scalefactor_index]

        _, results = RAT.run(problem, controls)
        chi_squared = results.calculationResults.sumChi

        return np.exp(-chi_squared / 2)

    vectorized_calc_posterior = np.vectorize(calculate_posterior)

    print("Calculating posterior directly...")
    probability_array = vectorized_calc_posterior(*np.indices((grid_size, grid_size, grid_size), dtype=int))

    return (
        ns_results,
        dream_results,
        CalculationResults(x_data=[roughness, background, scalefactor], distribution=probability_array),
    )


def plot_posterior_comparison(
    ns_results: RAT.outputs.BayesResults, dream_results: RAT.outputs.BayesResults, calc_results: CalculationResults
):
    """Create a grid of marginalised posteriors comparing different calculation methods.

    Parameters
    ----------
    ns_results : RAT.BayesResults
        The BayesResults object from a nested sampler calculation.
    dream_results : RAT.BayesResults
        The BayesResults object from a DREAM calculation.
    calc_results : CalculationResults
        The results from a direct calculation.

    """
    num_params = calc_results.distribution.ndim
    fig, axes = plt.subplots(3, num_params, figsize=(3 * num_params, 9))

    def plot_marginalised_result(dimension: int, axes: plt.Axes, limits: tuple[float]):
        """Plot a histogram of a marginalised posterior from the calculation results.

        Parameters
        ----------
        dimension : int
            The dimension of the array to marginalise over.
        axes : plt.Axes
            The Axes object to plot the histogram onto.
        limits : tuple[float]
            The x-axis limits for the histogram.

        """
        # marginalise to the dimension
        # note we don't need to normalise here as np.histogram normalises for us
        sum_axes = tuple(i for i in range(0, num_params) if i != dimension)
        distribution = np.sum(calc_results.distribution, axis=sum_axes)
        distribution /= np.sum(calc_results.distribution)

        # create histogram
        axes.hist(
            calc_results.x_data[dimension],
            bins=25,
            range=limits,
            weights=distribution,
            density=True,
            edgecolor="black",
            linewidth=1.2,
            color="white",
        )

    # row 0 contains NS histograms for each parameter
    # row 1 contains direct calculation histograms for each parameter
    for i in range(0, num_params):
        RATplot.plot_one_hist(ns_results, i, axes=axes[0][i])
        RATplot.plot_one_hist(dream_results, i, axes=axes[1][i])
        # we want all 3 plots to have the same x-axis
        axes[1][i].set_xlim(*axes[0][i].get_xlim())
        axes[1][i].set_title("")
        plot_marginalised_result(i, axes[2][i], limits=axes[0][i].get_xlim())

    axes[0][0].set_ylabel("nested sampler")
    axes[1][0].set_ylabel("DREAM")
    axes[2][0].set_ylabel("direct calculation")

    fig.tight_layout()
    fig.show()


if __name__ == "__main__":
    ns_2d, dream_2d, calc_2d = bayes_benchmark_2d(30)
    ns_3d, dream_3d, calc_3d = bayes_benchmark_3d(30)

    plot_posterior_comparison(ns_2d, dream_2d, calc_2d)
    plot_posterior_comparison(ns_3d, dream_3d, calc_3d)
