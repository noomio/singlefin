/*
*
* @author :  Nikolas Karakotas
* @date   :  08/01/2021
*
*/
#include "fin_internal.h"

struct adc_map {
	adc_channel_t adc_num;
	qapi_ADC_Handle_t handle;
	qapi_Adc_Input_Properties_Type_t properties;
	const char *channel_name;
};

struct adc_map adc[ADC_MAX_NO] = {
	{ADC0, NULL, {0,0}, ADC_INPUT_ADC0},
	{ADC1, NULL, {0,0}, ADC_INPUT_ADC1}
};



int adc_config(adc_channel_t adc_num){
	if(adc_num >= 0 && adc_num < ADC_MAX_NO){
		if(qapi_ADC_Open(&adc[adc_num].handle, 0) == QAPI_OK){
			return qapi_ADC_Get_Input_Properties(adc[adc_num].handle, 
				adc[adc_num].channel_name, 
				sizeof(adc[adc_num].channel_name), 
				&adc[adc_num].properties);
		}

	}

	return 1;
}

uint32_t adc_read_microvolts(adc_channel_t adc_num){
	qapi_ADC_Read_Result_t res;

	if(adc_num >= 0 && adc_num < ADC_MAX_NO){
		if( qapi_ADC_Read_Channel(adc[adc_num].handle, &adc[adc_num].properties, &res) == QAPI_OK){
			return res.nMicrovolts;
		}
	}

	return -1;

}

uint32_t adc_read_physical(adc_channel_t adc_num){
	qapi_ADC_Read_Result_t res;

	if(adc_num >= 0 && adc_num < ADC_MAX_NO){
		if( qapi_ADC_Read_Channel(adc[adc_num].handle, &adc[adc_num].properties, &res) == QAPI_OK){
			return res.nPhysical;
		}
	}

	return -1;
}

uint32_t adc_read_percent(adc_channel_t adc_num){
	qapi_ADC_Read_Result_t res;

	if(adc_num >= 0 && adc_num < ADC_MAX_NO){
		if( qapi_ADC_Read_Channel(adc[adc_num].handle, &adc[adc_num].properties, &res) == QAPI_OK){
			return res.nPercent;
		}
	}

	return -1;
}

uint32_t adc_read_raw(adc_channel_t adc_num){
	qapi_ADC_Read_Result_t res;

	if(adc_num >= 0 && adc_num < ADC_MAX_NO){
		if( qapi_ADC_Read_Channel(adc[adc_num].handle, &adc[adc_num].properties, &res) == QAPI_OK){
			return res.nCode;
		}
	}

	return -1;
}
