/*
 * Автор: Чурушкин Кирилл
 *
 * Наименование модуля: Формирование сигналов целей
 *
 * Назначение: Данный модуль формирует массив сигналов пассивных помех
 * В данном файле описывается функция заполнения массива
 *
 */

#include "ClutterFormationSignals.h"

int FormClutterSignal(struct ClutterFormationParam *p,
		struct ClutterResponseOut *PosIn,
           struct UTimeParam *timeP,
		   struct ClutterSignalsOut *out)
{

    if (PosIn == NULL || timeP == NULL || p == NULL) {
        return 1;
    }
    long long maxCnt = timeP->max_sampling_cnt;
    if (p->enable==1){
		int idx=0;
		for (int i = 0; i < PosIn->cntPP_find; i++) {
			idx = PosIn->PP_map_find[i].discredNum;

			if (idx >= 0 && idx < maxCnt) {
				out->clutter_signals[idx]= PosIn->PP_map_find[i].amplitude;
			}
		}
    }
    return 0;
}


