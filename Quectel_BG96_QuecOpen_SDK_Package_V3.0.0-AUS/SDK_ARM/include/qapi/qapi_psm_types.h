/**
 * @file qapi_psm_types.h
 *
 * @brief Type definitions for PSM Client QAPIs.
*/

/*
 * Copyright (C) 2017-2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2018-04-11   rkr    Fix doxygen comment in psm status type
2018-01-15   leo    (Tech Comm) Edited/added Doxygen comments and markup.
2017-04-25   leo    (Tech Comm) Edited/added Doxygen comments and markup.
2017-03-17   rkr    Create.

===========================================================================*/


#ifndef QAPI_PSM_TYPES_H
#define QAPI_PSM_TYPES_H

#include "qapi_status.h"


/*==============================================================================
                              MACROS
=============================================================================*/


/*=============================================================================
                                   ENUMS
=============================================================================*/

/** @addtogroup qapi_psm_types
    @{
  */

/** Enumeration of status types. */
typedef enum {
    PSM_STATUS_NONE,
    PSM_STATUS_REJECT,            /**< PSM enter request is rejected */
    PSM_STATUS_READY,             /**< Ready to enter PSM mode. */
    PSM_STATUS_NOT_READY,         /**< Not ready to enter PSM. */
    PSM_STATUS_COMPLETE,          /**< Entered PSM mode; the system might shut down at any time. */
    PSM_STATUS_DISCONNECTED,      /**< PSM server is down. */
    PSM_STATUS_MODEM_LOADED,      /**< Modem is loaded as part of bootup. */
    PSM_STATUS_MODEM_NOT_LOADED,  /**< Modem is not loaded as part of bootup.*/
    PSM_STATUS_NW_OOS,            /**< Network is OOS. */
    PSM_STATUS_NW_LIMITED_SERVICE,/**< Network is in Limited Service state. */
    PSM_STATUS_HEALTH_CHECK,      /**< Application health check.*/
    PSM_STATUS_FEATURE_ENABLED,   /**< Feature is dynamically enabled.*/
    PSM_STATUS_FEATURE_DISABLED,  /**< Feature is dynamically disabled.*/
    PSM_STATUS_MAX
} psm_status_type_e;

/** Enumeration of reasons for rejection. */
typedef enum {
    PSM_REJECT_REASON_NONE,                /**< No reject reason. */
    PSM_REJECT_REASON_NOT_ENABLED,         /**< PSM feature is not enabled. */
    PSM_REJECT_REASON_MODEM_NOT_READY,     /**< Modem is not ready to enter PSM mode. */
    PSM_REJECT_REASON_DURATION_TOO_SHORT,  /**< PSM duration is too short to enter PSM mode. */
    PSM_REJECT_REASON_INCORRECT_USAGE,     /**< PSM usage is wrong. Incorrect voting,
                                                re-vote before cancel, and other aspects
                                                will be rejected with this message. */
    PSM_REJECT_REASON_MAX
} psm_reject_reason_type_e;

/** Enumeration of PSM error types. */
typedef enum {
    PSM_ERR_NONE,                  /**< Success. */
    PSM_ERR_FAIL,                  /**< Failure. */
    PSM_ERR_GENERIC_FAILURE,       /**< Miscellaneous failure.*/
    PSM_ERR_APP_NOT_REGISTERED,    /**< Application is not registered with the PSM server. */
    PSM_ERR_WRONG_ARGUMENTS,       /**< Wrong input arguments.*/
    PSM_ERR_IPC_FAILURE,           /**< Failure to communicate with the PSM server. */
    PSM_ERR_INVALID_ACTIVE_TIME   /**< Invalid active time value passed.
                                       Refer to 3GPP TS 24.008, Table 10.5.172 and
                                       Table 10.5.163a for valid active time values. */
} psm_error_type_e;

/** PSM time format. */
typedef enum {
    PSM_TIME_IN_TM,        /**< Specify time in broken down format.*/
    PSM_TIME_IN_SECS       /**< Specify time in seconds.*/
} psm_time_format_type_e;


/** PSM wakeup type. */
typedef enum {
    PSM_WAKEUP_MEASUREMENT_ONLY,        /**< Next wake up from PSM is for measurement purpose only. */
    PSM_WAKEUP_MEASUREMENT_NW_ACCESS    /**< Next wake up from PSM is for measurement and network access. */
} psm_wakeup_type_e;


/*============================================================================
                                  STRUCTURES
============================================================================*/

/** PSM time information. */
typedef struct {
    psm_time_format_type_e  time_format_flag;       /**< Time format. See #psm_time_format_type_e. */
    pm_rtc_julian_type      wakeup_time;            /**< Time in broken down format if the time_format_flag is set to PSM_TIME_IN_TM. */
    int                     psm_duration_in_secs;   /**< Time in seconds if the time_format_flag is set to PSM_TIME_IN_SECS. */
}psm_time_info_type;


/** PSM information type. */
typedef struct {
    int                 active_time_in_secs;  /**< Active time is the duration the PSM server
                                                   must wait before entering PSM mode.
                                                   The purpose of this time is to provide a
                                                   chance for the MTC server to react. */
    psm_wakeup_type_e   psm_wakeup_type;      /**< Next wake up from PSM mode is for
    measurement purpose or measurement and network access. */
    psm_time_info_type  psm_time_info;        /**< PSM time information. See #psm_time_info_type. */
}psm_info_type;


/** PSM status message type. */
typedef struct  {
    int   client_id; /**< Client ID. */
    int   status;    /**< PSM status. See #psm_status_type_e. */
    int   reason;    /**< PSM reject reason. See #psm_reject_reason_type_e.*/
}psm_status_msg_type;


/*======================================================================
                          TYPEDEFS
 ======================================================================*/

/** PSM status callback type. */
typedef void (*psm_client_cb_type)(psm_status_msg_type *);

/** PSM timer expiry callback type. */
typedef void (*psm_util_timer_expiry_cb_type)(void *, size_t);

typedef int32_t qapi_PSM_Status_t;

/** @} */

#endif /** QAPI_PSM_TYPES_H*/

