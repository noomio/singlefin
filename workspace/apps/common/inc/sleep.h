#ifndef __920X_SLEEP_H__
#define __920X_SLEEP_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <qapi_timer.h>
	
#define sleep(T) qapi_Timer_Sleep(T, QAPI_TIMER_UNIT_USEC, true)


#ifdef __cplusplus
}
#endif

#endif