#ifndef MATLAB_CALLER_IMPL_HPP
#define MATLAB_CALLER_IMPL_HPP

#include "engine.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <cstring>

using namespace std::chrono;

class MatlabCaller
{

public:
    MatlabCaller(MatlabCaller const&) = delete;
    MatlabCaller& operator=(MatlabCaller const&) = delete;
    ~MatlabCaller() {}

    void setEngine(){
        if (!(matlabPtr = engOpen(""))) {
            throw std::runtime_error("\nCan't start MATLAB engine\n");
        }
        engSetVisible(matlabPtr, 0);
    };
    
    void cd(std::string path){
        this->currentDirectory = path;
        dirChanged = true;
    };

    void call(std::string functionName, std::vector<double>& xdata, std::vector<double>& params, std::vector<double>& output)
    {   
        if (!matlabPtr)
            setEngine();  
        
        if (dirChanged){
            std::string cdCmd =   "cd('" + (this->currentDirectory + "')");
            engEvalString(this->matlabPtr, cdCmd.c_str());
        }

        dirChanged = false;
        mxArray *XDATA = mxCreateDoubleMatrix(1,xdata.size(),mxREAL);
        memcpy(mxGetPr(XDATA), &xdata[0], xdata.size()*sizeof(double));
        engPutVariable(this->matlabPtr, "xdata", XDATA);
        mxArray *PARAMS = mxCreateDoubleMatrix(1,params.size(),mxREAL);
        memcpy(mxGetPr(PARAMS), &params[0], params.size()*sizeof(double));
        engPutVariable(this->matlabPtr, "params", PARAMS);

        std::string customCmd = "[output, subRough] = " + (functionName + "(params, bulkIn, bulkOut, contrast)");
        engPutVariable(this->matlabPtr, "myFunction", mxCreateString(customCmd.c_str()));
        engOutputBuffer(this->matlabPtr, NULL, 0);
        engEvalString(this->matlabPtr, "eval(myFunction)");

        mxArray *matOutput = engGetVariable(this->matlabPtr, "output");
        if (matOutput == NULL)
        {
            throw std::runtime_error("ERROR: Results could not be extracted from MATLAB engine.");
        }
	    
        const mwSize* dims = mxGetDimensions(matOutput);
        double* s = (double *)mxGetData(matOutput);
        for (int i=0; i < dims[0] * dims[1]; i++)
            output.push_back(s[i]);
    };
    
    void call(std::string functionName, std::vector<double>& params, std::vector<double>& bulkIn, 
              std::vector<double>& bulkOut, int contrast, int domain, std::vector<double>& output, double* outputSize, double* rough)
    {   
        if (!matlabPtr)
            setEngine();  
        
        if (dirChanged){
            std::string cdCmd =   "cd('" + (this->currentDirectory + "')");
            engEvalString(this->matlabPtr, cdCmd.c_str());
        }

        dirChanged = false;
        mxArray *PARAMS = mxCreateDoubleMatrix(1,params.size(),mxREAL);
        memcpy(mxGetPr(PARAMS), &params[0], params.size()*sizeof(double));
        engPutVariable(this->matlabPtr, "params", PARAMS);
        mxArray *BULKIN = mxCreateDoubleMatrix(1,bulkIn.size(),mxREAL);
        memcpy((void *)mxGetPr(BULKIN), &bulkIn[0], bulkIn.size()*sizeof(double));
        engPutVariable(this->matlabPtr, "bulkIn", BULKIN);
        mxArray *BULKOUT = mxCreateDoubleMatrix(1,bulkOut.size(),mxREAL);
        memcpy((void *)mxGetPr(BULKOUT), &bulkOut[0], bulkOut.size()*sizeof(double));
        engPutVariable(this->matlabPtr, "bulkOut", BULKOUT);
        mxArray *CONTRAST = mxCreateDoubleScalar(contrast);
        engPutVariable(this->matlabPtr, "contrast", CONTRAST);
        std::string customCmd;
        if (domain > 0){
            mxArray *DOMAIN_NUM = mxCreateDoubleScalar(domain);
            engPutVariable(this->matlabPtr, "domain", DOMAIN_NUM);
            customCmd = "[output, subRough] = " + (functionName + "(params, bulkIn, bulkOut, contrast, domain)");
        }
        else {
            customCmd = "[output, subRough] = " + (functionName + "(params, bulkIn, bulkOut, contrast)");
        }
        engPutVariable(this->matlabPtr, "myFunction", mxCreateString(customCmd.c_str()));
        engOutputBuffer(this->matlabPtr, NULL, 0);
        engEvalString(this->matlabPtr, "eval(myFunction)");

        mxArray *matOutput = engGetVariable(this->matlabPtr, "output");
        mxArray *subRough = engGetVariable(this->matlabPtr, "subRough");
        if (matOutput == NULL || subRough == NULL)
        {
            throw std::runtime_error("ERROR: Results could not be extracted from MATLAB engine.");
        }
	    
        *rough = (double)mxGetScalar(subRough);
        const mwSize* dims = mxGetDimensions(matOutput);
        outputSize[0] = (double) dims[0];
        outputSize[1] = (double) dims[1];
        double* s = (double *)mxGetData(matOutput);
        for (int i=0; i < dims[0] * dims[1]; i++)
            output.push_back(s[i]);
    };

    static MatlabCaller* get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static MatlabCaller instance{};
        return &instance;
    };

private:
    explicit MatlabCaller() {}
    Engine *matlabPtr;
    std::string currentDirectory;
    bool dirChanged = false;
};

#endif // MATLAB_CALLER_IMPL_HPP
