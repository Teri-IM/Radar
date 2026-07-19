#include <stdlib.h>
#include <string.h>

/* 1. ПЕРВЫМ делом подключаем базовый хедер, чтобы компилятор узнал 
      все структуры (data, Codogramm, Instruct) ДО подключения остальных модулей! */
#include "../include/processing_module.h"
#include "../include/processing_module_param.h"

/* 2. Оборачиваем Си-модули коллег в extern "C" прямо здесь, 
      так как в их хедерах (nn.h, DDC.h) этой обертки нет. 
      Это раз и навсегда решает проблему линковщика "ld returned 1 exit status". */
#ifdef __cplusplus
extern "C" {
#endif

#include "../include/DDC.h"
#include "../include/suppression_NIP.h"
#include "../include/nn.h"
#include "../include/codogramm.h"

#ifdef __cplusplus
}
#endif

/* Хедер threshold.h уже имеет внутренний extern "C", его можно включать просто так */
#include "../include/threshold.h"

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    /* Выделяем память под промежуточные структуры в куче.
       sizeof(struct data) теперь точно известен компилятору! */
    struct data *ddc_out  = (struct data*)calloc(1, sizeof(struct data));
    struct data *sNIP_out = (struct data*)calloc(1, sizeof(struct data));
    struct data *NN_out   = (struct data*)calloc(1, sizeof(struct data));

    struct Codogramm *threshold_generator_out = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));
    struct Codogramm *threshold_device_out    = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));
    struct Codogramm *Final_Sign              = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));

    /* Проверка на NULL (nullptr в Си не поддерживается) */
    if (ddc_out == NULL || sNIP_out == NULL || NN_out == NULL || 
        threshold_generator_out == NULL || threshold_device_out == NULL || Final_Sign == NULL) {
        free(ddc_out); free(sNIP_out); free(NN_out);
        free(threshold_generator_out); free(threshold_device_out); free(Final_Sign);
        return -1;
    }

    /* Прогоняем данные по цепочке обработки РЛС ваших модулей */
    DDC_Process(Params, Input_Sign, ddc_out);
    suppression_NIP(Params, ddc_out, sNIP_out);
    NN_Process(Params, sNIP_out, NN_out);

    /* Генерация порога */
    threshold_generator(Params, NN_out, threshold_generator_out);

    /* БЕЗОПАСНОСТЬ: Насыщаем массив amplitude (размером 1024) из сумматора имитатора.
       Заполняем только первые 256 элементов, так как порог рассчитан на 256 объектов.
       Благодаря calloc остальные 768 элементов будут гарантированно занулены. */
    if (Input_Sign != NULL && Input_Sign->SummatorData != NULL && Input_Sign->SummatorData->sum_signals != NULL) {
        int idx;
        for (idx = 0; idx < 256; ++idx) {
            NN_out->amplitude[idx] = Input_Sign->SummatorData->sum_signals[idx];
        }
    }

    /* Безопасные вызовы без порчи памяти и разрушения мьютексов Qt */
    threshold_device(NN_out, threshold_generator_out, threshold_device_out);
    former_codogramm(threshold_device_out, Final_Sign);

    /* Формируем финальную кодограмму для передачи в поток отрисовки GUI */
    Output_sign->TimeData = Input_Sign->TimeData;
    Output_sign->AzimuthData = Input_Sign->AzimuthData;
    Output_sign->index = 0;
    Output_sign->number_of_objects = 0;

    if (Input_Sign->TargetPositionData != NULL && Input_Sign->TargetPositionData->Target_map != NULL) {
        int threshold = (Params != NULL) ? Params->threshold.threshold : 0;
        int count = Input_Sign->TargetPositionData->cntTarget;
        int i;

        for (i = 0; i < count && Output_sign->number_of_objects < 256; ++i) {
            struct Point target = Input_Sign->TargetPositionData->Target_map[i];
            if (target.amplitude <= 0.0f) {
                continue;
            }

            Output_sign->sign[Output_sign->number_of_objects].distance = (int)target.distance;
            Output_sign->sign[Output_sign->number_of_objects].AzimuthData = Input_Sign->AzimuthData;
            Output_sign->sign[Output_sign->number_of_objects].amplitude = (int)target.amplitude;
            Output_sign->sign[Output_sign->number_of_objects].threshold = threshold;
            Output_sign->sign[Output_sign->number_of_objects].number = Output_sign->number_of_objects + 1;
            ++Output_sign->number_of_objects;
        }
    }

    /* Освобождаем всю выделенную динамическую память */
    free(ddc_out); free(sNIP_out); free(NN_out);
    free(threshold_generator_out); free(threshold_device_out); free(Final_Sign);

    return 0;
}