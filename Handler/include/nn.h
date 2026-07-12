/*
   Модуль: Некогерентное накопление
   Выполнил: Филатов Павел Александрович
   Группа: С25-РЭС
   Последние правки: 12.07.2026
*/

#ifndef NN_H
#define NN_H

#include "processing_module.h"

int NN_Process(struct GlobalProcessingParam *param, struct data *in, struct data *out);

#endif
