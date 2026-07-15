#ifndef PROCESSING_MODULE_H
#define PROCESSING_MODULE_H


#include "processing_module_param.h"
#include "../../Imitator/Imitator.h"


struct Instruct
{
    struct AzimutSensorOut *AzimuthData;
    struct UnifedTimeOut *TimeData;
    int index;
    struct ImitSummatorOut *UAD; /*useful amount of data*/
};


struct threshold_device_out
{
    int distance;
    struct AzimutSensorOut *AzimuthData;
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
    struct UnifedTimeOut *TimeData;
    struct AzimutSensorOut *AzimuthData;
    int index;
    int number_of_objects;
    struct threshold_device_out sign[256];
}; //структура, которую мы отдаем в визуал

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign);
#endif