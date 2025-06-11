#ifndef MATLAB_CALLER_IMPL_HPP
#define MATLAB_CALLER_IMPL_HPP

#include "engine.h"
#include <vector>
#include <iostream>
#include <chrono>

using namespace std::chrono;

class MatlabCaller
{

public:
    MatlabCaller(MatlabCaller const&) = delete;
    MatlabCaller& operator=(MatlabCaller const&) = delete;
    ~MatlabCaller() {}

    void setEngine(){
        if (!(matlabPtr = engOpen(""))) {
            throw("\nCan't start MATLAB engine\n");
        }
    };

    void startMatlab(){
       // this->matlabFuture = startMATLABAsync();
    };
    
    void cd(std::string path){
        this->currentDirectory = path;
        dirChanged = true;
    };

    void call(std::string functionName, std::vector<double>& params, std::vector<double>& bulkIn, 
              std::vector<double>& bulkOut, int contrast, int domain, std::vector<double>& output, double* outputSize, double* rough)
    {   
        if (!this->matlabPtr)
            this->setEngine();  
        if (dirChanged){
            std::string cdCmd =   "cd('" + (this->currentDirectory + "')");
            engEvalString(this->matlabPtr, cdCmd.c_str());
        }
        //this->matlabPtr->feval(u"cd", factory.createCharArray(this->currentDirectory));
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
        // memcpy((void *)mxGetPr(CONTRAST), &contrast, 1*sizeof(double));
        engPutVariable(this->matlabPtr, "contrast", CONTRAST);
        // if (domain > 0)
        //     args.push_back(factory.createScalar<int>(domain));
        std::string customCmd = "[output, subRough] = " + (functionName + "(params, bulkIn, bulkOut, contrast)");
        engPutVariable(this->matlabPtr, "myFunction", mxCreateString(customCmd.c_str()));
        engOutputBuffer(this->matlabPtr, NULL, 0);
        //auto start = high_resolution_clock::now();
        // std::vector<matlab::data::Array> results = this->matlabPtr->feval(functionName, 2, args);
        engEvalString(this->matlabPtr, "eval(myFunction)");
        //auto stop = high_resolution_clock::now();
        //auto duration = duration_cast<microseconds>(stop - start);
        //std::cout << duration.count() << "Usec" << std::endl;

        mxArray *matOutput = engGetVariable(this->matlabPtr, "output");
        if (matOutput == NULL)
        {
            throw("FAILED!");
        }
	    mxArray *subRough = engGetVariable(this->matlabPtr, "subRough");
        if (subRough == NULL)
        {
            throw("FAILED!");
        }
        *rough = (double)mxGetScalar(subRough);
        const mwSize* dims = mxGetDimensions(matOutput);
        outputSize[0] = (double) dims[0];
        outputSize[1] = (double) dims[1];
        //     output.push_back((double) matOutput[i]);
        double* s = (double *)mxGetData(matOutput);
        for (int i=0; i < dims[0] * dims[1]; i++)
            output.push_back(s[i]);
        //std::memcpy(output, (double *)mxGetData(matOutput), mxGetNumberOfElements(matOutput)* mxGetElementSize(matOutput));
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
