#include <stdio.h>
#include <qapi_timer.h>
#include "hwrandom.h"

int main(int argc, char * argv[])
{

	uint8_t buf[255];
	puts("random\r\n");

	
	for(;;){

		randombytes(buf, sizeof(buf));
		puts("\r\n\r\n");
		for(int i=0; i < sizeof (buf)-1; i++)
			printf("%u ",buf[i]);
		puts("\r\n");

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

