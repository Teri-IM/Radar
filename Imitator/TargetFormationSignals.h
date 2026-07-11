/*
 * Автор: Конхон Петр
 *
 * Наименование модуля: Формирование сигналов целей
 *
 * Назначение: Данный модуль формирует массив сигналов целей
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef TARGETFORMATIONSIGNALS_H_
#define TARGETFORMATIONSIGNALS_H_

#include "TargetFormationSignalsParam.h"
#include "TargetResponse.h"
#include "UnifiedTimeParam.h"
struct TargetSignalsOut{
    float *target_signals;
};

int FormTargetSignal(struct TargetFormationParam *p,
				struct TargetResponseOut *nipPosIn,
                   struct UTimeParam *timeP,
				   struct TargetSignalsOut *out);

#endif /* TARGETFORMATIONSIGNALS_H_ */
