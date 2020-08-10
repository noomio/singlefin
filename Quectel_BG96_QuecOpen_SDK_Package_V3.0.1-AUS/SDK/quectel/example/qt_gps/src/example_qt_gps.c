/******************************************************************************
*@file    example_qt_gps.c
*@brief   example of gps operation (Quectel QAPIs for GNSS)
*
*  ---------------------------------------------------------------------------
*
*  Copyright (c) 2018 Quectel Technologies, Inc.
*  All Rights Reserved.
*  Confidential and Proprietary - Quectel Technologies, Inc.
*  ---------------------------------------------------------------------------
*******************************************************************************/
#if defined(__EXAMPLE_QT_GPS__)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"
#include <locale.h>

#include "qapi_fs_types.h"
#include "qapi_uart.h"
#include "qapi_diag.h"

#include "quectel_utils.h"
#include "qapi_location.h"
#include "txm_module.h"
#include "quectel_uart_apis.h"
#include "qapi_quectel.h"
#include "example_qt_gps.h"
#include "qapi_timer.h"


/**************************************************************************
*								  GLOBAL
***************************************************************************/
TX_EVENT_FLAGS_GROUP* nmea_report_signal;
/* uart rx tx buffer */
qt_nmea_sentence nmea_data;

static char tx_buff[1024];
QT_UART_CONF_PARA uart_conf =
{
	NULL,
	QT_UART_PORT_02,
	tx_buff,
	sizeof(tx_buff),
	NULL,
	NULL,
	115200
};

/**************************************************************************
*                                 FUNCTION
***************************************************************************/




void quectel_loc_nmea_cb(char *nmea_data)
{
    qt_uart_dbg(uart_conf.hdlr, "%s", (char*)nmea_data);    // print is no need delay time
}


int quectel_task_entry(void)
{
	qapi_Status_t status = 0;
	int ret = -1;
	ULONG sig_event = 0;
	
	// MUST SETTING,TBD
	setlocale(LC_ALL, "C");
	
    uart_init(&uart_conf);
    qt_uart_dbg(uart_conf.hdlr,"Example for GPS by Quectel QAPIs");
    status = qapi_QT_Loc_Start(QT_LOC_EVENT_MASK_NMEA, quectel_loc_nmea_cb, &nmea_data);
    qt_uart_dbg(uart_conf.hdlr,"START status %d", status);
    while(1)
    {
        qapi_Timer_Sleep(1, QAPI_TIMER_UNIT_SEC, true);
    }
}

#endif /*__EXAMPLE_QT_GPS__*/

