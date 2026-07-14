/*
 * Автор: Стулова Анастасия
 *
 * Наименование модуля: Расчет положения нип
 *
 * Назначение: Создание карты нип а также определения номера дискредитизации
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef NIPPOSITION_H_
#define NIPPOSITION_H_
#include "NIPPositionParam.h"
#include "UnifedTime.h"
#include "ImitatorParam.h"
#include <math.h>


struct NIPPositionOut{
	struct PointWith_discredNum *NIP_map;
	int cntNip;
};

int createNipMap(struct NIPPosParam *p, struct ImitParam *imP);

int NIPPositionMap(struct NIPPosParam *p, struct UTimeParam *timep, struct UnifedTimeOut *in, struct NIPPositionOut *out);


#endif /* NIPPOSITION_H_ */
