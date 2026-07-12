#include "include\processing_module.h"
#include "include\processing_module_param.h"

int main()
{
    GlobalProcessingParam Param;
    data Input_Sign, Output_Sign;
    Param.ADC.enable = 1;
    Param.DDC.enable = 1;
    Param.NN.enable = 1;
    Param.suppression_NIP.enable = 1;
    Param.threshold.enable = 1;
    Param.threshold.threshold = 50;
    Input_Sign.data.azimuth = 1;
    Input_Sign.data.index = 1;
    Input_Sign.data.time = 1;
    Input_Sign.data.UAD = 10;
    for (int i=0; i<Input_Sign.data.UAD; i++)
    {
        Input_Sign.amplitude[i] = i;
    }
    int ProcessingModule(GlobalProcessingParam *Param, data *Input_Sign, data *Output_sign);
    
    return 0;
}