/*
 ============================================================================
Модуль: АЦП
Выполнил: Швец Иван
Группа: С25-РЭС
Последние правки: 13.07.2026
 ============================================================================
 */

#include "../include/ADC.h"

int adc_convert(struct GlobalProcessingParam *param, struct ImitOutData *in, struct data *out){
	if(param->ADC.enable == 1){
        *out->data.AzimuthData = *in->AzimuthData;
        *out->data.TimeData = *in->TimeData;
        *out->data.UAD = *in->SummatorData;
	}
	return 0;
}
