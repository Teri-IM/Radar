
/*
   Модуль: Подавление НИП
   Выполнил: Вольфензон Даниил
   Группа: С24-РЭС
   Последние правки: 07.07.2026
*/

#include "../include/suppression_NIP.h"


int suppression_NIP(struct GlobalProcessingParam *param, struct data *in, struct data *out)
{ 
    if (param->suppression_NIP.enable == 1)
        {
        *out = *in;
        }   
    return 0;
}