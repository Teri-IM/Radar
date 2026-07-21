/*
 * Автор: Назарычев Артём
 *
 * Наименование модуля: Имитатор
 *
 * Назначение: Модуль отладки с описанием фунции генерации выходных данных модуля
 *
 */
#ifndef IMITATOR_H_
#define IMITATOR_H_
#include "UnifiedImitatorParam.h"

struct ImitOutData{
    struct UnifedTimeOut *TimeData;
    struct AzimutSensorOut *AzimuthData;
    struct ImitSummatorOut *SummatorData;
    struct TargetPositionOut *TargetPositionData;
    struct TargetResponseOut *TargetResponseData;
};

int createImitatatorOutData(struct ImitSummatorOut *summator_data, struct UnifedTimeOut *time_data, struct AzimutSensorOut *azimuth_data, struct ImitOutData *out);
int Imitator(struct ImitatorParametrs *parametrs, struct ImitOutData *out);

#endif /* IMITATOR_H_ */
