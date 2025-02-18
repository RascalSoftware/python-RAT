"""A background function for an example."""

import numpy as np


def background_function(xdata, params):
    """Return the background function for a given set of points in q."""
    # Split up the params array
    Ao = params[0]
    k = params[1]
    back_const = params[2]

    # Make an exponential decay background
    background = Ao * np.exp(-k * np.array(xdata)) + back_const

    return background
