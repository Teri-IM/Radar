/*
 * Автор: Тутин Григорий
 *
 * Наименование модуля: Формирование сигнала НИП
 *
 * Назначение: Данный модуль формирует массив сигналов НИП
 * В данном файле: описывается функция заполнения массива сигналов
 *
 */

#include "NIPFormationSignals.h"

int FormNIPSignal(struct NIPFormationParam *p,
		struct NIPPositionOut *nipPosIn,
           struct UTimeParam *timeP,
		   struct NIPSignalsOut *out)
{

    if (nipPosIn == NULL || timeP == NULL || p == NULL) {
        return 1;
    }

    long long maxCnt = timeP->max_sampling_cnt;

    memset(out->nip_signals, 0, sizeof(out->nip_signals));
    if (p->enable==1){
		int idx=0;
		for (int i = 0; i < nipPosIn->cntNip; i++) {
			idx = nipPosIn->NIP_map[i].discredNum;

			if (idx >= 0 && idx < maxCnt) {
				out->nip_signals[idx]= nipPosIn->NIP_map[i].amplitude;
			}
		}
    }
    return 0;
}

