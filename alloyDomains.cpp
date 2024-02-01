//alloyDomains.cpp

#include <vector>

#if defined(_WIN32) || defined(_WIN64)
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif

// We user extern "C" decorator to avoid name mangling....
extern "C" {

    LIB_EXPORT void alloyDomains(std::vector<double>& params, std::vector<double>& bulkIn, std::vector<double>& bulkOut, int contrast, int domain, std::vector<double>& output, double* outputSize, double* rough)
    {
        double subRough = params[0];
        double alloyThick = params[1];
        double alloySLDup = params[2];
        double alloySLDdn = params[3];
        double alloyRough = params[4];
        double goldThick = params[5];
        double goldSLD = params[6];
        double goldRough = params[7];

        // Make the layers
        if (domain == 0) {
            // alloyUp
            output.push_back(alloyThick);
            output.push_back(alloySLDup);
            output.push_back(alloyRough);

        }else {
            // alloyDn
            output.push_back(alloyThick);
            output.push_back(alloySLDdn);
            output.push_back(alloyRough);
        }
        // Gold
        output.push_back(goldThick);
        output.push_back(goldSLD);
        output.push_back(goldRough);

        *rough = subRough;
        
        outputSize[0] = 2;     // row - Necessary to ouptut how many layers in stack
        outputSize[1] = 3;     // col - Should be different depending on calculation 
    }
} // extern "C"