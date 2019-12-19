/** @file qapi_mqtt.h
*/
/* Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
All rights reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.

*/

#ifndef _QAPI_NET_MQTT_H
#define _QAPI_NET_MQTT_H
#define MQTT_CLIENT_SECURITY
#define CLI_MQTT_PORT 1883
#define CLI_MQTT_SECURE_PORT 8883

#include "qapi_status.h"
#include "txm_module.h"
#include "qapi_driver_access.h"
#ifdef MQTT_CLIENT_SECURITY
#include "qapi_ssl.h"
#endif
#include "stdbool.h"	 //QuectelModifyFlag
#include "qapi_socket.h" //QuectelModifyFlag

// MQTT ID defines
#define  TXM_QAPI_MQTT_NEW                TXM_QAPI_MQTT_BASE + 1
#define  TXM_QAPI_MQTT_DESTROY            TXM_QAPI_MQTT_BASE + 2
#define  TXM_QAPI_MQTT_CONNECT            TXM_QAPI_MQTT_BASE + 3
#define  TXM_QAPI_MQTT_DISCONNECT         TXM_QAPI_MQTT_BASE + 4
#define  TXM_QAPI_MQTT_PUBLISH            TXM_QAPI_MQTT_BASE + 5
#define  TXM_QAPI_MQTT_SUBSCRIBE          TXM_QAPI_MQTT_BASE + 6
#define  TXM_QAPI_MQTT_UNSUSCRIBE         TXM_QAPI_MQTT_BASE + 7
#define  TXM_QAPI_MQTT_SET_CONNECT_CB     TXM_QAPI_MQTT_BASE + 8
#define  TXM_QAPI_MQTT_SET_SUBSCRIBE_CB   TXM_QAPI_MQTT_BASE + 9
#define  TXM_QAPI_MQTT_SET_MESSAGE_CB     TXM_QAPI_MQTT_BASE + 10
#define  TXM_QAPI_MQTT_SET_PUBLISH_CB     TXM_QAPI_MQTT_BASE + 11
#define  TXM_QAPI_MQTT_PUBLISH_ID         TXM_QAPI_MQTT_BASE + 12
#define  TXM_QAPI_MQTT_SET_POOL_PTR       TXM_QAPI_MQTT_BASE+13
#define  TXM_QAPI_MQTT_SET_ALLOW_UNSUB_PUB TXM_QAPI_MQTT_BASE + 14

    
/***********************************************************
 * Constant definitions
 **********************************************************/
/** @addtogroup qapi_net_mqtt_datatypes
@{ */

/** @name Net MQTT Length Defines
@{ */

#define QAPI_NET_MQTT_MAX_CLIENT_ID_LEN 128 /**< Maximum client ID length. The MQTT stack uses the same value. */
#define QAPI_NET_MQTT_MAX_TOPIC_LEN 128    /**< Maximum topic length. */

/** @} */ /* end_namegroup */

/** Reason codes for a subscription callback. */
enum /** @cond */QAPI_NET_MQTT_SUBSCRIBE_CBK_MSG /** @endcond */{
  QAPI_NET_MQTT_SUBSCRIBE_DENIED_E,  /**< Subscription is denied by the broker. */
  QAPI_NET_MQTT_SUBSCRIBE_GRANTED_E, /**< Subscription is granted by the broker. */
  QAPI_NET_MQTT_SUBSCRIBE_MSG_E      /**< Message was received from the broker. */
};

typedef enum QAPI_NET_MQTT_SUBSCRIBE_CBK_MSG qapi_Net_MQTT_Subscribe_Cbk_Msg_t;

/** Connection callback messages. */
enum QAPI_NET_MQTT_CONNECT_CBK_MSG {
  QAPI_NET_MQTT_CONNECT_SUCCEEDED_E,  /**< MQTT connect succeeded. */
  QAPI_NET_MQTT_TCP_CONNECT_FAILED_E, /**< TCP connect failed. */
  QAPI_NET_MQTT_SSL_CONNECT_FAILED_E, /**< SSL connect failed. */
  QAPI_NET_MQTT_CONNECT_FAILED_E,     /**< QAPI_MQTT connect failed. */
};

typedef enum QAPI_NET_MQTT_CONNECT_CBK_MSG qapi_Net_MQTT_Connect_Cbk_Msg_t;


/** Connection states. */
enum QAPI_NET_MQTT_CONN_STATE {
  QAPI_NET_MQTT_ST_DORMANT_E, /**< Connection is idle. */
  QAPI_NET_MQTT_ST_TCP_CONNECTING_E, /**< TCP is connecting. */
  QAPI_NET_MQTT_ST_TCP_CONNECTED_E, /**< TCP is connected. */
  QAPI_NET_MQTT_ST_SSL_CONNECTING_E,/**< SSL is connecting. */
  QAPI_NET_MQTT_ST_SSL_CONNECTED_E,/**< SSL is connected. */
  QAPI_NET_MQTT_ST_MQTT_CONNECTING_E,/**< MQTT is connecting. */
  QAPI_NET_MQTT_ST_MQTT_CONNECTED_E,/**< MQTT is connected. */
  QAPI_NET_MQTT_ST_MQTT_TERMINATING_E,/**< MQTT connection is terminating. */
  QAPI_NET_MQTT_ST_SSL_TERMINATING_E,/**< SSL connection is terminating. */
  QAPI_NET_MQTT_ST_TCP_TERMINATING_E,/**< TCP connection is terminating. */
  QAPI_NET_MQTT_ST_DYING_E, /**< MQTT connection is dying. */
  QAPI_NET_MQTT_ST_DEAD_E, /**< MQTT connection is dead. */
};

/** MQTT message types. */
enum QAPI_NET_MQTT_MSG_TYPES{
  QAPI_NET_MQTT_CONNECT = 1, /**< Connect. */
  QAPI_NET_MQTT_CONNACK,     /**< Connection acknowledgement. */
  QAPI_NET_MQTT_PUBLISH,	 /**< Publish. */
  QAPI_NET_MQTT_PUBACK,      /**< Publish acknowledgement. */
  QAPI_NET_MQTT_PUBREC,		 /**< PubRec. */
  QAPI_NET_MQTT_PUBREL,		 /**< PubRel. */
  QAPI_NET_MQTT_PUBCOMP,	 /**< PubComp. */
  QAPI_NET_MQTT_SUBSCRIBE,	 /**< Subscribe. */
  QAPI_NET_MQTT_SUBACK,		 /**< Subscribe acknowledgement. */
  QAPI_NET_MQTT_UNSUBSCRIBE, /**< Unsubscribe. */
  QAPI_NET_MQTT_UNSUBACK,	 /**< Unsubscribe acknowledgement. */
  QAPI_NET_MQTT_PINGREQ,	 /**< Ping request. */
  QAPI_NET_MQTT_PINGRESP,	 /**< Ping response. */
  QAPI_NET_MQTT_DISCONNECT,	 /**< Disconnect. */
  QAPI_NET_MQTT_MQTT_NO_RESPONSE_MSG_REQD, /**< No response message is required. */
  QAPI_NET_MQTT_INVALID_RESP, /**< Invalid response. */
};


typedef enum QAPI_NET_MQTT_CONN_STATE qapi_Net_MQTT_Conn_State_t;

/*========================================================
 * MQTT Structures
 ========================================================*/

/**
 * @brief MQTT socket options.
 */
typedef struct __qapi_Net_MQTT_Sock_Opts_s
{
    int32_t level;
    /**< Specifies the protocol level at which the option resides. */
    int32_t opt_name;
    /**< Socket option name. */
    void *opt_value;
    /**< Socket option value. */
    uint32_t opt_len;
    /**< Socket option length. */
}  qapi_Net_MQTT_Sock_Opts_t;

/** MQTT configuration. */
struct qapi_Net_MQTT_config_s {
  struct sockaddr local; /**< MQTT client IP address and port number. */
  struct sockaddr remote; /**< MQTT server IP address and port number. */
  bool nonblocking_connect; /**< Blocking or nonblocking MQTT connection. */
  uint8_t client_id[QAPI_NET_MQTT_MAX_CLIENT_ID_LEN]; /**< MQTT vlient ID. */
  int32_t client_id_len; /**< MQTT client ID length. */
  uint32_t keepalive_duration; /**< Conection keepalive duration in seconds. */
  uint8_t clean_session; /**< Clean session flag; 0 -- No clean session, 1 -- clean session. */
  uint8_t* will_topic; /**< Will topic name. */
  int32_t will_topic_len; /**< Will topic length. */
  uint8_t* will_message; /**< Will message. */
  int32_t will_message_len; /**< Will message length. */
  uint8_t will_retained; /**< Will retain flag. */
  uint8_t will_qos; /**< Will QOS. */
  uint8_t* username; /**< Client username. */
  int32_t username_len; /**< Client user length. */ 
  uint8_t* password; /**< Client password. */
  int32_t password_len; /**< Client password length. */
  uint32_t connack_timed_out_sec; /**<  Timeout value for which the client waits for the CONNACK packet from the server. */
  uint32_t sock_options_cnt; /**< Number of socket options. */
  qapi_Net_MQTT_Sock_Opts_t *sock_options; /**< Socket options*/
#ifdef MQTT_CLIENT_SECURITY
  qapi_Net_SSL_Config_t ssl_cfg; /**< SSL configuration. */
  qapi_Net_SSL_CAList_t ca_list; /**< SSL CA cert details. */
  qapi_Net_SSL_Cert_t cert; /**< SSL cert details. */
#endif
} ;
typedef struct qapi_Net_MQTT_config_s qapi_Net_MQTT_Config_t;

/** @} */ /* end_addtogroup qapi_net_mqtt_datatypes */


/*MQTT handle*/
typedef void* qapi_Net_MQTT_Hndl_t;

/*Connect callback typedef*/
typedef void (*qapi_Net_MQTT_Connect_CB_t)(qapi_Net_MQTT_Hndl_t mqtt, int32_t reason);

/*Subscribe callback typedef*/
typedef void (*qapi_Net_MQTT_Subscribe_CB_t)(qapi_Net_MQTT_Hndl_t mqtt,
    int32_t reason,
    const uint8_t* topic,
    int32_t topic_length,
    int32_t qos,
    const void* sid);

/* Message callback typedef*/
typedef void (*qapi_Net_MQTT_Message_CB_t)(qapi_Net_MQTT_Hndl_t mqtt,
    int32_t reason,
    const uint8_t* topic,
    int32_t topic_length,
    const uint8_t* msg,
    int32_t msg_length,
    int32_t qos,
    const void* sid);

typedef void (*qapi_Net_MQTT_Publish_CB_t)(qapi_Net_MQTT_Hndl_t mqtt,
    enum QAPI_NET_MQTT_MSG_TYPES msgtype,
    int qos,
    uint16_t msg_id);

static __inline int mqtt_set_byte_pool(qapi_Net_MQTT_Hndl_t  hndl, void *mqtt_byte_pool_ptr);

static __inline int mqtt_destroy_indirection(qapi_Net_MQTT_Hndl_t hndl);

static __inline void qapi_mqtt_cb_uspace_dispatcher(UINT cb_id,
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
  void (*pfn_app_cb1) (qapi_Net_MQTT_Hndl_t , int32_t);
  void (*pfn_app_cb2) (qapi_Net_MQTT_Hndl_t , int32_t , const uint8_t* ,
                        int32_t , int32_t , const void*);
  void (*pfn_app_cb3) (qapi_Net_MQTT_Hndl_t , int32_t , const uint8_t*, int32_t,
                       const uint8_t* , int32_t, int32_t, const void* );
  void (*pfn_app_cb4) (qapi_Net_MQTT_Hndl_t, enum QAPI_NET_MQTT_MSG_TYPES, int, uint16_t );
  if(cb_id ==DATA_CUSTOM_CB_MQTT_SET_CONNECT)
  {
    pfn_app_cb1 = (void (*)(qapi_Net_MQTT_Hndl_t, int32_t))app_cb;
	
   (pfn_app_cb1)((qapi_Net_MQTT_Hndl_t)cb_param1, 
                       (int32_t)cb_param2);
  }	
  else if (cb_id == DATA_CUSTOM_CB_MQTT_SET_SUBSCRIBE)
  {
    pfn_app_cb2 = (void (*)(qapi_Net_MQTT_Hndl_t , int32_t , const uint8_t* ,
                        int32_t , int32_t , const void*))app_cb;
	
    pfn_app_cb2 ((qapi_Net_MQTT_Hndl_t)cb_param1 , (int32_t)cb_param2, 
                    (const uint8_t*)cb_param3 , (int32_t)cb_param4 ,
                    (int32_t)cb_param5 ,(const void*)cb_param6);
  }
  else if (cb_id == DATA_CUSTOM_CB_MQTT_SET_MESSAGE)
  {
    pfn_app_cb3 = (void (*)(qapi_Net_MQTT_Hndl_t , int32_t , const uint8_t* ,
                        int32_t , const uint8_t*, int32_t, int32_t , const void*))app_cb;
	
    pfn_app_cb3 ((qapi_Net_MQTT_Hndl_t)cb_param1 , (int32_t)cb_param2, 
                    (const uint8_t*)cb_param3 , (int32_t)cb_param4 ,
                    (const uint8_t*)cb_param5 , (int32_t)cb_param6 ,
                    (int32_t)cb_param7 ,(const void*)cb_param8);
  }
  else if (cb_id == DATA_CUSTOM_CB_MQTT_SET_PUBLISH)
  {
    pfn_app_cb4 = (void (*)(qapi_Net_MQTT_Hndl_t, enum QAPI_NET_MQTT_MSG_TYPES, int, uint16_t))app_cb;
	
    pfn_app_cb4 ((qapi_Net_MQTT_Hndl_t)cb_param1 , (enum QAPI_NET_MQTT_MSG_TYPES)cb_param2, 
                    (int)cb_param3 , (uint16_t)cb_param4);
  }
}

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_Net_MQTT_New(a)                        ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_NEW, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/*
 * Macro that Releases Byte Pool Pointer for MQTT Application
 * Parameter 'a' : Handle 
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 * (This Macro is only used in DAM Space)
 */

#define qapi_Net_MQTT_Destroy(a)                     mqtt_destroy_indirection(a)

/** @} */

#define qapi_Net_MQTT_Connect(a,b)                  ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_CONNECT            , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Disconnect(a)                 ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_DISCONNECT         , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Publish(a,b,c,d,e,f)          ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_PUBLISH            , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Publish_Get_Msg_Id(a,b,c,d,e,f,g)     ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_PUBLISH_ID         , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) e, (ULONG) f, (ULONG) g, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Subscribe(a,b,c)              ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SUBSCRIBE          , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Unsubscribe(a,b)              ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_UNSUSCRIBE         , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Set_Connect_Callback(a,b)     ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SET_CONNECT_CB     , (ULONG) a, (ULONG) b, (ULONG) qapi_mqtt_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Set_Subscribe_Callback(a,b)   ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SET_SUBSCRIBE_CB   , (ULONG) a, (ULONG) b, (ULONG) qapi_mqtt_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Set_Message_Callback(a,b)     ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SET_MESSAGE_CB     , (ULONG) a, (ULONG) b, (ULONG) qapi_mqtt_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Set_Publish_Callback(a,b)     ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SET_PUBLISH_CB     , (ULONG) a, (ULONG) b, (ULONG) qapi_mqtt_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_Net_MQTT_Allow_Unsub_Publish(a,b)      ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_SET_ALLOW_UNSUB_PUB     , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/*
 * Macro that Passes Byte Pool Pointer for MQTT Application 
 * Parameter 'a' : Handle
 * Parameter 'b' : Pointer to Byte Pool 
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 * (This Macro is only used in DAM Space)
 */

#define qapi_Net_MQTT_Pass_Pool_Ptr(a,b)             mqtt_set_byte_pool(a,b)

/** @} */

static __inline int mqtt_set_byte_pool(qapi_Net_MQTT_Hndl_t   hndl, void *mqtt_byte_pool_ptr)
{
  qapi_cb_params_uspace_ptr_t *uspace_memory_mqtt = NULL;
  int ret = QAPI_ERROR;

  tx_byte_allocate(mqtt_byte_pool_ptr, (VOID **) &uspace_memory_mqtt,(sizeof(qapi_cb_params_uspace_ptr_t)), TX_NO_WAIT);
  if (NULL == uspace_memory_mqtt)
  {
    return QAPI_ERROR;
  }

  memset (uspace_memory_mqtt,0, sizeof(qapi_cb_params_uspace_ptr_t));

  tx_byte_allocate(mqtt_byte_pool_ptr, (VOID **) &(uspace_memory_mqtt->cb_param[0]),256, TX_NO_WAIT);
  if (NULL == uspace_memory_mqtt->cb_param[0])
  {
    return QAPI_ERROR;
  }
  memset ((uspace_memory_mqtt->cb_param[0]), 0, 256);
  uspace_memory_mqtt->cb_count++;
  
  tx_byte_allocate(mqtt_byte_pool_ptr, (VOID **) &(uspace_memory_mqtt->cb_param[1]),1024, TX_NO_WAIT);
  if (NULL == uspace_memory_mqtt->cb_param[1])
  {
    return QAPI_ERROR;
  }
  memset ((uspace_memory_mqtt->cb_param[1]), 0, 1024);
  uspace_memory_mqtt->cb_count++;



  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_MQTT ,uspace_memory_mqtt,1024);
  return ret;
}

static __inline int mqtt_destroy_indirection(qapi_Net_MQTT_Hndl_t hndl)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t* uspace_memory_mqtt = NULL;
  int i=0;

  if (qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_MQTT, (void**)&uspace_memory_mqtt) == QAPI_OK)
  {
    if (uspace_memory_mqtt != NULL)
    {
      for (i=0;i<uspace_memory_mqtt->cb_count; i++)
      {
        if (uspace_memory_mqtt->cb_param[i] != NULL)
        {
          tx_byte_release(uspace_memory_mqtt->cb_param[i]);
          uspace_memory_mqtt->cb_param[i]=NULL;
          uspace_memory_mqtt->cb_count--;		
        }
      }
    }
  }

    if (uspace_memory_mqtt != NULL)
    {
      tx_byte_release(uspace_memory_mqtt);
      uspace_memory_mqtt=NULL;
    }

  ret = ((qapi_Status_t)  (_txm_module_system_call12)(TXM_QAPI_MQTT_DESTROY , (ULONG) hndl, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0));
  return ret;
}

#else

UINT qapi_Net_MQTT_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/** @addtogroup qapi_net_mqtt
@{ */

/**
 * @brief Creates a new MQTT context.
 *
 * @param[out] hndl  Newly created MQTT context.
 *
 * @return QAPI_OK on success, QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Net_MQTT_New(qapi_Net_MQTT_Hndl_t *hndl);

/**
 * @brief Destroys an MQTT context.
 *
 * @param[in] hndl  Handle for the MQTT context to be destroyed.
 *
 * @return QAPI_OK on success or QAPI_ERROR on failure.
 */
qapi_Status_t qapi_Net_MQTT_Destroy(qapi_Net_MQTT_Hndl_t hndl);

/**
 * @brief Connects to an MQTT broker.
 *
 * @param[in] hndl    MQTT handle.
 * @param[in] config  MQTT client configuration.
 *
 * @return QAPI_OK on success or < 0 on failure. @newpage
 */
qapi_Status_t qapi_Net_MQTT_Connect(qapi_Net_MQTT_Hndl_t hndl, const qapi_Net_MQTT_Config_t* config);

/**
 * @brief Disconnects from an MQTT broker.
 *
 * @param[in] hndl  MQTT handle.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Disconnect(qapi_Net_MQTT_Hndl_t hndl);

/**
 * @brief Publishes a message to a particular topic.
 *
 * @param[in] hndl     MQTT handle.
 * @param[in] topic    MQTT topic.
 * @param[in] msg      MQTT payload.
 * @param[in] msg_len  MQTT payload length.
 * @param[in] qos      QOS to be used for the message.
 * @param[in] retain   Retain flag.
 *
 * @return QAPI_OK on success or <0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Publish(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic, const uint8_t* msg, int32_t msg_len, int32_t qos, bool retain);

/**
 * @brief Publishes a message to a particular topic.
 *
 * @param[in] hndl MQTT handle.
 * @param[in] topic MQTT topic.
 * @param[in] msg MQTT payload.
 * @param[in] msg_len MQTT payload length.
 * @param[in] qos QOS to be used for the message.
 * @param[in] retain Retain flag.
 * @param[out] msg_id Message ID of the MQTT publish message.
 *
 * @return QAPI_OK on success or <0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Publish_Get_Msg_Id(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic, const uint8_t* msg, int32_t msg_len, int32_t qos, bool retain, uint16_t *msg_id);


/**
 * @brief Subscribes to a topic.
 *
 * @param[in] hndl MQTT handle.
 * @param[in] topic Subscription topic.
 * @param[in] qos QOS to be used.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Subscribe(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic, int32_t qos);

/**
 * @brief Unsubscribes from a topic.
 *
 * @param[in] hndl   MQTT handle
 * @param[in] topic  Topic from which to unsubscribe.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Unsubscribe(qapi_Net_MQTT_Hndl_t hndl, const uint8_t* topic);

/**
 * @brief Sets a connect callback, which is invoked when the connect is successful.
 *
 * @param[in] hndl      MQTT handle.
 * @param[in] callback  Callback to be invoked.
 *
 * @return Success or failure.
 */

qapi_Status_t qapi_Net_MQTT_Set_Connect_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Connect_CB_t callback);

/**
 * @brief Sets a subscribe callback, which is invoked when a subscription is granted or denied.
 *
 * @param[in] hndl      MQTT handle.
 * @param[in] callback  Callback to be invoked.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Set_Subscribe_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Subscribe_CB_t callback);

/**
 * @brief Sets a message callback, which is invoked when a message is received for a subscribed topic.
 *
 * @param[in] hndl      MQTT handle.
 * @param[in] callback  Callback to be invoked.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Set_Message_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Message_CB_t callback);


/**
 * @brief Sets a publish callback, which is invoked when PUBACK(QOS1)/PUBREC,PUBCOMP(QOS2).
 *
 * @param[in] hndl MQTT handle.
 * @param[in] callback Callback to be invoked.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */
qapi_Status_t qapi_Net_MQTT_Set_Publish_Callback(qapi_Net_MQTT_Hndl_t hndl, qapi_Net_MQTT_Publish_CB_t callback);

/**
 * @brief Sets an unsubscribe callback, which will allow  messages to be received for an unsubscribed topic.
 *
 * @param[in] hndl MQTT handle.
 * @param[in] allow_unsub_pub condition that allows this behaviour.
 *
 * @return QAPI_OK on success or < 0 on failure.
 */

qapi_Status_t qapi_Net_MQTT_Allow_Unsub_Publish(qapi_Net_MQTT_Hndl_t hndl, bool  allow_unsub_pub);

/** @} */ /* end_addtogroup qapi_net_mqtt */
#endif /*!TXM_MODULE*/

#endif /*_QAPI_NET_MQTT_H*/
