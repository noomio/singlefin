/*
 * cli.c
 *
 *  Created on: 30 Nov.,2020
 *      Author: Nikolas Karakotas
 */
#include "fin_internal.h"

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

const char *prompt = "\r\n>> ";

static int cli_cmd_help(fin_cli_t *ctx){

	fin_cli_cmd_t *iter;

	if(ctx){
		iter = ctx->cmds;
		if(iter){
			puts("\r\nFor command help, type COMMAND -h. For example: ls -h.\r\n");
		}

		while(iter){
			puts(iter->name);
			puts(" ");
			iter = iter->next;
		}
	}

	puts("\r\n");

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
	while((opt=getopt(args, argv, ":hm:")) != EOF){
    	switch(opt) {
			case 'm':
					if(strcmp(optarg,"heap") == 0){
					/* Retrieve information about the previously created
					block pool "my_pool." */
					tx_byte_pool_info_get(malloc_get_pool(), 
						"memheap",
						&available, &fragments,&first_suspended, &suspended_count,
						&next_pool);
					printf("Available:\t\t%lu\nFragments:\t\t%lu\nFirst Suspended:\t%p\nSuspended Count:\t%lu\nNext Pool:\t\t%p\n", 
						available,fragments,first_suspended,suspended_count,next_pool);
				}
			break;
			case 'h':
			default:
				puts("Usage: meminfo -m STRING\r\n"
					"STRING:\r\n"
					"\theap\r\n\tDisplay the heap information used by malloc, calloc & realloc."
					);
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
    			goto usage;
			break;
    	}
    }

    /* Only way to get argumnet without option i.e rm /datatx/app.bin */
    if(args == 2 && optind == 1 && opt == -1 && optarg == NULL && argv[1] != NULL){
    	unlink(argv[1]);
    	return 0;
    }

usage:
	puts("\r\nrm\r\n"
		"Remove files (delete/unlink)\r\n\r\n"
		"Syntax:\r\nrm [options] FILE\r\n\r\n"
		"Options:\r\n\r\n"
		"\t-d\r\n\tRemove a directory\r\n");

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


void fin_cli_free(fin_cli_t *ctx){
	free(ctx->in);
//TODO	free(cli->cmd);
	free(ctx);
}

fin_cli_t *fin_cli_new(void){


	fin_cli_t *ctx = malloc(sizeof(cli_t));
	

	fin_cli_cmd_t *meminfo = malloc(sizeof(cli_cmd_t));
	meminfo->name = (char*)CLI_CMD_MEMINFO;
	meminfo->callback = cli_cmd_meminfo;
	meminfo->next = NULL;

	fin_cli_cmd_t *ls = malloc(sizeof(cli_cmd_t));
	ls->name = (char*)CLI_CMD_LS;
	ls->callback = cli_cmd_ls;
	ls->next = NULL;

	fin_cli_cmd_t *cat = malloc(sizeof(cli_cmd_t));
	cat->name = (char*)CLI_CMD_CAT;
	cat->callback = cli_cmd_cat;
	cat->next = NULL;

	fin_cli_cmd_t *rm = malloc(sizeof(cli_cmd_t));
	rm->name = (char*)CLI_CMD_RM;
	rm->callback = cli_cmd_rm;
	rm->next = NULL;

	fin_cli_cmd_t *mkdir = malloc(sizeof(cli_cmd_t));
	mkdir->name = (char*)CLI_CMD_MKDIR;
	mkdir->callback = cli_cmd_mkdir;
	mkdir->next = NULL;

	fin_cli_cmd_t *touch = malloc(sizeof(cli_cmd_t));
	touch->name = (char*)CLI_CMD_TOUCH;
	touch->callback = cli_cmd_touch;
	touch->next = NULL;

	fin_cli_cmd_t *echo = malloc(sizeof(cli_cmd_t));
	echo->name = (char*)CLI_CMD_ECHO;
	echo->callback = cli_cmd_echo;
	echo->next = NULL;

	touch->next = echo;
	mkdir->next = touch;
	rm->next = mkdir;
	cat->next = rm;
	ls->next = cat;
	meminfo->next = ls; 
	ctx->cmds = meminfo; 

	ctx->in = malloc(STDIO_IN_MAX);
	memset(&ctx->in[0],'\0',STDIO_IN_MAX);
	ctx->head = 0;	
	ctx->tail = 0;
	ctx->count = 0;
	puts(prompt);
	return ctx;
}

int fin_cli_register(fin_cli_t *ctx, const char *name, fin_cli_callback_t func){
	
	fin_cli_cmd_t *iter;

	if(name && func && ctx){

		iter = ctx->cmds;
		while(iter->next != NULL){
			if(strcmp(iter->name,name) == 0 )
				return 1;
			iter = iter->next;
		}

		fin_cli_cmd_t *cmd = malloc(sizeof(fin_cli_cmd_t));
		cmd->name = (char*)name;
		cmd->callback = func;
		cmd->next = NULL;
		
		iter->next = cmd;

		return 0;
	}

	return 1;
}



void fin_cli_input(fin_cli_t *ctx, char c){

	int args = 0;
	char *argv[STDIO_CMD_ARGS_MAX];

	if(c >= 0x20 && c <= 0x7E){ 

		if(ctx->count < STDIO_IN_MAX-1){
			ctx->in[ctx->head++] = c;
			ctx->count++;
		}else{
			ctx->in[STDIO_IN_MAX-1] = '\0';
			ctx->count = STDIO_IN_MAX;
		}

	}else{ // control ascii codes
		if(c != '\n' || c != '\r'){

		}
		else if(c == '\b' || c == 0x08 || c == 0x7F){ // backspace
			if(ctx->count){	
				ctx->head--;			
				ctx->in[ctx->head] = '\0';
				ctx->count--;					
			}
		}else if(c == 0x1b || c == 3){ // CTRL+C

		}

	}


	if((c == '\r' || c == '\n')){

		puts("\r\n");

		if(ctx->count == 0){
			puts(prompt);
			return;
		}

		ctx->in[ctx->count] = '\0';
		//printf("%s,%d,%d\r\n",ctx->in,ctx->count,ctx->head);
		memset(argv,0,STDIO_CMD_ARGS_MAX);
		char *token = strtok(ctx->in, " ");

		while( token != NULL ) {
			argv[args] = strdup(token);
			args++;
		  	token = strtok(NULL, " ");
		}

		if(argv[0] && strcmp(argv[0],"help") == 0){
			cli_cmd_help(ctx);
		}else{

			cli_cmd_t *cmd = ctx->cmds;
			optind = 1;
			opterr = 0;

			while(cmd){
				//printf("cmd->name=%s,argv[0]=%s",cmd->name,argv[0]);
				if(args && argv[0] && strcmp(cmd->name,argv[0]) == 0 )
					cmd->callback(args, argv);
				cmd = cmd->next;

			}

			while(args){
				free(argv[args-1]);
				args--;
			}

		}

		//fflush(stdin);
		memset(ctx->in,'\0',STDIO_IN_MAX);
		ctx->head = 0;
		ctx->tail = 0;
		ctx->count = 0;

		puts(prompt);

	}


}