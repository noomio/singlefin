#include "malloc.h"

char *strdup(const char *s){
	char *str = NULL;
	if(s){
		str = malloc(strlen(token)+1);
		strcpy(str,token);
	}

	return str;
}