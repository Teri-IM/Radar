/*
 * Автор: Чурушкин Кирилл
 *
 * Наименование модуля: Формирование сигналов целей
 *
 * Назначение: Данный модуль формирует массив сигналов пассивных помех
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef CLUTTERFORMATIONSIGNALS_H_
#define CLUTTERFORMATIONSIGNALS_H_
#include "ClutterFormationSignalsParam.h"
#include "ClutterResponse.h"
#include "UnifiedTimeParam.h"
struct ClutterSignalsOut{
    float *clutter_signals;
};

int FormClutterSignal(struct ClutterFormationParam *p,
				struct ClutterResponseOut *PosIn,
                   struct UTimeParam *timeP,
				   struct ClutterSignalsOut *out);


#endif /* CLUTTERFORMATIONSIGNALS_H_ */
