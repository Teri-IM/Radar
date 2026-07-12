//
// Created by Максим Мирясов on 10.07.2026.
//
#include "include/DDC.h"

int DDC_Process(struct GlobalProcessingParam *param, struct data *in, struct data *out)
{
    if (param->DDC.enable == 1)
        {
            *out = *in;
        }   
    return 0;
}
