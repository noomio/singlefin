/*===========================================================================
                         TEST_MQTT_MODULE.H

DESCRIPTION
   This header file contains declarations of MQTT routine
   functions.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

#ifndef TEST_MQTT_MODULE_H
#define TEST_MQTT_MODULE_H

#include "test_sample_module.h"

/* User configuration area -- start */

/* local port of MQTT client, 0 is the default value. */
#define MQTT_CLIENT_PORT 0

/* remote port of MQTT server, 1883 is the default value. */
#define MQTT_REMOTE_PORT 1883

/*IPv4 address of MQTT server*/
#define MQTT_REMOTE_SERVER_IPv4 "10.239.124.12"

/* If user want to use URL of mqtt server, it shall use DNS QAPI to resolve IP address of the server first.
#define MQTT_REMOTE_SERVER_URL "sample.mqttserver.com"
*/

/* Keep Alive timer in secs */
#define MQTT_KEEP_ALIVE_TIMER 60

/* User configuration area -- end */

#define htons(s) ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
#define QOS_LEVEL_0 0
#define QOS_LEVEL_1 1
#define QOS_LEVEL_2 2

/**
   @brief This function is to initial MQTT DAM after data call is connected.

*/
void mqtt_dam_sample_init_app_routine(void);

/**
   @brief callback funtion of MQTT connect request.

*/
void mqtt_dam_connect_callback(qapi_Net_MQTT_Hndl_t mqtt, int32_t reason);

/**
   @brief callback funtion of MQTT subscribe request.

*/	
void mqtt_dam_subscribe_callback(qapi_Net_MQTT_Hndl_t mqtt,
  int32_t reason,
  const uint8_t* topic,
  int32_t topic_length,
  int32_t qos,
  const void* sid);

/**
   @brief callback funtion when receiving MQTT message.

*/
void mqtt_dam_message_callback(qapi_Net_MQTT_Hndl_t mqtt,
  int32_t reason,
  const uint8_t* topic,
  int32_t topic_length,
  const uint8_t* msg,
  int32_t msg_length,
  int32_t qos,
  const void* sid);

/**
   @brief callback funtion of MQTT publish request.

*/
void mqtt_dam_publish_callback(qapi_Net_MQTT_Hndl_t mqtt,
  enum QAPI_NET_MQTT_MSG_TYPES msgtype,
  int qos,
  uint16_t msg_id);

/**
   @brief MQTT client configure routine.

*/
bool mqtt_dam_Config(qapi_Net_MQTT_Config_t* cfg);

/**
   @brief MQTT client subscribe/publish routine.

*/
void mqtt_dam_sample_app_service_routine(void);

/**
   @brief MQTT client unsubscribe/disconnect routine.

*/
void mqtt_dam_sample_app_cleanup_routine(void);

#endif

