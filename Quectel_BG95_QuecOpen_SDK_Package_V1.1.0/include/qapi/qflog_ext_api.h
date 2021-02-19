#ifndef QFLOG_EXT_API_H
#define QFLOG_EXT_API_H

/*===========================================================================
                         QFLOG_EXT_API_H

DESCRIPTION
  This header file contains API's which are exposed to external tasks/applications
  from QFLOG task

  Copyright (c)2018 - 2019 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
09/16/2016   sr     created file
==============================================================================*/
#include "comdef.h"
#include "qflog_utils.h"

void QFLOG_Printf(const uint8_t *format, ...);
void QFLOG_Printf_v2(qflog_App_Handler_t handle, const char *format, ...);

#endif

