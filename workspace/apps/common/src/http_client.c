#include <http_client.h>

static bool http_client_started = false; // one internal qapi client


#define is_http_client_start() \
do{	\
	if(!http_client_started){ \
		if(qapi_Net_HTTPc_Start() == QAPI_OK){ \
			http_client_started = true; \
		} \
	} \
}while(0) 

#define http_client_stop(ctx) \
do{	\
	if(http_client_started){ \
		if(qapi_Net_HTTPc_Stop() == QAPI_OK){ \
			http_client_started = false; \
		} \
	} \
}while(0) 

#define http_client_new_session(ctx,t,blen,hlen) \
do{ \
	ctx->handle = qapi_Net_HTTPc_New_sess(t, 0, NULL, NULL, blen, hlen); \
	if(ctx->handle){ \
		txm_module_object_allocate(&ctx->byte_pool, sizeof(TX_BYTE_POOL)); \
 		tx_byte_pool_create(ctx->byte_pool, "ctx mem", &ctx->mem, HTTP_CLIENT_BYTE_POOL_SIZE); \
		if(qapi_Net_HTTPc_Pass_Pool_Ptr(ctx->handle, http_client_mem) != QAPI_OK) \
			return NULL; \
	} \
	return ctx->handle; \
} while(0);

#define http_client_session_connect(ctx,url,port) \
do{ \
	return qapi_Net_HTTPc_Connect(ctx.handle,url, port); \
}while(0);

#define http_client_session_disconnect(ctx) \
do{ \
	if(qapi_Net_HTTPc_Disconnect(ctx->handle) == QAPI_OK){  \
		qapi_Net_HTTPc_Free_sess(ctx->byte_pool); \
		txm_module_object_deallocate(&ctx->byte_pool); \
		tx_byte_pool_delete(&ctx->byte_pool); \
		free(ctx->mem); \
	} \
}while(0);


http_client_ctx_t *htpp_client_new(void){
	return malloc(sizeof(http_client_ctx_t));
}

int htpp_client_free(http_client_ctx_t *ctx){
	free(ctx);
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

	http_client_start();

	if(http_client_new_session(ctx,HTTP_CLIENT_TIMEOUT,HTTP_CLIENT_BODY_LEN,HTTP_CLIENT_HEADER_LEN) != NULL){
		if(http_client_session_connect(ctx,host,port)){
			qapi_Net_HTTPc_Request(ctx->handle,QAPI_NET_HTTP_CLIENT_GET_E, path);
			http_client_session_disconnect(ctx);
		}
		qapi_Net_HTTPc_Clear_Header(ctx->handle);
	}		

}
