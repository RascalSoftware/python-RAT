def volume_thiol_bilayer(params, bulk_in, bulk_out, contrast):
    """VolumeThiolBilayer  RAT Custom Layer Model File.

    This file accepts 3 vectors containing the values for params, bulk in and bulk out.
    The final parameter is an index of the contrast being calculated

    The function should output a matrix of layer values, in the form...

    Output = [thick 1, SLD 1, Rough 1, Percent Hydration 1, Hydrate how 1
              ....
              thick n, SLD n, Rough n, Percent Hydration n, Hydration how n]

    The "hydrate how" parameter decides if the layer is hydrated with Bulk out or Bulk in phases.
    Set to 1 for Bulk out, zero for Bulk in.
    Alternatively, leave out hydration and just return...

    Output = [thick 1, SLD 1, Rough 1,
              ....
              thick n, SLD n, Rough n]

    The second output parameter should be the substrate roughness.
    """
    subRough = params[0]
    alloyThick = params[1]
    alloySLDUp = params[2]
    alloyISLDUp = params[3]
    alloySLDDown = params[4]
    alloyISLDDown = params[5]
    alloyRough = params[6]
    goldThick = params[7]
    goldRough = params[8]
    goldSLD = params[9]
    goldISLD = params[10]
    thiolAPM = params[11]
    thiolHeadHydr = params[12]
    thiolCoverage = params[13]
    cwThick = params[14]
    bilayerAPM = params[15]
    bilHeadHydr = params[16]
    bilayerRough = params[17]
    bilayerCoverage = params[18]

    # Make the metal layers
    gold = [goldThick, goldSLD, goldISLD, goldRough]
    alloyUp = [alloyThick, alloySLDUp, alloyISLDUp, alloyRough]
    alloyDown = [alloyThick, alloySLDDown, alloyISLDDown, alloyRough]

    # Neutron b's..
    # define all the neutron b's.
    bc = 0.6646e-4  # Carbon
    bo = 0.5843e-4  # Oxygen
    bh = -0.3739e-4  # Hydrogen
    bp = 0.513e-4  # Phosphorus
    bn = 0.936e-4  # Nitrogen

    # Work out the total scattering length in each fragment
    # Define scattering lengths
    # Hydrogenated version
    COO = (2 * bo) + (bc)
    GLYC = (3 * bc) + (5 * bh)
    CH3 = (1 * bc) + (3 * bh)
    PO4 = (1 * bp) + (4 * bo)
    CH2 = (1 * bc) + (2 * bh)
    CH = (1 * bc) + (1 * bh)
    CHOL = (5 * bc) + (12 * bh) + (1 * bn)

    # And also volumes
    vCH3 = 52.7  # CH3 volume in the paper appears to be for 2 * CH3's
    vCH2 = 28.1
    vCOO = 39.0
    vGLYC = 68.8
    vPO4 = 53.7
    vCHOL = 120.4
    vCHCH = 42.14

    vHead = vCHOL + vPO4 + vGLYC + 2 * vCOO
    vTail = (28 * vCH2) + (1 * vCHCH) + (2 * vCH3)  # Tail volume

    # Calculate sum_b's for other fragments
    sumbHead = CHOL + PO4 + GLYC + 2 * COO
    sumbTail = (28 * CH2) + (2 * CH) + 2 * CH3

    # Calculate SLDs and Thickness
    sldHead = sumbHead / vHead
    thickHead = vHead / thiolAPM

    sldTail = sumbTail / vTail
    thickTail = vTail / thiolAPM

    # Correct head SLD based on hydration
    thiolHeadHydr = thiolHeadHydr / 100
    sldHead = sldHead * (1 - thiolHeadHydr) + (thiolHeadHydr * bulk_out[contrast])

    # Now correct both the SLDs for the coverage parameter
    sldTail = (thiolCoverage * sldTail) + ((1 - thiolCoverage) * bulk_out[contrast])
    sldHead = (thiolCoverage * sldHead) + ((1 - thiolCoverage) * bulk_out[contrast])

    SAMTAILS = [thickTail, sldTail, 0, goldRough]
    SAMHEAD = [thickHead, sldHead, 0, goldRough]

    # Now do the same for the bilayer
    vHead = vCHOL + vPO4 + vGLYC + 2 * vCOO
    vTail = 28 * vCH2  # Tail volume
    vMe = 2 * vCH3

    sumbHead = CHOL + PO4 + GLYC + 2 * COO
    sumbTail = 28 * CH2
    sumbMe = 2 * CH3

    sldHead = sumbHead / vHead
    thickHead = vHead / bilayerAPM
    bilHeadHydr = bilHeadHydr / 100
    sldHead = sldHead * (1 - bilHeadHydr) + (bilHeadHydr * bulk_out[contrast])

    sldTail = sumbTail / vTail
    thickTail = vTail / bilayerAPM

    sldMe = sumbMe / vMe
    thickMe = vMe / bilayerAPM

    sldTail = (bilayerCoverage * sldTail) + ((1 - bilayerCoverage) * bulk_out[contrast])
    sldHead = (bilayerCoverage * sldHead) + ((1 - bilayerCoverage) * bulk_out[contrast])
    sldMe = (bilayerCoverage * sldMe) + ((1 - bilayerCoverage) * bulk_out[contrast])

    BILTAILS = [thickTail, sldTail, 0, bilayerRough]
    BILHEAD = [thickHead, sldHead, 0, bilayerRough]
    BILME = [thickMe, sldMe, 0, bilayerRough]

    BILAYER = [BILHEAD, BILTAILS, BILME, BILME, BILTAILS, BILHEAD]

    CW = [cwThick, bulk_out[contrast], 0, bilayerRough]

    if contrast == 1 or contrast == 3:
        output = [alloyUp, gold, SAMTAILS, SAMHEAD, CW, *BILAYER]
    else:
        output = [alloyDown, gold, SAMTAILS, SAMHEAD, CW, *BILAYER]

    return output, subRough
