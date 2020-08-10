/******************************************************************************

				Q U E C T E L _ U T I L S . H

******************************************************************************/
#ifndef __QUECTEL_UTILS_IF_H__
#define __QUECTEL_UTILS_IF_H__

#include "stdio.h"
#include "string.h"
#include "stdarg.h"
#include "qapi_diag.h"

#define QUECTEL_IOT_DEBUG

void quectel_format_log_msg
(
  char *buf_ptr,
  int buf_size,
  char *fmt,
  ...
);

/* Maximum length of log message */
#define MAX_DIAG_LOG_MSG_SIZE       512

/* Log message to Diag */
#define QUECTEL_LOG_MSG_DIAG( lvl, ... )  \
{  \
	char buf[MAX_DIAG_LOG_MSG_SIZE];  \
	/* Format message for logging */  \
	quectel_format_log_msg(buf, MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__);  \
	/* Log message to Diag */  \
	QAPI_MSG_SPRINTF(MSG_SSID_LINUX_DATA, lvl, "[DAM]%s", buf); \
}

#ifdef QUECTEL_IOT_DEBUG
/**
 *    @brief Info level logging macro.
 *
 *    Macro to expose desired log message.  Info messages do not include automatic function names and line numbers.
 */
#define IOT_INFO(...)  QUECTEL_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__)

/**
 *    @brief Debug level logging macro.
 *
 *     Macro to expose function, line number as well as desired log message.
 */
#define IOT_DEBUG(...) QUECTEL_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

#else

#define IOT_DEBUG(...)
#define IOT_INFO(...)

#endif /* QUECTEL_IOT_DEBUG */

#endif /* __QUECTEL_UTILS_IF_H__ */

