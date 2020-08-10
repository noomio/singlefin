/******************************************************************************
*@file    example_dam_fota.c
*@brief   Detection of dam fota download. Customers can download their new application image form their own Revision Control Server.
*			For example, they can use HTTP(s) to access the HTTP(s) server and download the upgrade image to the file system,
			and store it in the specified path.
*  ---------------------------------------------------------------------------
*
*  Copyright (c) 2018 Quectel Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Quectel Technologies, Inc.
*  ---------------------------------------------------------------------------
*******************************************************************************/
#if defined(__EXAMPLE_DAM_FOTA__)
/* Standard C headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"
#include <locale.h>

#include "qapi_diag.h"
#include "qapi_socket.h"
#include "qapi_dnsc.h"
#include "qapi_dss.h"
#include "qapi_timer.h"
#include "qapi_uart.h"
#include "qapi_fs.h"
#include "qapi_status.h"
#include "qapi_netservices.h"
#include "qapi_httpc.h"
#include "qapi_device_info.h"
#include "qapi_fs_types.h"


#include "quectel_utils.h"
#include "quectel_uart_apis.h"
#include "example_dam_fota.h"

/*===========================================================================
						   Header file
===========================================================================*/
/*===========================================================================
                             DEFINITION
===========================================================================*/
#define QL_DEF_APN	        ""//"CMNET"
#define DSS_ADDR_INFO_SIZE	5
#define DSS_ADDR_SIZE       16

#define GET_ADDR_INFO_MIN(a, b) ((a) > (b) ? (b) : (a))

#define QUEC_HTTP_UART_DBG
#ifdef QUEC_HTTP_UART_DBG
#define DAM_FOTA_UART_DBG(...)	\
{\
	dam_fota_uart_debug_print(__VA_ARGS__);	\
}
#else
#define DAM_FOTA_UART_DBG(...)
#endif

/*===========================================================================
                           Global variable
===========================================================================*/
#define TRUE 1
#define FALSE 0
typedef  unsigned char      boolean; 

TX_EVENT_FLAGS_GROUP *http_signal_handle;
TX_EVENT_FLAGS_GROUP *http_release_handle;

#define HTTP_MAX_DATA_RETRY (3)
#define HTPP_MAX_CONNECT_RETRY (6)
#define RELEASE_FLAG 0x00001
#define RE_DOWNLOAD_FLAG 0x00010

static int dl_file_handle;
static boolean file_opened = FALSE;

void* http_nw_signal_handle = NULL;	/* Related to nework indication */

qapi_Device_Info_Hndl_t device_info_hndl;

qapi_DSS_Hndl_t http_dss_handle = NULL;	            /* Related to DSS netctrl */
qapi_Net_HTTPc_handle_t  http_hadle = NULL;

static char apn[QUEC_APN_LEN];					/* APN */
static char apn_username[QUEC_APN_USER_LEN];	/* APN username */
static char apn_passwd[QUEC_APN_PASS_LEN];		/* APN password */

bool upgrade_mode = false;
bool download_status = false;

#define FILE_PATH "/datatx/upgrade/quectel_demo_dam_fota_upgrade.bin"
#define FILE_TEMP_PATH "/datatx/quectel_demo_dam_fota_cpy.bin"
#define ORIGINAL_DAM_PATH "/datatx/quectel_demo_dam_fota.bin"
#define DAM_FOTA_DIR_PATH "/datatx/upgrade"
#define OEM_APP_PATH "/datatx/oem_app_path.ini"

#define S_IFDIR        	0040000 	/**< Directory */
#define S_IFMT          0170000		/**< Mask of all values */
#define S_ISDIR(m)     	(((m) & S_IFMT) == S_IFDIR)

/* @Note: If netctrl library fail to initialize, set this value will be 1,
 * We should not release library when it is 1. 
 */
DSS_Lib_Status_e http_netctl_lib_status = DSS_LIB_STAT_INVALID_E;
uint8 http_datacall_status = DSS_EVT_INVALID_E;
static http_session_policy_t http_session_policy;          /* http session policy */

/* Uart Dbg */
qapi_UART_Handle_t http_dbg_uart_handler;
static char quec_dbg_buffer[128];

#ifdef QUECTEL_HTTPS_SUPPORT

/* *.pem format */
#define HTTPS_CA_PEM			"/datatx/fota_cacert.pem"
#define HTTPS_CERT_PEM			"/datatx/fota_client_cert.pem"
#define HTTPS_PREKEY_PEM		"/datatx/fota_client_key.pem"

/* *.bin format */
#define HTTPS_CA_BIN			"fota_cacert.bin"
#define HTTPS_CERT_BIN			"fota_clientcert.bin"
#define HTTPS_PREKEY_BIN		"fota_clientkey.bin"

uint32_t ssl_ctx_id = 0;		/* http or https identification */
qapi_Net_SSL_Obj_Hdl_t ssl_obj_hdl = 0;

#endif	/* QUECTEL_HTTPS_SUPPORT */

#define BYTE_POOL_SIZE       (30720)

ULONG   free_memory_http[BYTE_POOL_SIZE];
TX_BYTE_POOL  *byte_pool_httpm;

/* uart rx tx buffer */
static char *uart_rx_buff = NULL;	/*!!! should keep this buffer as 4K Bytes */
static char *uart_tx_buff = NULL;

/* uart config para*/
static QT_UART_CONF_PARA uart_conf =
{
	NULL,
	QT_UART_PORT_02,
	NULL,
	0,
	NULL,
	0,
	115200
};

/*===========================================================================
                               FUNCTION
===========================================================================*/
void http_uart_dbg_init(void)
{
  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_httpm, (VOID *)&uart_rx_buff, 4*1024, TX_NO_WAIT))
  	{
  		IOT_DEBUG("tx_byte_allocate [uart_rx_buff] failed!");
    	return;
  	}

  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_httpm, (VOID *)&uart_tx_buff, 4*1024, TX_NO_WAIT))
  	{
  		IOT_DEBUG("tx_byte_allocate [uart_tx_buff] failed!");
    	return;
  	}

    uart_conf.rx_buff = uart_rx_buff;
	uart_conf.tx_buff = uart_tx_buff;
	uart_conf.tx_len = 4096;
	uart_conf.rx_len = 4096;

	/* debug uart init 			*/
	uart_init(&uart_conf);
	/* start uart receive */
	uart_recv(&uart_conf);
}

void dam_fota_uart_debug_print(const char* fmt, ...) 
{
	va_list arg_list;
    char dbg_buffer[512] = {0};
    
	va_start(arg_list, fmt);
    vsnprintf((char *)(dbg_buffer), sizeof(dbg_buffer), (char *)fmt, arg_list);
    va_end(arg_list);
		
	qt_uart_dbg(uart_conf.hdlr, dbg_buffer);
}

/*
@func
	dss_net_event_cb
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static void http_net_event_cb
( 
	qapi_DSS_Hndl_t 		hndl,
	void 				   *user_data,
	qapi_DSS_Net_Evt_t 		evt,
	qapi_DSS_Evt_Payload_t *payload_ptr 
)
{
	
	DAM_FOTA_UART_DBG("Data test event callback, event: %d\n", evt);

	switch (evt)
	{
		case QAPI_DSS_EVT_NET_NEWADDR_E:
		case QAPI_DSS_EVT_NET_IS_CONN_E:
		{
			DAM_FOTA_UART_DBG("Data Call Connected.\n");
			http_show_sysinfo();
			/* Signal main task */
  			tx_event_flags_set(http_signal_handle, DSS_SIG_EVT_CONN_E, TX_OR);
			http_datacall_status = DSS_EVT_NET_IS_CONN_E;
			
			break;
		}
		case QAPI_DSS_EVT_NET_DELADDR_E:
		case QAPI_DSS_EVT_NET_NO_NET_E:
		{
			DAM_FOTA_UART_DBG("Data Call Disconnected.\n");
			
			if (DSS_EVT_NET_IS_CONN_E == http_datacall_status)
			{
				http_datacall_status = DSS_EVT_NET_NO_NET_E;
                tx_event_flags_set(http_signal_handle, DSS_SIG_EVT_EXIT_E, TX_OR);
			}
            else
            {
                tx_event_flags_set(http_signal_handle, DSS_SIG_EVT_NO_CONN_E, TX_OR);
            }
			break;
		}
        
		default:
		{
			DAM_FOTA_UART_DBG("Data Call status is invalid.\n");
			tx_event_flags_set(http_signal_handle, DSS_SIG_EVT_INV_E, TX_OR);
			http_datacall_status = DSS_EVT_INVALID_E;
			break;
		}
	}
}

void http_show_sysinfo(void)
{
	int i   = 0;
	int j 	= 0;
	unsigned int len = 0;
	uint8 buff[DSS_ADDR_SIZE] = {0}; 
	qapi_Status_t status;
	qapi_DSS_Addr_Info_t info_ptr[DSS_ADDR_INFO_SIZE];

	status = qapi_DSS_Get_IP_Addr_Count(http_dss_handle, &len);
	if (QAPI_ERROR == status)
	{
		DAM_FOTA_UART_DBG("Get IP address count error\n");
		return;
	}
		
	status = qapi_DSS_Get_IP_Addr(http_dss_handle, info_ptr, len);
	if (QAPI_ERROR == status)
	{
		DAM_FOTA_UART_DBG("Get IP address error\n");
		return;
	}
	
	j = GET_ADDR_INFO_MIN(len, DSS_ADDR_INFO_SIZE);
	
	for (i = 0; i < j; i++)
	{
		DAM_FOTA_UART_DBG("<--- static IP address information --->\n");
		http_inet_ntoa(info_ptr[i].iface_addr_s, buff, DSS_ADDR_SIZE);
		DAM_FOTA_UART_DBG("static IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		http_inet_ntoa(info_ptr[i].gtwy_addr_s, buff, DSS_ADDR_SIZE);
		DAM_FOTA_UART_DBG("Gateway IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		http_inet_ntoa(info_ptr[i].dnsp_addr_s, buff, DSS_ADDR_SIZE);
		DAM_FOTA_UART_DBG("Primary DNS IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		http_inet_ntoa(info_ptr[i].dnss_addr_s, buff, DSS_ADDR_SIZE);
		DAM_FOTA_UART_DBG("Second DNS IP: %s\n", buff);
	}

	DAM_FOTA_UART_DBG("<--- End of system info --->\n");
}

/*
@func
	http_set_data_param
@brief
	Set the Parameter for Data Call, such as APN and network tech.
*/
static int http_set_data_param(void)
{
    qapi_DSS_Call_Param_Value_t param_info;
	
	/* Initial Data Call Parameter */
	memset(apn, 0, sizeof(apn));
	memset(apn_username, 0, sizeof(apn_username));
	memset(apn_passwd, 0, sizeof(apn_passwd));
	strlcpy(apn, QL_DEF_APN, QAPI_DSS_CALL_INFO_APN_MAX_LEN);

    if (NULL != http_dss_handle)
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;	//Automatic mode(or DSS_RADIO_TECH_LTE)
        DAM_FOTA_UART_DBG("Setting tech to Automatic\n");
        qapi_DSS_Set_Data_Call_Param(http_dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);

		/* set apn */
        param_info.buf_val = apn;
        param_info.num_val = strlen(apn);
        DAM_FOTA_UART_DBG("Setting APN - %s\n", apn);
        qapi_DSS_Set_Data_Call_Param(http_dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);
#ifdef QUEC_CUSTOM_APN
		/* set apn username */
		param_info.buf_val = apn_username;
        param_info.num_val = strlen(apn_username);
        DAM_FOTA_UART_DBG("Setting APN USER - %s\n", apn_username);
        qapi_DSS_Set_Data_Call_Param(http_dss_handle, QAPI_DSS_CALL_INFO_USERNAME_E, &param_info);

		/* set apn password */
		param_info.buf_val = apn_passwd;
        param_info.num_val = strlen(apn_passwd);
        DAM_FOTA_UART_DBG("Setting APN PASSWORD - %s\n", apn_passwd);
        qapi_DSS_Set_Data_Call_Param(http_dss_handle, QAPI_DSS_CALL_INFO_PASSWORD_E, &param_info);

        /* set auth type */
		param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_AUTH_PREF_PAP_CHAP_BOTH_ALLOWED_E; //PAP and CHAP auth
        //DAM_FOTA_UART_DBG("Setting AUTH - %d\n", param_info.num_val);
        qapi_DSS_Set_Data_Call_Param(handle, QAPI_DSS_CALL_INFO_AUTH_PREF_E, &param_info);
#endif
		/* set IP version(IPv4 or IPv6) */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_IP_VERSION_4;
        DAM_FOTA_UART_DBG("Setting family to IPv%d\n", param_info.num_val);
        qapi_DSS_Set_Data_Call_Param(http_dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);
    }
    else
    {
        DAM_FOTA_UART_DBG("Dss handler is NULL!!!\n");
		return -1;
    }
	
    return 0;
}

/*
@func
	http_inet_ntoa
@brief
	utility interface to translate ip from "int" to x.x.x.x format.
*/
int32 http_inet_ntoa
(
	const qapi_DSS_Addr_t    inaddr, /* IPv4 address to be converted         */
	uint8                   *buf,    /* Buffer to hold the converted address */
	int32                    buflen  /* Length of buffer                     */
)
{
	uint8 *paddr  = (uint8 *)&inaddr.addr.v4;
	int32  rc = 0;

	if ((NULL == buf) || (0 >= buflen))
	{
		rc = -1;
	}
	else
	{
		if (-1 == snprintf((char*)buf, (unsigned int)buflen, "%d.%d.%d.%d",
							paddr[0],
							paddr[1],
							paddr[2],
							paddr[3]))
		{
			rc = -1;
		}
	}

	return rc;
} /* http_inet_ntoa() */

/*
@func
	http_netctrl_init
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static int http_netctrl_init(void)
{
	int ret_val = 0;
	qapi_Status_t status = QAPI_OK;

	DAM_FOTA_UART_DBG("Initializes the DSS netctrl library\n");

	/* Initializes the DSS netctrl library */
	if (QAPI_OK == qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
	{
		http_netctl_lib_status = DSS_LIB_STAT_SUCCESS_E;
		DAM_FOTA_UART_DBG("qapi_DSS_Init success\n");
	}
	else
	{
		/* @Note: netctrl library has been initialized */
		http_netctl_lib_status = DSS_LIB_STAT_FAIL_E;
		DAM_FOTA_UART_DBG("DSS netctrl library has been initialized.\n");
	}
	
	/* Registering callback http_dss_handleR */
	do
	{
		DAM_FOTA_UART_DBG("Registering Callback http_dss_handle\n");
		
		/* Obtain data service handle */
		status = qapi_DSS_Get_Data_Srvc_Hndl(http_net_event_cb, NULL, &http_dss_handle);
		DAM_FOTA_UART_DBG("http_dss_handle %d, status %d\n", http_dss_handle, status);
		
		if (NULL != http_dss_handle)
		{
			DAM_FOTA_UART_DBG("Registed http_dss_handler success\n");
			break;
		}

		/* Obtain data service handle failure, try again after 10ms */
		qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
	} while(1);

	return ret_val;
}

/*
@func
	http_netctrl_start
@brief
	Start the DSS netctrl library, and startup data call.
*/
int http_netctrl_start(void)
{
	int rc = 0;
	qapi_Status_t status = QAPI_OK;
		
	rc = http_netctrl_init();
	if (0 == rc)
	{
		/* Get valid DSS handler and set the data call parameter */
		http_set_data_param();
	}
	else
	{
		DAM_FOTA_UART_DBG("quectel_dss_init fail.\n");
		return -1;
	}

	DAM_FOTA_UART_DBG("qapi_DSS_Start_Data_Call start!!!.\n");
	status = qapi_DSS_Start_Data_Call(http_dss_handle);
	if (QAPI_OK == status)
	{
		DAM_FOTA_UART_DBG("Start Data service success.\n");
		return 0;
	}
	else
	{
        DAM_FOTA_UART_DBG("Start Data service failed.\n");
		return -1;
	}
}

/*
@func
	http_netctrl_release
@brief
	Cleans up the DSS netctrl library and close data service.
*/
static void http_netctrl_stop(void)
{
	qapi_Status_t stat = QAPI_OK;
	
	if (http_dss_handle)
	{
		stat = qapi_DSS_Stop_Data_Call(http_dss_handle);
		if (QAPI_OK == stat)
		{
			DAM_FOTA_UART_DBG("Stop data call success\n");
		}
        stat = qapi_DSS_Rel_Data_Srvc_Hndl(http_dss_handle);
		if (QAPI_OK != stat)
		{
			DAM_FOTA_UART_DBG("Release data service handle failed:%d\n", stat);
		}
		http_dss_handle = NULL;
	}
	if (DSS_LIB_STAT_SUCCESS_E == http_netctl_lib_status)
	{
		qapi_DSS_Release(QAPI_DSS_MODE_GENERAL);
		http_netctl_lib_status = DSS_LIB_STAT_INVALID_E;
	}
	
}	


static void http_session_release(void)
{
	
	/* stop http client first */
	qapi_Net_HTTPc_Stop();

	if (NULL != http_hadle)
	{
		qapi_Net_HTTPc_Disconnect(http_hadle);
		qapi_Net_HTTPc_Free_sess(http_hadle);
	}
	#ifdef QUECTEL_HTTPS_SUPPORT
	if (0 != ssl_obj_hdl)
	{
		qapi_Net_SSL_Obj_Free(ssl_obj_hdl);
	}
    #endif	/* QUECTEL_HTTPS_SUPPORT */
}

static void quectel_download_release(void)
{
	file_opened = FALSE;
	http_netctrl_stop();
    qapi_FS_Close(dl_file_handle);
	memset(&http_session_policy, 0, sizeof(http_session_policy));
	http_session_release();
//	tx_event_flags_delete(http_release_handle);
}

boolean write_update_file(int32 handle, char* src, uint32 size)
{
    uint32 len;
    boolean ret_val = FALSE;
	ret_val = qapi_FS_Write(handle, src, size, &len);
	if(size == len && ret_val == QAPI_OK)
	{
        return TRUE;
	}
	else
	{
        return FALSE;
    }
}

void http_client_cb(void* arg, int32 state, void* http_resp)
{
	boolean ret_val = FALSE;
	qapi_Net_HTTPc_Response_t * resp = NULL;
	
	resp = (qapi_Net_HTTPc_Response_t *)http_resp;
	http_session_policy.session_state = HTTP_SESSION_DOWNLOADING;
	DAM_FOTA_UART_DBG("http_client_cb:%x,%ld,len:%lu,code:%lu,%x",arg,state,resp->length,resp->resp_Code,resp->data);
	
	if( resp->resp_Code >= 200 && resp->resp_Code < 300)
	{
		if(resp->data != NULL && state >= 0)
		{
			http_session_policy.last_pos += resp->length;
			http_session_policy.reason_code = state;
			http_session_policy.data_retry = 0;
			if(file_opened == FALSE)
			{
				if(upgrade_mode == true)
				{
					ret_val = qapi_FS_Open(FILE_TEMP_PATH, QAPI_FS_O_WRONLY_E | QAPI_FS_O_CREAT_E | QAPI_FS_O_TRUNC_E, &dl_file_handle);

					if(ret_val == 0)
					{
						DAM_FOTA_UART_DBG("open %s success", FILE_TEMP_PATH);
						file_opened = TRUE;
					}  
					else
					{
						DAM_FOTA_UART_DBG("open failed:%d",ret_val);
					}
				}
				else
				{
					ret_val = qapi_FS_Open(FILE_PATH, QAPI_FS_O_WRONLY_E | QAPI_FS_O_CREAT_E | QAPI_FS_O_TRUNC_E, &dl_file_handle);
					if(ret_val == 0)
					{
						DAM_FOTA_UART_DBG("open %s success", FILE_PATH);
						file_opened = TRUE;
					}  
					else
					{
						DAM_FOTA_UART_DBG("open failed:%d",ret_val);
					}
				}
				
			}
			if (file_opened == TRUE)
			{
				ret_val = write_update_file(dl_file_handle, (char*)resp->data, resp->length);
				if(ret_val != TRUE)
				{
					DAM_FOTA_UART_DBG("write_update_file failed");
				}
			}
			if (state == 0)
			{
				DAM_FOTA_UART_DBG("download success");
				http_session_policy.session_state = HTTP_SESSION_DL_FIN;
			}  
		}
		else if(resp->data == NULL && state == 0)
		{
			http_session_policy.reason_code = state;
			http_session_policy.data_retry = 0;
			DAM_FOTA_UART_DBG("download success");
			http_session_policy.session_state = HTTP_SESSION_DL_FIN;
		}
		else if (resp->data == NULL && state > 0)
		{
			//do nothing, wait and reveive next packet
			http_session_policy.data_retry = 0;
			DAM_FOTA_UART_DBG("no data received, continue...");
			http_session_policy.session_state = HTTP_SESSION_DOWNLOADING;
		}    
		else
		{

			/* data retransmission */
			if (http_session_policy.data_retry++ < HTTP_MAX_DATA_RETRY)
			{
				DAM_FOTA_UART_DBG("download timeout, try again - %d", http_session_policy.data_retry);

				tx_event_flags_set(http_release_handle, RE_DOWNLOAD_FLAG, TX_OR);
			}
			else
			{
				http_session_policy.session_state = HTTP_SESSION_DL_FAIL;
			}
		}
	}
	else if(resp->resp_Code == 0)
	{
		if (http_session_policy.data_retry++ < HTTP_MAX_DATA_RETRY)
		{
			DAM_FOTA_UART_DBG("rsp code = 0, try again - %d", http_session_policy.data_retry);
			tx_event_flags_set(http_release_handle, RE_DOWNLOAD_FLAG, TX_OR);
		}
		else
		{
			http_session_policy.session_state = HTTP_SESSION_DL_FAIL;
		}
	}
	else
	{
		/* if response code is not 2xx, don't need try again */
		http_session_policy.session_state = HTTP_SESSION_DL_FAIL;
	}
	
	if (http_session_policy.session_state != HTTP_SESSION_DOWNLOADING)
	{
		if(http_session_policy.session_state == HTTP_SESSION_DL_FIN)
		{
			download_status = true;
		}
		tx_event_flags_set(http_release_handle, RELEASE_FLAG,TX_OR);
	}
}
/*
@func
	quec_dataservice_entry
@brief
	The entry of data service task.
*/

#ifdef QUECTEL_HTTPS_SUPPORT
int http_store_read_from_EFS_file(const char *name, void **buffer_ptr, size_t *buffer_size)
{
	int bytes_read;
	int efs_fd = -1;
	struct qapi_FS_Stat_Type_s stat;
	uint8 *file_buf = NULL;
	stat.st_size = 0;

	if ((!name) || (!buffer_ptr) ||(!buffer_size))
	{
		DAM_FOTA_UART_DBG("Reading SSL from EFS file failed!\n");
		return QAPI_ERROR;
	}

	if (qapi_FS_Open(name, QAPI_FS_O_RDONLY_E, &efs_fd) < 0)
	{
		DAM_FOTA_UART_DBG("Opening EFS EFS file %s failed\n", name);
		return QAPI_ERROR;
	}

	if (qapi_FS_Stat_With_Handle(efs_fd, &stat) < 0) 
	{
		DAM_FOTA_UART_DBG("Reading EFS file %s failed\n", name);
		return QAPI_ERROR;
	}

	DAM_FOTA_UART_DBG("Reading EFS file size %d \n", stat.st_size);
	
	tx_byte_allocate(byte_pool_httpm, (VOID **) &file_buf, stat.st_size, TX_NO_WAIT);
	if (file_buf  == NULL) 
	{
		DAM_FOTA_UART_DBG("SSL_cert_store: QAPI_ERR_NO_MEMORY \n");
		return QAPI_ERR_NO_MEMORY;
	}

	qapi_FS_Read(efs_fd, file_buf, stat.st_size, &bytes_read);
	if ((bytes_read < 0) || (bytes_read != stat.st_size)) 
	{
		tx_byte_release(file_buf);
		DAM_FOTA_UART_DBG("SSL_cert_store: Reading EFS file error\n");
		return QAPI_ERROR;
	}

	*buffer_ptr = file_buf;
	*buffer_size = stat.st_size;

	qapi_FS_Close(efs_fd);

	return QAPI_OK;
}


static qapi_Status_t http_ssl_conn_obj_config(SSL_INST *ssl)
{
	qapi_Status_t ret_val = QAPI_OK;
	
	/* default eight encryption suites */
	ssl->config.cipher[0] = QAPI_NET_TLS_RSA_WITH_AES_128_CBC_SHA;
	ssl->config.cipher[1] = QAPI_NET_TLS_RSA_WITH_AES_256_CBC_SHA;
	ssl->config.cipher[2] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA;
	ssl->config.cipher[3] = QAPI_NET_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA;
	ssl->config.cipher[4] = QAPI_NET_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA;
	ssl->config.cipher[5] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA;
	ssl->config.cipher[6] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
	ssl->config.cipher[7] = QAPI_NET_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
	
	ssl->config.max_Frag_Len = 4096;
	ssl->config.max_Frag_Len_Neg_Disable = 0;
	ssl->config.protocol = QAPI_NET_SSL_PROTOCOL_TLS_1_2;
	ssl->config.verify.domain = 0;
	ssl->config.verify.match_Name[0] = '\0';
	ssl->config.verify.send_Alert = 0;
	ssl->config.verify.time_Validity = 0;	/* Don't check certification expiration */

	return ret_val;
}

static int http_ssl_verify_method(ssl_verify_method_e *verify_method)
{
	int ca_fd = -1;
	int cert_fd = -1;
	int key_fd = -1;
	qapi_FS_Status_t fs_stat = QAPI_OK;

	*verify_method = QHTTPS_SSL_NO_METHOD;

	/* check CAList is exist or not */
	fs_stat = qapi_FS_Open(HTTPS_CA_PEM, QAPI_FS_O_RDONLY_E, &ca_fd);
	if ((fs_stat != QAPI_OK) || (ca_fd < 0))
	{
		DAM_FOTA_UART_DBG("Cannot find or open CAList file in EFS");
		*verify_method = QHTTPS_SSL_NO_METHOD;
	}
	else
	{
		*verify_method = QHTTPS_SSL_ONEWAY_METHOD;
		
		/* check client certificate is exist or not */
		fs_stat = qapi_FS_Open(HTTPS_CERT_PEM, QAPI_FS_O_RDONLY_E, &cert_fd);
		if ((fs_stat != QAPI_OK) || (cert_fd < 0))
		{
			DAM_FOTA_UART_DBG("Cannot find or open Cert file in EFS");
			*verify_method = QHTTPS_SSL_ONEWAY_METHOD;
		}
		else
		{
			*verify_method = QHTTPS_SSL_TWOWAY_METHOD;
			
			/* if client certificate is exist, client private key must be existed */
			fs_stat = qapi_FS_Open(HTTPS_PREKEY_PEM, QAPI_FS_O_RDONLY_E, &key_fd);
			if ((fs_stat != QAPI_OK) || (key_fd < 0))
			{
				DAM_FOTA_UART_DBG("Cannot find or open key file in EFS");
				return -1;	//miss client key 
			}
			
			qapi_FS_Close(cert_fd);
		}

		qapi_FS_Close(ca_fd);
	}

	return 0;
}

static qapi_Status_t http_ssl_cert_load(SSL_INST *ssl)
{
	qapi_Status_t result = QAPI_OK;

	int32_t ret_val = 0;

#if 0	
	int cert_data_buf_len;
	char *cert_data_buf;
#endif

    uint8_t *cert_Buf = NULL; 
    uint32_t cert_Size = 0;
	uint8_t *key_Buf = NULL;
	uint32_t key_Size = 0;

//	int32 cert_size = 0;
	qapi_Net_SSL_Cert_Info_t cert_info;
	qapi_NET_SSL_CA_Info_t calist_info[QAPI_NET_SSL_MAX_CA_LIST];

	ssl_verify_method_e verify_method;

	if (ssl_ctx_id)
	{
		DAM_FOTA_UART_DBG("Start https process");

		if (0 != http_ssl_verify_method(&verify_method))
		{
			DAM_FOTA_UART_DBG("Miss certificates in EFS, error return");
			return QAPI_ERROR;
		}
		
		switch (verify_method)
		{
			case QHTTPS_SSL_TWOWAY_METHOD:
			{
				/* Read the client certificate information */
			    ret_val = http_store_read_from_EFS_file((char *)HTTPS_CERT_PEM, (void **)&cert_Buf, (size_t *)&cert_Size);
				DAM_FOTA_UART_DBG("Read %s, result %d", HTTPS_CERT_PEM, ret_val);
			    if (QAPI_OK != ret_val) 
			    {
			         DAM_FOTA_UART_DBG("ERROR: Reading client certificate from EFS failed!! \r\n");
			         result = QAPI_ERROR;
			    }

			    /* Read the client key information */
			    ret_val = http_store_read_from_EFS_file((char *)HTTPS_PREKEY_PEM, (void **)&key_Buf, (size_t *)&key_Size);
				DAM_FOTA_UART_DBG("Read %s, result %d", HTTPS_PREKEY_PEM, ret_val);
			    if (QAPI_OK != ret_val) 
			    {
			         DAM_FOTA_UART_DBG("ERROR: Reading key information from EFS failed!! \r\n");
			         result = QAPI_ERROR;
			    }

				/* Update the client certificate information */
				cert_info.cert_Type = QAPI_NET_SSL_CERTIFICATE_E; 
			    cert_info.info.cert.cert_Buf = cert_Buf;
			    cert_info.info.cert.cert_Size = cert_Size;
			    cert_info.info.cert.key_Buf = key_Buf;
			    cert_info.info.cert.key_Size = key_Size;
			    //cert_info.info.cert.pass_Key = https_fota_pass;

				/* Convert and store the certificate */ 
				result = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, HTTPS_CERT_BIN);
				DAM_FOTA_UART_DBG("%s qapi_Net_SSL_Cert_Convert_And_Store: %d", HTTPS_CERT_BIN, result);
				
				if (result == QAPI_OK)
				{
					if (qapi_Net_SSL_Cert_Load(ssl->sslCtx, QAPI_NET_SSL_CERTIFICATE_E, HTTPS_CERT_BIN) < 0)
					{
						DAM_FOTA_UART_DBG("ERROR: Unable to load client cert from FLASH");
						//return QCLI_STATUS_ERROR_E;
					}
				}

				/* continue to load CAList */
			}
			case QHTTPS_SSL_ONEWAY_METHOD:
			{
				/* Store CA List */
				ret_val = http_store_read_from_EFS_file(HTTPS_CA_PEM, (void **)&calist_info[0].ca_Buf, (size_t *)&calist_info[0].ca_Size);
				DAM_FOTA_UART_DBG("Read %s, result %d", HTTPS_CA_PEM, ret_val);

			    if (QAPI_OK != ret_val) 
			    {
			         DAM_FOTA_UART_DBG("ERROR: Reading ca information from EFS failed!! \r\n");
			         result = QAPI_ERROR;
			    }

				cert_info.info.ca_List.ca_Info[0] = &calist_info[0];
			    cert_info.info.ca_List.ca_Cnt = 1;
				cert_info.cert_Type = QAPI_NET_SSL_CA_LIST_E;
				
				result = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, HTTPS_CA_BIN);
				//result = qapi_Net_SSL_Cert_Store(HTTPS_CA_BIN, QAPI_NET_SSL_CA_LIST_E, cert_data_buf, cert_data_buf_len);
				DAM_FOTA_UART_DBG("%s qapi_Net_SSL_Cert_Convert_And_Store: %d", HTTPS_CA_BIN, result);
				
				if (result == QAPI_OK)
				{
					if (qapi_Net_SSL_Cert_Load(ssl->sslCtx, QAPI_NET_SSL_CA_LIST_E, HTTPS_CA_BIN) < 0)
					{
						DAM_FOTA_UART_DBG("ERROR: Unable to load CA from FLASH");
						//return QCLI_STATUS_ERROR_E;
					}
				}

				break;
			}			
			default:
			{
				DAM_FOTA_UART_DBG("Don't need to verify certifications");
				break;
			}
		}
	}
	
#if 0	/* not use */	
	/* Store PSK */
	cert_info.cert_Type = QAPI_NET_SSL_PSK_TABLE_E;
	result = qapi_Net_SSL_Cert_Convert_And_Store(&cert_info, HTTPS_PREKEY_BIN);
	DAM_FOTA_UART_DBG("%s convert and store failed: %d", HTTPS_PREKEY_BIN, result);
	//result = qapi_Net_SSL_Cert_Store(HTTPS_FOTA_CLIENT_KEY, QAPI_NET_SSL_PSK_TABLE_E, cert_data_buf, cert_data_buf_len);
	if (result == QAPI_OK)
	{
		if (qapi_Net_SSL_Cert_Load(ssl->sslCtx, QAPI_NET_SSL_PSK_TABLE_E, HTTPS_PREKEY_BIN) < 0)
		{
			DAM_FOTA_UART_DBG("ERROR: Unable to load PSK from FLASH");
			//return QCLI_STATUS_ERROR_E;
		}
	}
#endif
	return result;
}

int http_ssl_config(SSL_INST *ssl)
{
	int ret_val = 0;	
	qapi_Status_t result = QAPI_OK;
	qapi_Net_SSL_Role_t role = QAPI_NET_SSL_CLIENT_E;	/* TLS Client object creation. */

	memset(ssl, 0, sizeof(SSL_INST));	
	ssl->role = role;
	ssl->sslCtx = qapi_Net_SSL_Obj_New(role);
	if (ssl->sslCtx == QAPI_NET_SSL_INVALID_HANDLE)
	{
		DAM_FOTA_UART_DBG("ERROR: Unable to create SSL context");
		return -1;
	}
	else
	{
		ssl_obj_hdl = ssl->sslCtx;
	}
	DAM_FOTA_UART_DBG("--->http ssl config: New SSL Object ssl->sslCtx - %lu", ssl->sslCtx);

	/* TLS Configuration of a Connection Object */
	result = http_ssl_conn_obj_config(ssl);
	
	/* 
	 * allocate memory and read the certificate from certificate server or EFS.
	 * Once cert_data_buf filled with valid SSL certificate, Call QAPI to Store and Load 
	 */
	result = http_ssl_cert_load(ssl);
	if (result != QAPI_OK)
	{
		DAM_FOTA_UART_DBG("http_ssl_cert_load return error");
		ret_val = -1;
	}
	
	return ret_val;
}
#endif	/* QUECTEL_HTTPS_SUPPORT */


/*
* return
*  0: No error
* -1: create SSL context failed
* -2: SSL config failed
* -3: create HTTP session failed
* -4: HTTP connect failed
* -5: HTPP request failed
*/
static int http_download(char* host, uint16 port, uint8* file_name)
{
    qapi_Status_t ret = -1;
	uint8 reconnect_count = 0;
#ifdef QUECTEL_HTTPS_SUPPORT
	SSL_INST http_ssl;
#endif

    /* Start HTTPc service */
    qapi_Net_HTTPc_Start();
    memset(&http_session_policy, 0, sizeof(http_session_policy_t));
#ifdef QUECTEL_HTTPS_SUPPORT
    if (ssl_ctx_id)
    {
    	/* config object and certificate */
    	if (0 != http_ssl_config(&http_ssl))
    	{
    		DAM_FOTA_UART_DBG("ERROR: Config ssl object and certificates error");
    		
    	}
	/* @Justice_20190413 qapi_Net_HTTPc_Connect" will internally take care of creating SSL connection 
	 * corresponding to HTTP connection. And creating SSL connection using "qapi_Net_SSL_Con_New" which 
	 * is not necessary. (refer to TX3.0.1 P8101-35 Doc) */

       http_hadle = qapi_Net_HTTPc_New_sess(20000, http_ssl.sslCtx, http_client_cb, NULL, 1024, 1024);
    	ret = qapi_Net_HTTPc_Configure_SSL(http_hadle, &http_ssl.config);
		if(ret != QAPI_OK)
		{
			DAM_FOTA_UART_DBG("Start HTTPS connection, SSL config ret[%d]", ret);
			return -2;
    	}

    }
    else
    {
        http_hadle = qapi_Net_HTTPc_New_sess(20000, 0, http_client_cb, NULL, 1024, 1024);

    }
#else
    http_hadle = qapi_Net_HTTPc_New_sess(20000, 0, http_client_cb, NULL, 1024, 1024);
#endif /* QUECTEL_HTTPS_SUPPORT */

	http_session_policy.session_state = HTTP_SESSION_INIT;

    if ( http_hadle == NULL)
    {
        DAM_FOTA_UART_DBG("qapi_Net_HTTPc_New_sess ERROR");
        return -3;
    }

    qapi_Net_HTTPc_Pass_Pool_Ptr(http_hadle, byte_pool_httpm);
    
    do
    {
	    ret = qapi_Net_HTTPc_Connect(http_hadle, host, port);
	    if (ret != QAPI_OK)
	    {
	        reconnect_count++;
			DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Connect ERROR ret:%d, re-connect times:%d",ret,reconnect_count);
			if(reconnect_count >= HTPP_MAX_CONNECT_RETRY)
			{
			    return -4;
			}
			qapi_Timer_Sleep(3, QAPI_TIMER_UNIT_SEC, true);
	    }
	    else
	    {
	    	DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Connect success :%d", ret);
			break;
	    }
    } while (1);
    http_session_policy.session_state = HTTP_SESSION_CONN;

    /* get response content */
    ret = qapi_Net_HTTPc_Request(http_hadle, QAPI_NET_HTTP_CLIENT_GET_E, (char*)file_name); 
    if (ret != QAPI_OK)
    {
        DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Request ERROR :%d",ret);
        return -5;
    }
    return 0;
}

/*
* return
*  0: No error
* -1: input http handler is null
* -2: add header field failed
* -3: HTTP connect failed
* -4: HTPP request failed
*/
int http_download_process(qapi_Net_HTTPc_handle_t http_handler, char *host, uint16 port, char *file)
{
    uint8 reconn_counter = 0;
	qapi_Status_t ret = QAPI_ERROR;
	char range_buffer[32] = {0};
	http_session_policy_t http_session_req = http_session_policy;

	if (http_handler == NULL)
	{
	    DAM_FOTA_UART_DBG("http_handler is null");
		return -1;
	}
	
	/* Reconnection may failed with network Latency, please try again */
    do
    {
		/* add range filed to re-download data from last breakpoint */
		memset(range_buffer, 0, sizeof(range_buffer));
		http_session_req.start_pos = http_session_req.last_pos;
		sprintf(range_buffer, "bytes=%lu-", http_session_req.start_pos);
		
		DAM_FOTA_UART_DBG("http_download: %s", range_buffer);
		
		ret = qapi_Net_HTTPc_Add_Header_Field(http_handler, "Range", range_buffer);
		if (ret != QAPI_OK)
		{
			DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Add_Header_Field content-type ERROR :%d", ret);
            return -2;
		}

	    ret = qapi_Net_HTTPc_Connect(http_handler, host, port);
	    if (ret != QAPI_OK)
	    {
	        reconn_counter++;
			DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Connect ERROR :%d,reconn_counter:%d",ret, reconn_counter);
			if(reconn_counter >= HTPP_MAX_CONNECT_RETRY)
			{
				return -3;
			}
			qapi_Timer_Sleep(3, QAPI_TIMER_UNIT_SEC, true);
	    }
	    else
	    {
	    	DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Connect success :%d", ret);
			break;
	    }
		DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Connect re-content times:%d", reconn_counter);
    } while (1);
    
    /* get response content */
    ret = qapi_Net_HTTPc_Request(http_handler, QAPI_NET_HTTP_CLIENT_GET_E, (char*)file); 
    if (ret != QAPI_OK)
    {
        DAM_FOTA_UART_DBG("qapi_Net_HTTPc_Request ERROR :%d",ret);
        return -4;
    }
    return 0;
}

void qt_dam_fota_init_device_info(void)
{
	qapi_Status_t status = 0;

	status = qapi_Device_Info_Init_v2(&device_info_hndl);
	if(status != QAPI_OK)
	{
		DAM_FOTA_UART_DBG( "~ qapi_Device_Info_Init fail [%d]", status);
	}
	else
	{
		DAM_FOTA_UART_DBG( "~ qapi_Device_Info_Init OK [%d]", status);
	}
}

int quectel_efs_is_dir_exists(const char *path)
{
     int result, ret_val = 0;
     struct qapi_FS_Stat_Type_s sbuf;

     memset (&sbuf, 0, sizeof (sbuf));

     result = qapi_FS_Stat(path, &sbuf);

     if (result != 0)
          goto End;

     if (S_ISDIR (sbuf.st_Mode) == 1)
          ret_val = 1;

     End:
          return ret_val;
}

/*
* return
* -0: oem_app_path is double path
* -1: oem_app_path is single path
* -else: read oem_app_path.ini failed
*/
int oem_app_path_is_single_path(void)
{
	int file_handle;
	qapi_FS_Status_t ret_val = -1;
	struct qapi_FS_Stat_Type_s sbuf;
	uint32 real_read;
	uint8 buf[100] = {0};
	char *ppath = NULL;

	ret_val = qapi_FS_Stat(OEM_APP_PATH, &sbuf);
    if (ret_val != 0)
    {
        return -1;
    }
    ret_val = qapi_FS_Open(OEM_APP_PATH, QAPI_FS_O_RDONLY_E , &file_handle);         
    if(ret_val!= 0)
    {
        DAM_FOTA_UART_DBG("open %s failed", OEM_APP_PATH);
        return -1;
    }

    ret_val = qapi_FS_Read(file_handle, (uint8*)buf, sbuf.st_size, &real_read );
    if(ret_val != 0 || real_read != sbuf.st_size)
    {
        DAM_FOTA_UART_DBG("read %s failed", OEM_APP_PATH);
        return -1;
    }
    qapi_FS_Close(file_handle);

    ppath = strstr((char *)buf, (char *)":/datatx");
    if(ppath == NULL)
    {
		return 1;
    }
    else
    {
		return 0;
    }
}

int quectel_task_entry(void)
{
    int ret = -1;
    uint32 dss_event = 0;
    uint32 session_pocess_sig = 0;
    int result = 0;
    uint8 file_name[] = "quectel_demo_dam_fota_upgrade.bin";
    char host[] = "220.180.239.212";
    uint8 buf[300] = {0};
    uint16 port = 8005;
    bool network_enabled = false;
    qapi_Device_Info_t info;
    static uint32 run_times = 0;
    qapi_FS_Status_t status = QAPI_OK;

	// MUST SETTING,TBD
	setlocale(LC_ALL, "C");
    
    /* create a memory pool */
    txm_module_object_allocate(&byte_pool_httpm, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_httpm, "http application pool", free_memory_http, BYTE_POOL_SIZE);

    http_uart_dbg_init();
    DAM_FOTA_UART_DBG("DAM_FOTA Task1 Start...");

    qt_dam_fota_init_device_info();

    DAM_FOTA_UART_DBG("wait network enabled...");
    do
	{
		qapi_Timer_Sleep(200, QAPI_TIMER_UNIT_MSEC, true);
		memset(&info,  0x00, sizeof(qapi_Device_Info_t));
		qapi_Device_Info_Get_v2(device_info_hndl, QAPI_DEVICE_INFO_SERVICE_STATE_E, &info);
		if(info.u.valueint == 0)
		{
			network_enabled = false;
		}
		else
		{
			network_enabled = true;
			DAM_FOTA_UART_DBG("network enabled!");
		}
	}while(!network_enabled);

	ret = oem_app_path_is_single_path();
	if(ret == 1)
	{
		upgrade_mode = true;	
	}
	else if(ret == 0)
	{
		upgrade_mode = false;
		if(quectel_efs_is_dir_exists(DAM_FOTA_DIR_PATH) != 1)
		{
			status = qapi_FS_Mk_Dir(DAM_FOTA_DIR_PATH, 0677);
			DAM_FOTA_UART_DBG("qapi_FS_Mk_Dir %d", status);
		}
	}
	else
	{
		DAM_FOTA_UART_DBG("read oem app path failed");
		return FALSE;
	}

    /* Create event signal handle and clear signals */
    txm_module_object_allocate(&http_signal_handle, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(http_signal_handle, "dss_signal_event");
	tx_event_flags_set(http_signal_handle, 0x0, TX_AND);

    txm_module_object_allocate(&http_release_handle, sizeof(TX_EVENT_FLAGS_GROUP));
    tx_event_flags_create(http_release_handle, "http_release_event");
	tx_event_flags_set(http_release_handle, 0x0, TX_AND);

    ret = http_netctrl_start();
	if (ret != 0)
	{
        return FALSE;
	}
    
    tx_event_flags_get(http_signal_handle, DSS_SIG_EVT_CONN_E|DSS_SIG_EVT_DIS_E|DSS_SIG_EVT_EXIT_E|DSS_SIG_EVT_NO_CONN_E, TX_OR_CLEAR, &dss_event, TX_WAIT_FOREVER);
    tx_event_flags_delete(http_signal_handle);
    if(dss_event&DSS_SIG_EVT_CONN_E)
    {
        DAM_FOTA_UART_DBG("dss_event:%x",dss_event);
        if(qapi_Net_DNSc_Is_Started() == 0)
        {
            qapi_Net_DNSc_Command(QAPI_NET_DNS_START_E);
            qapi_Net_DNSc_Add_Server("223.5.5.5", 0);
            qapi_Net_DNSc_Add_Server("114.114.114.114", 1);

        }
    }
    else if(dss_event&DSS_SIG_EVT_EXIT_E)
    {
        http_netctrl_stop();
        tx_event_flags_delete(http_release_handle);
        return FALSE;
    }
    else
    {
        tx_event_flags_delete(http_release_handle);
        return FALSE;
    }

/* if use HTTPs server to download the new application image, please set HTTPs identification. */
#if 0
#ifdef QUECTEL_HTTPS_SUPPORT
	ssl_ctx_id = 1;
#endif
#endif

    
    result = http_download(host, port, file_name);
    if(result < 0)
    {
        quectel_download_release();
        return FALSE;
    }

    while (1)
    {
        
        tx_event_flags_get(http_release_handle, RELEASE_FLAG | RE_DOWNLOAD_FLAG, TX_OR_CLEAR, &session_pocess_sig, TX_WAIT_FOREVER);
        DAM_FOTA_UART_DBG("@release_sig [0x%x]", session_pocess_sig);

        if (session_pocess_sig & RELEASE_FLAG)
        {
        	quectel_download_release();
        	if(download_status == true)
			{
				/* CRC check to do. */
				
				if(upgrade_mode == true)
				{
					status = qapi_FS_Unlink(ORIGINAL_DAM_PATH);
					qapi_FS_Rename(FILE_TEMP_PATH, ORIGINAL_DAM_PATH);
				}
			}
          break;
        }
        else if (session_pocess_sig & RE_DOWNLOAD_FLAG)
        {
        	/* trigger next download process */
        	DAM_FOTA_UART_DBG("Re-download process again, current pos: %d, try - %d", http_session_policy.last_pos, http_session_policy.data_retry);
        	
        	result = http_download_process(http_hadle, host, port, (char*)file_name);
        	if (result < 0)
        	{
        		quectel_download_release();	
        		break;
        	}

        }
    }

    while(1)
    {
		run_times++;
		DAM_FOTA_UART_DBG("run_times:%d", run_times);
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
    }
    return TRUE;
}

#endif /*__EXAMPLE_DAM_FOTA__*/

