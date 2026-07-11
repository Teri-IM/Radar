#include "Imitator.h"
#include "Imitator/UnifiedImitatorParam.h"
#include <stdlib.h> // Для calloc и free
#include <time.h>   // Для clock_gettime
#define tickRate 10

int Imitator(struct ImitatorParametrs *parametrs, struct ImitOutData *out) {
    clock_t start, end;
    double cpu_time_used;
    // Безопасность: проверяем, что сам указатель и критически важные подструктуры не NULL
    if (parametrs == NULL || parametrs->uTime == NULL || parametrs->imitator == NULL) {
        return -1;
    }

    // Инициализация выходных структур (они остаются локальными на стеке)
    struct UnifedTimeOut unifed_time_out;
    struct NoiseGeneratorOut noise_generator_out;
    struct AzimutSensorOut azimuth_sensor_out;

    struct PPPositionOut pp_pos_out;
    struct ClutterResponseOut pp_find_out;
    struct ClutterSignalsOut pp_signals_out;

    struct NIPPositionOut nip_pos_out;
    struct NIPSignalsOut nip_formation_signals_out;

    struct TargetPositionOut target_pos_out;
    struct TargetResponseOut target_find_out;
    struct TargetSignalsOut target_signals_out;

    struct ImitSummatorOut summator_out;

    // Динамическая зависимость параметров (установка времени зондирования для NIP)
    if (parametrs->nipPosition != NULL) {
        parametrs->nipPosition->MaxNIP_probingTime = parametrs->uTime->probing_time;
    }

    // Прединициализация выходных данных
    clock_gettime(CLOCK_MONOTONIC, &unifed_time_out.start_time);
    unifed_time_out.sampling_cnt = (parametrs->uTime->probing_time - parametrs->uTime->pulse_time) * parametrs->uTime->sampling_rate;

    if (parametrs->azimuth != NULL) {
        azimuth_sensor_out.angularVelocity = parametrs->azimuth->angularVelocity;
        azimuth_sensor_out.azimuth_new = parametrs->azimuth->startAngle;
    }

    // Выделение памяти под сигнальные массивы
    noise_generator_out.noise_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    pp_signals_out.clutter_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    nip_formation_signals_out.nip_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    target_signals_out.target_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));
    summator_out.sum_signals = (float *)calloc(parametrs->uTime->max_sampling_cnt, sizeof(float));

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
    start = clock();
    PPPositionMap(parametrs->ppPosition, parametrs->imitator, &pp_pos_out);
    end = clock();
            cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Time PPPositionMap: %f секунд\n", cpu_time_used);
    start = clock();
    createNipMap(parametrs->nipPosition, parametrs->imitator);
    end = clock();
            cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Time createNipMap: %f секунд\n", cpu_time_used);
    start = clock();
    TargetPositionMap(parametrs->targetPosition, parametrs->imitator, &target_pos_out);
    end = clock();
            cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
            printf("Time TargetPositionMap: %f секунд\n", cpu_time_used);

    // Цикл работы имитатора
    for (int i = 0; i < tickRate; i++) {


        timeTick(parametrs->uTime, &unifed_time_out);


        generateNoise(parametrs->noise, &unifed_time_out, &noise_generator_out);


        AzimuthSensor(parametrs->azimuth, parametrs->uTime, &unifed_time_out, &azimuth_sensor_out, tickRate);


        NIPPositionMap(parametrs->nipPosition, parametrs->uTime, &unifed_time_out, &nip_pos_out);


        NIPLevelCalculate(parametrs->nipLevel, parametrs->uTime, &azimuth_sensor_out, &nip_pos_out);


        FormNIPSignal(parametrs->nipFormation, &nip_pos_out, parametrs->uTime, &nip_formation_signals_out);



        clutter_response_calc(parametrs->clutterResponse, parametrs->uTime, &unifed_time_out, &azimuth_sensor_out, &pp_pos_out, &pp_find_out);



        FormClutterSignal(parametrs->clutterFormation, &pp_find_out, parametrs->uTime, &pp_signals_out);



        target_response_calc(parametrs->targetResponse, parametrs->uTime, &unifed_time_out, &azimuth_sensor_out, &target_pos_out, &target_find_out);



        FormTargetSignal(parametrs->targetFormation, &target_find_out, parametrs->uTime, &target_signals_out);



        Summator(parametrs->summator, &unifed_time_out, parametrs->uTime, &target_signals_out, &pp_signals_out, &nip_formation_signals_out, &noise_generator_out, &summator_out);



        FrequencyConverter(parametrs->frequencyConverter, &unifed_time_out, &summator_out);


        createImitatatorOutData(&summator_out, &unifed_time_out, &azimuth_sensor_out, out);

    }

    // Освобождение памяти
    free(noise_generator_out.noise_signals);
    free(pp_signals_out.clutter_signals);
    free(nip_formation_signals_out.nip_signals);
    free(target_signals_out.target_signals);
    free(summator_out.sum_signals);

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
