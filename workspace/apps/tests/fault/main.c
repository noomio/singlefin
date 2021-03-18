#include <stdio.h>
#include "sleep.h"

int main(int argc, char * argv[])
{

	puts("in main fault app\r\n");

	for(;;){

		sleep(5);

	}


}

