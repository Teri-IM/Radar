/*
 * Автор: Мокров Иван
 *
 * Наименование модуля: Обновление карты ПП
 *
 * Назначение: создает карту пассивных помех
 * В данном файле: описывается структура параметров модуля
 *
 */
#ifndef PPPOSITIONPARAM_H_
#define PPPOSITIONPARAM_H_

struct PPPosParam{
	int enable; //включение модуля
	int cntPP; //количество пассивных помех
	float PPamplitude; //амплитуда пассивных помех (пока одинаковая у всех точек)
};

#endif /* PPPOSITIONPARAM_H_ */
