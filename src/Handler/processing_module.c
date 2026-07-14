#include "processing_module.h"
#include "Handler/DDC.h"
#include "Handler/codogramm.h"
#include "Handler/suppression_NIP.h"
#include "processing_module_param.h"

int ProcessingModule(struct GlobalProcessingParam *Params, struct data *Input_Sign, struct Codogramm *Output_sign)
{
    struct data ddc_out;
    struct data sNIP_out;
    struct data Final_Sign;

    DDC_Process(&(Params->DDC), Input_Sign, &ddc_out);

    suppression_NIP(&(Params->suppression_NIP), &ddc_out, &sNIP_out);

    former_codogramm(&sNIP_out, &Final_Sign);
    return 0;
}