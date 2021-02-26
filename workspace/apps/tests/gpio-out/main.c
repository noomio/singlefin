#include <stdio.h>
#include <qapi_timer.h>
#include "gpio.h"

int main(int argc, char * argv[])
{

	puts("gpio_out\r\n");
	
	gpio_pin_config(18,QAPI_GPIO_NO_PULL_E,QAPI_GPIO_12MA_E,QAPI_GPIO_OUTPUT_E);
	gpio_pin_config(19,QAPI_GPIO_NO_PULL_E,QAPI_GPIO_12MA_E,QAPI_GPIO_OUTPUT_E);
	gpio_pin_config(26,QAPI_GPIO_NO_PULL_E,QAPI_GPIO_12MA_E,QAPI_GPIO_OUTPUT_E);


	for(;;){

		gpio_pin_write(18,true);
		gpio_pin_write(19,true);
		gpio_pin_write(26,true);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
		gpio_pin_write(18,false);
		gpio_pin_write(19,false);
		gpio_pin_write(26,false);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

