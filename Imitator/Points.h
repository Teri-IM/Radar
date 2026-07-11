/*
 * Автор: Назарычев Артём
 *
 * Доп модуль описывающие точки которые существуют в имитаторе
 */

#ifndef POINTS_H_
#define POINTS_H_

struct Point{ //стуктура точки
	float distance;
	float angle;
	float amplitude;
};

struct PointWith_discredNum{ //структура точки с значением ячейки дискредитизации
	float distance;
	float angle;
	float amplitude;
	int discredNum;
};

struct NIPPointParam{ //стуктура точки НИП для автоматической генерации
	float distance;
	float angle;
	long long NIPprobing_time;
};

#endif /* POINTS_H_ */
