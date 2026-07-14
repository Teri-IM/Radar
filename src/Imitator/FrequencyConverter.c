/*
 * Автор: Васин Артём
 *
 * Наименование модуля: Частотное преобразование
 *
 * В данном файле: описывается функиция увиличения амплитуды
 *
 */
#include "FrequencyConverter.h"
#include <stddef.h> // Для NULL

int FrequencyConverter(struct FreqConvertorParam *p, struct UnifedTimeOut *timeIn, struct ImitSummatorOut *inOut) {
    // 1. ИСПРАВЛЕНО: Безопасность превыше всего. Проверяем указатели ДО их разыменования!
    if (p == NULL || timeIn == NULL || inOut == NULL) {
        return 1;
    }

    // 2. Если модуль выключен, просто выходим без тормозов
    if (p->enable != 1) {
        return 0;
    }

    long long count = timeIn->sampling_cnt;

    // 3. Локализуем массив во внутренний restrict-указатель для автовекторизации
    float * restrict signals = inOut->sum_signals;

    // 4. Оптимизированный цикл (GCC превратит его в эффективные векторные инструкции)
    for (long long i = 0; i < count; i++) {
        signals[i] *= 2.0f;
    }

    return 0;
}


