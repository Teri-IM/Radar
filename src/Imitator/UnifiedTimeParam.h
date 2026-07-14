/*
 * Автор: Асеев Тимур
 *
 * Наименование модуля: Единое время
 *
 * Назначение: Данный модуль сохраняет информацию о времени
 * В данном файле: описывается структура параметров модуля
 *
 */

#ifndef UNIFIEDTIMEPARAM_H_
#define UNIFIEDTIMEPARAM_H_

struct UTimeParam{
	long long pulse_time; //время импульсяа в наносекундах
	long long probing_time; //время зондирования
	long long sampling_rate; //частота дискредитизации
	long long max_sampling_cnt; //максимальное количество элементов дискредитизации
};


#endif /* UNIFIEDTIMEPARAM_H_ */
