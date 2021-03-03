#include "qapi_fs_types.h"
#include "qapi_status.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"

#include "qapi_timer.h"

#include "debug.h"
#include "httpd.h"
#include "dss.h"



int main(void){

	qapi_Net_Ifnameindex_t itf[10];
	int itf_no = qapi_Net_Get_All_Ifnames (&itf);

	for(int i=0; i < itf_no; i++){
		const char * address = 0;
		uint32_t address_length = 0;
		TX_DEBUGF(ITF_DBG,("interface = %d, %s, %d\r\n",i,itf[i].interface_Name,itf[i].if_Is_Up));
       	if(qapi_Net_Interface_Get_Physical_Address(itf[i].interface_Name, &address, &address_length) == 0)
       		TX_DEBUGF(ITF_DBG,("physical address = %c.%c.%c.%c\r\n",address[0],address[1],address[2],address[3]));
	}

	httpd_t *httpd_ctx = httpd_new(DSS_NET_IP,9999);
	dss_ctx_t *dss_ctx = dss_new("telstra.internet",NULL,NULL);
	dss_set_notify(dss_ctx,httpd_ctx);
	dss_start(dss_ctx);

	httpd_start(httpd_ctx);

	return 1;

}


