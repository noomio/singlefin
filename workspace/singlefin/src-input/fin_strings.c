#include "fin_internal.h"

char *strdup(const char *s){
	char *str = NULL;
	if(s){
		str = malloc(strlen(s)+1);
		strcpy(str,s);
	}

	return str;
}