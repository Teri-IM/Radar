/*
 * Автор: Кромберг Даниил
 *
 * Наименование модуля: Собственный шум
 *
 * Назначение: Генерация естественного шума системы
 * В данном файле: описывается функция заполнения массива шумма
 *
 */
#include "NoiseGenerator.h"

int generateNoise(struct NoiseParam *p, struct UnifedTimeOut *in, struct NoiseGeneratorOut *out){
	if (p == NULL || in == NULL) {
		return 1;
	}
	if (p->enable==1){
        for (int i = 0; i < in->sampling_cnt; i++)
         {
                out->noise_signals[i] = abs(p->mean + p->sigma * (double)rand() / RAND_MAX);
         }
	}else{
        memset(out->noise_signals, 0, sizeof(out->noise_signals));
    }
	return 0;
}
