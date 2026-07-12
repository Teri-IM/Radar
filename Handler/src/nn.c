/*
   Модуль: Некогерентное накопление
   Выполнил: Филатов Павел Александрович
   Группа: С25-РЭС
   Последние правки: 12.07.2026
*/
#include "include/nn.h"
int NN_Process(struct GlobalProcessingParam *param, struct data *in, struct data *out)
{
    if (param->NN.enable == 1)
        {
            *out = *in;
        }
    return 0;
}
