/*
 * Автор: Дормидонтов Захар
 *
 * Наименование модуля: Датчик азимута
 *
 * Назначение: Данный модуль осуществляет изменение азимута направления излучение антенны
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef AZIMUTHSENSOR_H_
#define AZIMUTHSENSOR_H_
#include "AzimuthSensorParam.h"
#include "UnifedTime.h"

struct AzimutSensorOut{
	float azimuth_new; //Новое положение азимута (град)
	float azimuth_old; //Старое положение азимута (град)
	float angularVelocity; //Угравая скорость (град/с)
};

int AzimuthSensor(struct AzimutParam *p, struct UTimeParam *timeParam, struct UnifedTimeOut *in, struct AzimutSensorOut *out, int tickRate);


#endif /* AZIMUTHSENSOR_H_ */
