/*
 * Автор: Ниязбаев Александр
 *
 * Наименование модуля: Расчет откликов целей
 *
 * Назначение: Данный модуль обнаружает цели при прохождении антенны через них и определяет номер дискредитизации
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */
#ifndef TARGETRESPONSE_H_
#define TARGETRESPONSE_H_
#include "AzimuthSensor.h"
#include "TargetPosition.h"
#include "UnifedTime.h"
#include <math.h>
#include "TargetResponseParam.h"

struct TargetResponseOut{
	struct PointWith_discredNum *target_map_find;
	int cntTarget_find;
};

int target_response_calc(
    struct TargetResponseParams *params,
	struct UTimeParam *timeParam,
    struct UnifedTimeOut *time_data,
    struct AzimutSensorOut *azimut_data,
    struct TargetPositionOut *points_data,
	struct TargetResponseOut *out
);



#endif /* TARGETRESPONSE_H_ */
