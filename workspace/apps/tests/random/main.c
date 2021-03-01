#include <stdio.h>
#include <qapi_timer.h>
#include "hwrandom.h"
#include "sodium.h"
#include "randombytes_custom_random.h"

int main(int argc, char * argv[])
{

	uint8_t buf[255];
	puts("random\r\n");

	if(sodium_init() == -1) {
        puts("sodium_init: fail\r\n");
    }

	randombytes_set_implementation(&randombytes_custom_implementation);
	
	for(;;){

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

