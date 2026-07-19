//
// Created by Максим Мирясов on 10.07.2026.
//
#include "../include/DDC.h"

int DDC_Process(struct GlobalProcessingParam *param, struct ImitOutData *in, struct data *out)
{
    if (param->DDC.enable == 1)
        {
            *out->data.AzimuthData = *in->AzimuthData;
            *out->data.TimeData = *in->TimeData;
            *out->data.UAD = *in->SummatorData;
        }   
    return 0;
}
