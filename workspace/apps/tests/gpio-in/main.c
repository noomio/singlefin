#include <stdio.h>
#include <qapi_timer.h>
#include "gpio.h"

int main(int argc, char * argv[])
{

	puts("gpio_in\r\n");
	
	gpio_pin_config(18,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_INPUT_E);
	gpio_pin_config(19,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_INPUT_E);
	gpio_pin_config(26,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_INPUT_E);

	for(;;){

		printf("18=%u,19=%u,26=%d\r\n",gpio_pin_read(18),gpio_pin_read(19),gpio_pin_read(26));
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

