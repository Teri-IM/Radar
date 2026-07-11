/*
   Модуль: Подавление НИП
   Выполнил: Вольфензон Даниил
   Группа: С24-РЭС
   Последние правки: 07.07.2026
*/


#ifndef NIP_H
#define NIP_H

#include "processing_module.h"
#include "suppression_NIP_param.h"


int suppression_NIP(struct suppression_NIP_Param *param, struct data *in, struct data *out);

#endif