/*
 * Автор: Назарычев Артём
 *
 * Наименование модуля: Сумматор
 *
 * Назначение: Модуль сумирует массивы сигналов в один единственный
 * В данном файле: описывается функция сложения
 *
 */
#include "ImitSummator.h"

int Summator(
    struct SummatorParam *p,
    struct UnifedTimeOut *time,
    struct UTimeParam *timeP,
    struct TargetSignalsOut *tagret,
    struct ClutterSignalsOut *pp,
    struct NIPSignalsOut *nip,
    struct NoiseGeneratorOut *noise,
    struct ImitSummatorOut *out)
{
    // Быстрая проверка указателей
    if (p == NULL || timeP == NULL || tagret == NULL || pp == NULL || noise == NULL || nip == NULL || time == NULL) {
        return 1;
    }


    if (p->enable == 1) {
        // Кэшируем размер цикла в локальную переменную (чтобы компилятор не перечитывал структуру)
        const int limit = time->sampling_cnt;

        // Ключевое слово restrict сообщает компилятору, что массивы не пересекаются в памяти.
        // Это развязывает ему руки для жесткой SIMD (AVX/SSE) оптимизации.
        const float * restrict src_target = tagret->target_signals;
        const float * restrict src_clutter = pp->clutter_signals;
        const float * restrict src_noise   = noise->noise_signals;
        const float * restrict src_nip     = nip->nip_signals;
        float * restrict dst_sum           = out->sum_signals;
        for (int i = 0; i < limit; i++) {
            dst_sum[i] = src_target[i] + src_clutter[i] + src_noise[i] + src_nip[i];
        }
    }

    return 0;
}


