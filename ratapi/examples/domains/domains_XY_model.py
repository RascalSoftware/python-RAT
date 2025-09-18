"""Custom model file for the domains custom XY example."""

from math import sqrt

import numpy as np
from scipy.special import erf


def domains_XY_model(params, bulk_in, bulk_out, contrast, domain):
    """Calculate the SLD profile for a domains custom XY model."""
    # Split up the parameters for convenience
    subRough = params[0]
    oxideThick = params[1]
    layerThick = params[2]
    layerSLD = params[3]
    layerRough = params[4]
    domainSLD = params[5]

    # Make an array of z values for our model
    z = np.arange(0, 141)

    # Make the volume fraction distribution for our Silicon substrate
    [vfSilicon, siSurf] = make_layer(z, -25, 50, 1, subRough, subRough)

    # ... and the Oxide ...
    [vfOxide, oxSurface] = make_layer(z, siSurf, oxideThick, 1, subRough, subRough)

    # ... and also our layer.
    [vfLayer, laySurface] = make_layer(z, oxSurface, layerThick, 1, subRough, layerRough)

    # Everything that is not already occupied will be filled will water
    totalVF = vfSilicon + vfOxide + vfLayer
    vfWater = 1 - totalVF

    # Now convert the Volume Fractions to SLDs
    siSLD = vfSilicon * bulk_in
    oxSLD = vfOxide * 3.41e-6

    # Layer SLD depends on whether we are calculating the domain or not
    if domain == 0:
        laySLD = vfLayer * layerSLD
    else:
        laySLD = vfLayer * domainSLD

    # ... and finally the water SLD.
    waterSLD = vfWater * bulk_out[contrast]

    # Make the total SLD by just adding them all up
    totalSLD = siSLD + oxSLD + laySLD + waterSLD

    # The output is just a [n x 2] array of z against SLD
    SLD = np.column_stack([z, totalSLD])

    return SLD, subRough


def make_layer(z, prevLaySurf, thickness, height, Sigma_L, Sigma_R):
    """Produce a layer, with a defined thickness, height and roughness.

    Each side of the layer has its own roughness value.
    """
    # Find the edges
    left = prevLaySurf
    right = prevLaySurf + thickness

    # Make our heaviside
    erf_left = erf((z - left) / (sqrt(2) * Sigma_L))
    erf_right = erf((z - right) / (sqrt(2) * Sigma_R))

    VF = np.array((0.5 * height) * (erf_left - erf_right))

    return VF, right
