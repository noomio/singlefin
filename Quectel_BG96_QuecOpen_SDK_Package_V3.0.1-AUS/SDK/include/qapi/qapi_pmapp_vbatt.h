
#ifndef __QAPI_PMAPP_VBATT_H__
#define __QAPI_PMAPP_VBATT_H__

/*===========================================================================
              QAPI P M   VBATT   A P P   H E A D E R   F I L E

===========================================================================*/
/**
 * @file qapi_pmapp_vbatt.h
 *
 * @addtogroup qapi_pmapp_vbatt
 * @{ 
 *
 * @brief 
 *
 * @details This module provides the definitions to get the battery status information.
 * 
 *
 * @}
 *  
 */

/*===========================================================================
  Copyright (c) 2017-2018 Qualcomm Technologies, Inc.  All Rights Reserved.  
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
============================================================================*/


/*=============================================================================
                            EDIT HISTORY FOR File

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/15/18   leo     (Tech Comm) Edited/added Doxygen comments and markup.
10/11/17   kcs     Updated the file for more Battery management API's (CR -2110414).
09/02/17   leo     (Tech Comm) Edited/added Doxygen comments and markup.
05/29/17   kcs     created
=============================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#ifdef __cplusplus
extern "C" {
#endif

#include "qapi_pmic_status.h"
#include "qapi_types.h"



// PMIC VBATT Base ID
#define TXM_QAPI_PM_VBATT_BASE  TXM_QAPI_PMIC_BASE + 10

/* =========================================================================
                       USER_Mode_DEFS
========================================================================= */

// Driver ID defines
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_STATUS             TXM_QAPI_PM_VBATT_BASE + 1
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_HEALTH             TXM_QAPI_PM_VBATT_BASE + 2
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_TEMPARATURE        TXM_QAPI_PM_VBATT_BASE + 3
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_TECHNOLOGY         TXM_QAPI_PM_VBATT_BASE + 4
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGE_STATUS      TXM_QAPI_PM_VBATT_BASE + 5
#define  TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGER_SOURCE     TXM_QAPI_PM_VBATT_BASE + 6


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */

/** @addtogroup qapi_pmapp_vbatt
@{ */

/** PMIC battery technology.
*/
typedef enum
{
  QAPI_PMIC_BAT_TECH_LI_ION_E,        /**< Li-Ion battery. */
  QAPI_PMIC_BAT_TECH_LI_POLYMER_E,    /**< Li-Polymer battery. */
} qapi_PM_Battery_Technology_t;

/** PMIC SMB presence.
*/	 	 
typedef enum
{
  QAPI_SMB_ABSENT_E = 0,             /**< SMB is present. */
  QAPI_SMB_PRESENT_E,                /**< SMB is absent. */
} qapi_PM_Smb_Presence_t;

/** Battery temperature.
*/ 
typedef enum
{
  QAPI_BAT_COLD_E,                   /**< Battery is cold. */
  QAPI_BAT_HOT_E,                    /**< Battery is hot. */
  QAPI_BAT_GOOD_E,                   /**< Battery temperature is good. */
} qapi_PM_Battery_Temperature_t;

/** Battery health.
*/
typedef enum
{
  QAPI_BAT_OV_E,                    /**< Battery is over voltage. */
  QAPI_BAT_UV_E,                    /**< Battery is under voltage. */
  QAPI_BAT_MISSING_E,               /**< Battery is missing. */
  QAPI_BAT_GOOD_HEALTH_E,           /**< Battery health is good. */
} qapi_PM_Battery_Health_t;

/** Battery chargin status.
*/ 
typedef enum
{
  QAPI_BAT_DISCHARGING_E,          /**< Battery is discharging. */
  QAPI_BAT_CHARGING_E,             /**< Battery is charging. */
} qapi_PM_Battery_Chg_Status_t;

/** Battery charger source.
*/ 
typedef enum
{
  QAPI_USB_CDP_E,                 /**< Charger type is USB_CDP. */
  QAPI_USB_DCP_E,                 /**< Charger type is USB_DCP. */
  QAPI_USB_SDP_E,                 /**< Charger type is USB_SDP. */
  QAPI_CHG_ABSENT_E,              /**< Charger is absent. */
} qapi_PM_Battery_Chg_Src_t;

/** @} */ /* end_addtogroup qapi_pmapp_vbatt */


#ifdef  QAPI_TXM_MODULE

// API Definitions 
#define qapi_Pmapp_Vbatt_Get_Battery_Status(batt_stat)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_STATUS,\
    (ULONG) batt_stat,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_Pmapp_Vbatt_Get_Battery_Health(batt_health)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_HEALTH,\
    (ULONG) batt_health,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_Pmapp_Vbatt_Get_Battery_Temparature(batt_temp)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_TEMPARATURE,\
    (ULONG) batt_temp,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_Pmapp_Vbatt_Get_Battery_Technology(batt_tech)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_TECHNOLOGY,\
    (ULONG) batt_tech,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_Pmapp_Vbatt_Get_Battery_Charge_Status(batt_charge_status)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGE_STATUS,\
    (ULONG) batt_charge_status,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))

#define qapi_Pmapp_Vbatt_Get_Battery_Charger_Source(batt_charge_resource)\
    ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_PMIC_VBATT_GET_BATTERY_CHARGER_SOURCE,\
    (ULONG) batt_charge_resource,(ULONG) 0, (ULONG) 0, (ULONG) 0,(ULONG) 0,\
    (ULONG) 0,(ULONG) 0,(ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, \
    (ULONG) 0))
    
	
#elif defined QAPI_TXM_SOURCE_CODE
	

/*===========================================================================
                   KERNEL_Mode_DEFS
===========================================================================*/

/** @addtogroup qapi_pmapp_vbatt
@{ */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Status                            
===========================================================================*/

/**
   Gets the battery charge percentage.

  @param[out] qapi_batt_status Buffer from which to get the battery charge percentage.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                -- Operation succeeded.
   - QAPI_ERR_INVALID_PARAM -- Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED -- Feature is not supported.
   - QAPI_ERROR             -- Other errors.
*/
qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Status(uint8_t *qapi_batt_status);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Status */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Health
===========================================================================*/                            

/**
   Gets the battery health.

  @param[out] qapi_batt_health  Buffer from which to get the battery health.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                --  Operation succeeded.
   - QAPI_ERR_INVALID_PARAM --  Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED --  Feature is not supported.
   - QAPI_ERROR             --  Other errors.
*/

qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Health(qapi_PM_Battery_Health_t *qapi_batt_health);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Health */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Temparature
===========================================================================*/                            

/**
   Gets the battery temparature.

  @param[out] qapi_batt_temp  Buffer from which to get the battery temparature.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                --  Operation succeeded.
   - QAPI_ERR_INVALID_PARAM --  Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED --  Feature is not supported.
   - QAPI_ERROR             --  Other errors.
*/

qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Temparature(qapi_PM_Battery_Temperature_t *qapi_batt_temp);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Temparature */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Technology
===========================================================================*/                            

/**
   Gets the battery technology.

  @param[out] qapi_batt_tech  Buffer from which to get the battery technology.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                --  Operation succeeded.
   - QAPI_ERR_INVALID_PARAM --  Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED --  Feature is not supported.
   - QAPI_ERROR             --  Other errors.
*/

qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Technology(qapi_PM_Battery_Technology_t *qapi_batt_tech);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Technology */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Charge_Status
===========================================================================*/

/**
   Gets the battery charging status.

  @param[out] qapi_charge_status  Buffer from which to get the battery charging status.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                --  Operation succeeded.
   - QAPI_ERR_INVALID_PARAM --  Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED --  Feature is not supported.
   - QAPI_ERROR             --  Other errors.
*/

qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Charge_Status(qapi_PM_Battery_Chg_Status_t *qapi_charge_status);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Charge_Status */

/*===========================================================================
FUNCTION   qapi_Pmapp_Vbatt_Get_Battery_Charger_Source
===========================================================================*/

/**
   Gets the charger type.

  @param[out] qapi_charger_source  Buffer from whichto get the battery charge source type,
                                   i.e., the charger type.

  @return
   See qapi_Status_t. Possible values: \n
   - QAPI_OK                --  Operation succeeded.
   - QAPI_ERR_INVALID_PARAM --  Invalid parameter.
   - QAPI_ERR_NOT_SUPPORTED --  Feature is not supported.
   - QAPI_ERROR             --  Other errors.
*/

qapi_Status_t
qapi_Pmapp_Vbatt_Get_Battery_Charger_Source(qapi_PM_Battery_Chg_Src_t *qapi_charger_source);
/*~ FUNCTION qapi_Pmapp_Vbatt_Get_Battery_Charger_Source */

#else   // DEF_END

#error "No QAPI flags defined"

#endif
#ifdef __cplusplus
} /* closing brace for extern "C" */

#endif

#endif /* __QAPI_PMAPP_VBATT_H__  */

/** @} */ /* end_addtogroup qapi_pmapp_vbatt */
