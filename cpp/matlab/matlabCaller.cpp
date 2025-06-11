#include "matlabCaller.h"

LIB_EXPORT void startMatlab()
{
    MatlabCaller::get_instance()->setEngine();;
}

LIB_EXPORT void cd(std::string path)
{
    MatlabCaller::get_instance()->cd(path);
}

LIB_EXPORT void callFunction(std::string functionName, std::vector<double>& params, std::vector<double>& bulkIn, 
                             std::vector<double>& bulkOut, int contrast, int domain, std::vector<double>& output, double* outputSize, double* rough)
{
    MatlabCaller::get_instance()->call(functionName, params, bulkIn, bulkOut, contrast, domain, output, outputSize, rough);
}
