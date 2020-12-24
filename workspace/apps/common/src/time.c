#include <time.h>
#include <string.h>

struct tm *localtime (const time_t *t){
	static struct tm tm;

	memset(&tm,0,sizeof(struct tm));

	return &tm;
}