/******************************************************************************

                        Q U E C T E L _ U T I L S . C

******************************************************************************/
/******************************************************************************

  @file    quectel_utils.c
  @brief   quectel common interface file

******************************************************************************/
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "qapi_diag.h"

void quectel_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
)
{
	va_list ap;

	/* validate input param */
	if( NULL == buf_ptr || buf_size <= 0)
	{
		QAPI_MSG_SPRINTF(MSG_SSID_LINUX_DATA,MSG_LEGACY_ERROR,
						 "[DAM ERR]Bad Param buf_ptr:[%p], buf_size:%d",
						 buf_ptr, buf_size);
		return;
	}

	va_start(ap, fmt);

	vsnprintf(buf_ptr, (size_t)buf_size, fmt, ap);

	va_end(ap);

} /* sen_format_log_msg */

