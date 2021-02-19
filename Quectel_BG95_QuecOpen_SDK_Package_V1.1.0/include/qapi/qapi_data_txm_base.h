/**
 Copyright (c) 2018-2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
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
 *  
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 ======================================================================*/
 
/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY.�
*/

 /**
  @file qapi_data_txm_base.h 
  Contains QAPI Driver ID mappings for Dataservices

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/

#ifndef QAPI_DATA_TXM_BASE_H
#define QAPI_DATA_TXM_BASE_H 


#ifdef __cplusplus
extern "C" {
#endif

#include "qapi_txm_base.h"

/** Maximum IDs for dataservices is defined by macro TXM_QAPI_DATA_SERVICES_NUM_IDS. */

#define  TXM_QAPI_DSS_BASE                        TXM_QAPI_DATA_SERVICES_BASE
#define  TXM_QAPI_DSS_NUM_IDS                     30

#define  TXM_QAPI_SOCKET_BASE                     TXM_QAPI_DSS_BASE + TXM_QAPI_DSS_NUM_IDS
#define  TXM_QAPI_SOCKET_NUM_IDS                  30

#define  TXM_QAPI_NET_BASE                        TXM_QAPI_SOCKET_BASE + TXM_QAPI_SOCKET_NUM_IDS
#define  TXM_QAPI_NET_NUM_IDS                     30

#define  TXM_QAPI_NET_SSL_BASE                    TXM_QAPI_NET_BASE + TXM_QAPI_NET_NUM_IDS
#define  TXM_QAPI_NET_SSL_NUM_IDS                 20

#define  TXM_QAPI_NET_DNSC_BASE                   TXM_QAPI_NET_SSL_BASE + TXM_QAPI_NET_SSL_NUM_IDS
#define  TXM_QAPI_NET_DNSC_NUM_IDS                20

#define  TXM_QAPI_NET_HTTP_BASE                   TXM_QAPI_NET_DNSC_BASE + TXM_QAPI_NET_DNSC_NUM_IDS
#define  TXM_QAPI_NET_HTTP_NUM_IDS                20

#define  TXM_QAPI_MQTT_BASE                       TXM_QAPI_NET_HTTP_BASE + TXM_QAPI_NET_HTTP_NUM_IDS
#define  TXM_QAPI_MQTT_NUM_IDS                    15

#define TXM_QAPI_DEVICE_INFO_BASE                 TXM_QAPI_MQTT_BASE + TXM_QAPI_MQTT_NUM_IDS
#define TXM_QAPI_DEVICE_INFO_NUM_IDS              5

#define TXM_QAPI_NET_LWM2M_BASE                   TXM_QAPI_DEVICE_INFO_BASE + TXM_QAPI_DEVICE_INFO_NUM_IDS
#define TXM_QAPI_NET_LWM2M_NUM_IDS                20

#define TXM_QAPI_ATFWD_BASE                       TXM_QAPI_NET_LWM2M_BASE + TXM_QAPI_NET_LWM2M_NUM_IDS
#define TXM_QAPI_ATFWD_NUM_IDS                    5

#define TXM_DATA_DRIVER_BASE                      TXM_QAPI_ATFWD_BASE + TXM_QAPI_ATFWD_NUM_IDS
#define TXM_DATA_DRIVER_NUM_IDS                   2

#define TXM_QAPI_CLI_BASE                         TXM_DATA_DRIVER_BASE + TXM_DATA_DRIVER_NUM_IDS
#define TXM_QAPI_CLI_NUM_IDS                      2

#define TXM_DATA_QFLOG_BASE                       TXM_QAPI_CLI_BASE + TXM_QAPI_CLI_NUM_IDS
#define TXM_DATA_QFLOG_NUM_IDS                    2

#define TXM_QAPI_NET_LWM2M_BASE_V2                TXM_DATA_QFLOG_BASE + TXM_DATA_QFLOG_NUM_IDS
#define TXM_QAPI_NET_LWM2M_NUM_IDS_V2             10

#define TXM_QAPI_DEVICE_INFO_BASE_V2              TXM_QAPI_NET_LWM2M_BASE_V2 + TXM_QAPI_NET_LWM2M_NUM_IDS_V2
#define TXM_QAPI_DEVICE_INFO_NUM_IDS_V2           10

#define TXM_QAPI_COAP_BASE                        TXM_QAPI_DEVICE_INFO_BASE_V2 + TXM_QAPI_DEVICE_INFO_NUM_IDS_V2
#define TXM_DATA_COAP_NUM_IDS                     15

#define TXM_QAPI_MQTT_BASE2                       TXM_QAPI_COAP_BASE + TXM_DATA_COAP_NUM_IDS
#define TXM_QAPI_MQTT_NUM_IDS_2                   10

#define TXM_QAPI_VERSION_BASE                     TXM_QAPI_MQTT_BASE2 + TXM_QAPI_MQTT_NUM_IDS_2
#define TXM_DATA_VERSION_NUM_IDS                  5

#define TXM_DATA_QFLOG_BASE_V2                    TXM_QAPI_VERSION_BASE + TXM_DATA_VERSION_NUM_IDS
#define TXM_DATA_QFLOG_NUM_IDS_V2                 5

#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QAPI_TXM_BASE_H */
