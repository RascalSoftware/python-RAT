def alloy_domains(params, bulkIn, bulkOut, contrast, domain):
    """Simple custom model for testing incoherent summing.
    Simple two layer of permalloy / gold, with up/down domains.
    """
    # Split up the parameters
    subRough = params[0]
    alloyThick = params[1]
    alloySLDup = params[2]
    alloySLDdn = params[3]
    alloyRough = params[4]
    goldThick = params[5]
    goldSLD = params[6]
    goldRough = params[7]

    # Make the layers
    alloyUp = [alloyThick, alloySLDup, alloyRough]
    alloyDn = [alloyThick, alloySLDdn, alloyRough]
    gold = [goldThick, goldSLD, goldRough]

    # Make the model depending on which domain we are looking at
    if domain == 0:
        output = [alloyUp, gold]
    else:
        output = [alloyDn, gold]

    return output, subRough
