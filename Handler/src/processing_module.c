#include "../include/processing_module.h"
#include "../include/processing_module_param.h"
#include "../include/ADC.h"
#include "../include/DDC.h"
#include "../include/codogramm.h"
#include "../include/suppression_NIP.h"
#include "../include/nn.h"

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    //struct data adc_out;
    //struct data ddc_out;
    //struct data sNIP_out;
    //struct data NN_out;
    //struct Codogramm threshold_generator_out;
    //struct Codogramm threshold_device_out;
    //struct Codogramm Final_Sign;
     

    //DDC_Process(&(Params->DDC), Input_Sign, &ddc_out);

    //suppression_NIP(&(Params->suppression_NIP), &ddc_out, &sNIP_out);

    //NN_Process(&(Params->NN), &sNIP_out, &NN_out);

    //threshold_generator(&(Params->threshold), &NN_out, &threshold_generator_out);

    //threshold_device (&threshold_generator_out, &threshold_device_out)

    //former_codogramm(&NN_out, &Final_Sign);

    Output_sign->TimeData = Input_Sign->TimeData;
    Output_sign->AzimuthData = Input_Sign->AzimuthData;

    return 0;
}