"""Custom model file for the domains custom layers example."""


def alloy_domains(params, bulkIn, bulkOut, contrast, domain):
    """Calculate custom model layers for a permalloy/gold model with domains.

    Simple custom model for testing incoherent summing.
    Simple two layer of permalloy / gold, with up/down domains.
    """
    # Note - The first contrast number is 1 (not 0) so be careful if you use
    # this variable for array indexing. Same applies to the domain number.

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
    if domain == 1:
        output = [alloyUp, gold]
    elif domain == 2:
        output = [alloyDn, gold]

    return output, subRough
