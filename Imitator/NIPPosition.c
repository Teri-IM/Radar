/*
 * Автор: Стулова Анастасия
 *
 * Наименование модуля: Расчет положения нип
 *
 * Назначение: Создание карты нип а также определения номера дискредитизации
 * В данном файле: описывается функция создания карты а также функция определения номера дискредитизации
 *
 */
#include "NIPPosition.h"

int createNipMap(struct NIPPosParam *p, struct ImitParam *imP){
	if (p == NULL ) {
		return 1;
	}
	if (p->enable==1){

		for(int i = 0; i<p->cntNIP; i++){
			p->NIPMap[i].angle=((float)rand() / (float)RAND_MAX)*360.0f;
            p->NIPMap[i].distance=1.0f + (((float)rand() / (float)RAND_MAX)*(imP->maxDistance-1.0f));
			p->NIPMap[i].NIPprobing_time=(long long)(rand()%p->MaxNIP_probingTime);
		}
    }else{

		for(int i = 0; i<p->cntNIP; i++){
			p->NIPMap[i].angle=0;
			p->NIPMap[i].distance=0;
			p->NIPMap[i].NIPprobing_time=0;
		}
	}
	return 0;
}

int NIPPositionMap(struct NIPPosParam *p, struct UTimeParam *timep, struct UnifedTimeOut *in, struct NIPPositionOut *out){
	if (p == NULL || timep == NULL || in == NULL) {
		return 1;
	}
	long long NIPtime=0;
	out->cntNip=p->cntNIP;
    if (p->enable==1){
        for(int i = 0; i<p->cntNIP; i++){
            NIPtime=in->total_ns%p->NIPMap[i].NIPprobing_time;
            if (NIPtime-timep->probing_time<0){
                out->NIP_map[i].amplitude=p->NIPamplitude;
                out->NIP_map[i].angle=p->NIPMap[i].angle;
                out->NIP_map[i].distance=p->NIPMap[i].distance;
                out->NIP_map[i].discredNum=floor(llabs(NIPtime-(timep->probing_time-timep->pulse_time))*timep->sampling_rate);
            }else{
                out->NIP_map[i].amplitude=0;
                out->NIP_map[i].angle=0;
                out->NIP_map[i].distance=0;
                out->NIP_map[i].discredNum=0;
            }
        }
    }else{
        for(int i = 0; i<p->cntNIP; i++){
                out->NIP_map[i].amplitude=0;
                out->NIP_map[i].angle=0;
                out->NIP_map[i].distance=0;
                out->NIP_map[i].discredNum=0;
        }
    }
	return 0;
}

