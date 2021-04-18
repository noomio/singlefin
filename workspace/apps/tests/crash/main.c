#include "singlefin.h"

int main(int argc, char * argv[])
{

	puts("crash-test\r\n");
	
	qapi_UART_Open_Config_t uart_cfg;
	
	for(;;){

		qapi_UART_Open(NULL, 1, &uart_cfg);
		qapi_UART_Receive ((void*)0xFFFF, (void*)0xFFFFF, 0xFFFFF, (void*)0xAAAA);
	}


}

