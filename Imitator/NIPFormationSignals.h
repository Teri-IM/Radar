/*
 * Автор: Тутин Григорий
 *
 * Наименование модуля: Формирование сигнала НИП
 *
 * Назначение: Данный модуль формирует массив сигналов НИП
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef NIPFORMATIONSIGNALS_H_
#define NIPFORMATIONSIGNALS_H_
#include "NIPFormationSignalsParam.h"
#include "NIPPosition.h"
#include "UnifiedTimeParam.h"
struct NIPSignalsOut{
    float *nip_signals;
};

int FormNIPSignal(struct NIPFormationParam *p,
				struct NIPPositionOut *nipPosIn,
                   struct UTimeParam *timeP,
				   struct NIPSignalsOut *out);


#endif /* NIPFORMATIONSIGNALS_H_ */
