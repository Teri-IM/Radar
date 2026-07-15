#include "../include/codogramm.h"

int former_codogramm(struct ImitOutData *in, struct data *out)
{
    *out->data.AzimuthData = *in->AzimuthData;
    *out->data.TimeData = *in->TimeData;
    *out->data.UAD = *in->SummatorData;
    return 0;
}