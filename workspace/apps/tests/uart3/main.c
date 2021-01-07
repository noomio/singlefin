#include <stdio.h>
#include <qapi_timer.h>
#include "uart.h"

int main(int argc, char * argv[])
{

	uint8_t str[32];
	uint8_t in[32];
	int32_t i=9283;

	puts("uart3\r\n");
	
	uart_config(3,115200,QAPI_UART_1_0_STOP_BITS_E,QAPI_UART_8_BITS_PER_CHAR_E,QAPI_UART_NO_PARITY_E,QAPI_FCTL_OFF_E);


	for(;;){

		sprintf(str,"%u\r\n",i++);
		uart_write(3,str,strlen(str));

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

		int recvd = uart_read(3,in,sizeof(in));

		if(recvd){
			in[recvd+1] = '\0';
			printf("recvd=%u,in=%s",recvd,in);
		}



	}

	return 1;
}

