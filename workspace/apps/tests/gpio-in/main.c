#include "singlefin.h"

int main(int argc, char * argv[])
{

	puts("gpio-in-test\r\n");
	
	fin_gpio_pin_config(PIN18,PullUp,Drive12mA,Input);
	fin_gpio_pin_config(PIN19,PullUp,Drive12mA,Input);
	fin_gpio_pin_config(PIN26,PullUp,Drive12mA,Input);

	for(;;){

		printf("PIN18=%u,PIN19=%u,PIN26=%d\r\n",
			fin_gpio_pin_read(PIN18),
			fin_gpio_pin_read(PIN19),
			fin_gpio_pin_read(PIN26));

		fin_sleep(1000);

	}

	return 1;
}

