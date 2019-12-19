/*===========================================================================
                         TEST_APP_MODULE.H

DESCRIPTION
   This header file contains definitions used internally by the sensor object test
   Module. 
  
   Copyright (c) 2017 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "txm_module.h"
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qapi_lwm2m.h"
#include "qapi_uart.h"
#include "qapi_diag.h"
#include "qapi_timer.h"
#include "time.h"
#include "qapi_cli.h"


#define PRESENCE_SENSOR_OBJECT_ID              3302

#define RES_M_DIG_INPUT_STATE                  5500
#define RES_O_DIG_INPUT_COUNTER                5501
#define RES_O_RESET_DIG_INPUT_COUNTER          5505
#define RES_O_SENSOR_TYPE                      5751
#define RES_O_BUSY_TO_CLEAR_DELAY              5903
#define RES_O_CLEAR_TO_BUSY_DELAY              5904


#define PRV_MAX_STRSIZE                255
#define BIT64_BUFFER_SIZE              8

#define URL_INFO_BUFFER_SIZE  255


/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef TX_QUEUE* sen_obj_task_cmd_q;

typedef enum
{
  SEN_OBJ_MIN_CMD_EVT,
  SEN_OBJ_APP_ORIGINATED_EVT,
  SEN_OBJ_CALL_BACK_EVT,
  SEN_OBJ_CMD_FUNC_EVT,
  SEN_OBJ_MAX_CMD_EVT
}sen_obj_cmd_id_e;

/*------------------------------------------------------------------------------
  Structure Declaration
------------------------------------------------------------------------------*/

typedef struct
{
  TX_EVENT_FLAGS_GROUP*          sen_obj_signal;
  TX_MUTEX*                      sen_obj_mutex;
  TX_QUEUE*                      cmd_q_ptr;
}sen_obj_info_t;


typedef struct{
  sen_obj_cmd_id_e                cmd_id;
}sen_obj_cmd_hdr_t;


typedef struct{
  void *data;
}sen_obj_cmd_data_t;


typedef struct{
  sen_obj_cmd_hdr_t               cmd_hdr;
  sen_obj_cmd_data_t              cmd_data;
} sen_obj_cmd_t;

/*------------------------------------------------------------------------------
  Signal Related Macros
------------------------------------------------------------------------------*/
#define SEN_OBJ_SIGNAL_MASK       0x00000001

#define SEN_OBJ_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

#define SEN_OBJ_TASK_CLEAR_SIG(signal, signal_mask) \
                                    tx_event_flags_set(signal,signal_mask, TX_AND)

#define SEN_OBJ_TASK_SET_SIG(signal, signal_mask) \
                                    tx_event_flags_set(signal, signal_mask, TX_OR)


/*------------------------------------------------------------------------------
  Command Queue Related Macros
------------------------------------------------------------------------------*/

#define SEN_OBJ_CMD_ID_RANGE_CHECK(cmd_id) \
                                  ( (cmd_id > SEN_OBJ_MIN_CMD_EVT) && \
                                    (cmd_id < SEN_OBJ_MAX_CMD_EVT) )

#define ERANGE  34 /* Math result not representable */
#define LWM2M_ATTR_FLAG_NUMERIC (QAPI_NET_LWM2M_LESS_THAN_E| QAPI_NET_LWM2M_GREATER_THAN_E| QAPI_NET_LWM2M_STEP_E)

#define LWM2M_ATTR_SERVER_ID_STR       "ep="
#define LWM2M_ATTR_SERVER_ID_LEN       3
#define LWM2M_ATTR_MIN_PERIOD_STR      "pmin="
#define LWM2M_ATTR_MIN_PERIOD_LEN      5
#define LWM2M_ATTR_MAX_PERIOD_STR      "pmax="
#define LWM2M_ATTR_MAX_PERIOD_LEN      5
#define LWM2M_ATTR_GREATER_THAN_STR    "gt="
#define LWM2M_ATTR_GREATER_THAN_LEN    3
#define LWM2M_ATTR_LESS_THAN_STR       "lt="
#define LWM2M_ATTR_LESS_THAN_LEN       3
#define LWM2M_ATTR_STEP_STR            "st="
#define LWM2M_ATTR_STEP_LEN            3
#define LWM2M_ATTR_DIMENSION_STR       "dim="
#define LWM2M_ATTR_DIMENSION_LEN       4

#define LWM2M_URI_MAX_STRING_LEN       18 
#define _PRV_64BIT_BUFFER_SIZE         8

#define LWM2M_LINK_ITEM_START             "<"
#define LWM2M_LINK_ITEM_START_SIZE        1
#define LWM2M_LINK_ITEM_END               ">,"
#define LWM2M_LINK_ITEM_END_SIZE          2
#define LWM2M_LINK_ITEM_DIM_START         ">;dim="
#define LWM2M_LINK_ITEM_DIM_START_SIZE    6
#define LWM2M_LINK_ITEM_ATTR_END          ","
#define LWM2M_LINK_ITEM_ATTR_END_SIZE     1
#define LWM2M_LINK_URI_SEPARATOR          "/"
#define LWM2M_LINK_URI_SEPARATOR_SIZE     1
#define LWM2M_LINK_ATTR_SEPARATOR         ";"
#define LWM2M_LINK_ATTR_SEPARATOR_SIZE    1

typedef enum
{
  URI_DEPTH_OBJECT,
  URI_DEPTH_OBJECT_INSTANCE,
  URI_DEPTH_RESOURCE,
  URI_DEPTH_RESOURCE_INSTANCE
}qapi_Net_LWM2M_Uri_Depth_t;

typedef struct qapi_Net_LWM2M_Uri_s
{
  uint32_t    flag;           // indicates which segments are set
  uint16_t    objectId;
  uint16_t    instanceId;
  uint16_t    resourceId;
  uint16_t    resourceInstId;
}qapi_Net_LWM2M_Uri_t;

typedef struct qapi_Net_LWM2M_Observe_info_s
{
  struct qapi_Net_LWM2M_Observe_info_s *next;
  bool active;
  bool update;
  qapi_Net_LWM2M_Attributes_t *attributes;
  time_t last_time;
  uint32_t seq_num;
  union
  {
     int64_t asInteger;
     double  asFloat;
  }lastvalue;
  uint8_t msg_id_len;
  uint8_t msg_id[QAPI_MAX_LWM2M_MSG_ID_LENGTH];
  uint16_t not_id;
}qapi_Net_LWM2M_Observe_info_t;

typedef struct qapi_Net_LWM2M_Observed_s
{
  struct qapi_Net_LWM2M_Observed_s *next;
  qapi_Net_LWM2M_Uri_t uri;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
}qapi_Net_LWM2M_Observed_t;

typedef struct qapi_Net_LWM2M_Pending_Observed_s
{
  struct qapi_Net_LWM2M_Pending_Observed_s *next;
  qapi_Net_LWM2M_Observe_info_t *observe_info;
  qapi_Net_LWM2M_Uri_t uri;
  void *message;
}qapi_Net_LWM2M_Pending_Observed_t;

typedef enum qapi_Net_LWM2M_Sen_App_Msg_e
{
  QAPI_NET_LWM2M_SEN_AUTO_E,
  QAPI_NET_LWM2M_SEN_SET_COUNT_E,
  QAPI_NET_LWM2M_SEN_NOTIFY_E,
  QAPI_NET_LWM2M_SEN_BUSY_E,
  QAPI_NET_LWM2M_SEN_CLEAR_E
}qapi_Net_LWM2M_Sen_App_Msg_t;

typedef struct qapi_Net_LWM2M_Sen_Obj_s
{
  qapi_Net_LWM2M_App_Handler_t handler;
  qapi_Net_LWM2M_Server_Data_t *lwm2m_srv_data;
  void                         *data;
}qapi_Net_LWM2M_Sen_Obj_t;

typedef struct qapi_Net_LWM2M_Sen_App_s
{
  uint16_t id;
  qapi_Net_LWM2M_Sen_App_Msg_t msg_type;
  qapi_Net_LWM2M_Obj_Info_t obj_info;
}qapi_Net_LWM2M_Sen_App_t;

typedef struct qapi_Net_LWM2M_Sen_Cmd_s
{
  qapi_CLI_App_Handler_t handle; 
  qapi_CLI_Parameter_Data_t *param_data;
  void *user_data;
}qapi_Net_LWM2M_Sen_Cmd_t;

typedef struct qapi_Net_LWM2M_Sen_s
{
  union
  {
    qapi_Net_LWM2M_Sen_Obj_t dl_op;
    qapi_Net_LWM2M_Sen_App_t ul_op;
    qapi_Net_LWM2M_Sen_Cmd_t cmd_op;
  }app_data;
}qapi_Net_LWM2M_Sen_t;

/* Maximum length of log message */
#define MAX_DIAG_LOG_MSG_SIZE       512

/* Log message to Diag */
#define SENSOR_LOG_MSG_DIAG( lvl, ... )  \
  { \
    char buf[ MAX_DIAG_LOG_MSG_SIZE ]; \
     \
    /* Format message for logging */  \
    sen_format_log_msg( buf, MAX_DIAG_LOG_MSG_SIZE, __VA_ARGS__ );\
    \
    /* Log message to Diag */  \
    QAPI_MSG_SPRINTF( MSG_SSID_LINUX_DATA, lvl, "%s", buf );  \
  }

#define LOG_INFO(...) SENSOR_LOG_MSG_DIAG(MSG_LEGACY_MED, __VA_ARGS__)

UINT memscpy(
          void   *dst,
          UINT   dst_size,
          const  void  *src,
          UINT   src_size
          );

void Initialize_Sensor_App(void);

qapi_Status_t auto_create(uint32_t Parameter_Count,qapi_CLI_Parameter_t *Parameter_List);
qapi_Status_t set_input_cnt(uint32_t Parameter_Count,qapi_CLI_Parameter_t *Parameter_List);

void sen_obj_set_signal(void);

sen_obj_cmd_t* sen_obj_get_cmd_buffer(void);

void sen_obj_enqueue_cmd(sen_obj_cmd_t* cmd_ptr);

void sen_obj_process_commands(void);

void sen_obj_cmdq_init (sen_obj_info_t * sen_obj_state);

qapi_Status_t create_object(int id);

qapi_Status_t ext_object_register(int id);

qapi_Status_t ext_object_deregister(int id);

qapi_Net_LWM2M_Instance_Info_t *find_object_instance(uint16_t object_id, uint8_t instance_id);

qapi_Net_LWM2M_Resource_Info_t *find_resource_info(uint16_t object_id, uint8_t instance_id,
                                                   uint8_t resource_id);

qapi_Status_t create_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t delete_object_instance(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t read_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Net_LWM2M_Data_t* lwm2m_object_add(qapi_Net_LWM2M_Data_t* head,
                                            qapi_Net_LWM2M_Data_t* node);


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_add(qapi_Net_LWM2M_Instance_Info_t* head,
                                                   qapi_Net_LWM2M_Instance_Info_t* node);


qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_add(qapi_Net_LWM2M_Resource_Info_t* head,
                                                   qapi_Net_LWM2M_Resource_Info_t* node);

qapi_Net_LWM2M_Data_t* lwm2m_object_remove(qapi_Net_LWM2M_Data_t *head,
                                           qapi_Net_LWM2M_Data_t ** nodeP,
                                           uint8_t id);


qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_remove(qapi_Net_LWM2M_Instance_Info_t *head,
                                                      qapi_Net_LWM2M_Instance_Info_t ** nodeP,
                                                      uint8_t id);

qapi_Net_LWM2M_Data_t* lwm2m_object_find(qapi_Net_LWM2M_Data_t* head,
                                         uint16_t id);

qapi_Net_LWM2M_Instance_Info_t* lwm2m_instance_find(qapi_Net_LWM2M_Instance_Info_t* head,
                                                    uint16_t id);

qapi_Net_LWM2M_Resource_Info_t* lwm2m_resource_find(qapi_Net_LWM2M_Resource_Info_t* head,
                                                    uint16_t id);

qapi_Status_t write_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                            qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t exec_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t disc_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                           qapi_Net_LWM2M_Server_Data_t op_data);

void send_response_message(qapi_Net_LWM2M_App_Handler_t handler,
                           qapi_Net_LWM2M_Server_Data_t op_data,
                           qapi_Net_LWM2M_Response_Code_t status);

qapi_Status_t sen_obj_handle_event(qapi_Net_LWM2M_App_Handler_t handler, 
                                   qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t sen_obj_delete_all(qapi_Net_LWM2M_App_Handler_t handler, 
                                 qapi_Net_LWM2M_Server_Data_t op_data);

void test_obj_task_entry(void *thrd);

qapi_Status_t read_data_sen(uint16_t instanceid, int *size, qapi_Net_LWM2M_Flat_Data_t **data,
                            qapi_Net_LWM2M_Data_t *object);

qapi_Status_t observe_cancel_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                                     qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t sen_object_write_attr(qapi_Net_LWM2M_App_Handler_t handler, 
                                    qapi_Net_LWM2M_Server_Data_t op_data);

qapi_Status_t observe_sen_obj(qapi_Net_LWM2M_App_Handler_t handler, 
                              qapi_Net_LWM2M_Server_Data_t op_data);

uint8_t sen_object_set_observe_param(qapi_Net_LWM2M_App_Handler_t handler,
                                     qapi_Net_LWM2M_Server_Data_t op_data);

void sen_object_notify(int id,
                       time_t current_time,
                       time_t *timeout);

qapi_Net_LWM2M_Instance_Info_t* obj_create_presence_sensor(qapi_Net_LWM2M_Data_t *lwm2m_data);

void stop_sen_obs_notify_timer(qapi_Net_LWM2M_App_Handler_t handler);

qapi_Status_t sensor_auto_create(void);

qapi_Status_t set_dig_cnt(uint16_t count);

qapi_Status_t update_state_busy(void);

qapi_Status_t update_state_clear(void);

int sen_obj_discover_serialize(qapi_Net_LWM2M_App_Handler_t handler,
                               qapi_Net_LWM2M_Server_Data_t op_data,
                               qapi_Net_LWM2M_Uri_t *uri,
                               int size,
                               qapi_Net_LWM2M_Flat_Data_t *data,
                               uint8_t **buffer);

qapi_Net_LWM2M_Flat_Data_t *data_malloc(int size);

void data_free(int size,
                      qapi_Net_LWM2M_Flat_Data_t *data);

void copy_value(void * dst,
                const void *src,
                size_t len);

size_t int_to_text(int64_t data,
                   uint8_t *string,
                   size_t length);

size_t float_to_text(double data,
                     uint8_t *string,
                     size_t length);

size_t int64_to_text(int64_t data,
                     uint8_t **buffer);

size_t float64_to_text(double data,
                       uint8_t **buffer);

size_t bool_to_text(bool data,
                    uint8_t **buffer);

size_t serialize_text(qapi_Net_LWM2M_Flat_Data_t *data,
                      uint8_t **buffer);

int uri_to_string(qapi_Net_LWM2M_Uri_t *uri,
                  uint8_t * buffer,
                  size_t buffer_len,
                  qapi_Net_LWM2M_Uri_Depth_t *depth);

int set_buffer(qapi_Net_LWM2M_Flat_Data_t *data,
               uint8_t *buffer,
               size_t buffer_len);

size_t encode_int(int64_t data,
                  uint8_t data_buffer[BIT64_BUFFER_SIZE]);

size_t encode_float(double data,
                    uint8_t data_buffer[BIT64_BUFFER_SIZE]);

void encode_opaque_data(uint8_t * buffer,
                        size_t length,
                        qapi_Net_LWM2M_Flat_Data_t *data);

uint8_t get_header_type(qapi_Net_LWM2M_Value_Type_t type);

qapi_Net_LWM2M_Value_Type_t get_data_type(uint8_t type);

int get_header_length(uint16_t id,
                      size_t data_len);

int create_header(uint8_t *header,
                  bool is_instance,
                  qapi_Net_LWM2M_Value_Type_t type,
                  uint16_t id,
                  size_t data_len);

int tlv_decode(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Value_Type_t *type,
               uint16_t *id,
               size_t *data_index,
               size_t *data_len);

int tlv_parser(uint8_t *buffer,
               size_t buffer_len,
               qapi_Net_LWM2M_Flat_Data_t **data);

size_t get_item_length(int size,
                       qapi_Net_LWM2M_Flat_Data_t *data);

size_t tlv_encode(bool is_resource_instance, 
                  int size,
                  qapi_Net_LWM2M_Flat_Data_t *data,
                  uint8_t ** buffer);

int lwm2m_data_decode_payload(qapi_Net_LWM2M_Uri_t * uri,
                              uint8_t *buffer,
                              size_t buffer_len,
                              qapi_Net_LWM2M_Content_Type_t format,
                              qapi_Net_LWM2M_Flat_Data_t **data);

size_t lwm2m_data_encode_payload(qapi_Net_LWM2M_Uri_t *uri,
                                 int size,
                                 qapi_Net_LWM2M_Flat_Data_t *data,
                                 qapi_Net_LWM2M_Content_Type_t *format,
                                 uint8_t **buffer);

void observe_delete_sen_handle_req(qapi_Net_LWM2M_App_Handler_t handler,
                                   qapi_Net_LWM2M_Uri_t *uri);

void sen_format_log_msg
(
 char *buf_ptr,
 int buf_size,
 char *fmt,
 ...
);

void sen_obj_resource_value_changed(qapi_Net_LWM2M_App_Handler_t handler,
                                    qapi_Net_LWM2M_Uri_t *uri);



