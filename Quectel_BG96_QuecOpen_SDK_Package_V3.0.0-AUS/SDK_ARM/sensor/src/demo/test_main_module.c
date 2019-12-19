/*==============================================================================

                             TEST_MAIN_MODULE.C

GENERAL DESCRIPTION
  This file deals with the task Initialization to support various test sensor  
  object mechanisms
 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The task init and task start routines do the work necessary to ensure that
  all data structures are in the desired state to continue with normal
  operation.

Copyright (c) 2017 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------

==============================================================================*/

#include "test_app_module.h"


#define     Q_NO_OF_MSGS         100
#define     Q_MESSAGE_SIZE       TX_1_ULONG
#define     QUEUE_SIZE           Q_NO_OF_MSGS*Q_MESSAGE_SIZE*sizeof(sen_obj_cmd_t)

extern TX_BYTE_POOL *byte_pool_sensor;

sen_obj_info_t          sen_obj_param;
sen_obj_task_cmd_q      sen_obj_cmdq;
extern int ob_count;
extern bool app_registerd;

/*=============================================================================

FUNCTION SEN_OBJ_SET_SIGNAL

DESCRIPTION
  Set the sensor object signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void sen_obj_set_signal(void)
{
  LOG_INFO("sen_obj_set_signal \n");

  tx_mutex_get(sen_obj_param.sen_obj_mutex, TX_WAIT_FOREVER);

  SEN_OBJ_TASK_SET_SIG(sen_obj_param.sen_obj_signal,SEN_OBJ_SIGNAL_MASK);

  tx_mutex_put(sen_obj_param.sen_obj_mutex);

  return;
}

/*=============================================================================

FUNCTION SEN_OBJ_MUTEX_INIT

DESCRIPTION
  sensor object mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void sen_obj_mutex_init(void)
{
  LOG_INFO("sen_obj_mutex_init \n");

  /* allocate mutex required for thread*/ 
  txm_module_object_allocate(&sen_obj_param.sen_obj_mutex, sizeof(TX_MUTEX));
  tx_mutex_create(sen_obj_param.sen_obj_mutex, "sen_obj_mutex", TX_NO_INHERIT);
  return;
}


/*=============================================================================

FUNCTION SEN_OBJ_SIGNAL_INIT

DESCRIPTION
  Sensor Object Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void sen_obj_signal_init(void)
{
  LOG_INFO("sen_obj_signal_init \n");
  /* allocate signal required for thread*/ 
  txm_module_object_allocate(&sen_obj_param.sen_obj_signal, sizeof(TX_EVENT_FLAGS_GROUP));
  tx_event_flags_create(sen_obj_param.sen_obj_signal, "sen_obj_sig");
  return;
}


/*=============================================================================

FUNCTION SEN_OBJ_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the SEN_OBJ signal.
  The SEN_OBJ signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void sen_obj_signal_wait(void)
{
  uint32_t received_sigs = 0;

  LOG_INFO("sen_obj_signal_wait \n");

  while (1)
  {
/*============================================================================
  Wait for the SEN_OBJ Signal to be set
=============================================================================*/
    tx_event_flags_get(sen_obj_param.sen_obj_signal, SEN_OBJ_SIGNAL_MASK,
                       TX_OR_CLEAR, &received_sigs, TX_WAIT_FOREVER);

    if (SEN_OBJ_TASK_IS_SIG_SET(received_sigs, SEN_OBJ_SIGNAL_MASK))
    {
      // SEN_OBJ Signal is set Dequeue the SEN_OBJ Command queue
      sen_obj_process_commands();
    }
  }
}


/*================================================================

FUNCTION SEN_OBJ_TASK_INIT

DESCRIPTION
  Sensor object Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void sen_obj_task_init(void)
{

  LOG_INFO("sen_obj_task_init \n");
/*============================================================================
  Reseting the Global SEN_OBJ Structure
=============================================================================*/

  memset(&sen_obj_param, 0x00, sizeof (sen_obj_info_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   sen_obj_info struct to the appropriate values.
----------------------------------------------------------------------------*/
  
  sen_obj_cmdq_init(&sen_obj_param);

}





/*=============================================================================

FUNCTION TEST_OBJ_TASK_ENTRY

DESCRIPTION
  This is the entry point for Sensor object task
  Reset the Global SEN_OBJ structure
  Initalize the SEN_OBJ Qurt signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void test_obj_task_entry(void *thread_ctxt)
{

  LOG_INFO("test_obj_task_entry \n");

  sen_obj_task_init();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  sen_obj_mutex_init();

/*=============================================================================
Initialize Signal for SEN_OBJ
==============================================================================*/

  sen_obj_signal_init();

/*=============================================================================
  Register the command group with QCLI
  ==============================================================================*/

  Initialize_Sensor_App();

/*=============================================================================
Wait on the SEN_OBJ Signal
==============================================================================*/
  sen_obj_signal_wait();

}

/*===========================================================================

FUNCTION SEN_OBJ_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  sen_obj_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void sen_obj_cmdq_init (sen_obj_info_t * sen_obj_state)
{
  void *cmdq_ptr = NULL;

  if (sen_obj_state == NULL)
  {
    LOG_INFO("SEN_OBJ State is NULL ");
    return;
  }

  tx_byte_allocate(byte_pool_sensor, (VOID **)&cmdq_ptr, QUEUE_SIZE, TX_NO_WAIT);

  LOG_INFO("sen_obj_cmdq_init");

  sen_obj_param = *sen_obj_state;

  /* allocate queue required for thread*/ 
  txm_module_object_allocate(&sen_obj_cmdq, sizeof(TX_QUEUE));
  tx_queue_create(sen_obj_cmdq, "sen_obj_cmdq", Q_MESSAGE_SIZE,cmdq_ptr, QUEUE_SIZE);

  sen_obj_param.cmd_q_ptr = sen_obj_cmdq;
}


/*===========================================================================

FUNCTION SEN_OBJ_GET_CMD_BUFFER

DESCRIPTION
  This function is used to allocate a command buffer that the client can
  then enqueue into the command queue.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the command

SIDE EFFECTS
  None

=========================================================================*/
sen_obj_cmd_t* sen_obj_get_cmd_buffer()
{
  sen_obj_cmd_t*                 cmd_ptr = NULL;

  LOG_INFO("sen_obj_get_cmd_buffer \n ");

  tx_byte_allocate(byte_pool_sensor, (VOID **)&cmd_ptr, sizeof(sen_obj_cmd_t), TX_NO_WAIT);

  if (cmd_ptr == NULL) 
  {
    LOG_INFO("Command buffer allocation failed");
  }

  return cmd_ptr;
}


/*===========================================================================

FUNCTION  SEN_OBJ_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the SEN_OBJ queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void sen_obj_enqueue_cmd(sen_obj_cmd_t* cmd_ptr)
{
  //LOG_INFO("Cmd_ptr: 0x%x with cmd_id: %d, posted to int Q,",
  //                           cmd_ptr, cmd_ptr->cmd_hdr.cmd_id);

  tx_queue_send(sen_obj_cmdq, &cmd_ptr, TX_WAIT_FOREVER);

  return;
}

/*===========================================================================

FUNCTION SEN_OBJ_DEQUEUE_CMD

DESCRIPTION
  This function de-queues and returns a command from the Sensor object task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static sen_obj_cmd_t* sen_obj_dequeue_cmd( void )
{
  sen_obj_cmd_t *cmd_ptr = NULL;

  LOG_INFO("sen_obj_dequeue_cmd called");

  tx_queue_receive(sen_obj_cmdq, &cmd_ptr, TX_WAIT_FOREVER);

  return cmd_ptr;
}

/*==============================================================================
FUNCTION SEN_OBJ_PROCESS_COMMANDS()

DESCRIPTION
  This function dequeues all outstanding commands from and dispatches the
  processor functions.

PARAMETERS
  NONE

RETURN VALUE
  None

DEPENDENCIES 
  None 

SIDE EFFECTS
  None
==============================================================================*/
void sen_obj_process_commands(void)
{
  sen_obj_cmd_t                *sen_obj_cmd_ptr = NULL;
  qapi_Net_LWM2M_Sen_t         *obj_buffer      = NULL;
  qapi_Status_t                 result          = QAPI_ERROR;
  bool flag = false;

  while ((sen_obj_cmd_ptr = sen_obj_dequeue_cmd()) != NULL ) 
  {
    LOG_INFO("Command ptr: 0x%x, cmd_id: %d", 
                             sen_obj_cmd_ptr, 
                             sen_obj_cmd_ptr->cmd_hdr.cmd_id);
/*--------------------------------------------------------------------------
      Bound check to ensure command id is bounded
--------------------------------------------------------------------------*/
    if (!SEN_OBJ_CMD_ID_RANGE_CHECK(sen_obj_cmd_ptr->cmd_hdr.cmd_id))
    {
      LOG_INFO("Command ID Range check failed"); 
      continue;
    }

    switch(sen_obj_cmd_ptr->cmd_hdr.cmd_id)
    {
      case SEN_OBJ_CALL_BACK_EVT:
      {
        obj_buffer = (qapi_Net_LWM2M_Sen_t *)sen_obj_cmd_ptr->cmd_data.data;
        if(obj_buffer != NULL && obj_buffer->app_data.dl_op.lwm2m_srv_data != NULL)
        {
          switch(obj_buffer->app_data.dl_op.lwm2m_srv_data->msg_type)
          {
            case QAPI_NET_LWM2M_READ_REQ_E:
            {
              result = read_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_REPLACE_REQ_E:
            {
              result = write_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_PARTIAL_UPDATE_REQ_E:
            {
              result = write_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_WRITE_ATTR_REQ_E:
            {
              result = sen_object_write_attr(obj_buffer->app_data.dl_op.handler, 
                                            *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DISCOVER_REQ_E:
            {
              result = disc_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_EXECUTE_REQ_E:
            {
              result = exec_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                    *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CREATE_REQ_E:
            {
              result = create_object_instance(obj_buffer->app_data.dl_op.handler, 
                                                  *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_REQ_E:
            {
              result = delete_object_instance(obj_buffer->app_data.dl_op.handler, 
                                              *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_OBSERVE_REQ_E:
            {
              result = observe_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                      *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_CANCEL_OBSERVE_REQ_E:
            {
              result = observe_cancel_sen_obj(obj_buffer->app_data.dl_op.handler, 
                                             *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_ACK_MSG_E:
            break;
            case QAPI_NET_LWM2M_INTERNAL_CLIENT_IND_E:
            {
              if(obj_buffer->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_REBOOT_E ||
                 obj_buffer->app_data.dl_op.lwm2m_srv_data->event == QAPI_NET_LWM2M_DEVICE_FACTORY_RESET_E)
              {
                flag = true;
              }
              result = sen_obj_handle_event(obj_buffer->app_data.dl_op.handler, 
                                            *obj_buffer->app_data.dl_op.lwm2m_srv_data);
            }
            break;
            case QAPI_NET_LWM2M_DELETE_ALL_REQ_E:
            {
              //result = sen_obj_delete_all(obj_buffer->app_data.dl_op.handler, 
              //                             *obj_buffer->app_data.dl_op.lwm2m_srv_data);
			   result = QAPI_OK;
            }
            break;
          }
          if(obj_buffer)
          {
            if(obj_buffer->app_data.dl_op.lwm2m_srv_data)
            {
              if(obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr)
              {
                tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr);
                obj_buffer->app_data.dl_op.lwm2m_srv_data->lwm2m_attr = NULL;
              }
              if(obj_buffer->app_data.dl_op.lwm2m_srv_data->payload)
              {
                tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data->payload);
                obj_buffer->app_data.dl_op.lwm2m_srv_data->payload = NULL;
              }
              tx_byte_release(obj_buffer->app_data.dl_op.lwm2m_srv_data);
              obj_buffer->app_data.dl_op.lwm2m_srv_data = NULL;
            }
            tx_byte_release(obj_buffer);
            obj_buffer = NULL;
          }
         LOG_INFO("Operation result is %d ", result);
        }
      }

      break;

      case SEN_OBJ_APP_ORIGINATED_EVT:
      {
        obj_buffer = (qapi_Net_LWM2M_Sen_t *)sen_obj_cmd_ptr->cmd_data.data;
        if(obj_buffer != NULL)
        {
          switch(obj_buffer->app_data.ul_op.msg_type)
          {
            case QAPI_NET_LWM2M_SEN_AUTO_E:
            {
              result = sensor_auto_create();
            }
            break;
 
            case QAPI_NET_LWM2M_SEN_SET_COUNT_E:
            {
              result = set_dig_cnt(obj_buffer->app_data.ul_op.id);
            }
            break;

            case QAPI_NET_LWM2M_SEN_BUSY_E:
            {
              result = update_state_busy();
            }
            break;

            case QAPI_NET_LWM2M_SEN_CLEAR_E:
            {
              result = update_state_clear();
            }
            break;

            case QAPI_NET_LWM2M_SEN_NOTIFY_E:
            {
              time_t timeout = 5;
              qapi_time_get_t current_time;
              qapi_time_get(QAPI_TIME_SECS, &current_time);

              sen_object_notify(0,current_time.time_secs, &timeout);
            }
            break;

            default:
            LOG_INFO("Invalid request from application.");
            break;
          }
          if(obj_buffer)
          {
            tx_byte_release(obj_buffer);
            obj_buffer = NULL;
          }
        }
      }
      
      break;

      case SEN_OBJ_CMD_FUNC_EVT:
      {
        int i = 0;
        obj_buffer = (qapi_Net_LWM2M_Sen_t *)sen_obj_cmd_ptr->cmd_data.data;
        if(obj_buffer != NULL && obj_buffer->app_data.cmd_op.param_data != NULL)
        {
          switch(obj_buffer->app_data.cmd_op.param_data->index)
          {
            case 1:
            {
              result = auto_create(obj_buffer->app_data.cmd_op.param_data->parameter_count,
                                   obj_buffer->app_data.cmd_op.param_data->parameter_list);
            }
            break;
 
            case 2:
            {
              result = set_input_cnt(obj_buffer->app_data.cmd_op.param_data->parameter_count,
                                     obj_buffer->app_data.cmd_op.param_data->parameter_list);
            }
            break;

            default:
            LOG_INFO("Invalid command request.");
            break;
          }
          if(obj_buffer)
          {
            if(obj_buffer->app_data.cmd_op.param_data)
            {
              for(i = 0; i< obj_buffer->app_data.cmd_op.param_data->parameter_count ;i++)
              {
                if(obj_buffer->app_data.cmd_op.param_data->parameter_list)
                {
                  if(obj_buffer->app_data.cmd_op.param_data->parameter_list->String_Value)
                  {
                    tx_byte_release(obj_buffer->app_data.cmd_op.param_data->parameter_list->String_Value);
                    obj_buffer->app_data.cmd_op.param_data->parameter_list->String_Value = NULL;
                  }
                  tx_byte_release(obj_buffer->app_data.cmd_op.param_data->parameter_list);
                  obj_buffer->app_data.cmd_op.param_data->parameter_list = NULL;
                }
              }
              tx_byte_release(obj_buffer->app_data.cmd_op.param_data);
              obj_buffer->app_data.cmd_op.param_data = NULL;
            }
            tx_byte_release(obj_buffer);
            obj_buffer = NULL;
          }
        }
      }
      
      break;
    }
    if(result == QAPI_OK && (sen_obj_cmd_ptr->cmd_hdr.cmd_id == SEN_OBJ_APP_ORIGINATED_EVT)
       && flag == true)
    {
      tx_byte_release(sen_obj_cmd_ptr);
    }
    else if(result == QAPI_OK && (sen_obj_cmd_ptr->cmd_hdr.cmd_id == SEN_OBJ_CALL_BACK_EVT)
            && flag == true)
    {
      tx_byte_release(sen_obj_cmd_ptr);
    }
    else
    {
      tx_byte_release(sen_obj_cmd_ptr);
    }
  } /* Loop to dequeue all outstanding commands*/
  return;
} /* sen_obj_process_commands */ 

