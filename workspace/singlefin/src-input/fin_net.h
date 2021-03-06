#if !defined(FIN_API_NET_H_INCLUDED)
#define FIN_API_NET_H_INCLUDED

#define fin_dump_net_interfaces() \
do{ \
	qapi_Net_Ifnameindex_t itf[10]; \
	int itf_no = qapi_Net_Get_All_Ifnames (&itf); \
	for(int i=0; i < itf_no; i++){ \
		const char * address = 0; \
		uint32_t address_length = 0; \
		printf("Interface = %d, %s, %d\r\n",i,itf[i].interface_Name,itf[i].if_Is_Up); \
       	if(qapi_Net_Interface_Get_Physical_Address(itf[i].interface_Name, &address, &address_length) == 0) \
       		printf("physical address = %c.%c.%c.%c\r\n",address[0],address[1],address[2],address[3]); \
	} \
}while(0); \

#define fin_dump_dns_server_list() \
do { \
	qapi_Net_DNS_Server_List_t svr_list; \
	/* iface_index can be 0,1,2,or 3 */ \
	if (qapi_Net_DNSc_Get_Server_List(&svr_list,0) == 0){ \
		int i; \
		char ip_str[48]; \
		for (i = 0; i < MAX_DNS_SVR_NUM; ++i){ \
			if (svr_list.svr[i].type != AF_UNSPEC){ \
				printf("DNS Server: %s\r\n",	inet_ntop(svr_list.svr[i].type,	&svr_list.svr[i].a, ip_str, sizeof(ip_str))); \
			} \
		} \
	} \
} while(0)

const char *fin_resolve_host_itf(const char *domain, const char *itf, char *buf, size_t buf_len);

#define fin_resolve_host(domain, itf) \
({char itf##_ip_str[48]; \
	fin_resolve_host_itf(domain, #itf, itf##_ip_str, sizeof(itf##_ip_str)); \
    itf##_ip_str;\
})

#endif  /* FIN_API_NET_H_INCLUDED */