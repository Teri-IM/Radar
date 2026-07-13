/*
 ============================================================================
Модуль: АЦП
Выполнил: Швец Иван
Группа: С25-РЭС
Последние правки: 13.07.2026
 ============================================================================
 */

#ifndef ADC_H_
#define ADC_H_

#include "processing_module.h"

int adc_convert(struct GlobalProcessingParam *param, struct data *in, struct data *out);

#endif /* ADC_H_ */
