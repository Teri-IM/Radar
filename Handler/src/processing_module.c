int ProcessingModule(struct GlobalProcessingParam *Params, struct ImitOutData *Input_Sign, struct Codogramm *Output_sign)
{
    // Вместо создания на стеке, выделяем память в куче, чтобы не было Stack Overflow
    struct data ddc_out;
    struct data sNIP_out;
    struct data NN_out;
    
    // Переносим тяжелые структуры с дефицитного стека потока в кучу (Heap)
    auto *threshold_generator_out = static_cast<struct Codogramm*>(calloc(1, sizeof(struct Codogramm)));
    auto *threshold_device_out = static_cast<struct Codogramm*>(calloc(1, sizeof(struct Codogramm)));
    auto *Final_Sign = static_cast<struct Codogramm*>(calloc(1, sizeof(struct Codogramm)));

    if (!threshold_generator_out || !threshold_device_out || !Final_Sign) {
        free(threshold_generator_out);
        free(threshold_device_out);
        free(Final_Sign);
        return -1;
    }

    DDC_Process(Params, Input_Sign, &ddc_out);
    suppression_NIP(Params, &ddc_out, &sNIP_out);
    NN_Process(Params, &sNIP_out, &NN_out);

    threshold_generator(Params, &NN_out, threshold_generator_out);
    threshold_device(&NN_out, threshold_generator_out, threshold_device_out);
    former_codogramm(threshold_device_out, Final_Sign);

    // Заполнение выходных данных
    Output_sign->TimeData = Input_Sign->TimeData;
    Output_sign->AzimuthData = Input_Sign->AzimuthData;
    Output_sign->index = 0;
    Output_sign->number_of_objects = 0;

    if (Input_Sign != nullptr && Input_Sign->TargetPositionData != nullptr && Input_Sign->TargetPositionData->Target_map != nullptr) {
        int threshold = (Params != nullptr) ? Params->threshold.threshold : 0;
        int count = Input_Sign->TargetPositionData->cntTarget;

        for (int i = 0; i < count && Output_sign->number_of_objects < 256; ++i) {
            struct Point target = Input_Sign->TargetPositionData->Target_map[i];
            if (target.amplitude <= 0.0f) {
                continue;
            }

            Output_sign->sign[Output_sign->number_of_objects].distance = (int)target.distance;
            Output_sign->sign[Output_sign->number_of_objects].AzimuthData = Input_Sign->AzimuthData;
            Output_sign->sign[Output_sign->number_of_objects].amplitude = (int)target.amplitude;
            Output_sign->sign[Output_sign->number_of_objects].threshold = threshold;
            Output_sign->sign[Output_sign->number_of_objects].number = Output_sign->number_of_objects + 1;
            ++Output_sign->number_of_objects;
        }
    }

    // Обязательно освобождаем временную память, чтобы не было утечек (Memory Leaks)
    free(threshold_generator_out);
    free(threshold_device_out);
    free(Final_Sign);

    return 0;
}