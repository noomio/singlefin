#if !defined(FIN_API_DSS_H_INCLUDED)
#define FIN_API_DSS_H_INCLUDED

#define DSS_ADDR_SIZE 48

/* Number of simultaneously dss data calls supported */
#define DSS_CONCURRENT_NUM	4

typedef const void (*fin_dss_data_call_status_cb_t)(qapi_DSS_Net_Evt_t evt);

typedef struct fin_dss_notify{
	TX_EVENT_FLAGS_GROUP *evt;
	char addr[DSS_ADDR_SIZE];
	fin_dss_data_call_status_cb_t cb;
	bool timed_out;
}fin_dss_notify_t;

#define __FIN_DSS_NOTIFY__ fin_dss_notify_t notify

typedef struct fin_dss_ctx{ 
	char apn[QAPI_DSS_CALL_INFO_APN_MAX_LEN];
	char username[QAPI_DSS_CALL_INFO_USERNAME_MAX_LEN];
	char password[QAPI_DSS_CALL_INFO_PASSWORD_MAX_LEN];
	TX_EVENT_FLAGS_GROUP *dss_event_handle;
	fin_dss_notify_t *notify;
	TX_THREAD	*thread_handle; 
	TX_BYTE_POOL *thread_byte_pool;
	void *thread_stack;
	qapi_DSS_Hndl_t dss_handle;
	struct{
		TX_EVENT_FLAGS_GROUP *nw_event_handle;
		qapi_Device_Info_Hndl_t device_info_handle;
	};
} fin_dss_ctx_t;


extern fin_dss_ctx_t *dss_ctx_store[DSS_CONCURRENT_NUM];

#define fin_dss_set_notify(ctx, tobenotified) do { (ctx)->notify = &(tobenotified)->notify; } while (0) 
#define fin_dss_wait_conn_notify(N,T) \
do { 	\
	uint32_t N##_received_sigs = 0; \
	N->notify->timed_out = tx_event_flags_get(N->notify->evt, QAPI_DSS_EVT_NET_IS_CONN_E, TX_OR_CLEAR, &N##_received_sigs, T); \
} while (0)

fin_dss_ctx_t *fin_dss_new(const char *apn, const char *password, const char *username);
int fin_dss_start(fin_dss_ctx_t *ctx);
int fin_dss_restart(fin_dss_ctx_t *ctx);
int fin_dss_stop(fin_dss_ctx_t *ctx);
int fin_dss_free(fin_dss_ctx_t *ctx);

#endif  /* FIN_API_DSS_H_INCLUDED */