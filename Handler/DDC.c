//
// Created by Максим Мирясов on 10.07.2026.
//
#include "DDC.h"

int DDC_Process(struct DDC_Param *param, struct data *in, struct data *out)
{
    if (param->enable == 1)
        {
            *out = *in;
        }   
    return 0;
}
