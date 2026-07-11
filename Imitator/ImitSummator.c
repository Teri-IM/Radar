/*
 * Автор: Назарычев Артём
 *
 * Наименование модуля: Сумматор
 *
 * Назначение: Модуль сумирует массивы сигналов в один единственный
 * В данном файле: описывается функция сложения
 *
 */
#include "ImitSummator.h"
int Summator(struct SummatorParam *p, struct UnifedTimeOut *time, struct UTimeParam *timeP, struct TargetSignalsOut *tagret,  struct ClutterSignalsOut *pp, struct NIPSignalsOut *nip, struct NoiseGeneratorOut *noise, struct ImitSummatorOut *out){
	if (p == NULL || timeP == NULL || tagret == NULL || pp == NULL || noise == NULL || nip == NULL || time == NULL) {
		return 1;
	}


	long long maxCnt = timeP->max_sampling_cnt;

    memset(out->sum_signals, 0, sizeof(out->sum_signals));

	if (p->enable==1){
		for (int i = 0; i < time->sampling_cnt; i++) {
			   out->sum_signals[i]=tagret->target_signals[i]+pp->clutter_signals[i]+noise->noise_signals[i]+nip->nip_signals[i];
		}
	}
	return 0;
}



