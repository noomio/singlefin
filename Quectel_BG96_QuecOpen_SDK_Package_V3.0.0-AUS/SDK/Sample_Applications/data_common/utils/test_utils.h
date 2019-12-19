/*===========================================================================
                         TEST_UTILS.C

DESCRIPTION
   File contains the utilities for the DAM demo application 
  
   Copyright (c) 2017 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

#include "txm_module.h"
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qapi_diag.h"
#include "qflog_utils.h"

typedef  unsigned char      bool;
#define true   1   /**< Boolean TRUE value. */
#define false  0   /**< Boolean FALSE value. */

/* Maximum length of log message */
#define MAX_DIAG_LOG_MSG_SIZE       512

/* Log message to Diag */
#define DAM_LOG_MSG_DIAG( lvl, ... )  \
  { \
    char buf[ MAX_DIAG_LOG_MSG_SIZE ]; \
     \
    /* Format message for logging */  \
    dam_format_log_msg( buf, MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__ );\
    \
    /* Log message to Diag */  \
    QAPI_MSG_SPRINTF( MSG_SSID_LINUX_DATA, lvl, "%s", buf );  \
    \
    /* Log message to Terminal */  \
    QFLOG_MSG(MSG_SSID_DFLT, lvl, "%s", buf); \
  }

#define LOG_INFO(...) DAM_LOG_MSG_DIAG(MSG_LEGACY_HIGH, __VA_ARGS__)

#define LOG_DEBUG(...) DAM_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__)

#define LOG_ERROR(...) DAM_LOG_MSG_DIAG(MSG_LEGACY_ERROR, __VA_ARGS__)

#define LOG_FATAL(...) DAM_LOG_MSG_DIAG(MSG_LEGACY_FATAL, __VA_ARGS__)

qapi_Status_t dam_byte_pool_init(void);

void *data_malloc(uint32_t size);

void data_free(void *data);

void dam_format_log_msg
(
 char *buf_ptr,
 int buf_size,
 char *fmt,
 ...
);

UINT memscpy
(
  void   *dst,
  UINT   dst_size,
  const  void  *src,
  UINT   src_size
);

