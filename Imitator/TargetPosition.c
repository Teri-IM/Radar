/*
 * Автор: Асафьев Мирон
 *
 * Наименование модуля: Генерация карты целей
 *
 * Назначение: создает карту целей
 * В данном файле: описывается функция генерации карты целей
 *
 */
#include "TargetPosition.h"

int TargetPositionMap(struct TargetPosParam *p, struct ImitParam *imP, struct TargetPositionOut *out){
	if (p == NULL || imP == NULL ) {
		return 1;
	}
    out->cntTarget=p->cntTarget;
	if (p->enable==1){
		for(int i = 0; i<p->cntTarget; i++){
			out->Target_map[i].angle=((float)rand() / (float)RAND_MAX)*360.0f;
            out->Target_map[i].distance = 1.0f + (((float)rand() / (float)RAND_MAX) * (imP->maxDistance - 1.0f));
            out->Target_map[i].amplitude=p->Targetamplitude;
		}
	}else{
        out->cntTarget=0;
		for(int i = 0; i<p->cntTarget; i++){
			out->Target_map[i].angle=0;
			out->Target_map[i].distance=0;
            out->Target_map[i].amplitude=0;
		}
	}
	return 0;
}
