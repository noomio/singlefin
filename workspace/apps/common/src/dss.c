/*
 * dss.c
 *
 *  Created on: 1 Oct.,2019
 *      Author: Nikolas Karakotas
 */

#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "qapi_fs_types.h"
#include "qapi_status.h"
#include "qapi_dss.h"
#include "qapi_netservices.h"
#include "qapi_timer.h"
#include "debug.h"
#include "dss.h"


 /* Mask include all DAM signals */
#define DSS_OBJ_SIGNAL_MASK       QAPI_DSS_EVT_NET_IS_CONN_E | \
                                  QAPI_DSS_EVT_NET_NO_NET_E  \

#define DSS_DEVICE_MASK_SET			0xEF
#define DSS_DEVICE_SIGNAL_MASK		(QAPI_DEVICE_INFO_NETWORK_IND_E | DSS_DEVICE_MASK_SET) \
                                  	(QAPI_DEVICE_INFO_SIM_STATE_E | DSS_DEVICE_MASK_SET)
/*
*	TASK
*/
#define THREAD_PRIORITY      180



#define DSS_THREAD_STACK_SIZE			(1024 * 16)
#define DSS_THREAD_BYTE_POOL_SIZE		DSS_THREAD_STACK_SIZE * DSS_CONCURRENT_NUM
char dss_mem[DSS_THREAD_BYTE_POOL_SIZE];


dss_ctx_t *dss_ctx_store[DSS_CONCURRENT_NUM] = {0};

/*
@func
	dss_net_event_cb
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static void dss_net_event_cb(qapi_DSS_Hndl_t hndl, void *user_data,	qapi_DSS_Net_Evt_t evt,	qapi_DSS_Evt_Payload_t *payload_ptr ){
	
	switch (evt)
	{
		case QAPI_DSS_EVT_NET_IS_CONN_E:
		case QAPI_DSS_EVT_NET_NO_NET_E:
		default:	
			if(user_data != NULL)
				tx_event_flags_set(user_data, evt, TX_OR);
			break;
	}
}


void device_info_cb(qapi_Device_Info_Hndl_t device_info_hndl, const qapi_Device_Info_t *dev_info){

	TX_EVENT_FLAGS_GROUP *evt = NULL;
	for(int i = 0; i < DSS_CONCURRENT_NUM; i++){
		if(dss_ctx_store[i] != NULL){
			if(dss_ctx_store[i]->device_info_handle == device_info_hndl){
				evt = dss_ctx_store[i]->nw_event_handle;
			}
		}
	}

	if(dev_info->id == QAPI_DEVICE_INFO_NETWORK_IND_E)
	{
		switch(dev_info->info_type)
		{
			case QAPI_DEVICE_INFO_TYPE_BOOLEAN_E:
				//unsafe TX_DEBUGF(DSS_DBG,("%c: %d\r\n", dev_info->id, dev_info->u.valuebool));
				if(dev_info->u.valuebool == true){
					if(evt != NULL){
						tx_event_flags_set(evt, QAPI_DEVICE_INFO_NETWORK_IND_E, TX_OR); 
					}
				}
			break;

			case QAPI_DEVICE_INFO_TYPE_INTEGER_E:
				//unsafe TX_DEBUGF(DSS_DBG,("%c: %lld\r\n", dev_info->id, dev_info->u.valueint));
			break;

			case QAPI_DEVICE_INFO_TYPE_BUFFER_E:
				//unsafe TX_DEBUGF(DSS_DBG,("%c: %s\r\n", dev_info->id, dev_info->u.valuebuf.buf));
			default:
				//unsafe TX_DEBUGF(DSS_DBG,("%c: %s\r\n", dev_info->id, "NULL"));
			break;

		}

	}


}


int dss_show_addr_info(qapi_DSS_Addr_Info_t *info, size_t size){

	TX_ERROR(("dss_show_addr_info: info != NULL\r\n"),(info != NULL), return -1);
	TX_ASSERT("size != 0\r\n",(size != 0));

	char addr[DSS_ADDR_SIZE];

	for (int i = 0; i < size; i++){
		inet_ntop(AF_INET,&info[i].iface_addr_s, addr, DSS_ADDR_SIZE);
		TX_DEBUGF(DSS_DBG,("dss_show_addr_info: IPv4: %s\r\n", addr));
		//memcpy(server_ip, buff, sizeof(server_ip));

		inet_ntop(AF_INET,&info[i].gtwy_addr_s, addr, DSS_ADDR_SIZE);
		TX_DEBUGF(DSS_DBG,("dss_show_addr_info: gateway IPv4: %s\r\n", addr));

		inet_ntop(AF_INET,&info[i].dnsp_addr_s, addr, DSS_ADDR_SIZE);
		TX_DEBUGF(DSS_DBG,("dss_show_addr_info: primary DNS IPv4: %s\r\n", addr));

		inet_ntop(AF_INET,&info[i].dnss_addr_s, addr, DSS_ADDR_SIZE);
		TX_DEBUGF(DSS_DBG,("dss_show_addr_info: second DNS IPv4: %s\r\n", addr));
	}

	return 0;

}

int dss_get_addr_info(qapi_DSS_Hndl_t handle, qapi_DSS_Addr_Info_t *info, size_t size){

	unsigned int len = 0;	
	qapi_Status_t status;


	TX_ERROR(("dss_get_addr_info: handle != NULL\r\n"),(handle != NULL), return -1);
	TX_ERROR(("dss_get_addr_info: info != NULL\r\n"),(info != NULL), return -1);

	status = qapi_DSS_Get_IP_Addr_Count(handle, &len);
	TX_ERROR(("dss_get_addr_info: get IP address count error\r\n"),(status != QAPI_ERROR), return -1);

	TX_ASSERT("size > len", (size > len));
	status = qapi_DSS_Get_IP_Addr(handle, info, size);
	TX_ERROR(("dss_get_addr_info: get IP address error\r\n"),(status != QAPI_ERROR), return -1);
	if(len>size) len = size;
	return len;
}


int dss_set_data_parameters(dss_ctx_t *ctx){
    
    qapi_DSS_Call_Param_Value_t param_info;
	qapi_Status_t status;

	
	/* Initial Data Call Parameter */
	TX_ERROR(("dss_set_data_parameters: ctx->dss_handle != NULL\r\n"),(ctx->dss_handle != NULL), return -1);

    /* set data call param */
    param_info.buf_val = NULL;
    param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;	//Automatic mode(or DSS_RADIO_TECH_LTE)
    TX_DEBUGF(DSS_DBG,("dss: setting tech to Automatic\r\n"));
    status = qapi_DSS_Set_Data_Call_Param(ctx->dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);
	TX_ASSERT("status == QAPI_DSS_SUCCESSL\r\n", (status == QAPI_DSS_SUCCESS));

	/* set apn */
	TX_ASSERT("apn != NULL\r\n", (ctx->apn != NULL));
	if(ctx->apn[0] != '\0'){
	    param_info.buf_val = ctx->apn;
	    param_info.num_val = strlen(ctx->apn);
	    TX_DEBUGF(DSS_DBG,("dss: setting APN to '%s'\r\n", ctx->apn));
	    status = qapi_DSS_Set_Data_Call_Param(ctx->dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);
		TX_ASSERT("status == QAPI_DSS_SUCCESS\r\n", (status == QAPI_DSS_SUCCESS));
	}
	/* set apn username */
	//TX_ASSERT("username != NULL", (ctx->username != NULL));
	if(ctx->username[0] != '\0'){
		param_info.buf_val = ctx->username;
	    param_info.num_val = strlen(ctx->username);
	    TX_DEBUGF(DSS_DBG,("dss: setting APN user name to '%s'\r\n", ctx->username));
	    status = qapi_DSS_Set_Data_Call_Param(ctx->dss_handle, QAPI_DSS_CALL_INFO_USERNAME_E, &param_info);
		TX_ASSERT("status == QAPI_DSS_SUCCESS\r\n", (status == QAPI_DSS_SUCCESS));
	}
	/* set apn password */
	//TX_ASSERT("password != NULL", (ctx->password != NULL));
	if(ctx->password[0] != '\0'){
		param_info.buf_val = ctx->password;
	    param_info.num_val = strlen(ctx->password);
	    TX_DEBUGF(DSS_DBG,("dss: setting APN password to '%s'\r\n", ctx->password));
	    status = qapi_DSS_Set_Data_Call_Param(ctx->dss_handle, QAPI_DSS_CALL_INFO_PASSWORD_E, &param_info);
		TX_ASSERT("status == QAPI_DSS_SUCCESS\r\n", (status == QAPI_DSS_SUCCESS));
	}

	/* set IP version(IPv4 or IPv6) */
    param_info.buf_val = NULL;
    param_info.num_val = QAPI_DSS_IP_VERSION_4;
    TX_DEBUGF(DSS_DBG,("dss: setting family to IPv%d\r\n", param_info.num_val));
    status = qapi_DSS_Set_Data_Call_Param(ctx->dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);
	TX_ASSERT("status == QAPI_DSS_SUCCESS\r\n", (status == QAPI_DSS_SUCCESS));

    return 0;

}



static qapi_Status_t dss_net_ctrl_start(dss_ctx_t *ctx)
{
	qapi_Status_t status;
	uint32_t received_sigs = 0;

	TX_ERROR(("dss_net_ctrl_start: ctx->dss_handle == NULL\r\n"),(ctx->dss_handle == NULL), return QAPI_ERROR);
		
	do{

		qapi_DSS_Release(QAPI_DSS_MODE_GENERAL);
		/*
			as per doc: clients are asked to retry this function call repeatedly using a 500 ms timeout interval. 
		*/
		qapi_Timer_Sleep(1000, QAPI_TIMER_UNIT_MSEC, true);

		status = qapi_DSS_Init(QAPI_DSS_MODE_GENERAL);

		TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: dss initialised=%d\r\n",!status));
		
		TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: get service handler...\r\n"));
		
		status = qapi_DSS_Get_Data_Srvc_Hndl(dss_net_event_cb, ctx->dss_event_handle, &ctx->dss_handle);
		TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: dss_handle=%p, status=%d\r\n", ctx->dss_handle, status));
		TX_ASSERT("ctx->dss_handle != NULL\r\n", (ctx->dss_handle != NULL));

	} while(ctx->dss_handle == NULL);


	TX_ERROR(("dss_net_ctrl_start: dss data service obtain handler failed!\r\n"),(status == QAPI_OK), return QAPI_ERROR;);
	TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: dss data service obtain handler completed!\r\n"));

	dss_set_data_parameters(ctx);

	status = qapi_Device_Info_Init_v2(&ctx->device_info_handle);
	TX_ASSERT("status == QAPI_OK\r\n", (status == QAPI_OK));
	status = qapi_Device_Info_Pass_Pool_Ptr(ctx->device_info_handle, ctx->thread_byte_pool);
	TX_ASSERT("status == QAPI_OK\r\n", (status == QAPI_OK));

	
	tx_event_flags_set(ctx->nw_event_handle, 0x0, TX_AND);
	status = qapi_Device_Info_Set_Callback_v2(ctx->device_info_handle,QAPI_DEVICE_INFO_NETWORK_IND_E,device_info_cb);
	TX_ASSERT("status == QAPI_OK\r\n", (status == QAPI_OK));
	/* wait to register on the network - if APN not set i.e left empty it will not register! */
	tx_event_flags_get(ctx->nw_event_handle, QAPI_DEVICE_INFO_NETWORK_IND_E, TX_OR_CLEAR, &received_sigs, TX_WAIT_FOREVER);


#if 0
	qapi_Device_Info_t info;
	memset(&info,0,sizeof(info));

	do{
		status = qapi_Device_Info_Get_v2(ctx->device_info_handle,QAPI_DEVICE_INFO_NETWORK_IND_E,&info);
		TX_ASSERT("status == QAPI_OK\n", (status == QAPI_OK));
		//if(info.u.valuebool == true)
		//	tx_event_flags_set(ctx->nw_event_handle, QAPI_DEVICE_INFO_NETWORK_IND_E, TX_OR); 
	}while(status == QAPI_OK && info.u.valuebool != true);
#endif

	TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: starting data call -> %d\r\n",status));
	status = qapi_DSS_Start_Data_Call(ctx->dss_handle);

	TX_DEBUGF(DSS_DBG,("dss_net_ctrl_start: start data call -> %d\r\n",!status));

	return status;

}


int dss_net_ctrl_stop(qapi_DSS_Hndl_t handle){
	qapi_Status_t stat;

	TX_ERROR(("dss_set_data_parameters: handle != NULL\r\n"),(handle != NULL), return -1);

	stat = qapi_DSS_Stop_Data_Call(handle);
	TX_DEBUGF(DSS_DBG,("dss_net_ctrl_stop: %d\r\n",stat));

	return stat;
}	

void dss_net_no_net(qapi_DSS_Hndl_t hndl)
{
  qapi_DSS_CE_Reason_t ce_reason;
  
  TX_DEBUGF(DSS_DBG,("DAM_APP:Release DSS Handle \r\n"));

  memset(&ce_reason,0,sizeof(qapi_DSS_CE_Reason_t));

  /* Get call end reason */
  if(QAPI_OK == qapi_DSS_Get_Call_End_Reason(hndl, &ce_reason, QAPI_DSS_IP_FAMILY_V4_E)){
    TX_DEBUGF(DSS_DBG,("DAM_APP:Data call released, call end reason:%d \r\n", ce_reason.reason_type));    
  }

  qapi_DSS_Rel_Data_Srvc_Hndl(hndl);

}



static void dss_thread(ULONG param){

	uint32_t received_sigs = 0;
	qapi_Status_t status;
	dss_ctx_t *ctx = (dss_ctx_t*)param;
	qapi_DSS_Addr_Info_t network_info[6];
	char addr[16];
	char gw[16];
	char dnsp[16];
	char dnss[16];

	setlocale(LC_ALL, "C");

	ctx->dss_handle = NULL;
	/* Start data call */
	status = dss_net_ctrl_start(ctx);
	TX_ASSERT("dss_thread: status != QAPI_ERROR", (status != QAPI_ERROR));
	
	for (;;){

		tx_event_flags_get(ctx->dss_event_handle, DSS_OBJ_SIGNAL_MASK, TX_OR_CLEAR, &received_sigs, TX_WAIT_FOREVER);
		TX_DEBUGF(DSS_DBG,("dss_thread: evt [%d]\r\n", received_sigs));
		

		if (received_sigs & QAPI_DSS_EVT_NET_NO_NET_E){
			TX_DEBUGF(DSS_DBG,("dss_thread: QAPI_DSS_EVT_NET_NO_NET_E\r\n"));
			TX_DEBUGF(DSS_DBG,("dss_thread: Stop DNS\r\n"));
			qapi_Net_DNSc_Command(QAPI_NET_DNS_STOP_E);
			dss_net_no_net(ctx->dss_handle);
			if(ctx->notify != NULL){
				ctx->notify->addr[0] = '\0';
				tx_event_flags_set(ctx->notify->evt, QAPI_DSS_EVT_NET_NO_NET_E, TX_OR);
			}

		}
		else if (received_sigs & QAPI_DSS_EVT_NET_IS_CONN_E){
			TX_DEBUGF(DSS_DBG,("dss_thread: QAPI_DSS_EVT_NET_IS_CONN_E\r\n"));
			TX_DEBUGF(DSS_DBG,("dss_thread: Start DNS\r\n"));
			int dns = qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
			TX_ASSERT("dss_thread: dns != -1", (status != -1));
			int len = 0;
			if((len = dss_get_addr_info(ctx->dss_handle,&network_info[0],sizeof(network_info))) > 0){
				for(int i=0; i < len; i++){
					const char *ip_p = inet_ntop(AF_INET,&network_info[i].iface_addr_s.addr.v4,addr,sizeof(addr));
					if(ip_p != NULL && ctx->notify != NULL){
						strncpy(ctx->notify->addr,ip_p,strlen(ip_p));
						tx_event_flags_set(ctx->notify->evt, QAPI_DSS_EVT_NET_IS_CONN_E, TX_OR);
					}
					const char *gw_p = inet_ntop(AF_INET,&network_info[i].gtwy_addr_s.addr.v4,gw,sizeof(gw));
					TX_DEBUGF(DSS_DBG,("dss_thread: addr= %s, gw= %s\r\n",ip_p,gw_p));
					
					const char *dnsp_p = inet_ntop(AF_INET,&network_info[i].dnsp_addr_s.addr.v4,dnsp,sizeof(dnsp));
					const char *dnss_p = inet_ntop(AF_INET,&network_info[i].dnss_addr_s.addr.v4,dnss,sizeof(dnss));
					TX_DEBUGF(DSS_DBG,("dss_thread: dnsp= %s, dnss= %s\r\n",dnsp_p,dnss_p));

				}
			}

		}else{
			TX_DEBUGF(DSS_DBG,("dss_thread: unknown signal\r\n"));
		}


	}


}



int dss_free(dss_ctx_t *ctx){

	// free all notify events even if they are local.
	return 0;
}

static dss_notify_t dss_notify_local;

int dss_start(dss_ctx_t *ctx){

	TX_ERROR(("dss_start: ctx != NULL\r\n"),(ctx != NULL), return -1);

	TX_DEBUGF(DSS_DBG,("dss_start: notify: %p\r\n",ctx->notify));

	if(ctx->notify == NULL){ // create local notify flag to be used with macros
		ctx->notify = &dss_notify_local;
		memset(ctx->notify->addr,'\0',DSS_ADDR_SIZE);
		txm_module_object_allocate(&ctx->notify->evt, sizeof(TX_EVENT_FLAGS_GROUP));
		tx_event_flags_create(ctx->notify->evt, "dss_local_notify");
		TX_DEBUGF(DSS_DBG,("dss_start: local notify evt: %p\r\n",ctx->notify->evt));
	}

	tx_thread_resume(ctx->thread_handle);

	return 0;

}

int dss_stop(dss_ctx_t *ctx){
	return 0;
}

int dss_restart(dss_ctx_t *ctx){
	return 0;
}

dss_ctx_t *dss_new(const char *apn,const char *password, const char *username){

	int ret;

	dss_ctx_t *ctx = malloc(sizeof(dss_ctx_t));
	memset(ctx,'\0',sizeof(dss_ctx_t));

	if(apn != NULL) strncpy(ctx->apn,apn,QAPI_DSS_CALL_INFO_APN_MAX_LEN-1);
	if(password != NULL) strncpy(ctx->password,password,QAPI_DSS_CALL_INFO_PASSWORD_MAX_LEN-1);
	if(username != NULL) strncpy(ctx->username,username,QAPI_DSS_CALL_INFO_USERNAME_MAX_LEN-1);

	TX_DEBUGF(DSS_DBG,("dss_new: apn = %s, password=%s, username=%s,evt=%p\r\n",ctx->apn,ctx->password,ctx->username,ctx->dss_event_handle));

	/* Create event signal handle and clear signals */
	txm_module_object_allocate(&ctx->dss_event_handle, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(ctx->dss_event_handle, "dss_event");

	txm_module_object_allocate(&ctx->nw_event_handle, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(ctx->nw_event_handle, "nw_event_handle");

	txm_module_object_allocate(&ctx->thread_byte_pool, sizeof(TX_BYTE_POOL));
 	tx_byte_pool_create(ctx->thread_byte_pool, "dss_byte_pool", dss_mem, DSS_THREAD_BYTE_POOL_SIZE);

 	/* Start DSS thread, and detect iface status */
	ret = txm_module_object_allocate((VOID *)&ctx->thread_handle, sizeof(TX_THREAD));
	TX_ASSERT("ret == TX_SUCCESS\r\n",(ret == TX_SUCCESS));
	TX_DEBUGF(DSS_DBG,("dss_start: dss_thread_handle = %p\r\n",ctx->thread_handle));
 	
 	tx_byte_allocate(ctx->thread_byte_pool, (VOID **) &ctx->thread_stack, DSS_THREAD_STACK_SIZE, TX_NO_WAIT);		
	txm_module_object_allocate(&ctx->thread_handle, sizeof(TX_THREAD));

	for(int i = 0; i < DSS_CONCURRENT_NUM; i++){
		if(dss_ctx_store[i] == NULL){
			dss_ctx_store[i] = ctx;
			TX_DEBUGF(DSS_DBG,("dss_start: ctx = %p\r\n",dss_ctx_store[i]));
			break;
		}
	}

	ret = tx_thread_create(
		ctx->thread_handle,
		"dss_thread", 
		dss_thread, 
		(ULONG)ctx,
		ctx->thread_stack, 
		DSS_THREAD_STACK_SIZE, 
		THREAD_PRIORITY, 
		THREAD_PRIORITY, 
		TX_NO_TIME_SLICE, 
		TX_DONT_START
	 );

	TX_ASSERT("ret == TX_SUCCESS\r\n",(ret == TX_SUCCESS));
	
	return ctx;

}

