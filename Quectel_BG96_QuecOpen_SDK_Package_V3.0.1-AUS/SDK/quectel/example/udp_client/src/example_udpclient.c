/******************************************************************************
*@file    example_udpclient.c
*@brief   Detection of network state and notify the main task to create udp client.
*         If server send "Exit" to client, client will exit immediately.
*  ---------------------------------------------------------------------------
*
*  Copyright (c) 2018 Quectel Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Quectel Technologies, Inc.
*  ---------------------------------------------------------------------------
*******************************************************************************/

#if defined(__EXAMPLE_UDPCLIENT__)
/*===========================================================================
						   Header file
===========================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>

#include "qapi_fs_types.h"
#include "qapi_status.h"
#include "qapi_socket.h"
#include "qapi_dss.h"
#include "qapi_netservices.h"
#include "qapi_device_info.h"


#include "qapi_uart.h"
#include "qapi_timer.h"
#include "qapi_diag.h"
#include "quectel_utils.h"
#include "quectel_uart_apis.h"
#include "example_udpclient.h"

/*===========================================================================
                             DEFINITION
===========================================================================*/
#define QL_DEF_APN	        "CMNET"
#define DSS_ADDR_INFO_SIZE	5
#define DSS_ADDR_SIZE       16

#define GET_ADDR_INFO_MIN(a, b) ((a) > (b) ? (b) : (a))

#define QUEC_UDP_UART_DBG
#ifdef QUEC_UDP_UART_DBG
#define UDP_UART_DBG(...)	\
{\
	udp_uart_debug_print(__VA_ARGS__);	\
}
#else
#define UDP_UART_DBG(...)
#endif

#define THREAD_STACK_SIZE    (1024 * 16)
#define THREAD_PRIORITY      (180)
#define BYTE_POOL_SIZE       (1024 * 16)

#define CMD_BUF_SIZE  100

#define DEFAULT_PUB_TIME 5

//#define NW_SIGNAL_ENABLE
#define UDP_IO_SELECT//I/O multiplexing for receiving data
/*===========================================================================
                           Global variable
===========================================================================*/
/* UDPClient dss thread handle */
#ifdef QAPI_TXM_MODULE
	static TX_THREAD *dss_thread_handle; 
#else
	static TX_THREAD _dss_thread_handle;
	static TX_THREAD *ts_thread_handle = &_dss_thread_handle;
#endif

void* udp_nw_signal_handle = NULL;	/* Related to nework indication */

qapi_Device_Info_Hndl_t device_info_hndl;

static unsigned char udp_dss_stack[THREAD_STACK_SIZE];

TX_EVENT_FLAGS_GROUP *udp_signal_handle;

qapi_DSS_Hndl_t udp_dss_handle = NULL;	            /* Related to DSS netctrl */

static char apn[QUEC_APN_LEN];					/* APN */
static char apn_username[QUEC_APN_USER_LEN];	/* APN username */
static char apn_passwd[QUEC_APN_PASS_LEN];		/* APN password */

/* @Note: If netctrl library fail to initialize, set this value will be 1,
 * We should not release library when it is 1. 
 */
signed char udp_netctl_lib_status = DSS_LIB_STAT_INVALID_E;
unsigned char udp_datacall_status = DSS_EVT_INVALID_E;

TX_BYTE_POOL *byte_pool_udp;
#define UDP_BYTE_POOL_SIZE		10*8*1024
UCHAR free_memory_udp[UDP_BYTE_POOL_SIZE];

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
void udp_uart_dbg_init()
{
  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_udp, (VOID *)&uart_rx_buff, 4*1024, TX_NO_WAIT))
  	{
  		IOT_DEBUG("tx_byte_allocate [uart_rx_buff] failed!");
    	return;
  	}

  	if (TX_SUCCESS != tx_byte_allocate(byte_pool_udp, (VOID *)&uart_tx_buff, 4*1024, TX_NO_WAIT))
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
void udp_uart_debug_print(const char* fmt, ...) 
{
	va_list arg_list;
    char dbg_buffer[128] = {0};
    
	va_start(arg_list, fmt);
    vsnprintf((char *)(dbg_buffer), sizeof(dbg_buffer), (char *)fmt, arg_list);
    va_end(arg_list);
		
    qapi_UART_Transmit(uart_conf.hdlr, dbg_buffer, strlen(dbg_buffer), NULL);
}

/*
@func
	dss_net_event_cb
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static void udp_net_event_cb
( 
	qapi_DSS_Hndl_t 		hndl,
	void 				   *user_data,
	qapi_DSS_Net_Evt_t 		evt,
	qapi_DSS_Evt_Payload_t *payload_ptr 
)
{
	qapi_Status_t status = QAPI_OK;
	
	UDP_UART_DBG("Data test event callback, event: %d\n", evt);

	switch (evt)
	{
		case QAPI_DSS_EVT_NET_IS_CONN_E:
		{
			UDP_UART_DBG("Data Call Connected.\n");
			udp_show_sysinfo();
			/* Signal main task */
  			tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_CONN_E, TX_OR);
			udp_datacall_status = DSS_EVT_NET_IS_CONN_E;
			
			break;
		}
		case QAPI_DSS_EVT_NET_NO_NET_E:
		{
			UDP_UART_DBG("Data Call Disconnected.\n");
			
			if (DSS_EVT_NET_IS_CONN_E == udp_datacall_status)
			{
				/* Release Data service handle and netctrl library */
				if (udp_dss_handle)
				{
					status = qapi_DSS_Rel_Data_Srvc_Hndl(udp_dss_handle);
					if (QAPI_OK == status)
					{
						UDP_UART_DBG("Release data service handle success\n");
						tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_EXIT_E, TX_OR);
					}
				}
				
				if (DSS_LIB_STAT_SUCCESS_E == udp_netctl_lib_status)
				{
					qapi_DSS_Release(QAPI_DSS_MODE_GENERAL);
				}
			}
			else
			{
				/* DSS status maybe QAPI_DSS_EVT_NET_NO_NET_E before data call establishment */
				tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_NO_CONN_E, TX_OR);
			}

			break;
		}
		default:
		{
			UDP_UART_DBG("Data Call status is invalid.\n");
			
			/* Signal main task */
  			tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_INV_E, TX_OR);
			udp_datacall_status = DSS_EVT_INVALID_E;
			break;
		}
	}
}

void udp_show_sysinfo(void)
{
	int i   = 0;
	int j 	= 0;
	unsigned int len = 0;
	uint8 buff[DSS_ADDR_SIZE] = {0}; 
	qapi_Status_t status;
	qapi_DSS_Addr_Info_t info_ptr[DSS_ADDR_INFO_SIZE];

	status = qapi_DSS_Get_IP_Addr_Count(udp_dss_handle, &len);
	if (QAPI_ERROR == status)
	{
		UDP_UART_DBG("Get IP address count error\n");
		return;
	}
		
	status = qapi_DSS_Get_IP_Addr(udp_dss_handle, info_ptr, len);
	if (QAPI_ERROR == status)
	{
		UDP_UART_DBG("Get IP address error\n");
		return;
	}
	
	j = GET_ADDR_INFO_MIN(len, DSS_ADDR_INFO_SIZE);
	
	for (i = 0; i < j; i++)
	{
		UDP_UART_DBG("<--- static IP address information --->\n");
		udp_inet_ntoa(info_ptr[i].iface_addr_s, buff, DSS_ADDR_SIZE);
		UDP_UART_DBG("static IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		udp_inet_ntoa(info_ptr[i].gtwy_addr_s, buff, DSS_ADDR_SIZE);
		UDP_UART_DBG("Gateway IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		udp_inet_ntoa(info_ptr[i].dnsp_addr_s, buff, DSS_ADDR_SIZE);
		UDP_UART_DBG("Primary DNS IP: %s\n", buff);

		memset(buff, 0, sizeof(buff));
		udp_inet_ntoa(info_ptr[i].dnss_addr_s, buff, DSS_ADDR_SIZE);
		UDP_UART_DBG("Second DNS IP: %s\n", buff);
	}

	UDP_UART_DBG("<--- End of system info --->\n");
}

/*
@func
	udp_set_data_param
@brief
	Set the Parameter for Data Call, such as APN and network tech.
*/
static int udp_set_data_param(void)
{
    qapi_DSS_Call_Param_Value_t param_info;
	
	/* Initial Data Call Parameter */
	memset(apn, 0, sizeof(apn));
	memset(apn_username, 0, sizeof(apn_username));
	memset(apn_passwd, 0, sizeof(apn_passwd));
	strlcpy(apn, QL_DEF_APN, QAPI_DSS_CALL_INFO_APN_MAX_LEN);

    if (NULL != udp_dss_handle)
    {
        /* set data call param */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_RADIO_TECH_UNKNOWN;	//Automatic mode(or DSS_RADIO_TECH_LTE)
        UDP_UART_DBG("Setting tech to Automatic\n");
        qapi_DSS_Set_Data_Call_Param(udp_dss_handle, QAPI_DSS_CALL_INFO_TECH_PREF_E, &param_info);

		/* set apn */
        param_info.buf_val = apn;
        param_info.num_val = strlen(apn);
        UDP_UART_DBG("Setting APN - %s\n", apn);
        qapi_DSS_Set_Data_Call_Param(udp_dss_handle, QAPI_DSS_CALL_INFO_APN_NAME_E, &param_info);
#ifdef QUEC_CUSTOM_APN
		/* set apn username */
		param_info.buf_val = apn_username;
        param_info.num_val = strlen(apn_username);
        UDP_UART_DBG("Setting APN USER - %s\n", apn_username);
        qapi_DSS_Set_Data_Call_Param(udp_dss_handle, QAPI_DSS_CALL_INFO_USERNAME_E, &param_info);

		/* set apn password */
		param_info.buf_val = apn_passwd;
        param_info.num_val = strlen(apn_passwd);
        UDP_UART_DBG("Setting APN PASSWORD - %s\n", apn_passwd);
        qapi_DSS_Set_Data_Call_Param(udp_dss_handle, QAPI_DSS_CALL_INFO_PASSWORD_E, &param_info);
#endif
		/* set IP version(IPv4 or IPv6) */
        param_info.buf_val = NULL;
        param_info.num_val = QAPI_DSS_IP_VERSION_4;
        UDP_UART_DBG("Setting family to IPv%d\n", param_info.num_val);
        qapi_DSS_Set_Data_Call_Param(udp_dss_handle, QAPI_DSS_CALL_INFO_IP_VERSION_E, &param_info);
    }
    else
    {
        UDP_UART_DBG("Dss handler is NULL!!!\n");
		return -1;
    }
	
    return 0;
}

/*
@func
	udp_inet_ntoa
@brief
	utility interface to translate ip from "int" to x.x.x.x format.
*/
int32 udp_inet_ntoa
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
} /* udp_inet_ntoa() */

/*
@func
	udp_netctrl_init
@brief
	Initializes the DSS netctrl library for the specified operating mode.
*/
static int udp_netctrl_init(void)
{
	int ret_val = 0;
	qapi_Status_t status = QAPI_OK;

	UDP_UART_DBG("Initializes the DSS netctrl library\n");

	/* Initializes the DSS netctrl library */
	if (QAPI_OK == qapi_DSS_Init(QAPI_DSS_MODE_GENERAL))
	{
		udp_netctl_lib_status = DSS_LIB_STAT_SUCCESS_E;
		UDP_UART_DBG("qapi_DSS_Init success\n");
	}
	else
	{
		/* @Note: netctrl library has been initialized */
		udp_netctl_lib_status = DSS_LIB_STAT_FAIL_E;
		UDP_UART_DBG("DSS netctrl library has been initialized.\n");
	}
	
	/* Registering callback udp_dss_handleR */
	do
	{
		UDP_UART_DBG("Registering Callback udp_dss_handle\n");
		
		/* Obtain data service handle */
		status = qapi_DSS_Get_Data_Srvc_Hndl(udp_net_event_cb, NULL, &udp_dss_handle);
		UDP_UART_DBG("udp_dss_handle %d, status %d\n", udp_dss_handle, status);
		
		if (NULL != udp_dss_handle)
		{
			UDP_UART_DBG("Registed udp_dss_handler success\n");
			break;
		}

		/* Obtain data service handle failure, try again after 10ms */
		qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
	} while(1);

	return ret_val;
}

/*
@func
	udp_netctrl_start
@brief
	Start the DSS netctrl library, and startup data call.
*/
int udp_netctrl_start(void)
{
	int rc = 0;
	qapi_Status_t status = QAPI_OK;
		
	rc = udp_netctrl_init();
	if (0 == rc)
	{
		/* Get valid DSS handler and set the data call parameter */
		udp_set_data_param();
	}
	else
	{
		UDP_UART_DBG("quectel_dss_init fail.\n");
		return -1;
	}

	UDP_UART_DBG("qapi_DSS_Start_Data_Call start!!!.\n");
	status = qapi_DSS_Start_Data_Call(udp_dss_handle);
	if (QAPI_OK == status)
	{
		UDP_UART_DBG("Start Data service success.\n");
		return 0;
	}
	else
	{
		return -1;
	}
}

/*
@func
	udp_netctrl_release
@brief
	Cleans up the DSS netctrl library and close data service.
*/
int udp_netctrl_stop(void)
{
	qapi_Status_t stat = QAPI_OK;
	
	if (udp_dss_handle)
	{
		stat = qapi_DSS_Stop_Data_Call(udp_dss_handle);
		if (QAPI_OK == stat)
		{
			UDP_UART_DBG("Stop data call success\n");
		}
	}
	
	return 0;
}	

/*
@func
	quec_dataservice_entry
@brief
	The entry of data service task.
*/
void quec_dataservice_thread(ULONG param)
{
	ULONG dss_event = 0;
	
	/* Start data call */
	udp_netctrl_start();

	while (1)
	{
		/* Wait disconnect signal */
		tx_event_flags_get(udp_signal_handle, DSS_SIG_EVT_DIS_E, TX_OR, &dss_event, TX_WAIT_FOREVER);
		if (dss_event & DSS_SIG_EVT_DIS_E)
		{
			/* Stop data call and release resource */
			udp_netctrl_stop();
			UDP_UART_DBG("Data service task exit.\n");
			break;
		}
	}

	UDP_UART_DBG("Data Service Thread Exit!\n");
	return;
}

static int start_udp_session(void)
{
	int  sock_fd = -1;
	int  sent_len = 0;
	int  recv_len = 0;
	char sent_buff[SENT_BUF_SIZE];
	char recv_buff[RECV_BUF_SIZE];
	fd_set  fdset;
	int  ret_val = 0;
	int32 server_addr_len = 0;
	struct sockaddr_in server_addr;

	do
	{
		sock_fd = qapi_socket(AF_INET, DEF_SRC_TYPE, 0);
		if (sock_fd < 0)
		{
			UDP_UART_DBG("Create socket error\n");			
			break;
		}
		
		UDP_UART_DBG("<-- Create socket[%d] success -->\n", sock_fd);
		memset(sent_buff, 0, sizeof(sent_buff));
		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = _htons(DEF_SRV_PORT);
		server_addr.sin_addr.s_addr = inet_addr(DEF_SRV_ADDR);

		/* Start sending data to the specified server.
		If qapi_connect is called for connection with udp client and server, qapi_sendto is unable to be called 
		for sending data to specified destination address, and qapi_recvfrom is also unable to be called for
		receiving data from specified source address.*/
		strcpy(sent_buff, "Hello Quectel, Test UDP client Demo!");
		sent_len = qapi_sendto(sock_fd, sent_buff, strlen(sent_buff), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
		if (sent_len > 0)
		{
			UDP_UART_DBG("Client send data success, len: %d, data: %s\n", sent_len, sent_buff);
		}
		else
		{
			UDP_UART_DBG("send error, sent_len:%d\n", sent_len);
		}
		memset(&server_addr, 0, sizeof(server_addr));

		/* Block and wait for response */
#ifdef UDP_IO_SELECT /* I/O multiplexing */
		while(1)
		{
			qapi_fd_zero(&fdset);
			qapi_fd_set(sock_fd, &fdset);
			ret_val = qapi_select(&fdset, NULL, NULL, TX_WAIT_FOREVER);

			if(ret_val > 0)
			{
				if(qapi_fd_isset(sock_fd, &fdset) != 0)
				{
					memset(recv_buff, 0, sizeof(recv_buff));
					recv_len = qapi_recvfrom(sock_fd, recv_buff, RECV_BUF_SIZE, 0, (struct sockaddr*)&server_addr, &server_addr_len);
					if (recv_len > 0)
					{
						if (0 == strncmp(recv_buff, "Exit", 4))
						{
							qapi_socketclose(sock_fd);
							sock_fd = -1;
							tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_DIS_E, TX_OR);
							UDP_UART_DBG("UDP Client Exit!!!\n");
							break;
						}

						/* Reveive data and response*/
						UDP_UART_DBG("[UDP Client]@len[%d], @Recv: %s\n", recv_len, recv_buff);

						memset(sent_buff, 0, SENT_BUF_SIZE);
						snprintf(sent_buff, SENT_BUF_SIZE, "[UDP Server]received %d bytes data", recv_len);
						sent_len = qapi_sendto(sock_fd, sent_buff, strlen(sent_buff), 0, (struct sockaddr*)&server_addr, server_addr_len);
						if(sent_len < 0)
						{
							UDP_UART_DBG("UDP Server send data failed: %d\n", sent_len);
						}
					}
				}
			}
		}
#else /* Timer polling for receiving data */
		while (1)
		{			
			memset(recv_buff, 0, sizeof(recv_buff));
			
			recv_len = qapi_recvfrom(sock_fd, recv_buff, RECV_BUF_SIZE, 0, (struct sockaddr*)&server_addr, &server_addr_len);
			if (recv_len > 0)
			{
				if (0 == strncmp(recv_buff, "Exit", 4))
				{
					qapi_socketclose(sock_fd);
					sock_fd = -1;
					tx_event_flags_set(udp_signal_handle, DSS_SIG_EVT_DIS_E, TX_OR);
					UDP_UART_DBG("UDP Client Exit!!!\n");
					break;
				}

				/* Reveive data and response*/
				UDP_UART_DBG("[UDP Client]@len[%d], @Recv: %s\n", recv_len, recv_buff);
				
				memset(sent_buff, 0, SENT_BUF_SIZE);
				snprintf(sent_buff, SENT_BUF_SIZE, "[UDP Server]received %d bytes data", recv_len);
				sent_len = qapi_sendto(sock_fd, sent_buff, strlen(sent_buff), 0, (struct sockaddr*)&server_addr, server_addr_len);
				if(sent_len < 0)
				{
					UDP_UART_DBG("UDP Server send data failed: %d\n", sent_len);
				}
			}

			qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_MSEC, true);
		}
#endif
	} while (0);

	if (sock_fd >= 0)
	{
		qapi_socketclose(sock_fd);
	}
	
	return 0;
}

#ifdef NW_SIGNAL_ENABLE
void udp_network_indication_cb(qapi_Device_Info_Hndl_t device_info_hndl, const qapi_Device_Info_t *dev_info)
{
	if(dev_info->id == QAPI_DEVICE_INFO_NETWORK_IND_E)
	{
		switch(dev_info->info_type)
		{
			case QAPI_DEVICE_INFO_TYPE_BOOLEAN_E:
				UDP_UART_DBG( "~type[b] id[%d] status[%d]\n", dev_info->id, dev_info->u.valuebool);
				if(dev_info->u.valuebool == true)
				{
					tx_event_flags_set(udp_nw_signal_handle, NW_SIG_EVT_ENABLE, TX_OR);
				}
				else
				{
					tx_event_flags_set(udp_nw_signal_handle, NW_SIG_EVT_UNENABLE, TX_OR);
				}
			break;

			case QAPI_DEVICE_INFO_TYPE_INTEGER_E:
				UDP_UART_DBG( "~type[i] id[%d] status[%d]\n", dev_info->id, dev_info->u.valueint);
			break;

			case QAPI_DEVICE_INFO_TYPE_BUFFER_E:
				UDP_UART_DBG( "~type[s] id[%d] status[%s]\n", dev_info->id, dev_info->u.valuebuf.buf);
			break;

			default:
				UDP_UART_DBG( "~[Invalid][%d]\n", dev_info->id);
			break;
		}
	}
}

void qt_udp_init_device_info(void)
{
	qapi_Status_t status = 0;

	status = qapi_Device_Info_Init_v2(&device_info_hndl);
	if(status != QAPI_OK)
	{
		UDP_UART_DBG( "~ qapi_Device_Info_Init fail [%d]\n", status);
	}
	else
	{
		UDP_UART_DBG( "~ qapi_Device_Info_Init OK [%d]\n", status);
	}
}
#endif /* NW_SIGNAL_ENABLE */

/*
@func
	quectel_task_entry
@brief
	The entry of data service task.
*/
int quectel_task_entry(void)
{

	int     ret = 0;
	ULONG   dss_event = 0;
	int32   sig_mask;
#ifdef NW_SIGNAL_ENABLE 
	ULONG   nw_event = 0;
	int32   nw_sig_mask;
	qapi_Status_t status = QAPI_OK;
#endif

	// MUST SETTING,TBD
	setlocale(LC_ALL, "C");

	/* wait 10sec for device startup */
	qapi_Timer_Sleep(10, QAPI_TIMER_UNIT_SEC, true);

	/* Create a byte memory pool. */
	txm_module_object_allocate(&byte_pool_udp, sizeof(TX_BYTE_POOL));
	tx_byte_pool_create(byte_pool_udp, "udp application pool", free_memory_udp, UDP_BYTE_POOL_SIZE);

	/* Initial uart for debug */
	udp_uart_dbg_init();
	UDP_UART_DBG("UDPClient Task Start...\n");

#ifdef NW_SIGNAL_ENABLE 
	/* waiting for registering the network */
	txm_module_object_allocate(&udp_nw_signal_handle, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(udp_nw_signal_handle, "nw_signal_event");
	tx_event_flags_set(udp_nw_signal_handle, 0x0, TX_AND);
	
	qt_udp_init_device_info();
	status = qapi_Device_Info_Pass_Pool_Ptr(device_info_hndl, byte_pool_udp);
	UDP_UART_DBG("[dev_info] status, status %d\n", status);
	status = qapi_Device_Info_Set_Callback_v2(device_info_hndl, QAPI_DEVICE_INFO_NETWORK_IND_E, udp_network_indication_cb);
	UDP_UART_DBG("[dev_info] qapi_Device_Info_Set_Callback, status %d\n", status);

	nw_sig_mask = NW_SIG_EVT_UNENABLE | NW_SIG_EVT_ENABLE;
	while(1)
	{
		tx_event_flags_get(udp_nw_signal_handle, nw_sig_mask, TX_OR, &nw_event, TX_WAIT_FOREVER);

		if (nw_event & NW_SIG_EVT_UNENABLE)
		{
			UDP_UART_DBG("waiting for network registering...\n");
			tx_event_flags_set(udp_nw_signal_handle, ~NW_SIG_EVT_UNENABLE, TX_AND);
		}
		else if (nw_event & NW_SIG_EVT_ENABLE)
		{
			UDP_UART_DBG("network registered!\n");
			tx_event_flags_set(udp_nw_signal_handle, ~NW_SIG_EVT_ENABLE, TX_AND);
			break;
		}
	}
#endif /* NW_SIGNAL_ENABLE */
	
	/* Create event signal handle and clear signals */
	txm_module_object_allocate(&udp_signal_handle, sizeof(TX_EVENT_FLAGS_GROUP));
	tx_event_flags_create(udp_signal_handle, "dss_signal_event");
	tx_event_flags_set(udp_signal_handle, 0x0, TX_AND);

	/* Start DSS thread, and detect iface status */
#ifdef QAPI_TXM_MODULE
	if (TX_SUCCESS != txm_module_object_allocate((VOID *)&dss_thread_handle, sizeof(TX_THREAD))) 
	{
		return -1;
	}
#endif
	ret = tx_thread_create(dss_thread_handle, "UDPCLINET DSS Thread", quec_dataservice_thread, NULL,
							udp_dss_stack, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
	if (ret != TX_SUCCESS)
	{
		IOT_INFO("Thread creation failed\n");
	}

	sig_mask = DSS_SIG_EVT_INV_E | DSS_SIG_EVT_NO_CONN_E | DSS_SIG_EVT_CONN_E | DSS_SIG_EVT_EXIT_E;
	while (1)
	{
		/* UDPClient signal process */
		tx_event_flags_get(udp_signal_handle, sig_mask, TX_OR, &dss_event, TX_WAIT_FOREVER);
		UDP_UART_DBG("SIGNAL EVENT IS [%d]\n", dss_event);
		
		if (dss_event & DSS_SIG_EVT_INV_E)
		{
			UDP_UART_DBG("DSS_SIG_EVT_INV_E Signal\n");
			tx_event_flags_set(udp_signal_handle, ~DSS_SIG_EVT_INV_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_NO_CONN_E)
		{
			UDP_UART_DBG("DSS_SIG_EVT_NO_CONN_E Signal\n");
			tx_event_flags_set(udp_signal_handle, ~DSS_SIG_EVT_NO_CONN_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_CONN_E)
		{
			UDP_UART_DBG("DSS_SIG_EVT_CONN_E Signal\n");

			/* Create a udp client and comminucate with server */
			start_udp_session();
          tx_event_flags_set(udp_signal_handle, ~DSS_SIG_EVT_CONN_E, TX_AND);
		}
		else if (dss_event & DSS_SIG_EVT_EXIT_E)
		{
			UDP_UART_DBG("DSS_SIG_EVT_EXIT_E Signal\n");
			tx_event_flags_set(udp_signal_handle, ~DSS_SIG_EVT_EXIT_E, TX_AND);
			tx_event_flags_delete(udp_signal_handle);
			break;
		}
		else
		{
			UDP_UART_DBG("Unknown Signal\n");
		}

		/* Clear all signals and wait next notification */
		tx_event_flags_set(udp_signal_handle, 0x0, TX_AND);	//@Fixme:maybe not need
	}
	
	UDP_UART_DBG("Quectel UDP Client Demo is Over!");
	return 0;
}
#endif /*__EXAMPLE_UDPCLINET__*/
/* End of Example_network.c */
