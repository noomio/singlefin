/*===========================================================================
                         TEST_MQTT_MODULE.C

DESCRIPTION
   This file deals with MQTT related procedure.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_app_module.h"
#include "test_utils.h"
#include "qapi_dnsc.h"
#include "qapi_mqtt.h"
#include "test_mqtt_module.h"

qapi_Net_MQTT_Hndl_t mqtt_ctx = NULL; 
qapi_Net_MQTT_Config_t mqtt_cfg;
static bool mqtt_dam_running = false;

/* Subscriber */
char* mqtt_dam_sub_topic[3] = {"sampleAA0", "sampleAA1", "sampleAA2"};
UCHAR mqtt_dam_sub_qos = QOS_LEVEL_0;

/* Publish */
char* mqtt_dam_pub_topic[3] = {"sampleBB0", "sampleBB1", "sampleBB2"};
char* mqtt_dam_pub_string[3] = {"Hello, world, 0!", "Hello, world, 1!", "Hello, world, 2!"};
UCHAR mqtt_dam_pub_qos = QOS_LEVEL_0;
bool mqtt_dam_pub_retain = false;
UINT mqtt_dam_pub_msg_id_qos_1 = 0; // Store message Id of the published msg with QoS_1
UINT mqtt_dam_pub_msg_id_qos_2 = 0; // Store message Id of the published msg with QoS_2

/*Clean session flag cfg*/
bool mqtt_dam_cfg_clean_session = true;

extern TX_BYTE_POOL *byte_pool_test;
extern uint32_t local_ipaddr_v4;
extern bool local_ipaddr_v4_valid;

/**
   @brief This function is to exercise QAPIs for MQTT routine.

*/
void mqtt_dam_sample_init_app_routine(void)
{
  int ret = QAPI_DSS_ERROR;

  do
  {
    if(mqtt_dam_running || !byte_pool_test) 
    {
      LOG_INFO("MQTT DAM thread is already running, terminate it before running again\n");
      return;
    }

    mqtt_dam_running = true;

    if(QAPI_OK != qapi_Net_MQTT_New(&mqtt_ctx)) 
    {
      LOG_INFO("MQTT DAM initialized failed\n");
      mqtt_dam_running = false;
      return;
    }

    ret = qapi_Net_MQTT_Pass_Pool_Ptr(mqtt_ctx, byte_pool_test);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Could not pass byte pool ptr\n");
      break;
    }
    
    /* Connect callback, called by mqtt after receiving connection ack */
    qapi_Net_MQTT_Set_Connect_Callback(mqtt_ctx, mqtt_dam_connect_callback);
    /* Subcribe callback, called by mqtt after receving suback */
    qapi_Net_MQTT_Set_Subscribe_Callback(mqtt_ctx, mqtt_dam_subscribe_callback);
    /* Message Callback, called by mqtt after receving any cmd from Server */
    qapi_Net_MQTT_Set_Message_Callback(mqtt_ctx, mqtt_dam_message_callback);
    /* Publish Callback, called by mqtt after receving any cmd from Server */
    qapi_Net_MQTT_Set_Publish_Callback(mqtt_ctx, mqtt_dam_publish_callback);

    /* Config MQTT */
    ret = mqtt_dam_Config(&mqtt_cfg);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Config failure\n");
      break;
    }

    /* Connect to MQTT server */
    ret = qapi_Net_MQTT_Connect(mqtt_ctx, &mqtt_cfg);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Connect to MQTT server failure\n");
      break;
    }
	
  } while(0);

  if(QAPI_OK != ret || !mqtt_dam_running)
  {
    LOG_INFO("MQTT DAM failure: %d\n", ret);
    qapi_Net_MQTT_Destroy(mqtt_ctx);
    mqtt_ctx = NULL;
    mqtt_dam_running = false;
  }
  
}

/**
   @brief MQTT client configure routine.

*/
bool mqtt_dam_Config(qapi_Net_MQTT_Config_t* cfg)
{
  int ret = QAPI_DSS_ERROR;
  struct sockaddr_in* local_sin4  = NULL;
  struct sockaddr_in* remote_sin4 = NULL;

  LOG_INFO("MQTT DAM: mqtt_dam_Config start \n");

  do
  {
    if(!cfg || !local_ipaddr_v4_valid)
    {
      LOG_INFO("MQTT DAM: mqtt_dam_Config null cfg or no local IPv4 Addr \n");
      break;
    }

    /* Set local */
    local_sin4 = (struct sockaddr_in*)&cfg->local;
    local_sin4->sin_addr.s_addr = local_ipaddr_v4;
    local_sin4->sin_family  = AF_INET;
    local_sin4->sin_port    = htons(MQTT_CLIENT_PORT);

    /* Set remote */
    remote_sin4 = (struct sockaddr_in*)&cfg->remote;

    /* Set IP Address of remote server. */
    /* MQTT_REMOTE_SERVER_IPv4 is defined with IP address value.
        If user want to use URL of remote server, 
        please use DNS QAPI to do dns query for that URL,
        and set  sin_addr.s_addr with the resolve IP address.*/
    remote_sin4->sin_addr.s_addr = inet_addr(MQTT_REMOTE_SERVER_IPv4);
    remote_sin4->sin_family = AF_INET;
    remote_sin4->sin_port   = htons(MQTT_REMOTE_PORT);

    cfg->nonblocking_connect = false;
    cfg->client_id_len = 0;
    cfg->keepalive_duration = MQTT_KEEP_ALIVE_TIMER;
    cfg->clean_session      = mqtt_dam_cfg_clean_session;
    cfg->will_topic         = NULL;
    cfg->will_topic_len     = 0;
    cfg->will_message       = NULL;
    cfg->will_message_len   = 0;
    cfg->will_retained      = false;
    cfg->will_qos           = QOS_LEVEL_0;
    cfg->username           = NULL;
    cfg->username_len       = 0;
    cfg->password           = NULL;
    cfg->password_len       = 0;

    ret = QAPI_DSS_SUCCESS;
  } while(0);

  LOG_INFO("MQTT DAM: mqtt_dam_Config end with ret %d \n", ret);
  return ret;

}

/**
   @brief callback funtion of MQTT connect request.

*/
void mqtt_dam_connect_callback(qapi_Net_MQTT_Hndl_t mqtt, int32_t reason)
{
  switch(reason)
  {
    case QAPI_NET_MQTT_CONNECT_SUCCEEDED_E:
    {
      LOG_INFO("MQTT DAM: Connect Callback - Success\n");
      tx_event_flags_set(dam_obj_param.dam_obj_signal, DAM_SIG_MQTT_CONNECT, TX_OR);
    }
    break;

    case QAPI_NET_MQTT_TCP_CONNECT_FAILED_E:
    case QAPI_NET_MQTT_SSL_CONNECT_FAILED_E:
    case QAPI_NET_MQTT_CONNECT_FAILED_E:
    {
      LOG_INFO("MQTT DAM: Connect Callback - Failed with reason %d \n", reason);
      tx_event_flags_set(dam_obj_param.dam_obj_signal, DAM_SIG_MQTT_DISCONNECT, TX_OR);
    }
    break;

    default:
    {
      LOG_INFO("MQTT DAM: Connect Callback - Unknown reason %d \n", reason);
    }
  }
}

/**
   @brief callback funtion of MQTT subscribe request.

*/	
void mqtt_dam_subscribe_callback(qapi_Net_MQTT_Hndl_t mqtt,
  int32_t reason,
  const uint8_t* topic,
  int32_t topic_length,
  int32_t qos,
  const void* sid)
{
  switch(reason)
  {
    case QAPI_NET_MQTT_SUBSCRIBE_GRANTED_E:
    {
      LOG_INFO("MQTT DAM: Subscribe Granted\n");
    }
    break;

    case QAPI_NET_MQTT_SUBSCRIBE_DENIED_E:
    {
      LOG_INFO("MQTT DAM: Subscribe Denied\n");
    }
    break;

    case QAPI_NET_MQTT_SUBSCRIBE_MSG_E:
    {
      LOG_INFO("MQTT DAM: Subscribe Messafe\n");
    }
    break;

    default:
    {
      LOG_INFO("MQTT DAM: Subscribe Callback - Unknown reason %d \n", reason);
    }
  }
}

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
  const void* sid)
{
  int ret = QAPI_DSS_ERROR;

  LOG_INFO("MQTT DAM: mqtt_dam_message_callback start \n");
  
  if(topic)
  {
    LOG_INFO("MQTT DAM: mqtt_dam_message_callback:  topic_length %d, topic %s \n", topic_length, topic);
    ret = qapi_Net_MQTT_Unsubscribe(mqtt_ctx, (uint8_t *)topic);

    if(QAPI_DSS_SUCCESS != ret)
    {
      LOG_INFO("MQTT DAM: mqtt_dam_message_callback: unsubscribe failure \n");
    }
  }

  if(msg)
  {
    LOG_INFO("MQTT DAM: mqtt_dam_message_callback:  topic_length %d, topic %s \n", msg_length, msg);
  }

  LOG_INFO("MQTT DAM: mqtt_dam_message_callback qos %d reason %d \n", qos, reason);
  
}

/**
   @brief callback funtion of MQTT publish request.

*/
void mqtt_dam_publish_callback(qapi_Net_MQTT_Hndl_t mqtt,
  enum QAPI_NET_MQTT_MSG_TYPES msgtype,
  int qos,
  uint16_t msg_id)
{
  if(mqtt_dam_pub_msg_id_qos_1 == msg_id)
  {
    LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  published QoS_1 message with msg_id %d \n", msg_id);
    if (msgtype == QAPI_NET_MQTT_PUBACK)
    {
      LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  receive PUBACK for QoS_1 \n");
    }
  }
  else if(mqtt_dam_pub_msg_id_qos_2 == msg_id)
  {
    LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  published QoS_2 message with msg_id %d \n", msg_id);
    if (msgtype == QAPI_NET_MQTT_PUBREC)
    {
      LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  receive PUBREC for QoS_2 \n");
    }
    else if(msgtype == QAPI_NET_MQTT_PUBCOMP)
    {
      LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  receive PUBCOMP for QoS_2 \n");
     }
  }
  else
  {
    LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  unknown msg_id %d \n", msg_id);
  }

  LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  msgtype %d \n", msgtype);
  LOG_INFO("MQTT DAM: mqtt_dam_publish_callback:  qos %d \n", qos);
}

/**
   @brief MQTT client subscribe/publish routine.

*/
void mqtt_dam_sample_app_service_routine()
{
  int ret = QAPI_DSS_ERROR;

  LOG_INFO("MQTT DAM: mqtt_dam_sample_app_service_routine start \n");
  do
  {
    /* Subscribe topics */
    for(int i = QOS_LEVEL_0; i <= QOS_LEVEL_2; i++)
    {
      mqtt_dam_sub_qos = i;
      /* Subscribe topic for QoS_i */
      ret = qapi_Net_MQTT_Subscribe(mqtt_ctx, (uint8_t *)(mqtt_dam_sub_topic[i]), mqtt_dam_sub_qos);
      if(QAPI_OK != ret)
      {
        LOG_INFO("MQTT DAM: Subscribe failure of QoS %d \n", i);
        break;
      }
    }

    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Subscribe failure\n");
      break;
    }

    /* Publish Messages -QoS_0 */
    mqtt_dam_pub_qos = QOS_LEVEL_0;
    ret = qapi_Net_MQTT_Publish(mqtt_ctx, (uint8_t *)(mqtt_dam_pub_topic[QOS_LEVEL_0]), 
                                                     (uint8_t *)(mqtt_dam_pub_string[QOS_LEVEL_0]), strlen(mqtt_dam_pub_string[QOS_LEVEL_0]), 
                                                     mqtt_dam_pub_qos, mqtt_dam_pub_retain);

    LOG_INFO("MQTT DAM: do Publish topic sampleBB0 get ret %d \n", ret);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Publish failure\n");
      break;
    }

     /* Publish Messages -QoS_1 */
    mqtt_dam_pub_qos = QOS_LEVEL_1;
    ret = qapi_Net_MQTT_Publish_Get_Msg_Id(mqtt_ctx, (uint8_t *)(mqtt_dam_pub_topic[QOS_LEVEL_1]),
                                                                          (uint8_t *)(mqtt_dam_pub_string[QOS_LEVEL_1]), strlen(mqtt_dam_pub_string[QOS_LEVEL_1]),
                                                                          mqtt_dam_pub_qos, mqtt_dam_pub_retain, &mqtt_dam_pub_msg_id_qos_1);

    LOG_INFO("MQTT DAM: do Publish topic sampleBB1 get ret %d \n", ret);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Publish failure\n");
      break;
    }
    else
    {
      LOG_INFO("MQTT DAM: Published QoS_1 Message Id %d\n", mqtt_dam_pub_msg_id_qos_1);
    }

    /* Publish Messages -QoS_2 */
    mqtt_dam_pub_qos = QOS_LEVEL_2;
    ret = qapi_Net_MQTT_Publish_Get_Msg_Id(mqtt_ctx, (uint8_t *)(mqtt_dam_pub_topic[QOS_LEVEL_2]),
                                                                          (uint8_t *)(mqtt_dam_pub_string[QOS_LEVEL_2]), strlen(mqtt_dam_pub_string[QOS_LEVEL_2]),
                                                                          mqtt_dam_pub_qos, mqtt_dam_pub_retain, &mqtt_dam_pub_msg_id_qos_2);

    LOG_INFO("MQTT DAM: do Publish topic sampleBB2 get ret %d \n", ret);
    if(QAPI_OK != ret)
    {
      LOG_INFO("MQTT DAM: Publish failure\n");
      break;
    }
    else
    {
      LOG_INFO("MQTT DAM: Published QoS_2 Message Id %d\n", mqtt_dam_pub_msg_id_qos_2);
    }

  } while(0);

  LOG_INFO("MQTT DAM: mqtt_dam_sample_app_service_routine end with ret %d\n", ret);
  if(QAPI_OK != ret)
  {
    tx_event_flags_set(dam_obj_param.dam_obj_signal, DAM_SIG_MQTT_DISCONNECT, TX_OR);  
  }
}

/**
   @brief MQTT client unsubscribe/disconnect routine.

*/
void mqtt_dam_sample_app_cleanup_routine()
{
  LOG_INFO("MQTT DAM: mqtt_dam_sample_app_cleanup_routine start \n");

  /* Disconnect and Destory MQTT */
  if(mqtt_ctx)
  {
    LOG_INFO("MQTT DAM: Disconnect");
    qapi_Net_MQTT_Disconnect(mqtt_ctx);
    LOG_INFO("MQTT DAM: Destory");
    qapi_Net_MQTT_Destroy(mqtt_ctx);
    mqtt_ctx = NULL;
    mqtt_dam_running = false;
  }
}



/**
   @brief This function is common wrapper to run module routine after data setup.

*/
void dam_app_routine(qapi_DSS_Hndl_t hndl)
{
  mqtt_dam_sample_init_app_routine();
  return;
}

/**
   @brief This function is the common function to check and process signals for the sample app modules.

*/
void dam_app_signal_wait(uint32_t received_sigs)
{
  /* check and handle the signals that app interests */
  if(received_sigs & DAM_SIG_MQTT_CONNECT)
  {
    LOG_INFO("DAM_MQTT_CONNECT signal\n");
    mqtt_dam_sample_app_service_routine(); // Use QAPI for subscriber and publish
  }
  else if(received_sigs & DAM_SIG_MQTT_DISCONNECT)
  {
    LOG_INFO("DAM_MQTT_DISCONNECT signal\n");
    mqtt_dam_sample_app_cleanup_routine(); // Use QAPI for unsubscriber and disconnect
  }
  return;
}


