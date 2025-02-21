import numpy as np


def background_function(xdata, params):
    # Split up the params array
    Ao = params[0]
    k = params[1]
    back_const = params[2]

    # Make an exponential decay background
    background = Ao * np.exp(-k * np.array(xdata)) + back_const

    return background
