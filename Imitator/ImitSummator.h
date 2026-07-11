/*
 * Автор: Назарычев Артём
 *
 * Наименование модуля: Сумматор
 *
 * Назначение: Модуль сумирует массивы сигналов в один единственный
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef IMITSUMMATOR_H_
#define IMITSUMMATOR_H_
#include "ImitSummatorParam.h"
#include "NoiseGenerator.h"
#include "UnifedTime.h"
#include "NIPFormationSignals.h"
#include "TargetFormationSignals.h"
#include "ClutterFormationSignals.h"


struct ImitSummatorOut{
	float *sum_signals;
};

int Summator(struct SummatorParam *p, struct UnifedTimeOut *time, struct UTimeParam *timeP, struct TargetSignalsOut *tagret,  struct ClutterSignalsOut *pp, struct NIPSignalsOut *nip, struct NoiseGeneratorOut *noise, struct ImitSummatorOut *out);
#endif /* IMITSUMMATOR_H_ */
