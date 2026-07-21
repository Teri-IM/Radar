#include "../include/threshold.h"

int threshold_generator (struct GlobalProcessingParam *par, struct data *in, struct Codogramm *thr)
{
  /* Если параметр включения = 0, то выход из функции */
  if (par->threshold.enable == 0) {
    printf("turned off");
    return 1;
  }

  /* Цикл заполнения промежуточной структуры пороговыми значениями */
  int i;
  for(i = 0; i < 256; i++){
    thr->sign[i].threshold = par->threshold.threshold;
  }
  /**********************************
  thr->azimuth = in->azimuth;
  thr->worktime = in->worktime;
  thr->index = in->index;
  thr->sample_size = in->sample_size;
  **********************************/

  return 0;
}