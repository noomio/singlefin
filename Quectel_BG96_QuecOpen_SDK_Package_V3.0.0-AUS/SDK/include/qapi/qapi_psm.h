#ifndef QAPI_PSM_H
#define QAPI_PSM_H 

/**
  @file qapi_psm.h
  @brief    PSM header for dynamic modules to interact with underlying PSM Daemon(Server).

  @details  Refer to psm_test.c for example usage of PSM with DAM

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
 * 11/04/18   rkr     Add comments for backoff
 * 15/01/18   leo     (Tech Comm) Edited/added Doxygen comments and markup.
 * 13/11/17   rkr      Modify PSM Register for MMU Compliance
 * 04/09/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
 * 26/03/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
 * 17/03/17   rkr      Initial PSM QAPI
 ======================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "qapi_ril_base.h"

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

typedef unsigned short word;

/** @addtogroup qapi_psm_types
@{ */

/** PSM's version of the Julian time structure.
*/
typedef struct
{
   word year;            /**< Year [1980 to 2100]. */
   word month;           /**< Month of the year [1 to 12]. */
   word day;             /**< Day of the month [1 to 31]. */
   word hour;            /**< Hour of the day [0 to 23]. */
   word minute;          /**< Minute of the hour [0 to 59]. */
   word second;          /**< Second of the minute [0 to 59]. */
   word day_of_week;     /**< Day of the week [0 to 6]; Monday through Sunday. */
} pm_rtc_julian_type;

/** @} */ /* end_addtogroup qapi_psm_types */

#else

#include "pmapp_rtc.h"

#endif


#include "qapi_psm_types.h"


// Driver ID defines
#define    TXM_QAPI_PSM_CLIENT_REGISTER      TXM_QAPI_PSM_BASE + 1
#define    TXM_QAPI_PSM_CLIENT_UNREGISTER    TXM_QAPI_PSM_BASE + 2
#define    TXM_QAPI_PSM_CLIENT_ENTER_PSM     TXM_QAPI_PSM_BASE + 3
#define    TXM_QAPI_PSM_CLIENT_ENTER_BACKOFF TXM_QAPI_PSM_BASE + 4
#define    TXM_QAPI_PSM_CLIENT_CANCEL_PSM    TXM_QAPI_PSM_BASE + 5
#define    TXM_QAPI_PSM_CLIENT_LOAD_MODEM    TXM_QAPI_PSM_BASE + 6
#define    TXM_QAPI_PSM_CLIENT_HC_ACK        TXM_QAPI_PSM_BASE + 7

// Userspace callback ID defines
#define    TXM_QAPI_PSM_CLIENT_CB_FUNC       TXM_QAPI_PSM_CB_BASE + 1

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

void psm_user_space_dispatcher(UINT cb_id, void *app_cb,
                                      UINT cb_param1, UINT cb_param2,
                                      UINT cb_param3, UINT cb_param4);

#define    qapi_PSM_Client_Register(a,b,c)    ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_REGISTER       , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) psm_user_space_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Unregister(a)      ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_UNREGISTER     , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Enter_Psm(a,b)     ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_ENTER_PSM      , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Enter_Backoff(a)   ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_ENTER_BACKOFF  , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Cancel_Psm(a)      ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_CANCEL_PSM     , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Load_Modem(a)      ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_LOAD_MODEM     , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define    qapi_PSM_Client_Hc_Ack(a)          ((qapi_PSM_Status_t) (_txm_module_system_call12)(TXM_QAPI_PSM_CLIENT_HC_ACK         , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else

qapi_Status_t qapi_PSM_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/** @addtogroup qapi_psm_apis
@{ */

/*============================================================================
                qapi_PSM_Client_Register
============================================================================*/
/**
 *  @brief     Makes the application known to the PSM server as a PSM-aware application.
 *             This is the first API every PSM-aware application is to call.
 *             Every application that needs network related-functionality 
 *             must call this API.
 *
 *  @details Registering a client enabled the PSM-aware application to vote
 *           for the PSM time and readiness when required. The callback is used by
 *           the PSM server to inform the application of all PSM events.
 *           A maximum of 20 clients can be registered at a time with server.
 *
 *  @param[out] client_id Pointer to the stored ID (as an integer) of the registered client.
 *  @param[in] cb_func Callback function of type psm_client_cb_type. The server
 *                     invokes this function to notify the client of PSM events.
 *                     PSM events contain status and reason. See
 *                     #psm_status_type_e and #psm_reject_reason_type_e.
 *  @param[in] cb_msg  Callback message of type psm_status_msg_type passed when
 *                     the sent callback is invoked. This message contains the
 *                     ID, status, and reason. See #psm_status_msg_type, 
 *                     #psm_status_type_e, and #psm_reject_reason_type_e.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure. \n
 *  - QAPI_ERR_PSM_WRONG_ARGUMENTS -- One or more of the arguments are invalid or NULL.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Registration failed because the maximum
 *                                    client limit of 20 was exceeded.
 *  - QAPI_ERR_ESPIPE -- Some file descriptors (like pipes and FIFOs) are not seekable.
 */
qapi_Status_t qapi_PSM_Client_Register(int32_t *client_id, psm_client_cb_type cb_func, psm_status_msg_type* cb_msg);

/*============================================================================
                qapi_PSM_Client_Unregister
============================================================================*/
/**
 *  @brief Unregisters the PSM-aware application with the PSM server.
 *         Callbacks registered with the server by the application will no longer
 *         be used to send any messages by the server.
 *
 *  @details Unregistered applications cannot vote for PSM. Reregistration
 *           can be done using the qapi_PSM_Client_Register() call.
 *           Unregistered PSM-aware applications should be prepared for device
 *           shutdown without any further information.
 *
 *  @param[in] client_id Client ID obtained during registration.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure. \n
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
qapi_Status_t qapi_PSM_Client_Unregister(int32_t client_id);

/*============================================================================
                qapi_PSM_Client_Enter_Psm
============================================================================*/
/**
 *  @brief Used by the application to indicate its intent to enter PSM mode.
 *
 *  @details The application must pass active_time in seconds, time in PSM mode,
 *           and whether the next wake up is for measurement purposes or access
 *           to the network.
 *           PSM time can be accepted in either broken down format or in seconds.
 *           A PSM-aware application blocks PSM entry if this API is not 
 *           called indefinitely.
 *
 *  @param[in] client_id Client ID obtained during registration.
 *  @param[in] psm_info  Pointer to a psm_info_type structure consisting of
 *             active time, the next wakeup time (time in PSM), and the next
 *             wakeup type. Based on the wakeup type, the server decides whether
 *             to load the modem as part of bootup.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure.	\n
 *  - QAPI_ERR_PSM_WRONG_ARGUMENTS -- One or more of the arguments are invalid or NULL.
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
qapi_Status_t qapi_PSM_Client_Enter_Psm(int32_t client_id, psm_info_type *psm_info);

/*============================================================================
                qapi_PSM_Client_Enter_Backoff
============================================================================*/
/**
 *  @brief Used by the application to indicate its intent to enter PSM mode
 *         due to a network out-of-service state or if the MTC server is not reachable.
 *         MTC server refers to any entity that the PSM Client tries to communicate over 
 *         the Network.
 *
 *  @details The duration for which the application wants to enter PSM mode is 
 *           decided by the PSM server based on the NV item configuration
 *           NV73784 (psm_duration_due_to_oos).
 *           In a case where there is no PSM-aware application registered, 
 *           the server sets the device to the PSM state independently.
 *           PSM aware can even decide to use the Enter PSM API with the
 *           intended time on recieving such status indications.
 *
 *           Call to backoff will over write any valid PSM vote of the same client.
 *           Calling backoff when there is no network out-of-service will be treated
 *           as a vote to PSM with a default time as set in NV73784.
 *
 *  @param[in] client_id Client ID obtained during registration.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure.	\n
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
 qapi_Status_t qapi_PSM_Client_Enter_Backoff(int32_t client_id);

/*============================================================================
                qapi_PSM_Client_Cancel_Psm
============================================================================*/
/**
 *  @brief Cancels a previous request to enter PSM.
 * 
 *
 *  @param[in] client_id Client ID obtained during registration.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure. \n
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
qapi_Status_t qapi_PSM_Client_Cancel_Psm(int32_t client_id);

/*============================================================================
                qapi_PSM_Client_Load_Modem
============================================================================*/
/**
 *  @brief Requests the PSM server to load the modem if it is not already 
 *         loaded (PIL-based flavors only).
 *
 *  @details PSM-aware applications can load the modem dynamically based on
 *           the use case to save power. Applications are informed through
 *           the callback of the modem loading success/failure.
 *           Further, applications can vote for modem loading in the next
 *           bootup through the qapi_PSM_Client_Enter_Psm() call.
 *
 *  @param[in] client_id Client ID obtained during registration.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure.	\n
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
qapi_Status_t qapi_PSM_Client_Load_Modem(int32_t client_id);

/*============================================================================
                qapi_PSM_Client_Hc_Ack
============================================================================*/
/**
 *  @brief Application health check acknowledge API. 
 *         PSM-aware applications must call this API every time it receives 
 *         a PSM_STATUS_HEALTH_CHECK event.
 *
 *  @details This API ensures that every registered PSM-aware application is
 *           alive and functioning, and not stuck in a deadlock situation.
 *           Periodically, the PSM server uses the callback to send a
 *           PSM_STATUS_HEALTH_CHECK event. The application must call this API
 *           to acknowledge that the application is working.
 *           On failing to respond to Health Check, the application is treated
 *           as a dead application and the server votes for PSM on behalf of
 *           the dead application.
 *
 *           Time in PSM is as configured in NV setting
 *           NV73784 (psm_duration_due_to_oos).
 *
 *  @param[in] client_id Client ID obtained during registration.
 *
 *  @return  Returns QAPI_OK on success or a -ve error code on failure.	 \n
 *  - QAPI_ERR_PSM_APP_NOT_REGISTERED -- Invalid client ID.
 *  - QAPI_ERR_PSM_GENERIC_FAILURE -- Communication with the server failed.
 */
qapi_Status_t qapi_PSM_Client_Hc_Ack(int32_t client_id);

/** @} */ /* end_addtogroup qapi_psm_apis */

#endif

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QAPI_PSM_H */


