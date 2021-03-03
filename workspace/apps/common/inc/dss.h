#ifndef __920X_DSS_H__
#define __920X_DSS_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "qapi_fs_types.h"
#include "qapi_dss.h"
#include "qapi_uart.h"
#include "qapi_timer.h"
#include "qapi_device_info.h"
#include "qapi_socket.h"
#include "qapi_ns_utils.h"

#define DSS_ADDR_SIZE 48

/* Number of simultaneously dss data calls supported */
#define DSS_CONCURRENT_NUM	4

typedef const void (*dss_data_call_status_cb_t)(qapi_DSS_Net_Evt_t evt);

typedef struct dss_notify{
	TX_EVENT_FLAGS_GROUP *evt;
	char addr[DSS_ADDR_SIZE];
	dss_data_call_status_cb_t cb;
}dss_notify_t;

#define __DSS_NOTIFY__ dss_notify_t notify

typedef struct dss_ctx{ 
	char apn[QAPI_DSS_CALL_INFO_APN_MAX_LEN];
	char username[QAPI_DSS_CALL_INFO_USERNAME_MAX_LEN];
	char password[QAPI_DSS_CALL_INFO_PASSWORD_MAX_LEN];
	TX_EVENT_FLAGS_GROUP *dss_event_handle;
	dss_notify_t *notify;
	TX_THREAD	*thread_handle; 
	TX_BYTE_POOL *thread_byte_pool;
	void *thread_stack;
	qapi_DSS_Hndl_t dss_handle;
	struct{
		TX_EVENT_FLAGS_GROUP *nw_event_handle;
		qapi_Device_Info_Hndl_t device_info_handle;
	};
} dss_ctx_t;


extern dss_ctx_t *dss_ctx_store[DSS_CONCURRENT_NUM];

#define dss_set_notify(ctx, tobenotified) do { (ctx)->notify = &(tobenotified)->notify; } while (0) 
#define dss_wait_conn_notify(N) \
do { 	\
	uint32_t N##_received_sigs = 0; \
	tx_event_flags_get(N->notify->evt, QAPI_DSS_EVT_NET_IS_CONN_E, TX_OR_CLEAR, &N##_received_sigs, TX_WAIT_FOREVER); \
} while (0)

dss_ctx_t *dss_new(const char *apn, const char *password, const char *username);
int dss_start(dss_ctx_t *ctx);
int dss_restart(dss_ctx_t *ctx);
int dss_stop(dss_ctx_t *ctx);
int dss_free(dss_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif