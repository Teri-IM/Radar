#ifndef PROCESSING_MODULE_H
#define PROCESSING_MODULE_H

#include "ADC.h"
#include "DDC.h"
#include "codogramm.h"
#include "suppression_NIP_param.h"
#include "suppression_NIP.h"
#include "nn.h"
#include "processing_module_param.h"


struct Instruct
{
    int azimuth;
    int time; //будет структурой
    int index;
    int UAD; /*useful amount of data*/
};


struct threshold_device_out
{
    int distance;
    int azimuth;
    int amplitude;
    int threshold;
    int number;
}; //структура, которую создает пороговое устройство и составляет из них массив

struct data
{
    struct Instruct data;
    float amplitude[1024];
}; //структура, которую мы получаем от имитатора (сырая версия)

struct Codogramm
{
    int time;
    int index;
    int number_of_objects;
    struct threshold_device_out sign[256];
}; //структура, которую мы отдаем в визуал

int ProcessingModule(struct GlobalProcessingParam *Params, struct data *Input_Sign, struct data *Output_sign);
#endif