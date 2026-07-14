/*
 * Автор: Кромберг Даниил
 *
 * Наименование модуля: Собственный шум
 *
 * Назначение: Генерация естественного шума системы
 * В данном файле: описывается структура параметров модуля
 *
 */
#ifndef NOISEGENERATORPARAM_H_
#define NOISEGENERATORPARAM_H_

struct NoiseParam{
	int enable; //включение модуля
	double mean;// среднее значение шума
	double sigma;// среднеквадратическое отклонение
};


#endif /* NOISEGENERATORPARAM_H_ */
