/*==============================================================================

                             TEST_MAIN_MODULE.C

GENERAL DESCRIPTION
  This file deals with the task Initialization to support various test dam  
  object mechanisms
 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The task init and task start routines do the work necessary to ensure that
  all data structures are in the desired state to continue with normal
  operation.

Copyright (c) 2018 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
#include "test_app_module.h"
#include "test_utils.h"


#define     Q_NO_OF_MSGS         100
#define     Q_MESSAGE_SIZE       TX_1_ULONG
#define     QUEUE_SIZE           Q_NO_OF_MSGS*Q_MESSAGE_SIZE*sizeof(dam_obj_cmd_t)


dam_obj_info_t          dam_obj_param;
dam_obj_task_cmd_q      dam_obj_cmdq;

/*=============================================================================

FUNCTION DAM_OBJ_SET_SIGNAL

DESCRIPTION
  Set the DAM signal using lock protection

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_obj_set_signal(uint32_t signals)
{
  LOG_INFO("DAM_APP:dam_obj_set_signal \n");

  tx_mutex_get(dam_obj_param.dam_obj_mutex, TX_WAIT_FOREVER);

  DAM_OBJ_TASK_SET_SIG(dam_obj_param.dam_obj_signal, signals);

  tx_mutex_put(dam_obj_param.dam_obj_mutex);

  return;
}

/*=============================================================================

FUNCTION DAM_OBJ_MUTEX_INIT

DESCRIPTION
  DAM object mutex Initialization

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void dam_obj_mutex_init(void)
{
  LOG_INFO("DAM_APP:dam_obj_mutex_init \n");

  /* allocate mutex required for thread*/ 
  txm_module_object_allocate(&dam_obj_param.dam_obj_mutex, sizeof(TX_MUTEX));
  tx_mutex_create(dam_obj_param.dam_obj_mutex, "dam_obj_mutex", TX_NO_INHERIT);
  return;
}


/*=============================================================================

FUNCTION DAM_OBJ_SIGNAL_INIT

DESCRIPTION
  DAM Object Signal Initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void dam_obj_signal_init(void)
{
  LOG_INFO("DAM_APP:dam_obj_signal_init \n");
  /* allocate signal required for thread*/ 
  txm_module_object_allocate(&dam_obj_param.dam_obj_signal, sizeof(TX_EVENT_FLAGS_GROUP));
  tx_event_flags_create(dam_obj_param.dam_obj_signal, "test_obj_sig");
  return;
}

/*=============================================================================

FUNCTION DAM_OBJ_SIGNAL_WAIT

DESCRIPTION
  This API will wait on the DAM_OBJ signal.
  The DAM_OBJ signal will be set during various operations.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
static void dam_obj_signal_wait(void)
{
  uint32_t received_sigs = 0;

  LOG_INFO("DAM_APP:dam_obj_signal_wait \n");


  while (1)
  {

/*============================================================================
  Wait for the DAM_OBJ Signal to be set
=============================================================================*/
    tx_event_flags_get(dam_obj_param.dam_obj_signal, DAM_OBJ_SIGNAL_MASK,
                       TX_OR_CLEAR, &received_sigs, TX_WAIT_FOREVER);

    if (DAM_OBJ_TASK_IS_SIG_SET(received_sigs, DAM_OBJ_SIGNAL_MASK))
    {
      LOG_INFO("DAM_APP:Signal received 0x%x \n",received_sigs);      

      /* handle the signals for perticular DAM app */
      dam_app_signal_wait(received_sigs);

    }
  }
}

/*================================================================

FUNCTION DAM_OBJ_TASK_INIT

DESCRIPTION
  DAM object Task initialization
  
DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==================================================================*/
void dam_obj_task_init(void)
{

  LOG_INFO("DAM_APP:dam_obj_task_init \n");
/*============================================================================
  Reseting the Global DAM_OBJ Structure
=============================================================================*/

  memset(&dam_obj_param, 0x00, sizeof (dam_obj_info_t));

/*----------------------------------------------------------------------------
  Initialize the command queues and set the command queue pointers in the
   test_obj_info struct to the appropriate values.
----------------------------------------------------------------------------*/
  
  dam_obj_cmdq_init(&dam_obj_param);

}

/*=============================================================================

FUNCTION dam_obj_task_entry

DESCRIPTION
  This is the entry point for DAM object task
  Reset the Global DAM_OBJ structure
  Initalize the DAM_OBJ Qurt signal

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_obj_task_entry(void *thread_ctxt)
{

  LOG_INFO("DAM_APP:dam_obj_task_entry \n");

  dam_obj_task_init();

/*=============================================================================
Initialize Mutex Object
==============================================================================*/

  dam_obj_mutex_init();

/*=============================================================================
Initialize Signal for DAM_OBJ
==============================================================================*/

  dam_obj_signal_init();

/*=============================================================================
  execute the function to be test, 
  interface dam_app_routine need be implemented by each sample application
==============================================================================*/
 dam_app_routine();

/*=============================================================================
Wait on the DAM_OBJ Signal
==============================================================================*/
  dam_obj_signal_wait();

}

/*===========================================================================

FUNCTION DAM_OBJ_CMDQ_INIT()

DESCRIPTION
  Initialize the command queues and set the command queue pointers in the
  test_obj_info struct to the appropriate values.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None

=========================================================================*/
void dam_obj_cmdq_init (dam_obj_info_t * dam_obj_state)
{
  void *cmdq_ptr = NULL;

  if (NULL == dam_obj_state)
  {
    LOG_ERROR("DAM_APP:DAM_OBJ State is NULL ");
    return;
  }

  cmdq_ptr = data_malloc(QUEUE_SIZE);

  if (NULL == cmdq_ptr)
  {
    LOG_ERROR("DAM_APP:CMD queue pointer is NULL ");
    return;
  }

  LOG_INFO("DAM_APP:dam_obj_cmdq_init");

  dam_obj_param = *dam_obj_state;

  /* allocate queue required for thread*/ 
  txm_module_object_allocate(&dam_obj_cmdq, sizeof(TX_QUEUE));
  tx_queue_create(dam_obj_cmdq, "dam_obj_cmdq", Q_MESSAGE_SIZE,cmdq_ptr, QUEUE_SIZE);

  dam_obj_param.cmd_q_ptr = dam_obj_cmdq;
}


/*===========================================================================

FUNCTION DAM_OBJ_GET_CMD_BUFFER

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
dam_obj_cmd_t* dam_obj_get_cmd_buffer()
{
  dam_obj_cmd_t*                 cmd_ptr = NULL;

  LOG_INFO("DAM_APP:dam_obj_get_cmd_buffer \n ");

  cmd_ptr = (dam_obj_cmd_t*)data_malloc(sizeof(dam_obj_cmd_t));

  if (NULL == cmd_ptr) 
  {
    LOG_ERROR("DAM_APP:Command buffer allocation failed");
  }

  return cmd_ptr;
}


/*===========================================================================

FUNCTION  DAM_OBJ_ENQUEUE_CMD

DESCRIPTION
  This function is used to enqueue the command into the DAM_OBJ queue

DEPENDENCIES
  None.

PARAMETERS
  Pointer to the command to enqueue

RETURN VALUE
  Void

SIDE EFFECTS
  None

=========================================================================*/
void dam_obj_enqueue_cmd(dam_obj_cmd_t* cmd_ptr)
{
  //LOG_INFO("DAM_APP:Cmd_ptr: 0x%x with cmd_id: %d, posted to int Q,",
  //                           cmd_ptr, cmd_ptr->cmd_hdr.cmd_id);

  tx_queue_send(dam_obj_cmdq, &cmd_ptr, TX_WAIT_FOREVER);

  return;
}

/*===========================================================================

FUNCTION DAM_OBJ_DEQUEUE_CMD

DESCRIPTION
  This function de-queues and returns a command from the DAM object task
  command queues.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the command if present in the queue. NULL otherwise

SIDE EFFECTS
  None

==============================================================================*/
static dam_obj_cmd_t* dam_obj_dequeue_cmd( void )
{
  dam_obj_cmd_t *cmd_ptr = NULL;

  LOG_INFO("DAM_APP:dam_obj_dequeue_cmd called");

  tx_queue_receive(dam_obj_cmdq, &cmd_ptr, TX_WAIT_FOREVER);

  return cmd_ptr;
}


