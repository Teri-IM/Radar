/*
 * Автор: Дормидонтов Захар
 *
 * Наименование модуля: Датчик азимута
 *
 * Назначение: Данный модуль осуществляет изменение азимута направления излучение антенны
 * В Данном файле описывается функция изменения азимута
 *
 */

#include "AzimuthSensor.h"
#include <math.h>

int AzimuthSensor(struct AzimutParam *p, struct UTimeParam *timeParam, struct UnifedTimeOut *in, struct AzimutSensorOut *out, int tickRate)
{
	if (p == NULL || timeParam == NULL || in == NULL ) {
	        return 1;
	}

	out->azimuth_old = out->azimuth_new;
    float increment = (p->angularVelocity * timeParam->probing_time) / 50000.0f / tickRate;
    out->azimuth_new = fmodf(out->azimuth_old + increment, 360.0f);
    return 0;
}
