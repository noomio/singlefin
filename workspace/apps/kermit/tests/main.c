#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <locale.h>
#include <qapi_timer.h>

#include "cli.h"

#undef PATH_MAX
#define PATH_MAX 128

extern void ek(int argc, char ** argv);

static int cli_cmd_ek(int args, char *argv[]){
	#if 0
	int opt;
	char *f = NULL;
	while((opt=getopt(args, argv, "f:")) != -1){
    	switch(opt) {
			case 'f':
				f = strdup(optarg);
				break;
			default:
				break;
    	}
    }

	free(f);
#endif
	ek(args,argv);

	return 0;

}

int main(int argc, char * argv[])
{


	cli_t *cli = cli_new();
	cli_register(cli,"ek",cli_cmd_ek);

	for(;;){

		cli_input(cli);
		
	}

	return 0;
}

