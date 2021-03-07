#include <stdio.h>
#include <qapi_timer.h>
#include "dss.h"
#include "http_client.h"
#include "net.h"

int main(int argc, char * argv[])
{

	puts("http_client\r\n");
	
	dump_net_interfaces();
	dss_ctx_t *dss_ctx = dss_new("mdata.net.au",NULL,NULL);
	dss_start(dss_ctx);

	printf("waiting for network\r\n");
	dss_wait_conn_notify(dss_ctx,10000);

	http_client_ctx_t *ctx = htpp_client_new();
	htpp_client_get(ctx, "13.237.19.148", 80, "test.html");

	struct list_head *iter;
	http_client_for_each(iter,ctx){
		char *data = (char*)http_client_get_data(iter);
		char *header = (char*)http_client_get_header(iter);
		if(data)
			printf("%s",data);
		http_client_free_resource(iter, data);

		if(header)
			printf("%s",header);
		http_client_free_resource(iter, header);
	}

	for(;;){
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
	}

	return 1;
}

