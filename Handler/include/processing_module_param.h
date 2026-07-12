#ifndef PROCESSING_MODULE_PARAM_H
#define PROCESSING_MODULE_PARAM_H
#include <stdbool.h>

struct suppression_NIP_Param
{
    bool enable;
};

struct ADC_Param
{
    bool enable;
};

struct DDC_Param
{
    bool enable;
};

struct NN_Param
{
    bool enable;
};

struct Threshold_Param
{
    bool enable;
    int threshold;
};

struct GlobalProcessingParam
{
    struct ADC_Param ADC;
    struct DDC_Param DDC;
    struct NN_Param NN;
    struct suppression_NIP_Param suppression_NIP;
    struct Threshold_Param threshold;
};

#endif