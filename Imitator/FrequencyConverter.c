/*
 * Автор: Васин Артём
 *
 * Наименование модуля: Частотное преобразование
 *
 * В данном файле: описывается функиция увиличения амплитуды
 *
 */
#include "FrequencyConverter.h"
int FrequencyConverter(struct FreqConvertorParam *p, struct UnifedTimeOut *timeIn, struct ImitSummatorOut *inOut){

	if (p->enable==1){
		if (p==NULL || timeIn == NULL || inOut==NULL){
			return 1;
		}

		for (int i =0; i<timeIn->sampling_cnt; i++){
			inOut->sum_signals[i]=inOut->sum_signals[i]*2;
		}


	}
	return 0;
}


