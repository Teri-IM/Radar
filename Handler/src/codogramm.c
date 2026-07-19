#include "../include/codogramm.h"

int former_codogramm(struct Codogramm *in, struct Codogramm *out)
{
    int i;
    out->AzimuthData = in->AzimuthData;
    out->TimeData = in->TimeData;
    out->index = in->index;
    out->number_of_objects = in->number_of_objects;

    /* Безопасно переносим все 256 элементов (индексы 0 - 255) */
    for (i = 0; i < 256; i++) {
        out->sign[i] = in->sign[i];
    }
    return 0;
}
