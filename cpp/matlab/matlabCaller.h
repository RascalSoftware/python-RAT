#include "matlabCallerImpl.hpp"

#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_WIN32) || defined(_WIN64)
#define LIB_EXPORT __declspec(dllexport)
#else
#define LIB_EXPORT
#endif


LIB_EXPORT void startMatlab();

LIB_EXPORT void cd(std::string path);

LIB_EXPORT void callFunction(std::string functionName, std::vector<double>& params, std::vector<double>& bulkIn, 
                             std::vector<double>& bulkOut, int contrast, int domain, std::vector<double>& output, double* outputSize, double* rough);

#ifdef __cplusplus
}
#endif

#endif  // EVENT_MANAGER_H