#include <stdio.h>
#include <qapi_timer.h>
#include "spi.h"

#define CLRC663_ID      0b01010000   
#define ADDR1           (0 << 1)
#define ADDR2           (0 << 2)
#define CLRC663_ADDR    CLRC663_ID | ADDR1 | ADDR2
#define I2C_CHANNEL     0
#define I2C_HOST        1

#define CLRC663_VERSION_SUB ((1 << 4) | (0x0A))

/* Registers */
#define VERSION_REG_ADDR    0x7F


int main(int argc, char * argv[]){

	uint8_t addr = VERSION_REG_ADDR;
	uint8_t version;

	puts("spi1\r\n");

	int res = spi_config(SPI1);
	printf("spi1 config=%d\r\n",res);

	for(;;){

		res = spi_send_receive(SPI1,&addr,&version,1);

		if(res == 0 && CLRC663_VERSION_SUB == version){
			puts("CLRC663 detected\r\n");
		}else{
			puts("spi 1 error\r\n");
		}

		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

	}

	return 1;
}

