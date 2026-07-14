/*
 * Автор: Дормидонтов Захар
 *
 * Наименование модуля: Датчик азимута
 *
 * Назначение: Данный модуль осуществляет изменение азимута направления излучение антенны
 * В данном файле: описывается структура параметров модуля
 *
 */


#ifndef AZIMUTHSENSORPARAM_H_
#define AZIMUTHSENSORPARAM_H_

struct AzimutParam{
	int enable; //включение вращения
	float startAngle; //Начальный угол в градусах
	float angularVelocity; //угловая скорость в градусах
};

#endif /* AZIMUTHSENSORPARAM_H_ */
