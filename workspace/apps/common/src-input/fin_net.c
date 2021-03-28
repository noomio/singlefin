#include "fin_internal.h"


const char *fin_resolve_host_itf(const char *domain, const char *itf, char *buf, size_t buf_len){

	struct ip46addr ipaddr;
	int ret;
	if (!qapi_Net_DNSc_Is_Started())
	{
		printf("DNS client is not started yet.\n");
		return NULL;
	}
	memset(&ipaddr, 0, sizeof(ipaddr));
	/*If ipaddr.type = AF_INET, resolve to an IPv4 address
	If ipaddr.type = AF_INET6, resolve to an IPv6 address */
	ipaddr.type = AF_INET;
	if(buf){
		memset(buf,0,buf_len);
		if(qapi_Net_DNSc_Reshost_on_iface(domain, &ipaddr, itf) == QAPI_OK){
			return inet_ntop(AF_INET, &ipaddr.a, buf, buf_len);
		}
	}

	return NULL;

}