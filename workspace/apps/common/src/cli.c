/*
 * cli.c
 *
 *  Created on: 30 Nov.,2020
 *      Author: Nikolas Karakotas
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <getopt.h>
#include <ctype.h>
#include <txm_module.h>
#include <unistd.h>
#include <dirent.h>
#include <qapi_fs_types.h>
#include <sys/stat.h>
#include "cli.h"

#undef PATH_MAX
#define PATH_MAX 128

const char *CLI_CMD_HELP = "help";
const char *CLI_CMD_MEMINFO = "meminfo";
const char *CLI_CMD_LS = "ls";
const char *CLI_CMD_CAT = "cat";
const char *CLI_CMD_RM = "rm";
const char *CLI_CMD_MKDIR = "mkdir";
const char *CLI_CMD_TOUCH = "touch";
const char *CLI_CMD_ECHO = "echo";

static int cli_cmd_help(int args, char *argv[]){
	int opt;
	while((opt=getopt(args, argv, ":h")) != -1){
    	switch(opt) {
    		default:
				printf("%s\r\n", optarg);
				puts("Usage: \r\n");
			break;
    	}
    }

	printf("%s\r\n", optarg);
	puts("Usage: \r\n");  	


	return 0;

}

extern TX_BYTE_POOL *malloc_get_pool(void);
static int cli_cmd_meminfo(int args, char *argv[]){

	ULONG available;
	ULONG fragments;
	TX_THREAD *first_suspended;
	ULONG suspended_count;
	TX_BYTE_POOL *next_pool;
	UINT status;


	int opt;
	while((opt=getopt(args, argv, ":m")) != EOF){
    	switch(opt) {
			case 'm':
				printf("%s\r\n", optarg);
				/* Retrieve information about the previously created
				block pool "my_pool." */
				tx_byte_pool_info_get(malloc_get_pool(), 
					"memheap",
					&available, &fragments,&first_suspended, &suspended_count,
					&next_pool);
				printf("Available:\t\t%lu\nFragments:\t\t%lu\nFirst Suspended:\t%p\nSuspended Count:\t%lu\nNext Pool:\t\t%p\n", 
					available,fragments,first_suspended,suspended_count,next_pool);
			break;
			default:
				// usage
				return -1;
				break;
    	}
    }

	return 0;

}


static int cli_cmd_ls(int args, char *argv[]){
	char *arg = NULL;
	const char *root= "/";
	char tmp[PATH_MAX];
	DIR * dir;
	struct dirent * d;
	struct stat st;

	if(args == 1){
		/* Always do root when empty */
		if((lstat(root, &st) == 0) && S_ISDIR(st.st_mode)){
			if((dir = opendir(root)) != NULL){
				while((d = readdir(dir)) != NULL){
					printf("%s\r\n",d->d_name);
				}
				closedir(dir);
			}			

		}
	}else if(args == 2){
		arg = argv[1];

		if(!strchr(arg,'-')){
			if((lstat(arg, &st) == 0) && S_ISDIR(st.st_mode)){
				if((dir = opendir(arg)) != NULL){
					while((d = readdir(dir)) != NULL){
						printf("%s\r\n",d->d_name);
					}
					closedir(dir);
				}			

			}
		}else{
			puts("Usage: ls FOLDER\r\n");
		}
	}else
		puts("Usage: ls FOLDER\r\n");

	return 0;

}

static int cli_cmd_cat(int args, char *argv[]){
	FILE * fp;
	char *filename;
	void * buf;
	size_t l, len, rlen=0;
	struct stat st;

	if(args <= 1 || (args == 2 && strcmp(argv[1],"-h") == 0) ){
		// usage
	    puts("Usage: cat <file>\r\n");
	}
	else if(args == 2){
		filename = argv[1];
		if((lstat(filename, &st) == 0) && S_ISREG(st.st_mode)){
			fp = fopen(filename, "r");
			if(fp)
			{
				fseek(fp, 0L, SEEK_END);
				l = ftell(fp);
				fseek(fp, 0L, SEEK_SET);
				if(l > 0)
				{
					buf = malloc(256);
					if(buf)
					{
						for(len = 0; len < l; len += rlen){
							rlen = fread(buf + len, 1, l - len, fp);
							if(rlen == 0) break;
							write(0,buf,rlen);
						}

						free(buf);
					}
				}
				fclose(fp);
			}	

		}
	}

	return 0;

}

static int cli_cmd_rm(int args, char *argv[]){
	int opt;
	while((opt=getopt(args, argv, ":d:")) != -1){
    	switch(opt) {
    		case 'd':
    			rmdir (optarg);
    			break;
    		default:
				puts("\r\nrm\r\n"
					"Remove files (delete/unlink)\r\n\r\n"
					"Syntax:\r\nrm [options] FILE\r\n\r\n"
					"Options:\r\n\r\n"
					"\t-d\r\n\tRemove a directory\r\n");

			break;
    	}
    }

    /* Only way to get argumnet without option i.e rm /datatx/app.bin */
    if(args == 2 && optind == 1 && opt == -1 && optarg == NULL && argv[args-1] != NULL){
    	unlink(argv[args-1]);
    }


	return 0;

}

static int cli_cmd_mkdir(int args, char *argv[]){

    /* Only way to get argumnet without option i.e rm /datatx/app.bin */
    if(args == 2 && optind == 1 && optarg == NULL && argv[args-1] != NULL){
    	mkdir(argv[args-1],QAPI_FS_S_IRUSR_E|QAPI_FS_S_IWUSR_E);
    }else{
		puts("\r\nmkdir\r\n"
		"Create new folder(s), if they do not already exist.\r\n\r\n"
		"Syntax:\r\n\tmkdir FOLDER\r\n\r\n");    	
    }


	return 0;

}

static int cli_cmd_touch(int args, char *argv[]){

    /* Only way to get argumnet without option i.e rm /datatx/app.bin */
    if(args == 2 && optind == 1 && optarg == NULL && argv[args-1] != NULL){
    	FILE * fp = fopen(argv[args-1],"a+");
    	fclose(fp);
    }else{
		puts("\r\ntouch\r\n"
		"Create new file, if it does not already exist.\r\n\r\n"
		"Syntax:\r\n\ttouch FILE\r\n\r\n");    	
    }


	return 0;

}

static int cli_cmd_echo(int args, char *argv[]){
	int opt;
	char *file = NULL;
	char *str = argv[1];
	bool append = false;
	optind = 2;

	while((opt=getopt(args, argv, ":f:a")) != -1){
    	switch(opt) {
    		case 'f':
    			file = strdup(optarg);
    			break;
    		case 'a':
    			append = true;
    			break;
    		default:
    			goto usage;
			break;
    	}
    }

    if(file != NULL && str != NULL){
    	if(append){
    		FILE *fp = fopen(file,"a");
    		if(fp){
    			fwrite(str,1,strlen(str),fp);
    			fclose(fp);
    		}
    	}else{
      		FILE *fp = fopen(file,"w");
    		if(fp){
    			fwrite(str,1,strlen(str),fp);
    			fclose(fp);
    		}  		
    	}

    	free(file);

    	return 0;
    }

usage:
	puts("\r\necho\r\n"
	"Write or append to a file. Default is write.\r\n\r\n"
	"Syntax:\r\necho STRING [options]\r\n\r\n"
	"Options:\r\n\r\n"
	"\t-f\r\n\tFile to write or append to.\r\n"
	"\t-a\r\n\tAppend to a file.\r\n");

	return 0;

}


void cli_free(cli_t *ctx){
	free(ctx->in);
//TODO	free(cli->cmd);
	free(ctx);
}

cli_t *cli_new(void){


	cli_t *ctx = malloc(sizeof(cli_t));
	
	ctx->cmds = malloc(sizeof(cli_cmd_t));
	ctx->cmds->name = (char*)CLI_CMD_HELP;
	ctx->cmds->callback = cli_cmd_help;
	ctx->cmds->next = NULL;

	cli_cmd_t *meminfo = malloc(sizeof(cli_cmd_t));
	meminfo->name = (char*)CLI_CMD_MEMINFO;
	meminfo->callback = cli_cmd_meminfo;
	meminfo->next = NULL;

	cli_cmd_t *ls = malloc(sizeof(cli_cmd_t));
	ls->name = (char*)CLI_CMD_LS;
	ls->callback = cli_cmd_ls;
	ls->next = NULL;

	cli_cmd_t *cat = malloc(sizeof(cli_cmd_t));
	cat->name = (char*)CLI_CMD_CAT;
	cat->callback = cli_cmd_cat;
	cat->next = NULL;

	cli_cmd_t *rm = malloc(sizeof(cli_cmd_t));
	rm->name = (char*)CLI_CMD_RM;
	rm->callback = cli_cmd_rm;
	rm->next = NULL;

	cli_cmd_t *mkdir = malloc(sizeof(cli_cmd_t));
	mkdir->name = (char*)CLI_CMD_MKDIR;
	mkdir->callback = cli_cmd_mkdir;
	mkdir->next = NULL;

	cli_cmd_t *touch = malloc(sizeof(cli_cmd_t));
	touch->name = (char*)CLI_CMD_TOUCH;
	touch->callback = cli_cmd_touch;
	touch->next = NULL;

	cli_cmd_t *echo = malloc(sizeof(cli_cmd_t));
	echo->name = (char*)CLI_CMD_ECHO;
	echo->callback = cli_cmd_echo;
	echo->next = NULL;

	touch->next = echo;
	mkdir->next = touch;
	rm->next = mkdir;
	cat->next = rm;
	ls->next = cat;
	meminfo->next = ls; 
	ctx->cmds->next = meminfo; 

	ctx->in = calloc(1,STDIO_IN_MAX);

	return ctx;
}

int cli_register(cli_t *ctx, const char *name, cli_callback_t func){
	
	cli_cmd_t *iter;

	if(name && func && ctx){

		iter = ctx->cmds;
		while(iter->next != NULL){
			if(strcmp(iter->name,name) == 0 )
				return 1;
			iter = iter->next;
		}

		cli_cmd_t *cmd = malloc(sizeof(cli_cmd_t));
		cmd->name = (char*)name;
		cmd->callback = func;
		cmd->next = NULL;
		
		iter->next = cmd;

		return 0;
	}

	return 1;
}

char *cli_input(cli_t *ctx){

	int got_eof = 0;
	int args = 0;
	char *str;
	char *argv[STDIO_CMD_ARGS_MAX];

	memset(argv,0,STDIO_CMD_ARGS_MAX);
	memset(ctx->in,'\0',STDIO_IN_MAX);
	str = ctx->in;

	puts("\r\n>> ");
	
	while (!got_eof) {

		for (;;) {
			int c = getchar();

			if (c == EOF || str >= (ctx->in+STDIO_IN_MAX)) {
				ctx->in[STDIO_IN_MAX-1] = '\0';
				got_eof = 1;
				break;
			} else if (c == '\n' || c == '\r') {
				got_eof = 1;
				break;
			} else if(c == '\b' || c == 0x08 || c == 127){ // backspace
				if(str > ctx->in){
					str--;
					*str = '\0';
					putchar(c);
				}
			}else if(c == 0x1b || c == 3){ // CTRL+C
				puts("\r\nctrl+c\r\n");
				return NULL;
			}
			else {
				*(str++) = (char)c;
				putchar(c);
			}
		}

	}


	if(ctx->in){

		puts("\r\n");
		char *token = strtok(ctx->in, " ");

		while( token != NULL ) {
			argv[args] = strdup(token);
			args++;
		  	token = strtok(NULL, " ");
		}

		cli_cmd_t *cmd = ctx->cmds;
		optind = 1;
		opterr = 0;

		while(cmd){
			if(args && argv[0] && strcmp(cmd->name,argv[0]) == 0 )
				cmd->callback(args, argv);
			cmd = cmd->next;

		}

		while(args){
			free(argv[args-1]);
			args--;
		}

	}
	
	return ctx->in;

}