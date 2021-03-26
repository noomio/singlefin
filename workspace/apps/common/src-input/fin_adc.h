#if !defined(FIN_API_ADC_H_INCLUDED)
#define FIN_API_ADC_H_INCLUDED

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

#endif  /* FIN_API_ADC_H_INCLUDED */

