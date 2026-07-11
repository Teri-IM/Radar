/*
 * Автор: Коротин Михаил
 *
 * Наименование модуля: Расчет откликов целей
 *
 * Назначение: Данный модуль обнаружает цели при прохождении антенны через них и определяет номер дискредитизации
 * В данном файле: описывается функиция обнаружения точек пассивных помех с последующей записью в массив
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "ClutterResponse.h"
int clutter_response_calc(
		struct ClutterResponseParams *params,
		struct UTimeParam *timeParam,
		    struct UnifedTimeOut *time_data,
		    struct AzimutSensorOut *azimut_data,
		    struct PPPositionOut *points_data,
			struct ClutterResponseOut *out) {

	if (params == NULL || timeParam == NULL || time_data == NULL || azimut_data == NULL || points_data == NULL ) {

	        return 1;
	    }

    out->cntPP_find=0;
    for (int i = 0; i < points_data->cntPP; i++) {
        out->PP_map_find[i].angle = 0;
    	out->PP_map_find[i].distance = 0;
    	out->PP_map_find[i].amplitude = 0;
        out->PP_map_find[i].discredNum = 0;
    }
    if (params->enable==1){
		float start_angle = azimut_data->azimuth_old;
		float end_angle = azimut_data->azimuth_new;
		int hit_count = 0;
		float p_angle = 0;
		for (int i = 0; i < points_data->cntPP; i++) {
			p_angle = points_data->PP_map[i].angle;
			if (start_angle<=end_angle){
				if (p_angle >= start_angle && p_angle <= end_angle) {
					out->PP_map_find[hit_count].angle = points_data->PP_map[i].angle;
					out->PP_map_find[hit_count].distance = points_data->PP_map[i].distance;
					out->PP_map_find[hit_count].amplitude = points_data->PP_map[i].amplitude;
					out->PP_map_find[hit_count].discredNum = floor(((2*points_data->PP_map[i].distance)/300000000)*timeParam->sampling_rate);
					hit_count++;

				}
			}else{
				if (p_angle >= start_angle || (p_angle <= end_angle && p_angle <= start_angle)) {
					out->PP_map_find[hit_count].angle = points_data->PP_map[i].angle;
					out->PP_map_find[hit_count].distance = points_data->PP_map[i].distance;
					out->PP_map_find[hit_count].amplitude = points_data->PP_map[i].amplitude;
					out->PP_map_find[hit_count].discredNum = floor(((2*points_data->PP_map[i].distance)/300000000)*timeParam->sampling_rate);
					hit_count++;
					setbuf(stdout, NULL);
					printf("3\n");
				}
			}
		}


		out->cntPP_find = hit_count;
    }




    return 0;
}
