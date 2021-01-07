#include <stdio.h>
#include <qapi_timer.h>
#include "uart.h"

int main(int argc, char * argv[])
{

	puts("uart3\r\n");
	
	int res = uart_config(3,115200,QAPI_UART_1_0_STOP_BITS_E,QAPI_UART_8_BITS_PER_CHAR_E,QAPI_UART_NO_PARITY_E,QAPI_FCTL_OFF_E);
	printf("res=%d\r\n",res);

	for(;;){

		res = uart_write(3,"Hello!\r\n",8);
		printf("res=%d\r\n",res);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

