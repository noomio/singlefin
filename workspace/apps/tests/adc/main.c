#include <stdio.h>
#include <qapi_timer.h>
#include "adc.h"

int main(int argc, char * argv[])
{


	puts("adc\r\n");
	
	int res0 = adc_config(ADC0);
	int res1 = adc_config(ADC1);

	printf("Config ADC0=%u, ADC1=%u\r\n",res0,res1);

	for(;;){


		uint32_t adc0 = adc_read_microvolts(ADC0);
		uint32_t adc1 = adc_read_microvolts(ADC1);
		
		printf("ADC0=%u, ADC1=%u\r\n",adc0,adc1);

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

