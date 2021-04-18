#include "singlefin.h"
#include "sodium.h"
#include "randombytes_custom_random.h"


extern uint64_t cputimeusecs(void);

void sodium_misuse_handler(void){
 puts("sodium_misuse!\r\n");
}

int main(int argc, char * argv[])
{

	uint8_t buf[255];
	puts("random-test\r\n");

	sodium_set_misuse_handler(sodium_misuse_handler);
    randombytes_set_implementation(&randombytes_custom_implementation);

	if(sodium_init() == 0) {
		fin_randombytes(buf,sizeof(buf));
		puts("\r\n");
		for(int i=0; i < sizeof(buf)-1; i++)
			printf("%u ",buf[i]);
		puts("\r\n");
    }


	
	for(;;){

		fin_sleep(1000);

	}


}

