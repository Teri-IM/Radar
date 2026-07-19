#include "../include/codogramm.h"

int former_codogramm(struct Codogramm *in, struct Codogramm *out)
{
    int i;
    out->AzimuthData = in->AzimuthData;//[cite: 8]
    out->TimeData = in->TimeData;//[cite: 8]
    out->index = in->index;
    out->number_of_objects = in->number_of_objects;

    /* Безопасно копируем все 256 элементов массива (от 0 до 255) */
    for (i = 0; i < 256; i++) {
        out->sign[i] = in->sign[i];
    }
    return 0;
}