/*
 * Автор: Конхон Петр
 *
 * Наименование модуля: Формирование сигналов целей
 *
 * Назначение: Данный модуль формирует массив сигналов целей
 * В данном файле: описывается функция функция заполнения массива
 *
 */
#include "TargetFormationSignals.h"

int FormTargetSignal(struct TargetFormationParam *p,
		struct TargetResponseOut *PosIn,
           struct UTimeParam *timeP,
		   struct TargetSignalsOut *out)
{

    if (PosIn == NULL || timeP == NULL || p == NULL) {
        return 1;
    }

    long long maxCnt = timeP->max_sampling_cnt;

    for (int i = 0; i < timeP->max_sampling_cnt; i++) {

    	out->target_signals[i]=0;
       }
    if (p->enable==1){
		int idx=0;
		for (int i = 0; i < PosIn->cntTarget_find; i++) {
			idx = PosIn->target_map_find[i].discredNum;

			if (idx >= 0 && idx < maxCnt) {
				out->target_signals[idx]= PosIn->target_map_find[i].amplitude;
			}
		}
    }
    return 0;
}

