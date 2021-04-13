#include <singlefin.h>


int main(int argc, char * argv[]){

	printf("file-tests\r\n");


	for(int i=0; true; i++) {

        DIR *dirptr = opendir("datatx");
        if (!dirptr) {
            printf("opendir failed\r\n");
        }else{

	        printf("ii = %d dirptr = %p\n", i, dirptr);
	        int res = closedir(dirptr);

	        if (res != 0)
	            printf("closedir failed %d\n", res);
    	}
 
        fin_sleep(5);
	}


}

