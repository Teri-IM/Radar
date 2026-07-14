/*
 * Автор: Асеев Тимур
 *
 * Наименование модуля: Единое время
 *
 * Назначение: Данный модуль сохраняет информацию о времени
 * В данном файле: описывается структура выходных данных, осуществляется подключение дополнительных модулей, создается прототип функии
 *
 */

#ifndef UNIFEDTIME_H_
#define UNIFEDTIME_H_
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "UnifiedTimeParam.h"
#include "Points.h"

struct UnifedTimeOut{
	struct timespec start_time;
    uint64_t days;          // количество полных дней
    uint8_t  hours;         // часы (0–23)
    uint8_t  minutes;       // минуты (0–59)
    uint8_t  seconds;       // секунды (0–59)
    uint32_t nanoseconds;   // наносекунды (0–999999999)
    uint64_t total_ns;      // общее количество наносекунд (для удобства)
    long sampling_cnt; //число дискретизации
};


int timeTick(struct UTimeParam *p, struct UnifedTimeOut *out);

#endif /* UNIFEDTIME_H_ */
