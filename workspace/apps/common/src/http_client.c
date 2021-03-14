#include <http_client.h>
#include "net.h"
#include "debug.h"

char http_client_mem[HTTP_CLIENT_BYTE_POOL_SIZE];

static void http_client_cb(void* arg, int state, void* http_resp){
    
    http_client_ctx_t *ctx = (http_client_ctx_t*)arg;

    qapi_Net_HTTPc_Response_t * resp = (qapi_Net_HTTPc_Response_t *)http_resp;

	//unsafe TX_DEBUGF(HTTP_CLIENT_DBG,("%p,state=%d,len=%u,code=%u\r\n",arg,state,resp->length,resp->resp_Code));

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
    }else if(state == QAPI_NET_HTTPC_RX_ERROR_CONNECTION_CLOSED){
    	tx_event_flags_set(ctx->evt, HTTP_CLIENT_EVT_ERROR_CONNECTION_CLOSED, TX_OR);
    }else if(state == QAPI_NET_HTTPC_RX_ERROR_CLIENT_TIMEOUT){
    	tx_event_flags_set(ctx->evt, HTTP_CLIENT_EVT_ERROR_CLIENT_TIMEOUT, TX_OR);
    }

 
}

#define http_client_start() qapi_Net_HTTPc_Start()
#define http_client_stop() qapi_Net_HTTPc_Stop() 

#define htpp_client_set_default_config(ctx) \
do{ \
	memset(&ctx->so_linger, 0x00, sizeof(struct linger)); \
	ctx->so_linger.l_onoff = 1; \
	ctx->httpc_cfg = malloc(sizeof(qapi_Net_HTTPc_Config_t)); \
	ctx->httpc_cfg->sock_options = malloc(sizeof(qapi_Net_HTTPc_Sock_Opts_t)); \
    ctx->httpc_cfg->addr_type = AF_INET; \
	ctx->httpc_cfg->sock_options_cnt = 0; \
	ctx->httpc_cfg->sock_options->opt_name = SO_LINGER; \
	ctx->httpc_cfg->sock_options->opt_len = sizeof(struct linger); \
	ctx->httpc_cfg->sock_options->opt_value = &ctx->so_linger; \
	ctx->httpc_cfg->max_send_chunk = 2000; \
	ctx->httpc_cfg->max_send_chunk_delay_ms = 10; \
	ctx->httpc_cfg->max_send_chunk_retries = 200; \
	ctx->httpc_cfg->max_conn_poll_cnt = 15; \
	ctx->httpc_cfg->max_conn_poll_interval_ms = 500; \
	if(ctx->ssl.ctx != 0 &ctx->ssl.ctx != QAPI_NET_SSL_INVALID_HANDLE) \
		qapi_Net_HTTPc_Configure(ctx->handle, ctx->httpc_cfg); \
}while(0)

#define htpp_client_config_free(ctx) \
do{ free(ctx->httpc_cfg); free(ctx->httpc_cfg->sock_options); }while(0)

#define http_client_session_connect(ctx,url,port) \
qapi_Net_HTTPc_Connect(ctx->handle,url, port)

#define http_client_session_disconnect(ctx) \
do{ \
	if(qapi_Net_HTTPc_Disconnect(ctx->handle) == QAPI_OK){  \
		TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_disconnect\r\n")); \
	} \
}while(0)

#define http_client_session_free(ctx) \
do{ \
	TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_free\r\n")); \
	qapi_Net_HTTPc_Free_sess(ctx->byte_pool); \
}while(0)


static void *http_client_new_session(http_client_ctx_t *ctx, uint32_t t, uint32_t blen, uint32_t hlen){
	qapi_HTTPc_CB_t cb;
	int ret;
	cb = (ctx->user_callback != NULL) ? ctx->user_callback : http_client_cb;
	ctx->handle = qapi_Net_HTTPc_New_sess(t, ctx->ssl.ctx, cb, ctx, blen, hlen); 
	TX_ASSERT("http_client: session ctx->handle != NULL\r\n",(ctx->handle != NULL)); 
	TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session: setting up...\r\n"));
	if(ctx->handle){ 
		htpp_client_set_default_config(ctx);
		if(ctx->ssl.ctx != QAPI_NET_SSL_INVALID_HANDLE){
			if((ret = qapi_Net_HTTPc_Configure_SSL(ctx->handle, &ctx->ssl.config)) == QAPI_OK)
				TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session: ssl configured\r\n"));
			else
				TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session: ssl not configured=%d\r\n",ret));
		}
		qapi_Net_HTTPc_Pass_Pool_Ptr(ctx->handle, ctx->byte_pool);
		TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session: setup\r\n"));
	} 
	return ctx->handle; 
}

#define http_client_ssl_free(ctx) \
do{	\
	qapi_Net_SSL_Obj_Free(ctx->ssl.ctx); \
	ctx->ssl.ctx = 0; \
}while(0)

#define http_client_set_sni(ctx, domain) \
do{ \
	if(ctx){ \
		ctx->ssl.config.sni_Name=domain; \
		ctx->ssl.config.sni_Name_Size=strlen(domain); \
	} \
} while (0)

static int http_client_ssl_new(http_client_ctx_t *ctx){


	ctx->ssl.role = QAPI_NET_SSL_CLIENT_E;
	ctx->ssl.ctx = qapi_Net_SSL_Obj_New(QAPI_NET_SSL_CLIENT_E);
	TX_ASSERT("http_client_ssl_new: ctx->ssl.ctx != QAPI_NET_SSL_INVALID_HANDLE\r\n",(ctx->ssl.ctx != QAPI_NET_SSL_INVALID_HANDLE)); 

	ctx->ssl.config.cipher[0] = QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA;
	ctx->ssl.config.cipher[1] = QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA;
	ctx->ssl.config.cipher[2] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA;
	ctx->ssl.config.cipher[3] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA;
	ctx->ssl.config.cipher[4] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256;
	ctx->ssl.config.cipher[0] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA;
	ctx->ssl.config.cipher[6] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
	ctx->ssl.config.cipher[7] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
	
	ctx->ssl.config.max_Frag_Len = 4096;
	ctx->ssl.config.max_Frag_Len_Neg_Disable = 1;
	ctx->ssl.config.protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
	ctx->ssl.config.verify.domain = 0;
	ctx->ssl.config.verify.send_Alert = 0;
	ctx->ssl.config.verify.time_Validity = 0;	/* Don't check certification expiration */
	//strncpy(ctx->ssl.config.verify.match_Name,"api.algoexplorer.io",strlen("api.algoexplorer.io"));
	return 0;
}


http_client_ctx_t *htpp_client_new(void){
	http_client_start();
	http_client_ctx_t *ctx = malloc(sizeof(http_client_ctx_t));
	ctx->timeout = HTTP_CLIENT_TIMEOUT;
	ctx->ssl.ctx = 0;
	memset(&ctx->ssl, 0, sizeof(http_client_ssl_t));	
	ctx->use_https = false;
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
	const char *p_host = host;
	int ret;

	if(ctx == NULL)
		return -1;

	//printf("err=%d,%d\r\n", QAPI_ERR_SSL_CERT,QAPI_ERR_SSL_CONN);
	if(strstr(host,"https://") != NULL){
		snprintf(url,sizeof(url),"%s/%s",host,path);
		http_client_ssl_new(ctx);
		ctx->use_https = true;
	}
	else{
		snprintf(url,sizeof(url),"%s/%s",host,path);
		ctx->use_https = false;
	}


	if((p_host = strstr(host,"//")) != NULL)
		p_host += 2;

	if(ctx->use_https)
		http_client_set_sni(ctx,p_host);

	TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_get: host:%s, %s:%u \r\n",p_host,url,port));

	if(http_client_new_session(ctx,HTTP_CLIENT_TIMEOUT,HTTP_CLIENT_BODY_LEN,HTTP_CLIENT_HEADER_LEN) != NULL){
		if((ret = http_client_session_connect(ctx,p_host,port)) == QAPI_OK){
			TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_connected\r\n")); 
			if((ret = qapi_Net_HTTPc_Request(ctx->handle,QAPI_NET_HTTP_CLIENT_GET_E, url)) == QAPI_OK){
				TX_DEBUGF(HTTP_CLIENT_DBG,("http client request successful\r\n")); 
				// wait
				uint32_t signal = 0; 
				tx_event_flags_get(ctx->evt, HTTP_CLIENT_EVT_FINISHED | HTTP_CLIENT_EVT_ERROR_CONNECTION_CLOSED, TX_OR_CLEAR, &signal, ctx->timeout);
				TX_DEBUGF(HTTP_CLIENT_DBG,("http client evt:%d\r\n",signal)); 
				http_client_session_disconnect(ctx);

				err = 0;
			}

		}
		TX_DEBUGF(HTTP_CLIENT_DBG,("http_client_session_connect: ret=%d\r\n",ret)); 
		
		if(ctx->use_https)
			http_client_ssl_free(ctx);

		htpp_client_config_free(ctx);
		http_client_session_free(ctx);
	}

	qapi_Net_HTTPc_Clear_Header(ctx->handle);	
	return err;	

}


