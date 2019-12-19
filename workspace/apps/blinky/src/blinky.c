#include <qapi.h>
#include <qapi_txm_base.h>
#include "qapi_tlmm.h"
#include <qapi_timer.h>
#include "blinky.h"
#include "debug.h"

/*
*	In this example the PCM_CLK pin is used. Below is the mapping from BG96 to LTE OPEN V1.1
*	The final physical pin name to LTE OPEN is GPIO_78.
* 	Its needed to remove the pin header and link with a wire to LED1_CTRL on the LTE OPEN EVB.
*	
*	BG96 PIN NAME	|	BG96 PIN NO		|	TE-A	|	LTE OPEN V1.1 PIN NAME
*	PCM_CLK			|	4				|	32		|	GPIO_78
*
*/

int main(void)
{

	uint32_t tick = 0;
	qapi_GPIO_ID_t gpio_id;
	qapi_TLMM_Config_t tlmm_config;
	qapi_Status_t status = QAPI_OK;

	printf("Hello world!\n");
	
	tlmm_config.pin = 23;
	tlmm_config.func = 0; 
	// pin mux value 1
	tlmm_config.dir = QAPI_GPIO_OUTPUT_E;
	tlmm_config.pull = QAPI_GPIO_NO_PULL_E;
	tlmm_config.drive = QAPI_GPIO_2MA_E; // drive is for output pins, specify
	// the default here
	status = qapi_TLMM_Get_Gpio_ID( &tlmm_config, &gpio_id);
	
	if (status == QAPI_OK){
		status = qapi_TLMM_Config_Gpio(gpio_id, &tlmm_config);
		if (status != QAPI_OK){
			printf("GPIO config error: %d",status);
		}
	}
	
	
	for(;;){
		
		if (tick++ % 2) {
			status = qapi_TLMM_Drive_Gpio(gpio_id, tlmm_config.pin, QAPI_GPIO_HIGH_VALUE_E);
		} else {
			status = qapi_TLMM_Drive_Gpio(gpio_id, tlmm_config.pin, QAPI_GPIO_LOW_VALUE_E);
		}
	
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;

}


