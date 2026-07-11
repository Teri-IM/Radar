/*
 * Автор: Родионова Юлия
 *
 * Наименование модуля: Расчет уровней НИП
 *
 * Назначение: Понижает значение амплитуды сигнала НИП если он был получен не при засечении точки
 * В данном файле: осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef NIPLEVEL_H_
#define NIPLEVEL_H_
#include "NIPLevelParam.h"
#include "NIPPosition.h"
#include "UnifedTime.h"
#include "AzimuthSensor.h"

int NIPLevelCalculate(struct NIPLvlParam *p, struct UTimeParam *timeP, struct AzimutSensorOut *azimutIn, struct NIPPositionOut *nipPosInOut);
#endif /* NIPLEVEL_H_ */
