#include "Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"
#include <stdlib.h> // Для calloc и free
#include <time.h>   // Для clock_gettime
#define tickRate 10
#define ENABLE_PROFILING 0

#if ENABLE_PROFILING
static double get_elapsed_ms(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
}
#endif


int Imitator(struct ImitatorParametrs *parametrs, struct ImitOutData *out) {

    // Безопасность: проверяем, что сам указатель и критически важные подструктуры не NULL
    if (parametrs == NULL || parametrs->uTime == NULL || parametrs->imitator == NULL) {
        return -1;
    }
    #if ENABLE_PROFILING
    // Переменные для накопления времени выполнения каждой функции
    double t_timeTick = 0.0, t_generateNoise = 0.0, t_AzimuthSensor = 0.0;
    double t_NIPPositionMap = 0.0, t_NIPLevelCalculate = 0.0, t_FormNIPSignal = 0.0;
    double t_clutter_resp = 0.0, t_FormClutterSignal = 0.0;
    double t_target_resp = 0.0, t_FormTargetSignal = 0.0;
    double t_Summator = 0.0, t_FreqConverter = 0.0, t_createOutData = 0.0;

    struct timespec s_blk, e_block;
    #endif

    // Проверка внешних выходных буферов
    if (out == NULL || out->TimeData == NULL || out->AzimuthData == NULL || out->SummatorData == NULL || out->SummatorData->sum_signals == NULL) {
        return -1;
    }

    struct UnifedTimeOut *unifed_time_out = out->TimeData;
    struct NoiseGeneratorOut noise_generator_out;
    struct AzimutSensorOut *azimuth_sensor_out = out->AzimuthData;
    struct ImitSummatorOut *summator_out = out->SummatorData;

    struct PPPositionOut pp_pos_out;
    struct ClutterResponseOut pp_find_out;
    struct ClutterSignalsOut pp_signals_out;

    struct NIPPositionOut nip_pos_out;
    struct NIPSignalsOut nip_formation_signals_out;

    struct TargetPositionOut target_pos_out;
    struct TargetResponseOut target_find_out;
    struct TargetSignalsOut target_signals_out;

    // Динамическая зависимость параметров (установка времени зондирования для NIP)
    if (parametrs->nipPosition != NULL) {
        parametrs->nipPosition->MaxNIP_probingTime = parametrs->uTime->probing_time;
    }

    // Прединициализация выходных данных
    clock_gettime(CLOCK_MONOTONIC, &unifed_time_out->start_time);
    unifed_time_out->sampling_cnt = (parametrs->uTime->probing_time - parametrs->uTime->pulse_time) * parametrs->uTime->sampling_rate;

    if (parametrs->azimuth != NULL) {
        azimuth_sensor_out->angularVelocity = parametrs->azimuth->angularVelocity;
        azimuth_sensor_out->azimuth_new = parametrs->azimuth->startAngle;
    }

    // Выделение памяти под сигнальные массивы
    noise_generator_out.noise_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    pp_signals_out.clutter_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    nip_formation_signals_out.nip_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    target_signals_out.target_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));

    // Выделение памяти под карты объектов и результатов
    if (parametrs->nipPosition != NULL) {
        parametrs->nipPosition->NIPMap = (struct NIPPointParam *)calloc(parametrs->nipPosition->cntNIP, sizeof(struct NIPPointParam));
        nip_pos_out.NIP_map = (struct PointWith_discredNum *)calloc(parametrs->nipPosition->cntNIP, sizeof(struct PointWith_discredNum));
    }

    if (parametrs->ppPosition != NULL) {
        pp_pos_out.PP_map = (struct Point *)calloc(parametrs->ppPosition->cntPP, sizeof(struct Point));
        pp_find_out.PP_map_find = (struct PointWith_discredNum *)calloc(parametrs->ppPosition->cntPP, sizeof(struct PointWith_discredNum));
    }

    if (parametrs->targetPosition != NULL) {
        target_pos_out.Target_map = (struct Point *)calloc(parametrs->targetPosition->cntTarget, sizeof(struct Point));
        target_find_out.target_map_find = (struct PointWith_discredNum *)calloc(parametrs->targetPosition->cntTarget, sizeof(struct PointWith_discredNum));
    }

    // Первоначальное построение карт положений
    #if ENABLE_PROFILING
    clock_gettime(CLOCK_MONOTONIC, &s_blk);
    #endif
    PPPositionMap(parametrs->ppPosition, parametrs->imitator, &pp_pos_out);
    #if ENABLE_PROFILING
    clock_gettime(CLOCK_MONOTONIC, &e_block);
    printf("Time PPPositionMap:   %.5f ms\n",get_elapsed_ms(s_blk, e_block) );
    clock_gettime(CLOCK_MONOTONIC, &s_blk);
    #endif
    createNipMap(parametrs->nipPosition, parametrs->imitator);
    #if ENABLE_PROFILING
    clock_gettime(CLOCK_MONOTONIC, &e_block);
    printf("Time createNipMap:   %.5f ms\n", get_elapsed_ms(s_blk, e_block));
    clock_gettime(CLOCK_MONOTONIC, &s_blk);
    #endif
    TargetPositionMap(parametrs->targetPosition, parametrs->imitator, &target_pos_out);
    #if ENABLE_PROFILING
    clock_gettime(CLOCK_MONOTONIC, &e_block);
    printf("Time TargetPositionMap: %.5f ms\n", get_elapsed_ms(s_blk, e_block));
    #endif
    // Цикл работы имитатора
    for (int i = 0; i < tickRate; i++) {


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        timeTick(parametrs->uTime, unifed_time_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_timeTick += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        generateNoise(parametrs->noise, unifed_time_out, &noise_generator_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_generateNoise += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        AzimuthSensor(parametrs->azimuth, parametrs->uTime, unifed_time_out, azimuth_sensor_out, tickRate);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_AzimuthSensor += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        NIPPositionMap(parametrs->nipPosition, parametrs->uTime, unifed_time_out, &nip_pos_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_NIPPositionMap += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        NIPLevelCalculate(parametrs->nipLevel, parametrs->uTime, azimuth_sensor_out, &nip_pos_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_NIPLevelCalculate += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        FormNIPSignal(parametrs->nipFormation, &nip_pos_out, parametrs->uTime, &nip_formation_signals_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_FormNIPSignal += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        clutter_response_calc(parametrs->clutterResponse, parametrs->uTime, unifed_time_out, azimuth_sensor_out, &pp_pos_out, &pp_find_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_clutter_resp += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        FormClutterSignal(parametrs->clutterFormation, &pp_find_out, parametrs->uTime, &pp_signals_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_FormClutterSignal += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        target_response_calc(parametrs->targetResponse, parametrs->uTime, unifed_time_out, azimuth_sensor_out, &target_pos_out, &target_find_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_target_resp += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        FormTargetSignal(parametrs->targetFormation, &target_find_out, parametrs->uTime, &target_signals_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_FormTargetSignal += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        Summator(parametrs->summator, unifed_time_out, parametrs->uTime, &target_signals_out, &pp_signals_out, &nip_formation_signals_out, &noise_generator_out, summator_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_Summator += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        FrequencyConverter(parametrs->frequencyConverter, unifed_time_out, summator_out);
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_FreqConverter += get_elapsed_ms(s_blk, e_block);
        #endif


        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &s_blk);
        #endif
        #if ENABLE_PROFILING
        clock_gettime(CLOCK_MONOTONIC, &e_block); t_createOutData += get_elapsed_ms(s_blk, e_block);
        #endif

    }

    // Результаты уже записаны напрямую в буферы вызывающего кода.

    #if ENABLE_PROFILING
    // Вывод собранной статистики в консоль Qt
    printf("\n=================== [RLS PROFILER] ===================\n");
    printf("timeTick:              %.4f ms\n", t_timeTick);
    printf("generateNoise:         %.4f ms\n", t_generateNoise);
    printf("AzimuthSensor:         %.4f ms\n", t_AzimuthSensor);
    printf("NIPPositionMap:        %.4f ms\n", t_NIPPositionMap);
    printf("NIPLevelCalculate:     %.4f ms\n", t_NIPLevelCalculate);
    printf("FormNIPSignal:         %.4f ms\n", t_FormNIPSignal);
    printf("clutter_response_calc: %.4f ms\n", t_clutter_resp);
    printf("FormClutterSignal:     %.4f ms\n", t_FormClutterSignal);
    printf("target_response_calc:  %.4f ms\n", t_target_resp);
    printf("FormTargetSignal:      %.4f ms\n", t_FormTargetSignal);
    printf("Summator:              %.4f ms\n", t_Summator);
    printf("FrequencyConverter:    %.4f ms\n", t_FreqConverter);
    printf("createOutData:         %.4f ms\n", t_createOutData);
    printf("-------------------------------------------------------\n");
    printf("Final time:            %.2f ms\n", t_timeTick+t_generateNoise+
        t_AzimuthSensor+t_NIPPositionMap+t_NIPLevelCalculate+t_FormNIPSignal+
        t_clutter_resp+t_FormClutterSignal+t_target_resp+t_FormTargetSignal+
        t_Summator+t_FreqConverter+t_createOutData);
    printf("=======================================================\n");
    fflush(stdout);
    #endif

    // Освобождение памяти
    free(noise_generator_out.noise_signals);
    free(pp_signals_out.clutter_signals);
    free(nip_formation_signals_out.nip_signals);
    free(target_signals_out.target_signals);

    if (parametrs->nipPosition != NULL) {
        free(parametrs->nipPosition->NIPMap);
    }
    free(nip_pos_out.NIP_map);

    free(pp_pos_out.PP_map);
    free(pp_find_out.PP_map_find);

    free(target_pos_out.Target_map);
    free(target_find_out.target_map_find);

    return 0;
}

int createImitatatorOutData(struct ImitSummatorOut *summator_data, struct UnifedTimeOut *time_data, struct AzimutSensorOut *azimuth_data, struct ImitOutData *out){
    if (summator_data == NULL || time_data == NULL || azimuth_data == NULL || out == NULL){
        return 0;
    }
    out->SummatorData = summator_data;
    out->AzimuthData = azimuth_data;
    out->TimeData = time_data;
    return 0;
}

