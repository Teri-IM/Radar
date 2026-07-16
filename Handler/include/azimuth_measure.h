/*
   Модуль: Измерение азимута
   Выполнил: Лукашев Антон
   Группа: С25-РЭС
   Последние правки: 16.07.2026
*/

#ifndef AZIMUTH_MEASURE_H
#define AZIMUTH_MEASURE_H

#include "processing_module_param.h"
#include "processing_module.h"

int AzimuthMeasure(struct Codogramm *input_codogramm, struct data *input_data, struct Codogramm *output_codogramm);

#endif /* AZIMUTH_MEASURE_H */