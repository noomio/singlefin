#include <stdio.h>
#include <qapi_timer.h>


int main(int argc, char * argv[])
{


	puts("random\r\n");
	
	for(;;){


		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

