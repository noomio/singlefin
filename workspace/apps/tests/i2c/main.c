#include <stdio.h>
#include <qapi_timer.h>
#include "i2c.h"
#include "math.h"

int main(int argc, char * argv[])
{

	uint8_t txdata[1] = {0x05};
	uint8_t rxdata[2];

	puts("i2c\r\n");

	int res = i2c_config(I2C1);

	for(;;){

		res = i2c_transfer(I2C1,0x18,txdata,sizeof(txdata),rxdata,sizeof(rxdata),0);
		if(res == 0){
			rxdata[0] = rxdata[0] & 0x1F;
    		int sign = rxdata[0] & (1 << 5) ? -1 : 1;
    		float temperature = sign * (rxdata[0] * pow(2, 4) + rxdata[1] * pow(2, -4));
    		printf("%f Celcius\r\n",temperature);
		}
		
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

