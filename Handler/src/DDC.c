//
// Created by Максим Мирясов on 10.07.2026.
//
#include "../include/DDC.h"

int DDC_Process(struct GlobalProcessingParam *param, struct ImitOutData *in, struct data *out)
{
    if (param != 0 && param->DDC.enable == 1 && in != 0 && out != 0) {
        if (in->AzimuthData != 0 && out->data.AzimuthData != 0) {
            *out->data.AzimuthData = *in->AzimuthData;
        }
        if (in->TimeData != 0 && out->data.TimeData != 0) {
            *out->data.TimeData = *in->TimeData;
        }
        if (in->SummatorData != 0 && out->data.UAD != 0) {
            *out->data.UAD = *in->SummatorData;
        }
    }
    return 0;
}
