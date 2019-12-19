/* Copyright (c) 2017-2018 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 * @file qapi_driver_access.h
 */
/*
 * This file provides helper functions which can access list to store user
   space pointer details
 *
 */

#ifndef _QAPI_DRIVER_ACCESS_H
#define _QAPI_DRIVER_ACCESS_H

#include <stdint.h>
#include "qapi_data_txm_base.h"
#include "qapi_status.h"

/* Data driver REQUEST ID DEFINES */
#define  TXM_DATA_MAP_U_ADDR_TO_HANDLE TXM_DATA_DRIVER_BASE+ 1
#define  TXM_DATA_MAP_HANDLE_TO_U_ADDR TXM_DATA_DRIVER_BASE+2


#define DATA_CUSTOM_CB_DSS_SRVC_HNDL         CUSTOM_CB_QAPI_DATA_SERVICES_BASE+1
#define DATA_CUSTOM_CB_MQTT_SET_CONNECT      CUSTOM_CB_QAPI_DATA_SERVICES_BASE+2
#define DATA_CUSTOM_CB_MQTT_SET_SUBSCRIBE    CUSTOM_CB_QAPI_DATA_SERVICES_BASE+3
#define DATA_CUSTOM_CB_MQTT_SET_MESSAGE      CUSTOM_CB_QAPI_DATA_SERVICES_BASE+4
#define DATA_CUSTOM_CB_MQTT_SET_PUBLISH      CUSTOM_CB_QAPI_DATA_SERVICES_BASE+5
#define DATA_CUSTOM_CB_HTTP_NEW_SESS         CUSTOM_CB_QAPI_DATA_SERVICES_BASE+6
#define DATA_CUSTOM_CB_LWM2M_REG_APP_EXT     CUSTOM_CB_QAPI_DATA_SERVICES_BASE+7
#define DATA_CUSTOM_CB_LWM2M_OBSERVE         CUSTOM_CB_QAPI_DATA_SERVICES_BASE+8
#define DATA_CUSTOM_CB_ATFWD_REG             CUSTOM_CB_QAPI_DATA_SERVICES_BASE+9
#define DATA_CUSTOM_CB_CLI_CMD               CUSTOM_CB_QAPI_DATA_SERVICES_BASE+10
#define DATA_CUSTOM_CB_LWM2M_OBSERVE_V2      CUSTOM_CB_QAPI_DATA_SERVICES_BASE+11
#define DATA_CUSTOM_CB_SET_DEVICE_INFO       CUSTOM_CB_QAPI_DATA_SERVICES_BASE+12


/** @addtogroup qapi_driver_access
@{ */

/*
 * App Type to be used for MMU Translation
 *
*/
typedef enum{

   QAPI_APP_DSS = 0,
   QAPI_APP_MQTT,
   QAPI_APP_LWM2M,
   QAPI_APP_HTTP,
   QAPI_APP_SSL,
   QAPI_APP_NWSTACK,
   QAPI_APP_NIPD,
   QAPI_APP_ATFWD,
   QAPI_APP_DEVICE_INFO,
   QAPI_APP_CLI
   
}qapi_data_app_type;

/** @cond */

struct qapi_cb_params_uspace_ptr_s
{
  void * cb_param[8];
  int cb_count;
  void *usr_pool_ptr;
};

 typedef struct qapi_cb_params_uspace_ptr_s qapi_cb_params_uspace_ptr_t;

/** @endcond */

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS
 
#define qapi_data_map_u_addr_to_handle(a,b,c,d)   ((qapi_Status_t) (_txm_module_system_call12)(TXM_DATA_MAP_U_ADDR_TO_HANDLE  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d , (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_data_map_handle_to_u_addr(a,b,c)     ((qapi_Status_t) (_txm_module_system_call12)(TXM_DATA_MAP_HANDLE_TO_U_ADDR  , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0 , (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else


/**
 * @brief Utility API to map a user space address to the handle. Only 
 *        needs to be used in the DAM Application space.
 *
 * @param[in] handle Networking stack module handle.
 * @param[in] app_type Identifies the networking stack module. 
 * @param[in] u_addr User space address pointer.
 * @param[in] addr_len Size of the user space address that is allocated.
 *
 * @return QAPI_OK on success or <0 on failure.
 */
qapi_Status_t qapi_data_map_u_addr_to_handle(void * handle, unsigned int  app_type , void * u_addr , unsigned int addr_len);

/**
 * @brief Utility API to retrieve the user space address corresponding to 
 *        the handle. Only needs to be used in the DAM Application space.
 *
 * @param[in] handle Networking stack module handle.
 * @param[in] app_type Identifies the networking stack module. 
 * @param[out] u_addr User space address pointer mapped to the handle.
 *
 * @return QAPI_OK on success or <0 on failure.
 */
qapi_Status_t qapi_data_map_handle_to_u_addr(void * handle, unsigned int app_type , void ** u_addr);

UINT data_driver_access_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

#endif

#endif

/** @} */ /* end_addtogroup qapi_driver_access */
