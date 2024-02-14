import pathlib
import numpy as np
import matlab.engine
from rat import rat_core


class MatlabWrapper:
    def __init__(self, filename):
        self.engine = matlab.engine.start_matlab()
        path = pathlib.Path(filename)
        self.engine.cd(str(path.parent), nargout=0)
        self.function_name = path.stem

    def __del__(self):
        self.engine.quit()

    def getHandle(self):                          
        def handle(params, bulk_in, bulk_out, contrast, domain=-1):
            if domain == -1: 
                output, sub_rough = getattr(self.engine, self.function_name)(np.array(params, 'float'), 
                                                                             np.array(bulk_in, 'float'), 
                                                                             np.array(bulk_out, 'float'), 
                                                                             float(contrast + 1), nargout=2)                      
            else:
                output, sub_rough = getattr(self.engine, self.function_name)(np.array(params, 'float'), 
                                                                             np.array(bulk_in, 'float'), 
                                                                             np.array(bulk_out, 'float'), 
                                                                             float(contrast + 1), float(domain + 1), nargout=2)
            return output, sub_rough                         
        return handle 


class DylibWrapper:
    def __init__(self, filename, function_name):
        self.engine = rat_core.DylibEngine(filename, function_name)
    
    def getHandle(self):                           
        def handle(params, bulk_in, bulk_out, contrast, domain=-1):
            if domain == -1: 
                output, sub_rough = self.engine.invoke(params, bulk_in, bulk_out, contrast)
            else:
                output, sub_rough = self.engine.invoke(params, bulk_in, bulk_out, contrast, domain)
            return output, sub_rough                                               
        return handle
 