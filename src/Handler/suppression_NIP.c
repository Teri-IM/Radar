#include "suppression_NIP.h"

int suppression_NIP(struct suppression_NIP_Param *param, struct data *in, struct data *out)
{ 
    if (param->enable == 1)
        {
            *out = *in;
        }   
    return 0;
}