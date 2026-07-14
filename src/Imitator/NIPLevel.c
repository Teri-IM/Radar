
/*
 * Автор: Родионова Юлия
 *
 * Наименование модуля: Расчет уровней НИП
 *
 * Назначение: Понижает значение амплитуды сигнала НИП если он был получен не при засечении точки
 * В данном файле: описывается функция понижения амплитуды
 *
 */
#include "NIPLevel.h"
int NIPLevelCalculate(struct NIPLvlParam *p, struct UTimeParam *timeP, struct AzimutSensorOut *azimutIn, struct NIPPositionOut *nipPosInOut){
	if (p == NULL || timeP == NULL || azimutIn == NULL) {

	        return 1;
	    }
    float endAzimuth=azimutIn->azimuth_new;
	float startAzimuth = azimutIn->azimuth_old;

	if (p->enable==1){
		for(int i =0; i<nipPosInOut->cntNip; i++){
				if (startAzimuth <= endAzimuth){
					if((nipPosInOut->NIP_map[i].angle < startAzimuth) || (nipPosInOut->NIP_map[i].angle > endAzimuth)){
						nipPosInOut->NIP_map[i].amplitude=nipPosInOut->NIP_map[i].amplitude / p->amplitudeDecrease;

					}

				}else{
					if((nipPosInOut->NIP_map[i].angle < startAzimuth) && (nipPosInOut->NIP_map[i].angle > endAzimuth)){
						nipPosInOut->NIP_map[i].amplitude=nipPosInOut->NIP_map[i].amplitude / p->amplitudeDecrease;

					}

				}

		}
	}
	return 0;
}

