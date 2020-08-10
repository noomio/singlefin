/******************************************************************************
*@file    example_usb_func.c
*@brief   example of usb function enable or disable
*
*  ---------------------------------------------------------------------------
*
*  Copyright (c) 2018 Quectel Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Quectel Technologies, Inc.
*  ---------------------------------------------------------------------------
*******************************************************************************/
#if defined(__EXAMPLE_USB_FUNCTION__)
/**************************************************************************
*                                 INCLUDE
***************************************************************************/
#include "qapi_uart.h"
#include "qapi_timer.h"
#include "qapi_diag.h"
#include "quectel_utils.h"
#include "quectel_uart_apis.h"

#include "qapi_device_info.h"
#include "qapi_quectel.h"
#include "example_usb_function.h"

#include <locale.h>

#define True 1

/**************************************************************************
*                                 GLOBAL
***************************************************************************/
TX_BYTE_POOL *byte_pool_uart;
#define UART_BYTE_POOL_SIZE		10*8*1024
UCHAR free_memory_uart[UART_BYTE_POOL_SIZE];


/* uart rx tx buffer */
static char *rx_buff = NULL; /*!!! should keep this buffer as 4K Bytes */
static char *tx_buff = NULL;

/* uart config para*/
QT_UART_CONF_PARA uart_conf = 
{
	NULL,
	QT_UART_PORT_02,
	NULL,
	0,
	NULL,
	0,
	115200
};

/* conter used to count the total run times*/
unsigned long task_run_couter = 0;


/**************************************************************************
*                                 FUNCTION
***************************************************************************/

/*
@func
  quectel_task_entry
@brief
  Entry function for task. 
*/
int quectel_task_entry(void)
{
	int ret = -1;
	qapi_Status_t status=QAPI_QT_ERR_OK;

	IOT_DEBUG("QT# quectel_task_entry Start");
	unsigned char val;
	setlocale(LC_ALL, "C");

	ret = txm_module_object_allocate(&byte_pool_uart, sizeof(TX_BYTE_POOL));
  	if(ret != TX_SUCCESS)
  	{
  		IOT_DEBUG("txm_module_object_allocate [byte_pool_sensor] failed, %d", ret);
    	return ret;
  	}

	ret = tx_byte_pool_create(byte_pool_uart, "Sensor application pool", free_memory_uart, UART_BYTE_POOL_SIZE);
  	if(ret != TX_SUCCESS)
  	{
  		IOT_DEBUG("tx_byte_pool_create [byte_pool_sensor] failed, %d", ret);
    	return ret;
  	}
  	ret = tx_byte_allocate(byte_pool_uart, (VOID *)&rx_buff, 4*1024, TX_NO_WAIT);
  	if(ret != TX_SUCCESS)
  	{
  		IOT_DEBUG("tx_byte_allocate [rx_buff] failed, %d", ret);
    	return ret;
  	}

  	ret = tx_byte_allocate(byte_pool_uart, (VOID *)&tx_buff, 4*1024, TX_NO_WAIT);
  	if(ret != TX_SUCCESS)
  	{
  		IOT_DEBUG("tx_byte_allocate [tx_buff] failed, %d", ret);
    	return ret;
  	}

	uart_conf.tx_buff = tx_buff;
	uart_conf.rx_buff = rx_buff;
	uart_conf.tx_len = 4096;
	uart_conf.rx_len = 4096;

	/* uart  init 			*/
	uart_init(&uart_conf);
	/* start uart  receive */
	uart_recv(&uart_conf);
	/* prompt task running 	*/
	qt_uart_dbg(uart_conf.hdlr,"\r\n[uart] USB Function Example %d", UART_BYTE_POOL_SIZE/1024);

	/* disable USB	*/
	qapi_QT_USB_Mode_Set(1) ; 

	status=qapi_QT_USB_Switch_Get(&val);
	qt_uart_dbg(uart_conf.hdlr," qapi_QT_USB_Switch_Get status: %x", status);
	if(status==QAPI_QT_ERR_OK)
	{
		if(val==0)
		{
			status=qapi_QT_USB_Switch_Set(True)	;
			qt_uart_dbg(uart_conf.hdlr," qapi_QT_USB_Switch_Set status: %x", status);		
		}

	}
	while (1)
	{
		task_run_couter ++;

		qt_uart_dbg(uart_conf.hdlr,"[uart] task run times : %d", task_run_couter);

		IOT_DEBUG("QT# quectel_task_entry [%d]", task_run_couter);
		/* sleep 1 seconds */
		qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);

		//After 24 hour, reset the module;
		if(task_run_couter >= 86400)
		{
			//qapi_Device_Info_Reset();
		}
	}
}

#endif /*__EXAMPLE_USB_FUNCTION__*/

