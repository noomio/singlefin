#include <time.h>
#include <string.h>
#include <qapi_timer.h>
#include <sys/time.h>


struct tm *localtime (const time_t *t){
	static struct tm tm;

	memset(&tm,0,sizeof(struct tm));

	return &tm;
}

int gettimeofday(struct timeval *tv, void *tz){

	if (tv == NULL)	{
	  return -1;
	}
  
	qapi_time_get_t t;
	qapi_time_get(QAPI_TIME_MSECS, &t);
  	
  	tv->tv_sec = t.time_secs;
  	tv->tv_usec = 0UL;

  	return 0;
}