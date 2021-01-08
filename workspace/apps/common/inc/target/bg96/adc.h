
#ifndef __BG96_ADC_H__
#define __BG96_ADC_H__

#include "qapi_adc_types.h"

typedef enum adc_channel{	
	ADC0,
	ADC1,
	ADC_MAX_NO
}adc_channel_t;

int adc_config(adc_channel_t adc_num);
uint32_t adc_read_microvolts(adc_channel_t adc_num);
uint32_t adc_read_physical(adc_channel_t adc_num);
uint32_t adc_read_percent(adc_channel_t adc_num);
uint32_t adc_read_raw(adc_channel_t adc_num);

#endif