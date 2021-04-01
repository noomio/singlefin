#include <singlefin.h>


int main(int argc, char * argv[]){

	printf("Aloha!\r\n");
	void *p = malloc(10);
	free(p);

	fin_gpio_pin_config(PIN18,PullUp,Drive12mA,Input);
	int res = fin_i2c_config(I2C1);

	for(;;){

		fin_sleep(1000);

	}


}

