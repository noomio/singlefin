#ifndef QAPI_PSM_STATUS_H
#define QAPI_PSM_STATUS_H 

/**
  @file qapi_psm_status.h
  @brief    PSM header for PSM Client status messages.

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/
/*=======================================================================
 Copyright (c) 2017-2018  Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 * when       who     what, where, why
 * --------   ---     ----------------------------------------------------------
 * 01/15/18   leo     (Tech Comm) Edited/added Doxygen comments and markup.
 * 17/03/17   rkr      Initial PSM QAPI
 ======================================================================*/

#include "qapi_types.h"
#include "qapi_psm_types.h"

/** @addtogroup qapi_psm_types
@{ */

typedef int32_t qapi_PSM_Status_t;

#define __QAPI_PSM_ERROR(x)  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_RIL,  x)))


/*=============================================================================
                                   PSM Client Status Messages
=============================================================================*/

/** @name PSM Client Status Messages
@{ */

#define QAPI_ERR_PSM_FAIL                    __QAPI_PSM_ERROR(1)
/**< Failure or invalid operation (unused). */

#define QAPI_ERR_PSM_GENERIC_FAILURE         __QAPI_PSM_ERROR(2)
/**< Failure to send a request to the PSM Daemon. */

#define QAPI_ERR_PSM_APP_NOT_REGISTERED      __QAPI_PSM_ERROR(3)
/**< The client ID passed is not a registered application. */

#define QAPI_ERR_PSM_WRONG_ARGUMENTS         __QAPI_PSM_ERROR(4)
/**< NULL or invalid arguments were sent. */

#define QAPI_ERR_PSM_IPC_FAILURE             __QAPI_PSM_ERROR(5)
/**< Internal failure to establish communication with the PSM Daemon. */

#define QAPI_ERR_PSM_INVALID_ACTIVE_TIME     __QAPI_PSM_ERROR(6)
/**< An invalid active time was sent. */

/** @} */
#endif // ] #ifndef QAPI_PSM_STATUS_H

