#include "singlefin.h"

int main(int argc, char * argv[])
{

	puts("http-client-test\r\n");
	

	fin_dss_ctx_t *dss_ctx = fin_dss_new("mdata.net.au",NULL,NULL);
	fin_dss_start(dss_ctx);

	printf("waiting for network\r\n");
	fin_dss_wait_conn_notify(dss_ctx,10000);
	fin_dump_net_interfaces();
	fin_dump_dns_server_list();


	const char *ip = fin_resolve_host("api.algoexplorer.io",rmnet_data0);
	printf("resolved ip: %s\r\n",ip);

	fin_http_client_ctx_t *ctx = fin_htpp_client_new();
	fin_htpp_client_get(ctx, "https://api.algoexplorer.io", 443, "health");

	struct list_head *iter;
	fin_http_client_for_each(iter,ctx){
		char *data = (char*)fin_http_client_get_data(iter);
		char *header = (char*)fin_http_client_get_header(iter);
		if(data){
			printf("len=%d\r\n",fin_http_client_get_data_length(iter));
			printf("%s",data);
		}
		fin_http_client_free_resource(iter, data);

		if(header){
			printf("len=%d\r\n",fin_http_client_get_header_length(iter));
			printf("%s",header);
		}
		fin_http_client_free_resource(iter, header);
	}

	for(;;){
		fin_sleep(1000);
	}

	return 1;
}

