/*
 * Автор: Васин Артём
 *
 * Наименование модуля: Частотное преобразование
 *
* В данном файле: осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef FREQUENCYCONVERTER_H_
#define FREQUENCYCONVERTER_H_
#include "FrequencyConverterParam.h"
#include "ImitSummator.h"

int FrequencyConverter(struct FreqConvertorParam *p, struct UnifedTimeOut *timeIn, struct ImitSummatorOut *inOut);

#endif /* FREQUENCYCONVERTER_H_ */
