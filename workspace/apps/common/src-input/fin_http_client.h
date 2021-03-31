#if !defined(FIN_API_HTTP_H_INCLUDED)
#define FIN_API_HTTP_H_INCLUDED

#define HTTP_CLIENT_SESSIONS_MAX	1
#define HTTP_CLIENT_BYTE_POOL_SIZE		2048*HTTP_CLIENT_SESSIONS_MAX
#define HTTP_CLIENT_BODY_LEN_MAX		1024
#define HTTP_CLIENT_HEADER_LEN_MAX	512
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




#endif  /* FIN_API_HTTP_H_INCLUDED */