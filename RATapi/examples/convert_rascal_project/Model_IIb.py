from math import cos, radians


def Model_IIb(params, bulk_in, bulk_out, contrast):
    # converted from matlab file Model_IIb.m

    Roughness, APM, thickHead, theta = params

    # Make a flag to say which deuteration this is calculating
    # [subs head tail]
    deut = [[0, 0, 1], [1, 0, 1], [0, 1, 0], [1, 1, 0], [0, 1, 1], [1, 1, 1], [1, 0, 0]]

    bc = 0.6646e-4  # Carbon
    bo = 0.5843e-4  # Oxygen
    bh = -0.3739e-4  # Hydrogen
    bp = 0.513e-4  # Phosphorus
    bn = 0.936e-4  # Nitrogen
    bd = 0.6671e-4  # Deuterium

    # Work out the total scattering length in each fragment
    # for hydrogenated:
    COO = (4 * bo) + (2 * bc)
    GLYC = (3 * bc) + (5 * bh)
    CH3 = (2 * bc) + (6 * bh)
    PO4 = (1 * bp) + (4 * bo)
    CH2 = (1 * bc) + (2 * bh)
    CHOL = (5 * bc) + (12 * bh) + (1 * bn)
    H2O = (2 * bh) + (1 * bo)

    # for deuterated:
    dCH3 = (2 * bc) + (6 * bd)
    dCH2 = (1 * bc) + (2 * bd)
    dCHOL = (5 * bc) + (12 * bd) + (1 * bn)
    D2O = (2 * bd) + (1 * bo)

    # for volumes:
    vCH3 = 52.7
    vCH2 = 28.1
    vCOO = 39.0
    vGLYC = 68.8
    vPO4 = 53.7
    vCHOL = 120.4
    vWAT = 30.4

    vHead = vCHOL + vPO4 + vGLYC + 2 * vCOO
    vTail = 2 * (16 * vCH2) + 2 * (vCH3)

    # make SLDs
    thisMask = deut[contrast]

    if thisMask[0] == 0:
        thisWater = (H2O * 0.9249) + (D2O * 0.0871)
    else:
        thisWater = D2O

    # Calculate mole fraction of D2O from the bulk SLD
    d2o_molfr = (1 / D2O - H2O) * ((bulk_out[contrast] / 0.036182336306) - H2O)
    thisWater = (d2o_molfr * D2O) + ((1 - d2o_molfr) * H2O)

    if thisMask[1] == 0:
        thisHead = CHOL + PO4 + GLYC + COO
    else:
        thisHead = dCHOL + PO4 + GLYC + COO

    if thisMask[2] == 0:
        thisTail = (32 * CH2) + CH3
    else:
        thisTail = (32 * dCH2) + dCH3

    noWat = ((thickHead * APM) - vHead) / vWAT
    thisHead = thisHead + noWat * thisWater
    vHead = vHead + noWat * vWAT

    sldHead = thisHead / vHead

    sldTail = thisTail / vTail

    thickTail = (1.5 + 16 * 1.265) * cos(radians(theta))

    output = [[thickTail, sldTail, Roughness], [thickHead, sldHead, Roughness]]

    sub_rough = Roughness

    return output, sub_rough
