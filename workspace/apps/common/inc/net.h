#ifndef __920X_NET_H__
#define __920X_NET_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define dump_net_interfaces() \
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

char *resolve_host_itf(const char *itf, const char *domain);


#ifdef __cplusplus
}
#endif

#endif