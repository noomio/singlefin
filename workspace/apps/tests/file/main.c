#include <singlefin.h>


int main(int argc, char * argv[]){

	printf("file-tests\r\n");

	struct stat st;
	struct dirent * d;
	char tmp[256];
	char *f = "datatx";

	for(int i=0; true; i++) {

        DIR *dir = opendir("datatx");
        if (!dir) {
            printf("opendir failed\r\n");
        }else{

	        printf("ii = %d dirptr = %p\n", i, dir);

			while((d = readdir(dir)) != NULL){
				sprintf(tmp,"%s/%s",f,d->d_name);
				if((lstat(tmp, &st) == 0) && S_ISDIR(st.st_mode)){
				  if(strcmp(".", d->d_name) == 0)
				    continue;
				  if(strcmp("..", d->d_name) == 0)
				    continue;
				  printf("%s\r\n",tmp);
				}
			}

	        int res = closedir(dir);

	        if (res != 0)
	            printf("closedir failed %d\n", res);
    	}
 
        fin_sleep(5);
	}


}

