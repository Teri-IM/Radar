/*******************************************************************************
    Name        : threshold_generator; threshold_device
    Group       : S25-RES
    Author      : Wolfensohn G.G.
    Last change : 13.07.2026
*******************************************************************************/
#ifndef THRESHOLD_H
#define THRESHOLD_H

#include <stdio.h>
#include "processing_module.h"

#define CHUNK_SIZE  64

/* Структура, описывающая входной сигнал (упрощенная для данного модуля)
typedef struct input_signal_s {
    int azimuth;
    int worktime;
    int index;
    int sample_size;
    sample_t sample[CHUNK_SIZE];
} input_signal_t;

/* Структура, описывающая пороговый сигнал
 * (будет заполнена пороговыми значениями)
typedef struct threshold_signal_s {
    /***************
    int azimuth;
    int worktime;
    int index;
    int sample_size;
    **************
    sample_t sample[CHUNK_SIZE];
} threshold_signal_t;

/* Структура, описывающая обработанный сигнал (упрощенная для данного модуля)
typedef struct output_signal_s {
    int azimuth;
    sample_t sample;
} output_signal_t;

/* Конструкция, позволяющая запускать c-программы на компиляторе c++
 * (те конструкции, которые поддерживаются только на c нельзя просто так
 * запустить на c++) */
#ifdef __cplusplus
  extern "C" {
#endif

int threshold_generator (struct GlobalProcessingParam *par, struct data *in, struct Codogramm *thr);
int threshold_device (struct Codogramm *in, struct Codogramm *thr);


#ifdef __cplusplus
  }
#endif

#endif