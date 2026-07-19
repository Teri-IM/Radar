#include "../include/codogramm.h"

int former_codogramm(struct Codogramm *in, struct Codogramm *out)
{
    out->AzimuthData = in->AzimuthData;
    out->TimeData = in->TimeData;
    out->sign[256] = in->sign[256];
    return 0;
}