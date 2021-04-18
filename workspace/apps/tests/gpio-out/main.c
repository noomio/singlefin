#include "singlefin.h"

int main(int argc, char * argv[])
{

	puts("gpio-out-test\r\n");
	
	fin_gpio_pin_config(PIN18,PullUp,Drive12mA,Output);
	fin_gpio_pin_config(PIN19,PullUp,Drive12mA,Output);
	fin_gpio_pin_config(PIN26,PullUp,Drive12mA,Output);
	fin_gpio_config_dump(PIN19);

	for(;;){

		fin_gpio_pin_write(PIN18,true);
		fin_gpio_pin_write(PIN19,true);
		fin_gpio_pin_write(PIN26,true);
		fin_sleep(1000);
		fin_gpio_pin_write(PIN18,false);
		fin_gpio_pin_write(PIN19,false);
		fin_gpio_pin_write(PIN26,false);
		fin_sleep(1000);

	}

	return 1;
}

