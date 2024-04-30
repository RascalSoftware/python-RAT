"""
Custom XY Example for Supported DSPC layer.

In this example, we model the same data (DSPC supported bilayer) as the Custom Layers example, but this time we will
use continuous distributions of the volume fractions of each component to build up the SLD profiles (as described in
Shekhar et al, J. Appl. Phys., 110, 102216 (2011).)

In this type of model, each 'layer' in the sample is described by a roughened Heaviside step function (really, just two
error functions back to back). So, in our case, we will need an oxide, a (possible) intervening water layer, and then
the bilayer itself.

We can define our lipid in terms of an Area per Molecule, almost in its entirety, if we recognise that where the volume
is known, the thickness of the layer is simply given by the layer volume / APM

$d_{\textrm{layer}} =\frac{V_{\textrm{layer}} }{{\textrm{APM}}_{\textrm{layer}}}$.

We can then define the Volume Fraction of this layer with a roughened Heaviside of length dlayer and a height of 1.
Then, the total volume occupied will be given by the sum of the volume fractions across the interface. Of course, this
does not permit any hydration, so to deal with this, we can simply scale the (full occupation) Heaviside functions by
relevant coverage parameters. When this is correctly done, we can obtain the remaining water distribution as

$${\textrm{VF}}_{\textrm{wat}} =1-\sum_n {\textrm{VF}}_n$$

where VFn is the Volume Fraction of the n'th layer.
"""

import RAT
import numpy as np

# Start by making the class and setting it to a custom layers type:
problem = RAT.Project(name="Orso lipid example - custom XY", model="custom xy", geometry="substrate/liquid")

# We need to add the relevant parameters we are going to need to define the model (note that Substrate Roughness
# always exists as parameter 1
problem.parameters.append(name="Oxide Thickness", min=10.0, value=15.0, max=30.0, fit=True)
problem.parameters.append(name="Oxide Hydration", min=0.1, value=0.2, max=0.4, fit=True)
problem.parameters.append(name="Water Thickness", min=0.0, value=5.0, max=20.0, fit=True)
problem.parameters.append(name="Lipid APM", min=40.0, value=50.0, max=90.0, fit=True)
problem.parameters.append(name="Lipid Coverage", min=0.9, value=1.0, max=1.0, fit=True)
problem.parameters.append(name="Bilayer Roughness", min=3.0, value=5.0, max=8.0, fit=True)

problem.parameters.set_fields(0, min=1.0, max=10.0)

# Need to add the relevant Bulk SLDs. Change the bulk in from air to silicon, and add two additional water contrasts:
problem.bulk_in.set_fields(0, name="Silicon", min=2.07e-6, value=2.073e-6, max=2.08e-6, fit=False)

problem.bulk_out.append(name="SLD SMW", min=1.0e-6, value=2.073e-6, max=3.0e-6, fit=True)
problem.bulk_out.append(name="SLD H2O", min=-0.6e-6, value=-0.56e-6, max=-0.3e-6, fit=True)

problem.bulk_out.set_fields(0, min=5.0e-6, value=6.1e-6, fit=True)

# Now add the datafiles. We have three datasets we need to consider - the bilayer against D2O, Silicon Matched water
# and H2O.Load these datafiles in and put them in the data block

# Read in the datafiles
D2O_data = np.loadtxt("c_PLP0016596.dat", delimiter=",")
SMW_data = np.loadtxt("c_PLP0016601.dat", delimiter=",")
H2O_data = np.loadtxt("c_PLP0016607.dat", delimiter=",")

# Add the data to the project - note this data has a resolution 4th column
problem.data.append(name="Bilayer / D2O", data=D2O_data)#, data_range=[0.013, 0.37]) # data range is incorrect
problem.data.append(name="Bilayer / SMW", data=SMW_data)#, data_range=[0.013, 0.37]) # data range is incorrect
problem.data.append(name="Bilayer / H2O", data=H2O_data)#, data_range=[0.013, 0.37]) # data range is incorrect

# Add the custom file to the project
# (Note that here we are making an optional third output parameter, which we need later just for plotting, but not for
# the RAT fit. So, we make this output optional using a global flag, so that we can control it from outside our function)
problem.custom_files.append(name="DSPC Model", filename="customBilayerDSPC.m", language="matlab", path="pwd")

# Also, add the relevant background parameters - one each for each contrast:
problem.background_parameters.set_fields(0, name="Backs par D2O", fit=True, min=1.0e-10, max=1.0e-5, value=1.0e-07)

problem.background_parameters.append(name="Backs par SMW", min=0.0, value=1.0e-7, max=1.0e-5, fit=True)
problem.background_parameters.append(name="Backs par H2O", min=0.0, value=1.0e-7, max=1.0e-5, fit=True)

# And add the two new constant backgrounds
problem.backgrounds.append(name="Background SMW", type="constant", value_1="Backs par SMW")
problem.backgrounds.append(name="Background H2O", type="constant", value_1="Backs par H2O")

# And edit the other one....
problem.backgrounds.set_fields(0, name="Background D2O", value_1="Backs par D2O")


# Finally modify some of the other parameters to be more suitable values for a solid / liquid experiment
problem.scalefactors.set_fields(0, value=1.0, min=0.5, max=2.0, fit=True)

# Also, we are going to use the data resolution.
problem.resolutions.append(name="Data Resolution", type="data")

# Now add the three contrasts

problem.contrasts.append(name="Bilayer / D2O", background="Background D2O", resolution="Data Resolution",
                         scalefactor="Scalefactor 1", bulk_out="SLD D2O", bulk_in="Silicon", data="Bilayer / D2O",
                         model=["DSPC Model"])

problem.contrasts.append(name="Bilayer / SMW", background="Background SMW", resolution="Data Resolution",
                         scalefactor="Scalefactor 1", bulk_out="SLD SMW", bulk_in="Silicon", data="Bilayer / SMW",
                         model=["DSPC Model"])

problem.contrasts.append(name="Bilayer / H2O", background="Background H2O", resolution="Data Resolution",
                         scalefactor="Scalefactor 1", bulk_out="SLD H2O", bulk_in="Silicon", data="Bilayer / H2O",
                         model=["DSPC Model"])


# Running the Model.
# We do this by first making a controls block as previously. We'll run a Differential  Evolution, and then a Bayesian
# analysis.

# controls = RAT.set_controls(procedure = RAT.controls.Procedures.DE, parallel = 'contrasts', display = 'final')
#
#
# [problem,results] = RAT(problem,controls);
# %%
# % Plot what we have....
#
# plotRefSLD(problem,results);
# %%
# % This is not too bad..... now run Bayes...
#
# controls.procedure = 'dream';
# controls.adaptPCR = true;
# [problem,results] = RAT(problem,controls);
#
# %%
# % ..and plot this out....
#
#     figure(30); clf;
#     bayesShadedPlot(problem, results,'fit','average','KeepAxes',true,'interval',65,'q4',false)
#
#     h3 = figure(40); clf
#     plotHists(results,h3,'smooth',true)
#
#     h4 = figure(50); clf
#     cornerPlot(results,h4,'smooth',false)
# %% A Slightly Deeper Analysys - Plotting The Bayes Result as Volume Fractions
# % The model we're using here is built using Volume Fractions. It's convenient
# % to be able to use these as outputs, so that the result of our Bayesian analysis
# % in terms of the VF's of the various components can be visualised. To do this,
# % we make our global output flag true....
#
# outputVF = true;
# %%
# % Now we have an output for our VF's.
# %
# % We want to see how these are distributed in our Bayesian analysis. All the
# % information comes from our chain....
#
#     chain = results.chain;
# %%
# % Now, calculate the Volume Fractions for each sample of our Markov Chain, and
# % store our VF outputs as a cell array....
#
#     % Find the size of the chain
#     nSamples = size(chain,1);
#
#     % We don't need to calculate all of it, just take a random 1000 points
#     % from the chain. Make a set of indices...
#     samples = randsample(nSamples,1000);
#
#     % Make some empty arrays to store our data....
#     vfSi = []; vfOxide = []; vfHeadL = []; vfTails = []; vfHeadR = []; vfWat = [];
#
#     % Loop over all the samples...
#     for n = 1:length(samples)
#         % Take the n'th value from of set of indicies...
#         i = samples(n);
#
#         % Get these parameter values....
#         thisParams = chain(i,:);
#
#         % Run our model.....
#         [~,~,thisRes] = customXYDSPC(thisParams,2.07e-6,6.35e-6,1);
#
#         % Store them...
#         thisVfs = thisRes(:,2:end); % Column 1 is the z value...
#
#         % Add them to the arrays for storage...
#         vfSi = [vfSi ; thisVfs(:,1)'];  % Note the transpose.....
#         vfOxide  = [vfOxide ; thisVfs(:,2)'];
#         vfHeadL  = [vfHeadL ; thisVfs(:,3)'];
#         vfTails  = [vfTails ; thisVfs(:,4)'];
#         vfHeadR  = [vfHeadR ; thisVfs(:,5)'];
#         vfWat  = [vfWat ; thisVfs(:,6)'];
#     end
# %%
# %
# %%
# % For each collection of volume fractions, we need the mean and the 65% Percentile.....
#
# z = thisRes(:,1); nPoints = length(z);
#
# % Make some empty arrays....
# ciSi = []; ciOxide = []; ciHeadL = []; ciTails = []; ciHeadR = [];
# avSi = []; avOxide = []; avHeadL = []; avTails = []; avHeadR = [];
#
# % Make an inline function for calculating confidence intervals...
# CIFn = @(x,p)prctile(x,abs([0,100]-(100-p)/2)); % Percentile function
#
# % Work out average and confidence intervals...
# avSi = mean(vfSi); ciSi = CIFn(vfSi,65);
# avOxide = mean(vfOxide); ciOxide = CIFn(vfOxide,65);
# avHeadL = mean(vfHeadL); ciHeadL = CIFn(vfHeadL,65);
# avTails = mean(vfTails); ciTails = CIFn(vfTails,65);
# avHeadR = mean(vfHeadR); ciHeadR = CIFn(vfHeadR,65);
#
# % Make a plot.....
# figure; hold on; box on
#
# % In RAT, there is a useful function called 'shade' that we can use
# % here.....
# cols = get(gca,'ColorOrder');
# shade(z,ciSi(1,:),z,ciSi(2,:),'FillColor',cols(1,:),'FillType',[1 2;2 1],'FillAlpha',0.3);
# shade(z,ciOxide(1,:),z,ciOxide(2,:),'FillColor',cols(2,:),'FillType',[1 2;2 1],'FillAlpha',0.3);
# shade(z,ciHeadL(1,:),z,ciHeadL(2,:),'FillColor',cols(3,:),'FillType',[1 2;2 1],'FillAlpha',0.3);
# shade(z,ciTails(1,:),z,ciTails(2,:),'FillColor',cols(4,:),'FillType',[1 2;2 1],'FillAlpha',0.3);
# shade(z,ciHeadR(1,:),z,ciHeadR(2,:),'FillColor',cols(5,:),'FillType',[1 2;2 1],'FillAlpha',0.3);
# title('Volume Fractions');
# %%
# % .. and we are done.