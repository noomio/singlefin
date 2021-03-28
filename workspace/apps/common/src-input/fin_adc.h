#if !defined(FIN_API_ADC_H_INCLUDED)
#define FIN_API_ADC_H_INCLUDED

typedef enum fin_adc_channel{	
	ADC0,
	ADC1,
	ADC_MAX_NO
}fin_adc_channel_t;

int fin_adc_config(fin_adc_channel_t adc_num);
uint32_t fin_adc_read_microvolts(fin_adc_channel_t adc_num);
uint32_t fin_adc_read_physical(fin_adc_channel_t adc_num);
uint32_t fin_adc_read_percent(fin_adc_channel_t adc_num);
uint32_t fin_adc_read_raw(fin_adc_channel_t adc_num);

#endif  /* FIN_API_ADC_H_INCLUDED */

