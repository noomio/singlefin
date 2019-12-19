/*===========================================================================
                         TEST_APP_MODULE.H

DESCRIPTION
   This header file contains definitions used internally by the dam object test
   Module. 
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "txm_module.h"
#include "stdlib.h"
#include "stdint.h"
#include <stdarg.h>
#include "qapi_diag.h"
#include "qapi_timer.h"
#include "time.h"
#include "qapi_dss.h"
#include "qapi_ns_utils.h"
#include "test_sample_module.h"


/*------------------------------------------------------------------------------
  Typedef Declaration
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  This is the command queue that will be used to enqueue commands from
  external entities
------------------------------------------------------------------------------*/
typedef TX_QUEUE* dam_obj_task_cmd_q;

typedef enum
{
  DAM_OBJ_MIN_CMD_EVT,
  DAM_OBJ_APP_ORIGINATED_EVT,
  DAM_OBJ_CALL_BACK_EVT,
  DAM_OBJ_CMD_FUNC_EVT,
  DAM_OBJ_MAX_CMD_EVT
}dam_obj_cmd_id_e;


/*------------------------------------------------------------------------------
  Structure Declaration
------------------------------------------------------------------------------*/

typedef struct
{
  TX_EVENT_FLAGS_GROUP*          dam_obj_signal;
  TX_MUTEX*                      dam_obj_mutex;
  TX_QUEUE*                      cmd_q_ptr;
}dam_obj_info_t;


typedef struct{
  dam_obj_cmd_id_e                cmd_id;
}dam_obj_cmd_hdr_t;


typedef struct{
  void *data;
}dam_obj_cmd_data_t;


typedef struct{
  dam_obj_cmd_hdr_t               cmd_hdr;
  dam_obj_cmd_data_t              cmd_data;
} dam_obj_cmd_t;

/* Signal used in DAM object */
enum DAM_SIG
{
  DAM_SIG_DATA_CALL_CONN = (0x1<<1),
  DAM_SIG_DATA_CALL_NO_NET = (0x1<<2)
};


/* Mask include all DAM signals */
#define DAM_OBJ_SIGNAL_MASK       DAM_SIG_DATA_CALL_CONN | \
                                  DAM_SIG_DATA_CALL_NO_NET | \
                                  DAM_SIG_APP

/* Check if the signal is set in the mask */
#define DAM_OBJ_TASK_IS_SIG_SET(signal, signal_mask)  ((signal & signal_mask) != 0)

/* Clear the signal in the mask */
#define DAM_OBJ_TASK_CLEAR_SIG(signal, signal_mask) \
                                    tx_event_flags_set(signal,signal_mask, TX_AND)

/* Set the signal in the mask */
#define DAM_OBJ_TASK_SET_SIG(signal, signal_mask) \
                                    tx_event_flags_set(signal, signal_mask, TX_OR)

extern dam_obj_info_t  dam_obj_param;

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
void dam_obj_task_entry(void *thrd);

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
void dam_obj_enqueue_cmd(dam_obj_cmd_t* cmd_ptr);

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
void dam_obj_cmdq_init (dam_obj_info_t * dam_obj_state);


