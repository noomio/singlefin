/*===================================================================
Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===================================================================*/

/** @file qapi_QTEEInvoke.h
 *
 * @brief Client API used to get the opener object.
*/

#ifndef _TZCOM_H_
#define _TZCOM_H_

#include "object.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup qapi_qtee_invoke
@{ */

/**
 * Function used by clients to create an IOpener object. The call process is as follows:
 *
 * @code{.c}
 * #include <object.h>
 * #include <stdint.h>
 *
 * Object openerObj = Object_NULL;
 * Object myTAObj = Object_NULL;
 * int32_t err = Object_OK;
 * int32_t out = 0;
 *
 * // Start the TA you desire
 * struct QSEECom_handle *clnt_handle = NULL;
 * err = qapi_QSEECom_start_app(&clnt_handle, "/firmware/image", "my_ta", 0);
 *
 * // Get the opener object to open TA services with
 * err = qapi_QTEEInvoke_GetOpener(&openerObj);
 *
 * // Open an object referencing the TA service of choice
 * err = qapi_IOpener_open(openerObj, CMyTA_UID, &myTAObj);
 * 
 * // Call TA method using RPC
 * err = IMyTA_add(myTAObj, 5, 10, &out);
 *
 * // Release
 * IMyTA_release(myTAObj);
 * qapi_IOpener_release(openerObj);
 *
 * // Unload TA
 * err = qapi_QSEECom_shutdown_app(&clnt_handle);
 * @endcode
 *
 * @param[out] ClientOpenerObj  Object to be used with IOpener.
 *
 * @return 0 on success; -1 on failure.
 */
int qapi_QTEEInvoke_GetOpener (Object *ClientOpenerObj);

/** @} */ /* end_addtogroup qapi_qtee_invoke */

#ifdef __cplusplus
}
#endif

#endif

