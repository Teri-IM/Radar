#include <stdlib.h>
#include <string.h>

/* Базовые структуры РЛС */
#include "../include/processing_module.h"
#include "../include/processing_module_param.h"

/* Добавляем extern "C" прямо в код .c, так как в хедерах одногруппников его нет */
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

/* Модуль порогового устройства */
#include "Handler/include/threshold.h"

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    /* Выделение структур на стеке (оригинальная архитектура ветки) */
    struct data ddc_out;
    struct data sNIP_out;
    struct data NN_out;
    struct Codogramm threshold_generator_out;
    struct Codogramm threshold_device_out;
    struct Codogramm Final_Sign;

    /* ЗАЩИТА ПАМЯТИ: Стираем весь мусор со стека внутри структур.
       Теперь пороговое устройство не будет читать случайные адреса. */
    memset(&ddc_out, 0, sizeof(struct data));
    memset(&sNIP_out, 0, sizeof(struct data));
    memset(&NN_out, 0, sizeof(struct data));
    memset(&threshold_generator_out, 0, sizeof(struct Codogramm));
    memset(&threshold_device_out, 0, sizeof(struct Codogramm));
    memset(&Final_Sign, 0, sizeof(struct Codogramm));

    /* Прогоняем данные по цепочке обработки РЛС */
    DDC_Process(Params, Input_Sign, &ddc_out);
    suppression_NIP(Params, &ddc_out, &sNIP_out);
    NN_Process(Params, &sNIP_out, &NN_out);

    /* Генерация порога */
    threshold_generator(Params, &NN_out, &threshold_generator_out);

    /* Насыщаем массив амплитуд реальными отсчетами из сумматора имитатора */
    if (Input_Sign != NULL && Input_Sign->SummatorData != NULL && Input_Sign->SummatorData->sum_signals != NULL) {
        int idx;
        for (idx = 0; idx < 256; ++idx) {
            NN_out.amplitude[idx] = Input_Sign->SummatorData->sum_signals[idx];
        }
    }

    /* Пороговая обработка и формирование финальной кодограммы */
    threshold_device(&NN_out, &threshold_generator_out, &threshold_device_out);
    former_codogramm(&threshold_device_out, &Final_Sign);

    /* Наполнение выходного буфера для передачи в GUI-поток */
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

    return 0;
}
