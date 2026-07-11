/*
 * Автор: Коротин Михаил
 *
 * Наименование модуля: Расчет откликов целей
 *
 * Назначение: Данный модуль обнаружает цели при прохождении антенны через них и определяет номер дискредитизации
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef CLUTTER_RESPONSE_H
#define CLUTTER_RESPONSE_H
#include "AzimuthSensor.h"
#include "PPPosition.h"
#include "UnifedTime.h"
#include <math.h>
#include "ClutterResponseParams.h"

struct ClutterResponseOut{
	struct PointWith_discredNum *PP_map_find;
	int cntPP_find;
};

int clutter_response_calc(
    struct ClutterResponseParams *params,
	struct UTimeParam *timeParam,
    struct UnifedTimeOut *time_data,
    struct AzimutSensorOut *azimut_data,
    struct PPPositionOut *points_data,
	struct ClutterResponseOut *out
);
#endif

