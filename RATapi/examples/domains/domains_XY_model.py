import math

import numpy as np


def domains_XY_model(params, bulk_in, bulk_out, contrast, domain):
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
    [vfSilicon, siSurf] = makeLayer(z, -25, 50, 1, subRough, subRough)

    # ... and the Oxide ...
    [vfOxide, oxSurface] = makeLayer(z, siSurf, oxideThick, 1, subRough, subRough)

    # ... and also our layer.
    [vfLayer, laySurface] = makeLayer(z, oxSurface, layerThick, 1, subRough, layerRough)

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


def makeLayer(z, prevLaySurf, thickness, height, Sigma_L, Sigma_R):
    """This produces a layer, with a defined thickness, height and roughness.
    Each side of the layer has its own roughness value.
    """
    # Find the edges
    left = prevLaySurf
    right = prevLaySurf + thickness

    # Make our heaviside
    a = (z - left) / ((2**0.5) * Sigma_L)
    b = (z - right) / ((2**0.5) * Sigma_R)

    erf_a = np.array([math.erf(value) for value in a])
    erf_b = np.array([math.erf(value) for value in b])

    VF = np.array((height / 2) * (erf_a - erf_b))

    return VF, right
