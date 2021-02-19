/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/
  
  /*===========================================================================
  
                          EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: 
  
  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  19/04/19   baohan     created this file.
  20/06/19   baohan     update for separate component.
  ===========================================================================*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

/**
  @file
  qapi_version.h

  Version interface file to interact with the version module. 
*/

#ifndef _QAPI_VERSION_H_
#define _QAPI_VERSION_H_

#include <string.h>

#include "qapi_status.h"
#include "txm_module.h"
#include "qapi_data_txm_base.h"
#include "version_dataservice_def.h"
#include "version_core_def.h"
#include "version_location_def.h"
#include "version_modem_feature_def.h"


#define QAPI_VERSION_MAX_VERSION_STRING_LEN (20)

/** VERSION ID defines. */
#define  TXM_QAPI_VERSION_GET_KERNEL_VERSION        TXM_QAPI_VERSION_BASE + 1

#define _STR(str) #str
#define STR(str)  _STR(str)

typedef struct __qapi_Version_Object_s
{
  uint8_t  major_num; /**< Major number of version. */
  uint16_t  minor_num; /**< Minor number of version. */
  uint8_t patch_num; /**< Patch number of version. */

  char    ver_str[QAPI_VERSION_MAX_VERSION_STRING_LEN];
  /**< Specifies the version in string format. */
} qapi_Version_Object_t;

/** Version Categories. */
enum QAPI_VERSION_CATEGORY {
  QAPI_VERSION_CATEGORY_DATA_SERVICE, /**< Category data service. */
  QAPI_VERSION_CATEGORY_DRIVER, /**< Category driver. */
  QAPI_VERSION_CATEGORY_LOCATION, /**< Category location. */
  QAPI_VERSION_CATEGORY_MODEM_FEATURE, /**< Category modem feature. */
  QAPI_VERSION_CATEGORY_SECURITY, /**< Category security. */
  QAPI_VERSION_CATEGORY_CAT_MAX, /**< Total number of categories. */
};

typedef enum QAPI_VERSION_CATEGORY qapi_Version_Category_t;

static qapi_Version_Object_t versions[QAPI_VERSION_CATEGORY_CAT_MAX] =
{
  /** Category data service versions. */
  {DATA_SERVICE_MAJOR_NUM,  DATA_SERVICE_MINOR_NUM,  DATA_SERVICE_PATCH_NUM,  STR(DATA_SERVICE_VER_STR)},
  /** Category driver versions. */
  {DRIVER_MAJOR_NUM,        DRIVER_MINOR_NUM,        DRIVER_PATCH_NUM,        STR(DRIVER_VER_STR)},
  /** Category location versions. */
  {LOCATION_MAJOR_NUM,      LOCATION_MINOR_NUM,      LOCATION_PATCH_NUM,      STR(LOCATION_VER_STR)},
  /** Category modem feature versions. */
  {MODEM_FEATURE_MAJOR_NUM, MODEM_FEATURE_MINOR_NUM, MODEM_FEATURE_PATCH_NUM, STR(MODEM_FEATURE_VER_STR)},
  /** Category security versions. */
  {SECURITY_MAJOR_NUM,      SECURITY_MINOR_NUM,      SECURITY_PATCH_NUM,      STR(SECURITY_VER_STR)}
};


/**
 * Get app compiled QAPI version.
 *
 * @param[in]  cat      category for which to get QAPI version.
 * @param[out] version  QAPI version for the specific category.
 *
 * @return QAPI_OK on success or QAPI_ERROR on failure.
 */
static __inline qapi_Status_t qapi_Version_Get_App_Version(qapi_Version_Category_t cat, qapi_Version_Object_t *version)
{
  if(cat < QAPI_VERSION_CATEGORY_CAT_MAX && version != NULL)
  {
    memcpy(version, &versions[cat], sizeof(qapi_Version_Object_t));
    return QAPI_OK;
  }
  return QAPI_ERROR;
};

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_Version_Get_Kernel_Version(a,b)                  ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_VERSION_GET_KERNEL_VERSION, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else

UINT qapi_Version_Handler(UINT id,UINT a1,UINT a2,UINT a3,UINT a4,UINT a5,UINT a6,UINT a7,UINT a8,UINT a9,UINT a10,UINT a11,UINT a12);

/**
 * Get kernel QAPI version.
 *
 * @param[in]  cat      category for which to get QAPI version.
 * @param[out] version  QAPI version for the specific category.
 *
 * @return QAPI_OK on success or QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Version_Get_Kernel_Version(qapi_Version_Category_t cat, qapi_Version_Object_t *version);


/** @} */ /* end_addtogroup qapi_version_apis */

#endif /*!TXM_MODULE*/

#endif /* _QAPI_VERSION_H_ */
