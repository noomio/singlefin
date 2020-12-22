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

static int cli_cmd_kermit(int args, char *argv[]){
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


	return 0;

}

int main(int argc, char * argv[])
{

	setlocale(LC_ALL, "C");	

	cli_t *cli = cli_new();
	cli_register(cli,"kermit",cli_cmd_kermit);

	for(;;){

		cli_input(cli);

	}

	return 0;
}

