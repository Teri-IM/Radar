/*
 * Автор: Асафьев Мирон
 *
 * Наименование модуля: Генерация карты целей
 *
 * Назначение: создает карту целей
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef TARGETPOSITION_H_
#define TARGETPOSITION_H_
#include "TargetPositionParam.h"
#include "UnifedTime.h"
#include "ImitatorParam.h"

struct TargetPositionOut{
	struct Point *Target_map;
	int cntTarget;
};
int TargetPositionMap(struct TargetPosParam *p, struct ImitParam *imP, struct TargetPositionOut *out);



#endif /* TARGETPOSITION_H_ */
