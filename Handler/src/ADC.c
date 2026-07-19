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
	if(param != 0 && param->ADC.enable == 1 && in != 0 && out != 0){
        if (in->AzimuthData != 0 && out->data.AzimuthData != 0) {
            *out->data.AzimuthData = *in->AzimuthData;
        }
        if (in->TimeData != 0 && out->data.TimeData != 0) {
            *out->data.TimeData = *in->TimeData;
        }
        if (in->SummatorData != 0 && out->data.UAD != 0) {
            *out->data.UAD = *in->SummatorData;
        }
	}
	return 0;
}
