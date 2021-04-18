#include "singlefin.h"


int main(int argc, char * argv[]){


	puts("adc\r\n");
	
	int res0 = fin_adc_config(ADC0);
	int res1 = fin_adc_config(ADC1);

	printf("Config ADC0=%u, ADC1=%u\r\n",res0,res1);

	for(;;){


		uint32_t adc0 = fin_adc_read_microvolts(ADC0);
		uint32_t adc1 = fin_adc_read_microvolts(ADC1);
		
		printf("ADC0=%u, ADC1=%u\r\n",adc0,adc1);

		fin_sleep(1000);

	}

	return 1;
}

