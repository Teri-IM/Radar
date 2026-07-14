/*
 * Автор: Мокров Иван
 *
 * Наименование модуля: Обновление карты ПП
 *
 * Назначение: создает карту пассивных помех
 * В данном файле: описывается функция генерации карты ПП
 *
 */

#include "PPPosition.h"

int PPPositionMap(struct PPPosParam *p, struct ImitParam *imP, struct PPPositionOut *out){
	if (p == NULL || imP == NULL ) {
		return 1;
	}
	if (p->enable==1){
		for(int i = 0; i<p->cntPP; i++){
			out->PP_map[i].angle=((float)rand() / (float)RAND_MAX)*360.0f;
			out->PP_map[i].distance=1.0f + (((float)rand() / (float)RAND_MAX+1.0f)*(imP->maxDistance-1.0f));
			out->PP_map[i].amplitude=p->PPamplitude;
			out->cntPP=p->cntPP;
		}
	}else{
		for(int i = 0; i<p->cntPP; i++){
			out->PP_map[i].angle=0;
			out->PP_map[i].distance=0;
			out->PP_map[i].amplitude=0;
			out->cntPP=0;
		}
	}
	return 0;
}
