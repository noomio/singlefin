#include <http_client.h>
#include "debug.h"

char http_client_mem[HTTP_CLIENT_BYTE_POOL_SIZE];

static void http_client_cb(void* arg, int state, void* http_resp){
    
    http_client_ctx_t *ctx = (http_client_ctx_t*)arg;

    qapi_Net_HTTPc_Response_t * resp = (qapi_Net_HTTPc_Response_t *)http_resp;

	TX_DEBUGF(HTTP_CLIENT_DBG,("%p,state=%d,len=%u,code=%u\r\n",arg,state,resp->length,resp->resp_Code));

    http_client_entry_t *entry = malloc(sizeof(http_client_entry_t));
    if(entry)
    	entry->code = resp->resp_Code;

    if( state == QAPI_NET_HTTPC_RX_FINISHED || state == QAPI_NET_HTTPC_RX_MORE_DATA){
    	
    	if(entry){
    		entry->data = malloc(resp->length+1); // add null
    		if(entry->data){
    			memcpy(entry->data,resp->data,resp->length);
    			entry->data[resp->length+1] = '\0';
    			entry->data_len = resp->length;
    		}

    		entry->header = malloc(resp->rsp_hdr_len+1); // add null
			if(entry->header){
				memcpy(entry->header,resp->rsp_hdr,resp->rsp_hdr_len);
				entry->header[resp->rsp_hdr_len+1] = '\0';
    			entry->header_len = resp->rsp_hdr_len;
			}

			list_add(&entry->head,&ctx->list);
    		
    	}
    	if(state == QAPI_NET_HTTPC_RX_FINISHED)
    		tx_event_flags_set(ctx->evt, HTTP_CLIENT_EVT_FINISHED, TX_OR);
    }

 
}

#define http_client_start() qapi_Net_HTTPc_Start()
#define http_client_stop() qapi_Net_HTTPc_Stop() 

#define htpp_client_set_default_config(ctx) \
do{ \
	ctx->httpc_cfg = malloc(sizeof(qapi_Net_HTTPc_Config_t)); \
	ctx->httpc_cfg->sock_options = malloc(sizeof(qapi_Net_HTTPc_Sock_Opts_t)); \
    ctx->httpc_cfg->addr_type = AF_INET; \
	ctx->httpc_cfg->sock_options_cnt = 0; \
	ctx->httpc_cfg->max_send_chunk = 2000; \
	ctx->httpc_cfg->max_send_chunk_delay_ms = 10; \
	ctx->httpc_cfg->max_send_chunk_retries = 200; \
	ctx->httpc_cfg->max_conn_poll_cnt = 15; \
	ctx->httpc_cfg->max_conn_poll_interval_ms = 500; \
	qapi_Net_HTTPc_Configure(ctx->handle, ctx->httpc_cfg); \
}while(0)


#define http_client_session_connect(ctx,url,port) qapi_Net_HTTPc_Connect(ctx->handle,url, port)

#define http_client_session_disconnect(ctx) \
do{ \
	if(qapi_Net_HTTPc_Disconnect(ctx->handle) == QAPI_OK){  \
		TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_disconnect\r\n")); \
	} \
}while(0)

#define http_client_session_free(ctx) \
do{ \
	qapi_Net_HTTPc_Free_sess(ctx->byte_pool); \
	txm_module_object_deallocate(&ctx->byte_pool); \
	tx_byte_pool_delete(&ctx->byte_pool); \
}while(0)


static void *http_client_new_session(http_client_ctx_t *ctx, uint32_t t, uint32_t blen, uint32_t hlen){
	qapi_HTTPc_CB_t cb;
	cb = (ctx->user_callback != NULL) ? ctx->user_callback : http_client_cb;
	ctx->handle = qapi_Net_HTTPc_New_sess(t, NULL, cb, ctx, blen, hlen); 
	TX_ASSERT("http_client: session ctx->handle != NULL\r\n",(ctx->handle != NULL)); 
	if(ctx->handle){ 
		htpp_client_set_default_config(ctx);
		qapi_Net_HTTPc_Pass_Pool_Ptr(ctx->handle, ctx->byte_pool);
	} 
	return ctx->handle; 
}


http_client_ctx_t *htpp_client_new(void){
	http_client_start();
	http_client_ctx_t *ctx = malloc(sizeof(http_client_ctx_t));
	txm_module_object_allocate(&ctx->byte_pool, sizeof(TX_BYTE_POOL)); 
	tx_byte_pool_create(ctx->byte_pool, "http_client_mem", http_client_mem, sizeof(http_client_mem)); 
	txm_module_object_allocate(&ctx->evt, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(ctx->evt, "http_client evt");
	init_list_head(&ctx->list);
	return ctx;
}

int htpp_client_free(http_client_ctx_t *ctx){
	tx_byte_pool_delete(ctx->byte_pool);
	txm_module_object_deallocate(&ctx->byte_pool);
	tx_event_flags_delete(ctx->evt);
	txm_module_object_deallocate(&ctx->evt);
	free(ctx->httpc_cfg->sock_options);
	free(ctx->httpc_cfg);
	free(ctx);
	http_client_stop();
	return 0;
}


int htpp_client_set_header(http_client_ctx_t *ctx, const char *key, const char *value){
	if(ctx && ctx->handle)
		return qapi_Net_HTTPc_Add_Header_Field(ctx->handle, key, value);
	else
		return 1;
}

int htpp_client_set_parameter(http_client_ctx_t *ctx, const char *key, const char *value){
	if(ctx && ctx->handle)
		return qapi_Net_HTTPc_Set_Param(ctx->handle, key, value);
	else
		return 1;
}

int htpp_client_set_body(http_client_ctx_t *ctx, const char *body, uint32_t len){
	if(ctx && ctx->handle)
		return qapi_Net_HTTPc_Set_Body(ctx->handle, body, len);
	else
		return 1;

}



int htpp_client_get(http_client_ctx_t *ctx, const char *host, int port, const char *path){


	int err = 1;
	char url[128];

	snprintf(url,sizeof(url),"http://%s/%s",host,path);
	TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_get: url=%s port=%u \r\n",url,port));

	if(http_client_new_session(ctx,HTTP_CLIENT_TIMEOUT,HTTP_CLIENT_BODY_LEN,HTTP_CLIENT_HEADER_LEN) != NULL){
		if(http_client_session_connect(ctx,host,port) == QAPI_OK){
			TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_connected\r\n")); 
			if(qapi_Net_HTTPc_Request(ctx->handle,QAPI_NET_HTTP_CLIENT_GET_E, url) == QAPI_OK){
				TX_DEBUGF(HTTP_CLIENT_DBG,("http client request successful\r\n")); 
				// wait
				uint32_t signal = 0; 
				tx_event_flags_get(ctx->evt, HTTP_CLIENT_EVT_FINISHED, TX_OR_CLEAR, &signal, HTTP_CLIENT_TIMEOUT);
				http_client_session_disconnect(ctx);
				err = 0;
			}
		}
		http_client_session_free(ctx);
	}

	qapi_Net_HTTPc_Clear_Header(ctx->handle);	
	return err;	

}


