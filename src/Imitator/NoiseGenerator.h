/*
 * Автор: Кромберг Даниил
 *
 * Наименование модуля: Собственный шум
 *
 * Назначение: Генерация естественного шума системы
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef NOISEGENERATOR_H_
#define NOISEGENERATOR_H_
#include "UnifedTime.h"
#include "NoiseGeneratorParam.h"
#include <math.h>
#include <string.h>

struct NoiseGeneratorOut{
    float *noise_signals;
};

int generateNoise(struct NoiseParam *p, struct UnifedTimeOut *in, struct NoiseGeneratorOut *out);


#endif /* NOISEGENERATOR_H_ */
