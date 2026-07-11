#ifndef UNIFIEDIMITATORPARAM_H
#define UNIFIEDIMITATORPARAM_H


#include "UnifedTime.h"
#include "NoiseGenerator.h"
#include "AzimuthSensor.h"
#include "ClutterResponse.h"
#include "PPPosition.h"
#include "TargetPosition.h"
#include "NIPPosition.h"
#include "ImitatorParam.h"
#include "NIPLevel.h"
#include "NIPFormationSignals.h"
#include "TargetFormationSignals.h"
#include "ClutterFormationSignals.h"
#include "ImitSummator.h"
#include "FrequencyConverter.h"

struct ImitatorParametrs{
    struct UTimeParam *uTime;
    struct TargetResponseParams *targetResponse;
    struct TargetPosParam *targetPosition;
    struct TargetFormationParam *targetFormation;
    struct PPPosParam *ppPosition;
    struct NoiseParam *noise;
    struct NIPPosParam *nipPosition;
    struct NIPLvlParam *nipLevel;
    struct NIPFormationParam *nipFormation;
    struct SummatorParam *summator;
    struct ImitParam *imitator;
    struct FreqConvertorParam *frequencyConverter;
    struct ClutterResponseParams *clutterResponse;
    struct ClutterFormationParam *clutterFormation;
    struct AzimutParam *azimuth;
};

#endif // UNIFIEDIMITATORPARAM_H
