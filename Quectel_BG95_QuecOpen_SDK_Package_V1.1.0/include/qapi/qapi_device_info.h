/*
 Copyright (c) 2017-2019  Qualcomm Technologies, Inc.  All Rights Reserved.
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

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY.
*/


/**
  @file qapi_device_info.h

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/
 
#ifndef _QAPI_DEVICE_INFO_H
#define _QAPI_DEVICE_INFO_H

#include "stdbool.h"  //QuectelModifyFlag
#include "qapi_data_txm_base.h"
#include "qapi_status.h"
#include "qapi_driver_access.h"
#include "txm_module.h"

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
#define  TXM_QAPI_DEVICE_INFO_SET                TXM_QAPI_DEVICE_INFO_BASE_V2 + 6

/** Macros for mode preference (QAPI_DEVICE_INFO_MODE_PREF_E). */
#define QAPI_DEVICE_INFO_MODE_PREF_CDMA2000_1X     0x01   /**< Mode preference: CDMA 2000 1X. */
#define QAPI_DEVICE_INFO_MODE_PREF_CDMA2000_HRPD   0x02   /**< Mode preference: CDMA 2000 HRPD. */
#define QAPI_DEVICE_INFO_MODE_PREF_GSM             0x04   /**< Mode preference: GSM. */
#define QAPI_DEVICE_INFO_MODE_PREF_UMTS            0x08   /**< Mode preference: UMTS. */
#define QAPI_DEVICE_INFO_MODE_PREF_LTE             0x10   /**< Mode preference: LTE. */
#define QAPI_DEVICE_INFO_MODE_PREF_TDSCDMA         0x20   /**< Mode preference: TDSCDMA. */
#define QAPI_DEVICE_INFO_MODE_PREF_NR5G            0x40   /**< Mode preference: NR5G. */

/** Macros for ciot_mode_preference(QAPI_DEVICE_INFO_CIOT_LTE_OP_MODE_PREF_E). */
#define QAPI_DEVICE_INFO_LTE_OP_MODE_PREF_WB    ((uint64_t)0x01)   /**< LTE wideband. */
#define QAPI_DEVICE_INFO_LTE_OP_MODE_PREF_M1    ((uint64_t)0x02)   /**< LTE M1. */
#define QAPI_DEVICE_INFO_LTE_OP_MODE_PREF_NB1   ((uint64_t)0x04)   /**< LTE NB1. */

/** Valid values for Ciot_Acquisition_order (QAPI_DEVICE_INFO_RAT_ACQ_PREF_E). */
typedef enum {
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_MIN_ENUM_VAL = -2147483647,
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_NO_SVC = 0x00,   /**< No Service. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_CDMA_1X = 0x01,   /**< CDMA 1X. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_CDMA_1XEVDO = 0x02,   /**< CDMA HRPD. */ 
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_AMPS  = 0x03,   /**< AMPS. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_GSM = 0x04,   /**< GSM. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_UMTS = 0x05,   /**< UMTS. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_WLAN = 0x06,   /**< WLAN. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_GPS = 0x07,   /**< GPS. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_LTE  = 0x08,   /**< LTE. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_TDSCDMA = 0x09,   /**< TD-SCDMA. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_LTE_M1 = 0x0a,   /**< LTE M1. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_LTE_NB1 = 0x0b,   /**< LTE NB1. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_NR5G  = 0x0c,   /**< NR5G. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_NO_CHANGE = -1,   /**< No change. */
  QAPI_DEVICE_INFO_ACQ_ORDER_PREF_MAX_ENUM_VAL = 2147483647 
}acq_order_pref_enum;

/** Macros for band preference. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_1   ((uint64_t)0x000000000000001ull)  /**< E-UTRA Operating Band 1. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_2   ((uint64_t)0x000000000000002ull)  /**< E-UTRA Operating Band 2. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_3   ((uint64_t)0x000000000000004ull)  /**< E-UTRA Operating Band 3. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_5   ((uint64_t)0x000000000000010ull)  /**< E-UTRA Operating Band 5. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_6   ((uint64_t)0x000000000000020ull)  /**< E-UTRA Operating Band 6. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_7   ((uint64_t)0x000000000000040ull)  /**< E-UTRA Operating Band 7. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_8   ((uint64_t)0x000000000000080ull)  /**< E-UTRA Operating Band 8. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_9   ((uint64_t)0x000000000000100ull)  /**< E-UTRA Operating Band 9. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_10  ((uint64_t)0x000000000000200ull)  /**< E-UTRA Operating Band 10. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_11  ((uint64_t)0x000000000000400ull)  /**< E-UTRA Operating Band 11. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_12  ((uint64_t)0x000000000000800ull)  /**< E-UTRA Operating Band 12. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_13  ((uint64_t)0x000000000001000ull)  /**< E-UTRA Operating Band 13. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_14  ((uint64_t)0x000000000002000ull)  /**< E-UTRA Operating Band 14. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_17  ((uint64_t)0x000000000010000ull)  /**< E-UTRA Operating Band 17. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_18  ((uint64_t)0x000000000020000ull)  /**< E-UTRA Operating Band 18. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_19  ((uint64_t)0x000000000040000ull)  /**< E-UTRA Operating Band 19. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_20  ((uint64_t)0x000000000080000ull)  /**< E-UTRA Operating Band 20 */
#define QAPI_DEVICE_INFO_OPERATING_BAND_21  ((uint64_t)0x000000000100000ull)  /**< E-UTRA Operating Band 21. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_23  ((uint64_t)0x000000000400000ull)  /**< E-UTRA Operating Band 23. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_24  ((uint64_t)0x000000000800000ull)  /**< E-UTRA Operating Band 24. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_25  ((uint64_t)0x000000001000000ull)  /**< E-UTRA Operating Band 25. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_26  ((uint64_t)0x000000002000000ull)  /**< E-UTRA Operating Band 26. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_28  ((uint64_t)0x000000008000000ull)  /**< E-UTRA Operating Band 28. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_29  ((uint64_t)0x000000010000000ull)  /**< E-UTRA Operating Band 29. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_32  ((uint64_t)0x000000020000000ull)  /**< E-UTRA Operating Band 32. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_30  ((uint64_t)0x000000080000000ull)  /**< E-UTRA Operating Band 30. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_33  ((uint64_t)0x000000100000000ull)  /**< E-UTRA Operating Band 33. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_34  ((uint64_t)0x000000200000000ull)  /**< E-UTRA Operating Band 34. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_35  ((uint64_t)0x000000400000000ull)  /**< E-UTRA Operating Band 35. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_36  ((uint64_t)0x000000800000000ull)  /**< E-UTRA Operating Band 36. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_37  ((uint64_t)0x000001000000000ull)  /**< E-UTRA Operating Band 37. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_38  ((uint64_t)0x000002000000000ull)  /**< E-UTRA Operating Band 38. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_39  ((uint64_t)0x000004000000000ull)  /**< E-UTRA Operating Band 39. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_40  ((uint64_t)0x000008000000000ull)  /**< E-UTRA Operating Band 40. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_41  ((uint64_t)0x000010000000000ull)  /**< E-UTRA Operating Band 41. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_42  ((uint64_t)0x000020000000000ull)  /**< E-UTRA Operating Band 42. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_43  ((uint64_t)0x000040000000000ull)  /**< E-UTRA Operating Band 43. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_46  ((uint64_t)0x000200000000000ull)  /**< E-UTRA Operating Band 46. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_47  ((uint64_t)0x000400000000000ull)  /**< E-UTRA Operating Band 47. */
#define QAPI_DEVICE_INFO_OPERATING_BAND_48  ((uint64_t)0x000800000000000ull)  /**< E-UTRA Operating Band 48. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_125 ((uint64_t)0x1000000000000000ull)  /**< E-UTRA Operating Band 125. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_126 ((uint64_t)0x2000000000000000ull)  /**< E-UTRA Operating Band 126. */ 
#define QAPI_DEVICE_INFO_OPERATING_BAND_127 ((uint64_t)0x4000000000000000ull)  /**< E-UTRA Operating Band 127. */

/** Valid values for battery status (QAPI_DEVICE_INFO_BATTERY_STATUS_E). */
typedef enum
{
  QAPI_DEVICE_INFO_BAT_OV = 0x00,           /**< Over battery voltage. */
  QAPI_DEVICE_INFO_BAT_UV = 0x01,           /**< Low battery voltage. */
  QAPI_DEVICE_INFO_BAT_MISSING = 0x02,      /**< Battery is missing. */
  QAPI_DEVICE_INFO_BAT_GOOD_HEALTH = 0x03   /**< Battery voltage is good. */
} battery_status;


/** Valid values for network service status (QAPI_DEVICE_INFO_SERVICE_STATE_E). */
typedef enum
{
  QAPI_DEVICE_INFO_SRV_STATE_NO_SRV = 0, /**< No service. */
  QAPI_DEVICE_INFO_SRV_STATE_SRV = 1     /**< Service is available. */
}srv_status;


/** Valid values for network indication (QAPI_DEVICE_INFO_NETWORK_IND_E). */
typedef enum
{
  QAPI_DEVICE_INFO_NW_IND_NO_SRV = 0, /**< No service. */
  QAPI_DEVICE_INFO_NW_IND_SRV = 1     /**< Service is available. */
}nw_indication;


/** Valid values for network RRC state. */
typedef enum
{
  QAPI_DEVICE_INFO_RRC_IDLE = 0,       /**< Status: Idle. */
  QAPI_DEVICE_INFO_RRC_CONNECTED = 1   /**< Status: connected. */
}rrc_state;


/** Valid values for network Extended Mobility Management (EMM) state. */
typedef enum
{
  QAPI_DEVICE_INFO_EMM_NULL = 0,                              /**< Null. */
  QAPI_DEVICE_INFO_EMM_DEREGISTERED = 1,                      /**< Deregistered. */
  QAPI_DEVICE_INFO_EMM_REGISTERED_INITIATED = 2,              /**< Registered, initiated. */
  QAPI_DEVICE_INFO_EMM_REGISTERED = 3,                        /**< Registered. */
  QAPI_DEVICE_INFO_EMM_TRACKING_AREA_UPDATING_INITIATED = 4,  /**< Tracking area update initiated. */
  QAPI_DEVICE_INFO_EMM_SERVICE_REQUEST_INITIATED = 5,         /**< Service request initiated. */
  QAPI_DEVICE_INFO_EMM_DEREGISTERED_INITIATED = 6             /**< Deregistered, initiated. */
}emm_state;


/** Valid values for network roaming status (QAPI_DEVICE_INFO_ROAMING_E). */
typedef enum
{
  QAPI_DEVICE_INFO_ROAMING_STATUS_OFF = 0,  /**< Roaming status: OFF. */
  QAPI_DEVICE_INFO_ROAMING_STATUS_ON = 1    /**< Roaming status: ON. */
}roaming_info;


/** Valid value for sim state (QAPI_DEVICE_INFO_SIM_STATE_E). */
typedef enum
{
  QAPI_DEVICE_INFO_SIM_STATE_UNKNOWN = 0x00,            /**< Unknown. */
  QAPI_DEVICE_INFO_SIM_STATE_DETECTED = 0x01,           /**< Detected. */
  QAPI_DEVICE_INFO_SIM_STATE_PIN1_OR_UPIN_REQ = 0x02,   /**< PIN1 or UPIN is required. */
  QAPI_DEVICE_INFO_SIM_STATE_PUK1_OR_PUK_REQ = 0x03,    /**< PUK1 or PUK for UPIN is required. */
  QAPI_DEVICE_INFO_SIM_STATE_PERSON_CHECK_REQ = 0x04,   /**< Personalization state must be checked. */
  QAPI_DEVICE_INFO_SIM_STATE_PIN1_PERM_BLOCKED  = 0x05, /**< PIN1 is blocked. */ 
  QAPI_DEVICE_INFO_SIM_STATE_ILLEGAL = 0x06,            /**< Illegal. */
  QAPI_DEVICE_INFO_SIM_STATE_READY = 0x07               /**< Ready. */
}sim_state;

typedef enum
{
  QAPI_DEVICE_INFO_NO_SRV_V01  = 0x00, /**<  No service. */
  QAPI_DEVICE_INFO_LTE_WB_V01  = 0x01, /**<  Camped on LTE wideband. */
  QAPI_DEVICE_INFO_LTE_M1_V01  = 0x02, /**<  Camped on LTE M1. */
  QAPI_DEVICE_INFO_LTE_NB1_V01 = 0x03  /**<  Camped on LTE NB1. */  
}ciot_lte_op_mode;

/** Macro for Network Bearer values (QAPI_DEVICE_INFO_NETWORK_BEARER_E). */
#define QAPI_DEVICE_INFO_SRV_TYPE_LTE     1   /**< Nw bearer srvc type :LTE. */
#define QAPI_DEVICE_INFO_SRV_TYPE_GSM     2   /**< Nw bearer srvc type :GSM. */
#define QAPI_DEVICE_INFO_SRV_TYPE_WCDMA   3   /**< Nw bearer srvc type :WCDMA. */
#define QAPI_DEVICE_INFO_LTE_TDD          5   /**< Nw bearer srvc type :LTE  Mode: TDD. */
#define QAPI_DEVICE_INFO_LTE_FDD          6   /**< Nw bearer srvc type :LTE  Mode: FDD. */
#define QAPI_DEVICE_INFO_LTE_NB_IOT       7   /**< Nw bearer srvc type :LTE  Mode: NB-IOT.*/


/** @addtogroup qapi_device_info
@{ */

/** Device information types. */
typedef enum {

/** 
  Response type: Return value of the Device ID as specified in the enum qapi_Device_Info_Type_t.
  Operation: Query      -  Queries the device information using the API qapi_Device_Info_Get_v2().
             Set        -  Sets the device information using the API qapi_Device_Info_Set().
             Indication -  Sets the device information callback, registering for various indications 
                           using the API qapi_Device_Info_Set_Callback_v2().
 */

 QAPI_DEVICE_INFO_BUILD_ID_E,
 /**< Device BUILD_ID. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_IMEI_E,
 /**< Device IMEI. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_IMSI_E,
 /**< UIM IMSI. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_OS_VERSION_E,
 /**< Device OS version. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_MANUFACTURER_E,
 /**< Device manufacturer. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_MODEL_ID_E,
 /**< Device model ID. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_BATTERY_STATUS_E,
 /**< Device battery status. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_BATTERY_PERCENTAGE_E,
 /**< Device battery percentage. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEIVCE_INFO_TIME_ZONE_E,
 /**< Device time zone. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEIVCE_INFO_ICCID_E,
 /**< Device ICCID. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_4G_SIG_STRENGTH_E,
 /**< Network signal strength. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_BASE_STATION_ID_E,
 /**< Network base station ID. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_MCC_E,
 /**< Network MCC. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_MNC_E,
 /**< Network MNC. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_SERVICE_STATE_E,
 /**< Network service status. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_MDN_E,
 /**< Device MDN. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_TAC_E,
 /**< Network tracking area code. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_CELL_ID_E,
 /**< Network cell ID. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_RCCS_E,
 /**< Network RRC state. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_EMMS_E,
 /**< Network EMM state. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 DEPRACATED1,
 /* Information to keep enum numbering consistent. */

 QAPI_DEVICE_INFO_SERVING_PCI_E,
 /**< Network serving cell PCI. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_SERVING_RSRQ_E,
 /**< Serving cell RSRQ. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_SERVING_EARFCN_E,
 /**< Serving cell EARFCN. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

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
 /**< Network indication. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_ROAMING_E,
 /**< Roaming status. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_LAST_POWER_ON_E,
 /**< Last power on time. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_CHIPID_STRING_E,
 /**< Chipset name. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Query. */

 QAPI_DEVICE_INFO_APN_PROFILE_INDEX_E,
 /**< APN profile index. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_SIM_STATE_E,
 /**< SIM state. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_NETWORK_BEARER_E,
 /**< Network bearer. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query and Indication. */

 QAPI_DEVICE_INFO_LINK_QUALITY_E,
 /**< Network link quality. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Indication. */

 QAPI_DEVICE_INFO_TX_BYTES_E,
 /**< Device Tx bytes. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Indication. */

 QAPI_DEVICE_INFO_RX_BYTES_E,
 /**< Device Rx bytes. Response type:QAPI_DEVICE_INFO_TYPE_BUFFER_E. Operation: Indication. */

 QAPI_DEVICE_INFO_ANY,
 /**< Any device information. */

 QAPI_DEVICE_INFO_CIOT_LTE_OP_MODE_PREF_E,
 /**< Device lte op mode preference. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_LTE_M1_BAND_PREF_E,
 /**< Device lte m1 band preference. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_LTE_NB1_BAND_PREF_E,
 /**< Device lte nb1 band preference. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_MODE_PREF_E,
 /**< Device mode preference Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set and Query.. */

 QAPI_DEVICE_INFO_RAT_ACQ_PREF_E,
 /**< RAT Acquisition order preference. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_PS_DETACH_E,
 /**< Device PS Detach. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set. */

 QAPI_DEVICE_INFO_PS_ATTACH_E,
 /**< Device PS Attach. Response type:QAPI_DEVICE_INFO_TYPE_ARRAY_E. Operation: Set. */

 QAPI_DEVICE_INFO_PSM_TIMER_E,
 /**< Device PSM timer. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_ACTIVE_TIMER_E,
 /**< Device Power save mode Active timer value. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Set and Query. */

 QAPI_DEVICE_INFO_LTE_OP_MODE_E,
/**< Device LTE operational mode. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

 QAPI_DEVICE_INFO_LAC_E,
/**< Location Area Code. Response type:QAPI_DEVICE_INFO_TYPE_INTEGER_E. Operation: Query. */

} qapi_Device_Info_ID_t;


/** Device information response types. */
typedef enum {
 QAPI_DEVICE_INFO_TYPE_BOOLEAN_E,
 /**< Response type is Boolean. */
 QAPI_DEVICE_INFO_TYPE_INTEGER_E,
 /**< Response type is integer. */
 QAPI_DEVICE_INFO_TYPE_BUFFER_E,
 /**< Response type is buffer. */
 QAPI_DEVICE_INFO_TYPE_ARRAY_E,
 /**< Response type is array. */
} qapi_Device_Info_Type_t;

/*========================================================
 * Device Info structure                               *
 ========================================================*/

/** Maximum size of #qapi_Device_Info_t valuebuf. */
#define QAPI_DEVICE_INFO_BUF_SIZE 128

#define JULIAN_YEAR 1980
#define DEVICE_INFO_MAX_INSTANCES 5
#define MAX_LEN_VAL 5


/**
 QAPI device information structure.
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
    struct
    {
      int buf[MAX_LEN_VAL]; /**< Response type: integer array. */
      uint32_t len; /**< Length of the array. */
    }valint;
  }u;   /**< Union of values. */
}qapi_Device_Info_t;

typedef void * qapi_Device_Info_Hndl_t;

/*=======================================================================================================
 * QAPI Device Info Callback.
 * @deprecated in DAM space. Use callback qapi_Device_Info_Callback_t2() instead.
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
#define qapi_Device_Info_Set(a, b, c, d)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_DEVICE_INFO_SET,     (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

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
 * Initializes the device information context.
 *
 * This function must be called before invoking other qapi_Device_Info APIs.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Init_v2 instead
 */
qapi_Status_t qapi_Device_Info_Init(void);

/**
 * Initializes the device information context.
 *
 * @param[in] device_info_hndl Pointer to device info hndl
 *
 * This function must be called before invoking other qapi_Device_Info APIs.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Device_Info_Init_v2(qapi_Device_Info_Hndl_t *device_info_hndl);


/**
 * Gets the device information for specified ID.
 *
 * @param[in] id     Information ID.
 * @param[out] info  Information received for the specified ID.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Get_v2() instead.
 */
qapi_Status_t qapi_Device_Info_Get(qapi_Device_Info_ID_t id, qapi_Device_Info_t *info);

/**
 * Gets the device information for specified ID.
 *
 * @param[in] device_info_hndl  Device info hndl.
 * @param[in] id                Information ID.
 * @param[out] info             Information received for the specified ID.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2() must have been called.
 */
qapi_Status_t qapi_Device_Info_Get_v2(qapi_Device_Info_Hndl_t device_info_hndl, qapi_Device_Info_ID_t id, qapi_Device_Info_t *info);


/**
 * Sets a device information callback.
 *
 * @param[in] id        Information ID.
 * @param[in] callback  Callback to be registered.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Set_Callback_v2() instead.
 */
qapi_Status_t qapi_Device_Info_Set_Callback(qapi_Device_Info_ID_t id, qapi_Device_Info_Callback_t callback);

/**
 * Sets a device information callback.
 *
 * @param[in] device_info_hndl  Device info hndl.
 * @param[in] id                Information ID.
 * @param[in] callback          Callback to be registered.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2() must have been called.
 */
qapi_Status_t qapi_Device_Info_Set_Callback_v2(qapi_Device_Info_Hndl_t device_info_hndl, qapi_Device_Info_ID_t id, qapi_Device_Info_Callback_t_v2 callback);

/**
 * Releases the device information context.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init() must have been called.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Release_v2() instead.
 */
qapi_Status_t qapi_Device_Info_Release(void);

/**
 * Releases the device information context.
 *
 * @param[in] device_info_hndl  Device info hndl.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2() must have been called.
 */
qapi_Status_t qapi_Device_Info_Release_v2(qapi_Device_Info_Hndl_t device_info_hndl);


/**
 * Resets the device.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @deprecated in DAM space. Use API qapi_Device_Info_Reset_v2() instead.
 */
qapi_Status_t qapi_Device_Info_Reset(void);

/**
 * Resets the device.
 *
 * @param[in] device_info_hndl  Device info hndl.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Device_Info_Reset_v2(qapi_Device_Info_Hndl_t device_info_hndl);

/**
 * @versiontable{2.0,2.45,
 * Data\_Services 1.2.0  &  Introduced. @tblendline
 * }
 *
 * Sets the device information for specified ID.
 *
 * @param[in] device_info_hndl  Device info hndl.
 * @param[in] id                Information ID.
 * @param[in] len               Length of the values to be set for the specified ID.
 * @param[in] *val              Values to be set for the specified ID.
 *
 *  @return QAPI_OK on success, QAPI_ERROR on failure.
 *
 * @dependencies
 * Before calling this API, qapi_Device_Info_Init_v2() must have been called.
 */
qapi_Status_t qapi_Device_Info_Set(qapi_Device_Info_Hndl_t device_info_hndl, qapi_Device_Info_ID_t id, int len, int *val);


#endif

/** @} */ /* end_addtogroup qapi_device_info */

#endif
