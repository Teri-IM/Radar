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
		double sum = 0.0;
		for (int i = 0; i < in->sampling_cnt; i++)
		 {
				sum = 0.0;
				for (int i = 0; i < 12; i++) {
					sum += (double)rand() / RAND_MAX;  // равномерное [0,1]
				}
				sum -= 6.0;
				out->noise_signals[i] = abs(p->mean + p->sigma * sum);
		 }
	}else{
		for (int i = 0; i < in->sampling_cnt; i++)
				 {

						out->noise_signals[i] = 0;
				 }
	}
	return 0;
}
