/*
 * Принимает данные от предыдущего модуля (АД), передаёт дальше (АФП)
 * Модуль: Некогерентное накопление (НН)
 * Выполнил: Филатов Павел Александрович
 * Группа: С25-РЭС
 * Последние правки: 07.07.2026
 */

#ifndef NN_H
#define NN_H

typedef struct {
    int reserved;
} NN_Param;

typedef struct {
    int reserved;
} NN_Input;

typedef struct {
    int reserved;
} NN_Output;

void nn_process(NN_Param *p, NN_Input *in, NN_Output *out);

#endif
