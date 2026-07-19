#include "../include/processing_module.h"
#include "../include/processing_module_param.h"
//#include "../include/ADC.h"
#include "../include/DDC.h"
#include "../include/codogramm.h"
#include "../include/suppression_NIP.h"
#include "../include/nn.h"
#include "Handler/include/threshold.h"

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    //struct data adc_out;
    struct data ddc_out;
    struct data sNIP_out;
    struct data NN_out;
    struct Codogramm threshold_generator_out;
    struct Codogramm threshold_device_out;
    struct Codogramm Final_Sign;
    
     

    DDC_Process(Params, Input_Sign, &ddc_out);

    suppression_NIP(Params, &ddc_out, &sNIP_out);

    NN_Process(Params, &sNIP_out, &NN_out);

    threshold_generator(Params, &NN_out, &threshold_generator_out);

    threshold_device(&NN_out, &threshold_generator_out, &threshold_device_out);

    //AzimuthSensor(&threshold_device_out, &NN_out, &azimuth_measure);

    former_codogramm(&threshold_device_out, &Final_Sign);

    Output_sign->TimeData = Input_Sign->TimeData;
    Output_sign->AzimuthData = Input_Sign->AzimuthData;
    Output_sign->index = 0;
    Output_sign->number_of_objects = 0;

    if (Input_Sign == 0 || Input_Sign->TargetPositionData == 0 || Input_Sign->TargetPositionData->Target_map == 0) {
        return 0;
    }

    int threshold = (Params != 0) ? Params->threshold.threshold : 0;
    int count = Input_Sign->TargetPositionData->cntTarget;

    for (int i = 0; i < count && Output_sign->number_of_objects < 256; ++i) {
        struct Point target = Input_Sign->TargetPositionData->Target_map[i];
        if (target.amplitude <= 0.0f) {
            continue;
        }

        Output_sign->sign[Output_sign->number_of_objects].distance = (int)target.distance;
        Output_sign->sign[Output_sign->number_of_objects].AzimuthData = Input_Sign->AzimuthData;
        Output_sign->sign[Output_sign->number_of_objects].amplitude = (int)target.amplitude;
        Output_sign->sign[Output_sign->number_of_objects].threshold = threshold;
        Output_sign->sign[Output_sign->number_of_objects].number = Output_sign->number_of_objects + 1;
        ++Output_sign->number_of_objects;
    }

    return 0;
}