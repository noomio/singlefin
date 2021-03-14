#include <stdio.h>
#include "dss.h"
#include "http_client.h"
#include "net.h"
#include "sleep.h"

int main(int argc, char * argv[])
{

	puts("http_client\r\n");
	

	dss_ctx_t *dss_ctx = dss_new("mdata.net.au",NULL,NULL);
	dss_start(dss_ctx);

	printf("waiting for network\r\n");
	dss_wait_conn_notify(dss_ctx,10000);
	dump_net_interfaces();
	dump_dns_server_list();


	const char *ip = resolve_host("api.algoexplorer.io",rmnet_data0);
	printf("resolved ip: %s\r\n",ip);

	http_client_ctx_t *ctx = htpp_client_new();
	http_client_set_sni(ctx,"api.algoexplorer.io");
	htpp_client_get(ctx, "https://api.algoexplorer.io", 443, "health");

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
		sleep(1000);
	}

	return 1;
}

