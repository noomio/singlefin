/**
  @file qapi_device_info.h

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/

/*
 Copyright (c) 2017-2018  Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *        EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	
 *
 *
 * when 		who 	what, where, why
 * ---------- 	---   ------------------------------------------------
 * 2018-01-11   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 * 2017-09-02   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 * 2017-04-25   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 ======================================================================*/
 
#ifndef _QAPI_DEVICE_INFO_H
#define _QAPI_DEVICE_INFO_H

#include "qapi_data_txm_base.h"
#include "qapi_status.h"
#include "qapi_driver_access.h"
#include "txm_module.h"
#include "stdbool.h"//Quectel add by hyman

#define  TXM_QAPI_DEVICE_INFO_INIT               TXM_QAPI_DEVICE_INFO_BASE + 1
#define  TXM_QAPI_DEVICE_INFO_GET                TXM_QAPI_DEVICE_INFO_BASE + 2
#define  TXM_QAPI_DEVICE_INFO_RELEASE            TXM_QAPI_DEVICE_INFO_BASE + 3
#define  TXM_QAPI_DEVICE_INFO_SET_CB             TXM_QAPI_DEVICE_INFO_BASE + 4
#define  TXM_QAPI_DEVICE_INFO_RESET              TXM_QAPI_DEVICE_INFO_BASE + 5

#define  TXM_QAPI_DEVICE_INFO_INIT2              TXM_QAPI_DEVICE_INFO_BASE_V2 + 1
#define  TXM_QAPI_DEVICE_INFO_GET2               TXM_QAPI_DEVICE_INFO_BASE_V2 + 2
#define  TXM_QAPI_DEVICE_INFO_RELEASE2           TXM_QAPI_DEVICE_INFO_BASE_V2 + 3
#define  TXM_QAPI_DEVICE_INFO_SET_CB2            TXM_QAPI_DEVICE_INFO_BASE_V2 + 4
#define  TXM_QAPI_DEVICE_INFO_RESET2             TXM_QAPI_DEVICE_INFO_BASE_V2 + 5


/** @addtogroup qapi_device_info
@{ */

/** Device information types. */
typedef enum {

 QAPI_DEVICE_INFO_BUILD_ID_E,
 /**< Device BUILD_ID. */
 QAPI_DEVICE_INFO_IMEI_E,
 /**< Device IMEI. */
 QAPI_DEVICE_INFO_IMSI_E,
 /**< UIM IMSI. */
 QAPI_DEVICE_INFO_OS_VERSION_E,
 /**< Device OS version. */
 QAPI_DEVICE_INFO_MANUFACTURER_E,
 /**< Device manufacturer. */
 QAPI_DEVICE_INFO_MODEL_ID_E,
 /**< Device model ID. */
 QAPI_DEVICE_INFO_BATTERY_STATUS_E,
 /**< Device battery status. */
 QAPI_DEVICE_INFO_BATTERY_PERCENTAGE_E,
 /**< Device battery percentage. */
 QAPI_DEIVCE_INFO_TIME_ZONE_E,
 /**< Device time zone. */
 QAPI_DEIVCE_INFO_ICCID_E,
 /**< Device ICCID. */
 QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E,
 /**< Network signal strength. */
 QAPI_DEVICE_INFO_BASE_STATION_ID_E,
 /**< Network base station ID. */
 QAPI_DEVICE_INFO_MCC_E,
 /**< Network MCC. */
 QAPI_DEVICE_INFO_MNC_E,
 /**< Network MNC. */
 QAPI_DEVICE_INFO_SERVICE_STATE_E,
 /**< Network service status. */
 QAPI_DEVICE_INFO_MDN_E,
 /**< Device MDN. */
 QAPI_DEVICE_INFO_TAC_E,
 /**< Network tracking area code. */
 QAPI_DEVICE_INFO_CELL_ID_E,
 /**< Network cell ID. */
 QAPI_DEVICE_INFO_RCCS_E,
 /**< Network RRC state. */
 QAPI_DEVICE_INFO_EMMS_E,
 /**< Network EMM state. */
 DEPRACATED1,
 /* Information to keep enum numbering consistent. */
 QAPI_DEVICE_INFO_SERVING_PCI_E,
 /**< Network serving cell PCI. */
 QAPI_DEVICE_INFO_SERVING_RSRQ_E,
 /**< Serving cell RSRQ. */
 QAPI_DEVICE_INFO_SERVING_EARFCN_E,
 /**< Serving cell EARFCN. */
 DEPRACATED2,
 /* Information to keep enum numbering consistent. */
 DEPRACATED3,
 /* Information to keep enum numbering consistent. */
 DEPRACATED4,
 /* Information to keep enum numbering consistent. */
 DEPRACATED5,
 /* Information to keep enum numbering consistent. */
 DEPRACATED6,
 /* Information to keep enum numbering consistent. */
 QAPI_DEVICE_INFO_NETWORK_IND_E,
 /**< Network indication. */
 QAPI_DEVICE_INFO_ROAMING_E,
 /**< Roaming status. */
 QAPI_DEVICE_INFO_LAST_POWER_ON_E,
 /**< Last power on time. */
 QAPI_DEVICE_INFO_CHIPID_STRING_E,
 /**< Chipset name. */
 QAPI_DEVICE_INFO_APN_PROFILE_INDEX_E,
 /**< APN profile index. */
 QAPI_DEVICE_INFO_SIM_STATE_E,
 /**< SIM state. */
 QAPI_DEVICE_INFO_NETWORK_BEARER_E,
 /**< Network bearer. */
 QAPI_DEVICE_INFO_LINK_QUALITY_E,
 /**< Network link quality. */
 QAPI_DEVICE_INFO_TX_BYTES_E,
 /**< Device Tx bytes. */
 QAPI_DEVICE_INFO_RX_BYTES_E,
 /**< Device Rx bytes. */
 QAPI_DEVICE_INFO_ANY,
 /**< Any device information. */
} qapi_Device_Info_ID_t;

/** Device information response types. */
typedef enum {
 QAPI_DEVICE_INFO_TYPE_BOOLEAN_E,
 /**< Response type is Boolean. */
 QAPI_DEVICE_INFO_TYPE_INTEGER_E,
 /**< Response type is integer. */
 QAPI_DEVICE_INFO_TYPE_BUFFER_E,
 /**< Response type is buffer. */
} qapi_Device_Info_Type_t;

/*========================================================
 * Device Info structure                               *
 ========================================================*/

/** Maximum size of #qapi_Device_Info_t valuebuf. */
#define QAPI_DEVICE_INFO_BUF_SIZE 128

#define JULIAN_YEAR 1980
#define DEVICE_INFO_MAX_INSTANCES 5


/**
 @brief QAPI device information structure.
*/
typedef struct {
  qapi_Device_Info_ID_t id; /**< Required information ID. */
  qapi_Device_Info_Type_t info_type; /**< Response type. */
  union {
    struct {
      char buf[QAPI_DEVICE_INFO_BUF_SIZE]; /**< Response buffer. */
      uint32_t len; /**< Length of the response string. */
    }valuebuf;    /**< Union of buffer values. */
    int64_t valueint; /**< Response integer value. */
    bool valuebool; /**< Response Boolean value.*/
  }u;   /**< Union of values. */
}qapi_Device_Info_t;

typedef void * qapi_Device_Info_Hndl_t;

/*=======================================================================================================
 * QAPI Device Info Callback(deprecated in DAM space. Use callback qapi_Device_Info_Callback_t2 instead.)                         *
 ========================================================================================================*/
typedef void (*qapi_Device_Info_Callback_t)(const qapi_Device_Info_t *info);

/*====================================================
 * QAPI Device Info Callback                         *
 ====================================================*/
typedef void (*qapi_Device_Info_Callback_t_v2)(qapi_Device_Info_Hndl_t device_info_hndl, const qapi_Device_Info_t *info);

static __inline int device_info_set_byte_pool(qapi_Device_Info_Hndl_t hndl, void *device_info_byte_pool_ptr);
static __inline int device_info_release_indirection(qapi_Device_Info_Hndl_t hndl);


static __inline void qapi_device_info_cb_uspace_dispatcher(UINT cb_id,
                                                             void *app_cb,
                                                             UINT cb_param1,
                                                             UINT cb_param2,
                                                             UINT cb_param3,
                                                             UINT cb_param4,
                                                             UINT cb_param5,
                                                             UINT cb_param6,
                                                             UINT cb_param7,
                                                             UINT cb_param8)
{
  void (*pfn_app_cb1) (qapi_Device_Info_Hndl_t , qapi_Device_Info_t *);
  qapi_Device_Info_t *info = NULL;
  if(cb_id == DATA_CUSTOM_CB_SET_DEVICE_INFO)
  {
    pfn_app_cb1 = (void (*)(qapi_Device_Info_Hndl_t, qapi_Device_Info_t *))app_cb;
	info = (qapi_Device_Info_t *)cb_param2;
   (pfn_app_cb1)((qapi_Device_Info_Hndl_t)cb_param1, 
                       (qapi_Device_Info_t *)cb_param2);
   if(info)
   	 tx_byte_release(info);
  }	
}


#ifdef  QAPI_TXM_MODULE

#define qapi_Device_Info_Init()                 ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_INIT,    (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Get(a, b)              ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_GET,     (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Release()              ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_RELEASE, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Set_Callback(a, b)     ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_SET_CB,  (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Reset()                  ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_RESET,   (ULONG)0, (ULONG)0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#define qapi_Device_Info_Init_v2(a)               ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_INIT2,    (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Get_v2(a, b, c)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_GET2,     (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Release_v2(a)            device_info_release_indirection(a)
#define qapi_Device_Info_Set_Callback_v2(a, b, c) ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_SET_CB2,  (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) qapi_device_info_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Device_Info_Reset_v2(a)              ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_RESET2,   (ULONG)a, (ULONG)0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/*
 * Macro that Passes Byte Pool Pointer for Application invoking device info QAPI's 
 * Parameter 'a' : Handle
 * Parameter 'b' : Pointer to Byte Pool 
 * On success, QAPI_OK is returned. On error, appropriate QAPI_ERR_ code(qapi_status.h) is returned.
 * (This Macro is only used in DAM Space)
 */

#define qapi_Device_Info_Pass_Pool_Ptr(a,b)             device_info_set_byte_pool(a,b)

/** @} */

static __inline int device_info_set_byte_pool(qapi_Device_Info_Hndl_t hndl, void *device_info_byte_pool_ptr)
{
  qapi_cb_params_uspace_ptr_t *uspace_memory_device_info = NULL;
  int ret = QAPI_ERROR;

  /* input parameter validation*/
  if((hndl == NULL) || device_info_byte_pool_ptr == NULL)
  {
	return QAPI_ERR_INVALID_PARAM;
  }
  

  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_DEVICE_INFO, (void**)&uspace_memory_device_info);
  /* Handle to user space information already exists */
  if (uspace_memory_device_info)
  {
    return QAPI_OK;
  }

  tx_byte_allocate(device_info_byte_pool_ptr, (VOID **) &uspace_memory_device_info,(sizeof(qapi_cb_params_uspace_ptr_t)), TX_NO_WAIT);
  if (NULL == uspace_memory_device_info)
  {
    return QAPI_ERR_NO_MEMORY;
  }

  memset (uspace_memory_device_info,0, sizeof(qapi_cb_params_uspace_ptr_t));

  
  /* Initialize user space information */
  uspace_memory_device_info->usr_pool_ptr = device_info_byte_pool_ptr;  
  
  /* Map the user space information to the handle internally and store */ 
  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_DEVICE_INFO, uspace_memory_device_info, 0x00);
  if (ret != QAPI_OK)
  {
    /* Release the allocated resources */
    if (uspace_memory_device_info)
      tx_byte_release(uspace_memory_device_info);
  }
  
  return ret;
  
}

/*
 * Releases the user space allocated resources and dispatches the call to qapi_Devive_Info_Release2 
 * Parameter 'a' : Handle
 * On success, QAPI_OK is returned. On error, appropriate QAPI_ERR_ code(qapi_status.h) is returned.
 * (This is only used in DAM Space)
 */
static __inline int device_info_release_indirection(qapi_Device_Info_Hndl_t hndl)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_device_info = NULL;
  
  if(hndl == NULL)
  {
    return QAPI_ERR_INVALID_PARAM;
  }
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_DEVICE_INFO, (void**)&uspace_memory_device_info);
  
  /* Release the allocated resources */
  if (uspace_memory_device_info)
    tx_byte_release(uspace_memory_device_info);
  
  /* Dispatch the call to the module manager */
  ret = ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_RELEASE2, (ULONG) hndl, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0));
  return ret;
}

#else

qapi_Status_t qapi_Device_Info_Handler(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l);
qapi_Status_t qapi_Device_Info_Handler_v2(UINT id, UINT a, UINT b, UINT c, UINT d, UINT e, UINT f, UINT g, UINT h, UINT i, UINT j, UINT k, UINT l);


/*=======================================================
 * QAPI Device Info APIs                             *
 =======================================================*/

/**
 * @brief Initializes the device information context.
 *
 * This function must be called before invoking other qapi_Device_Info APIs.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Init_v2 instead
 */
qapi_Status_t qapi_Device_Info_Init(void);

/**
 * @brief Initializes the device information context.
 *
 * @param[in] device_info_hndl Pointer to device info hndl
 *
 * This function must be called before invoking other qapi_Device_Info APIs.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Device_Info_Init_v2(qapi_Device_Info_Hndl_t *device_info_hndl);


/**
 * @brief Gets the device information for specified ID.
 *
 * @param[in] id     Information ID.
 * @param[out] info  Information received for the specified ID.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Get_v2 instead.
 */
qapi_Status_t qapi_Device_Info_Get(qapi_Device_Info_ID_t id, qapi_Device_Info_t *info);

/**
 * @brief Gets the device information for specified ID.
 *
 * @param[in] device_info_hndl  Device info hndl.
 * @param[in] id                Information ID.
 * @param[out] info             Information received for the specified ID.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2 must have been called.
 */
qapi_Status_t qapi_Device_Info_Get_v2(qapi_Device_Info_Hndl_t device_info_hndl, qapi_Device_Info_ID_t id, qapi_Device_Info_t *info);


/**
 * @brief Sets a device information callback.
 *
 * @param[in] id        Information ID.
 * @param[in] callback  Callback to be registered.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Set_Callback_v2 instead.
 */
qapi_Status_t qapi_Device_Info_Set_Callback(qapi_Device_Info_ID_t id, qapi_Device_Info_Callback_t callback);

/**
 * @brief Sets a device information callback.
 *
 * @param[in] device_info_hndl  Device info hndl.
 * @param[in] id                Information ID.
 * @param[in] callback          Callback to be registered.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2 must have been called.
 */
qapi_Status_t qapi_Device_Info_Set_Callback_v2(qapi_Device_Info_Hndl_t device_info_hndl, qapi_Device_Info_ID_t id, qapi_Device_Info_Callback_t_v2 callback);

/**
 * @brief Releases the device information context.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Release_v2 instead.
 */
qapi_Status_t qapi_Device_Info_Release(void);

/**
 * @brief Releases the device information context.
 *
 * @param[in] device_info_hndl  Device info hndl.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2 must have been called.
 */
qapi_Status_t qapi_Device_Info_Release_v2(qapi_Device_Info_Hndl_t device_info_hndl);


/**
 * @brief Resets the device.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Reset_v2 instead.
 */
qapi_Status_t qapi_Device_Info_Reset(void);

/**
 * @brief Resets the device.
 *
 * @param[in] device_info_hndl  Device info hndl.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Device_Info_Reset_v2(qapi_Device_Info_Hndl_t device_info_hndl);


#endif

/** @} */ /* end_addtogroup qapi_device_info */

#endif
