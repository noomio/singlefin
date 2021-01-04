#include <stdio.h>
#include <qapi_timer.h>
#include "gpio.h"

int main(int argc, char * argv[])
{

	puts("gpio\r\n");
	//int res = gpio_output(18,QAPI_GPIO_12MA_E,QAPI_GPIO_NO_PULL_E);
	//printf("%d\r\n",res);

	for(;;){

		//gpio_write_pin(18,true);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
		//gpio_write_pin(18,false);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

