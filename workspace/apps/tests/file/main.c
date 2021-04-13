#include <singlefin.h>


int main(int argc, char * argv[]){

	printf("file-tests\r\n");

	struct dirent * d;
	char tmp[24];
	char *f = "datatx";

	for(int i=0; true; i++) {

        DIR *dir = opendir(f);
        if (!dir) {
            printf("opendir failed\r\n");
        }else{

	        printf("ii = %d dirptr = %p\n", i, dir);

	        int res = closedir(dir);

	        if (res != 0)
	            printf("closedir failed %d\n", res);
    	}
 
        fin_sleep(5);
	}


}

