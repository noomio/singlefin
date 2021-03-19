#include <stdio.h>
#include <qapi_timer.h>

int main(int argc, char * argv[])
{

	puts("in main fault app\r\n");

	for(;;){

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}


}

