#include <stdlib.h> /* Обязательно для calloc и free */

/* Исправляем пути к заголовочным файлам РЛС.
   Убедитесь, что эти файлы лежат именно по этим путям относительно файла .c */
#include "../include/processing_module.h"
#include "../include/processing_module_param.h"
#include "../include/DDC.h"
#include "../include/codogramm.h"
#include "../include/suppression_NIP.h"
#include "../include/nn.h"
#include "../include/threshold.h" /* Исправлен возможный неверный путь */

/* Если компилятор всё равно ругается на "Incomplete type", это значит, что в ваших
   заголовочных файлах выше нет структуры "data". В таком случае её нужно объявить.
   Раскомментируйте строчку ниже, если ошибка "storage size of ddc_out isn't known" останется: */
/* struct data { float *signals; int size; }; */

int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    struct data ddc_out;
    struct data sNIP_out;
    struct data NN_out;

    /* В Си вместо auto и static_cast используется явное указание типов и Си-приведение */
    struct Codogramm *threshold_generator_out = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));
    struct Codogramm *threshold_device_out    = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));
    struct Codogramm *Final_Sign              = (struct Codogramm*)calloc(1, sizeof(struct Codogramm));

    /* В Си нет слова nullptr, используется макрос NULL */
    if (threshold_generator_out == NULL || threshold_device_out == NULL || Final_Sign == NULL) {
        free(threshold_generator_out);
        free(threshold_device_out);
        free(Final_Sign);
        return -1;
    }

    DDC_Process(Params, Input_Sign, &ddc_out);
    suppression_NIP(Params, &ddc_out, &sNIP_out);
    NN_Process(Params, &sNIP_out, &NN_out);

    threshold_generator(Params, &NN_out, threshold_generator_out);
    threshold_device(&NN_out, threshold_generator_out, threshold_device_out);
    former_codogramm(threshold_device_out, Final_Sign);

    /* Заполнение выходных данных */
    Output_sign->TimeData = Input_Sign->TimeData;
    Output_sign->AzimuthData = Input_Sign->AzimuthData;
    Output_sign->index = 0;
    Output_sign->number_of_objects = 0;

    if (Input_Sign != NULL && Input_Sign->TargetPositionData != NULL && Input_Sign->TargetPositionData->Target_map != NULL) {
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

    /* Освобождаем динамическую память */
    free(threshold_generator_out);
    free(threshold_device_out);
    free(Final_Sign);

    return 0;
}