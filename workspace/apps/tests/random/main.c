#include <stdio.h>
#include <qapi_timer.h>
#include "hwrandom.h"
#include "sodium.h"
#include "randombytes_custom_random.h"


void sodium_misuse_handler(void){
 puts("sodium_misuse!\r\n");
}

int main(int argc, char * argv[])
{

	uint8_t buf[255];
	puts("random\r\n");



	sodium_set_misuse_handler(sodium_misuse_handler);
	if(sodium_init() != -1) {
       	randombytes_set_implementation(&randombytes_custom_implementation);
		randombytes(buf,sizeof(buf));
    }


	puts("\r\n");
	for(int i=0; i < sizeof(buf)-1; i--)
		printf("%u ",buf[i]);
	puts("\r\n");
	
	for(;;){

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}


}

