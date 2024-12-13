"""
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

PWD = Path(__file__).parents[0]


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
    problem = RAT.utils.convert.r1_to_project_class(str(PWD / "defaultR1ProjectTemplate.mat"))

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
        problem.parameters[0].value = roughness[int(roughness_index)]
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
    problem = RAT.utils.convert.r1_to_project_class(str(PWD / "defaultR1ProjectTemplate.mat"))
    problem.scalefactors[0].min = 0.07
    problem.scalefactors[0].max = 0.13

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

        # create histogram
        axes.hist(
            calc_results.x_data[i],
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
        plot_marginalised_result(i, axes[2][i], limits=axes[0][i].get_xlim())

    axes[0][0].set_ylabel("nested sampler")
    axes[1][0].set_ylabel("DREAM")
    axes[2][0].set_ylabel("direct calculation")

    fig.tight_layout()
    fig.show()


if __name__ == "__main__":
    ns_2d, dream_2d, calc_2d = bayes_benchmark_2d(30)
    ns_3d, dream_3d, calc_3d = bayes_benchmark_3d(40)

    plot_posterior_comparison(ns_2d, dream_2d, calc_2d)
    plot_posterior_comparison(ns_3d, dream_3d, calc_3d)
