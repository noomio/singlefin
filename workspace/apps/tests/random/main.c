#include <stdio.h>
#include <qapi_timer.h>
#include "hwrandom.h"
#include "sodium.h"
#include "randombytes_custom_random.h"
#include "time.h"
#include "sys/time.h"

void sodium_misuse_handler(void){
 puts("sodium_misuse!\r\n");
}

int main(int argc, char * argv[])
{

	uint8_t buf[255];
	puts("random\r\n");

    struct timeval tv;

    gettimeofday(&tv, NULL);
    randombytes_set_implementation(&randombytes_custom_implementation);

	if(sodium_init() == 0) {
		puts("sss");
		randombytes(buf,sizeof(buf));
    }


	puts("\r\n");
	for(int i=0; i < sizeof(buf)-1; i++)
		printf("%u ",buf[i]);
	puts("\r\n");
	
	for(;;){

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}


}

