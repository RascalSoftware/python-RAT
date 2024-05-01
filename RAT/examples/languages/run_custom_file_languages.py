"""Test custom function languages."""
import RAT.rat_core
import RAT.utils.plotting
import setup_problem

problem = setup_problem.make_example_problem()
controls = RAT.set_controls()

problem, cells, limits, priors, cpp_controls = RAT.make_input(problem, controls)

# Python
problem, results, bayes = RAT.rat_core.RATMain(problem, cells, limits, cpp_controls, priors)
# figure(1)
# plot_ref_sld(problem, results)

# Matlab
# problem.setCustomFile(1,'filename', 'customBilayer.m')
# problem.setCustomFile(1,'language','matlab')
#
# [problem, results] = RAT(problem,controls)
# figure(2); clf
# plotRefSLD(problem,results);
#
# # C++
# problem.setCustomFile(1,'filename', 'customBilayer.dll')
# problem.setCustomFile(1,'language','cpp')
#
# [problem, results] = RAT(problem,controls)
# figure(3); clf
# plotRefSLD(problem,results);
