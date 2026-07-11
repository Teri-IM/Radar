/*
 * Автор: Стулова Анастасия
 *
 * Наименование модуля: Расчет положения нип
 *
 * Назначение: Создание карты нип а также определения номера дискредитизации
 * В данном файле: описывается структура параметров модуля
 *
 */

#ifndef NIPPOSITIONPARAM_H_
#define NIPPOSITIONPARAM_H_
#include "Points.h"

struct NIPPosParam{
	int enable; //включение модуля
	int cntNIP; //кол-во точек НИП
	float NIPamplitude; //Амплитуда точек нип
	long long MaxNIP_probingTime; //Максимальное значение времени периода зондирования(пока используется для автоматической генерации точек)
	struct NIPPointParam * NIPMap; //Указатель на карту точек(Ту что генерится автоматически)
};

#endif /* NIPPOSITIONPARAM_H_ */
