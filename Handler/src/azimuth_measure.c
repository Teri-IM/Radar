/*
   Модуль: Измерение азимута
   Выполнил: Лукашев Антон
   Группа: С25-РЭС
   Последние правки: 16.07.2026
*/

#include "../include/azimuth_measure.h"
#include <string.h>
#include <stdlib.h>

int AzimuthMeasure(struct Codogramm *input_codogramm, struct data *input_data, struct Codogramm *output_codogramm)
{
    if (input_codogramm == NULL || input_data == NULL || output_codogramm == NULL)
    {
        return -1;
    }
    
    /* Копируем входную кодограмму в выходную */
    memcpy(output_codogramm, input_codogramm, sizeof(struct Codogramm));
    
    /* Из data вытаскиваем азимут и записываем в Codogramm */
    for (int i = 0; i < 256; i++)
    {
        output_codogramm->sign[i].AzimuthData->azimuth_new = input_data->data.AzimuthData->azimuth_new;
    }
    
    return 0;
}