
#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include <stdio.h>
#include "qapi_httpc.h"
#include "qapi_socket.h"
#include "qapi_netservices.h"
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_CLIENT_SESSIONS_MAX	1
#define HTTP_CLIENT_BYTE_POOL_SIZE		2048*HTTP_CLIENT_SESSIONS_MAX
#define HTTP_CLIENT_BODY_LEN	1024
#define HTTP_CLIENT_HEADER_LEN	512
#define HTTP_CLIENT_TIMEOUT		10000 // secs

#define HTTP_CLIENT_EVT_FINISHED (1 >> 0)
  /**< HTTP response -- response was received completely. */
#define HTTP_CLIENT_EVT_ERROR_SERVER_CLOSED (1 >> 1)
  /**< HTTP response error -- the server closed the connection. */
#define HTTP_CLIENT_EVT_ERROR_RX_PROCESS (1 >> 2)
  /**< HTTP response error -- response is processing. */
#define HTTP_CLIENT_EVT_ERROR_RX_HTTP_HEADER (1 >> 3)
  /**< HTTP response error -- header is processing. */
#define HTTP_CLIENT_EVT_ERROR_INVALID_RESPONSECODE (1 >> 4)
  /**< HTTP response error -- invalid response code. */
#define HTTP_CLIENT_EVT_ERROR_CLIENT_TIMEOUT (1 >> 5)
  /**< HTTP response error -- timeout waiting for a response. */
#define HTTP_CLIENT_EVT_ERROR_NO_BUFFER (1 >> 6)
  /**< HTTP response error -- memory is unavailable. */
#define HTTP_CLIENT_EVT_CONNECTION_CLOSED (1 >> 7)
  /**< HTTP response -- connection is closed. */
#define HTTP_CLIENT_EVT_ERROR_CONNECTION_CLOSED (1 >> 8)
  /**< HTTP response error -- connection is closed. */
#define HTTP_CLIENT_EVT_MORE_DATA (1 >> 9)

typedef struct http_entry_list {
	struct list_head head;
	char *data;
	uint32_t data_len;
	char *header;
	uint32_t header_len;
	uint32_t code;
} http_client_entry_t;

typedef struct http_client_ssl {
	qapi_Net_SSL_Obj_Hdl_t ctx; 
	qapi_Net_SSL_Con_Hdl_t con; 
	qapi_Net_SSL_Config_t config; 
	qapi_Net_SSL_Role_t role;
} http_client_ssl_t;

typedef struct http_client_certificates{
	qapi_Net_SSL_Cert_Info_t cert_info;
	qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];
} http_client_certificates_t;

typedef struct http_client_ctx{
	qapi_Net_HTTPc_handle_t handle;
	TX_BYTE_POOL *byte_pool;
	qapi_Net_HTTPc_Config_t *httpc_cfg;
	struct linger so_linger;
	TX_EVENT_FLAGS_GROUP *evt;
	struct list_head list;
	qapi_HTTPc_CB_t user_callback;
	uint32_t timeout;
	http_client_ssl_t ssl;
	bool use_https;
} http_client_ctx_t;

#define htpp_client_set_user_callback(ctx,cb) do{ ctx->user_callback = cb } while(0)
#define http_client_for_each(p,ctx) list_for_each(p,&ctx->list)
#define htpp_client_has_data(ctx) list_first_entry_or_null(&ctx->list, http_client_entry_t,head)
#define http_client_get_data(iter) list_entry(iter,http_client_entry_t, head )->data
#define http_client_get_data_length(iter) (uint32_t)list_entry(iter,http_client_entry_t, head )->data_len
#define http_client_get_header(iter) list_entry(iter,http_client_entry_t, head )->header
#define http_client_get_header_length(iter) (uint32_t)list_entry(iter,http_client_entry_t, head )->header_len
#define http_client_free_resource(iter,data)	\
do{ \
	list_del(iter); \
	free(data); \
}while (0)

#define http_client_set_timeout(ctx, t) do{ if(ctx){ ctx->timeout = t; } } while(0)


http_client_ctx_t *htpp_client_new(void);
int htpp_client_free(http_client_ctx_t *ctx);
int htpp_client_set_header(http_client_ctx_t *ctx, const char *key, const char *value);
int htpp_client_set_parameter(http_client_ctx_t *ctx, const char *key, const char *value);
int htpp_client_set_body(http_client_ctx_t *ctx, const char *body, uint32_t len);
int htpp_client_get(http_client_ctx_t *ctx, const char *host, int port, const char *path);


#ifdef __cplusplus
}
#endif

#endif 

