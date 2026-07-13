/*
 ============================================================================
Модуль: АЦП
Выполнил: Швец Иван
Группа: С25-РЭС
Последние правки: 13.07.2026
 ============================================================================
 */

#include "../include/ADC.h"

int adc_convert(struct GlobalProcessingParam *param, struct data *in, struct data *out){
	if(param->ADC.enable == 1){
		*out=*in;
		return out;
	}
	return 0;
}
