/*
 * Автор: Асафьев Мирон
 *
 * Наименование модуля: Генерация карты целей
 *
 * Назначение: создает карту целей
 * В данном файле: описывается структура параметров модуля
 *
 */
#ifndef TARGETPOSITIONPARAM_H_
#define TARGETPOSITIONPARAM_H_
struct TargetPosParam{
	int enable; //включение модуля
	int cntTarget; //Количество целей
	float Targetamplitude; //амплитуда целей помех (пока одинаковая у всех точек)
};


#endif /* TARGETPOSITIONPARAM_H_ */
