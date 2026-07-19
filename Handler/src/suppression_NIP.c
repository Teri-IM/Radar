
/*
   Модуль: Подавление НИП
   Выполнил: Вольфензон Даниил
   Группа: С24-РЭС
   Последние правки: 07.07.2026
*/

#include "../include/suppression_NIP.h"


int suppression_NIP(struct GlobalProcessingParam *param, struct ImitOutData *in, struct data *out)
{ 
    if (param->suppression_NIP.enable == 1)
        {
        *out->data.AzimuthData = *in->AzimuthData;
        *out->data.TimeData = *in->TimeData;
        *out->data.UAD = *in->SummatorData;
        }   
    return 0;
}