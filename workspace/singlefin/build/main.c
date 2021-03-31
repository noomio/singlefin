#include <singlefin.h>


int main(int argc, char * argv[]){

	printf("Aloha!\r\n");
	void *p = malloc(10);
	free(p);

	fin_gpio_pin_config(18,QAPI_GPIO_PULL_UP_E,QAPI_GPIO_12MA_E,QAPI_GPIO_INPUT_E);
	int res = fin_i2c_config(I2C1);

	for(;;){

		sleep(1000);

	}


}

