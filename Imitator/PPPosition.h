/*
 * Автор: Мокров Иван
 *
 * Наименование модуля: Обновление карты ПП
 *
 * Назначение: создает карту пассивных помех
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef PPPOSITION_H_
#define PPPOSITION_H_
#include "PPPositionParam.h"
#include "UnifedTime.h"
#include "ImitatorParam.h"

struct PPPositionOut{
	struct Point *PP_map;
	int cntPP;
};
int PPPositionMap(struct PPPosParam *p, struct ImitParam *imP, struct PPPositionOut *out);

#endif /* PPPOSITION_H_ */
