/*
 * Автор: Асеев Тимур
 *
 * Наименование модуля: Единое время
 *
 * Назначение: Данный модуль сохраняет информацию о времени
 * В данном файле: описывается функция записи времени работы программы
 *
 */
#include "UnifedTime.h"



int timeTick(struct UTimeParam *p, struct UnifedTimeOut *out){
	if (p == NULL) {
		return 1;
	}
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	uint64_t ns = (uint64_t)(now.tv_sec - out->start_time.tv_sec) * 1000000000ULL +(now.tv_nsec - out->start_time.tv_nsec);
	out->total_ns = ns;

	out->nanoseconds = ns % 1000000000ULL;
	uint64_t sec = ns / 1000000000ULL;

	out->seconds = sec % 60;
	uint64_t min = sec / 60;

	out->minutes = min % 60;
	uint64_t hours = min / 60;

	out->hours = hours % 24;
	out->days = hours / 24;
	return 0;
}
