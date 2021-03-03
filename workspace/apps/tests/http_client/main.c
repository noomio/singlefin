#include <stdio.h>
#include <qapi_timer.h>
#include "dss.h"
#include "http_client.h"

int main(int argc, char * argv[])
{

	puts("http_client\r\n");
	
	qapi_Net_Ifnameindex_t itf[10];
	int itf_no = qapi_Net_Get_All_Ifnames (&itf);

	for(int i=0; i < itf_no; i++){
		const char * address = 0;
		uint32_t address_length = 0;
		printf("interface = %d, %s, %d\r\n",i,itf[i].interface_Name,itf[i].if_Is_Up);
       	if(qapi_Net_Interface_Get_Physical_Address(itf[i].interface_Name, &address, &address_length) == 0)
       		printf("physical address = %c.%c.%c.%c\r\n",address[0],address[1],address[2],address[3]);
	}

	dss_ctx_t *dss_ctx = dss_new("mdata.net.au",NULL,NULL);
	dss_start(dss_ctx);
	dss_wait_conn_notify(dss_ctx);

	for(;;){
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
	}

	return 1;
}

