/*===========================================================================
                         TEST_Hello_World_MODULE.C

DESCRIPTION
   This file is Hello World application.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/
#include "test_app_module.h"
#include "test_utils.h"

static qapi_TIMER_handle_t dog_timer;

/*=============================================================================

FUNCTION DAM_APP_ROUTINE

DESCRIPTION
  timer expried call back function
  it will trigger a signal for application

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void timer_signal_cb(uint32 userData)
{
  LOG_INFO("Period Timer(5seconds) reach, send signal to task\n");
  dam_obj_set_signal(DAM_SIG_TIMER);
  return;
}

/*=============================================================================

FUNCTION DAM_APP_ROUTINE

DESCRIPTION
  This function is entry for sample applicion own logic.
  in this hello world sample appliation, here just start a 5 seconds timer

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_routine(qapi_DSS_Hndl_t hndl)
{
  qapi_Status_t result = QAPI_ERROR;

  qapi_TIMER_define_attr_t time_define_attr;
  time_define_attr.deferrable = FALSE;
  time_define_attr.cb_type = QAPI_TIMER_FUNC1_CB_TYPE;
  time_define_attr.sigs_func_ptr = timer_signal_cb;
  time_define_attr.sigs_mask_data = NULL;

  result = qapi_Timer_Def(&dog_timer, &time_define_attr);

  qapi_TIMER_set_attr_t time_set_attr;
  time_set_attr.unit = QAPI_TIMER_UNIT_SEC;
  time_set_attr.reload = FALSE;
  time_set_attr.time = 5;

  result = qapi_Timer_Set(dog_timer, &time_set_attr);
  if(result == QAPI_OK)
  {
     LOG_INFO("Timer set successfully.\n");
  }
  else
  {
    LOG_INFO("Timer set failed.\n");
  }
  
  return;
}

/*=============================================================================

FUNCTION DAM_APP_SIGNAL_WAIT

DESCRIPTION
  This function is to check and process signals for the sample app modules.
  In this hello world sample application, just restart the timer if the timer is expired and do print

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_signal_wait(uint32_t received_sigs)
{

  qapi_Status_t result = QAPI_ERROR;

  /* check and handle the signals that app interests */
  if(received_sigs & DAM_SIG_TIMER == DAM_SIG_TIMER)
  {
    LOG_INFO("Handle Timer(5seconds) Signal\n");

    qapi_TIMER_set_attr_t time_set_attr;
    time_set_attr.unit = QAPI_TIMER_UNIT_SEC;
    time_set_attr.reload = TRUE;
    time_set_attr.time = 5;

    result = qapi_Timer_Set(dog_timer, &time_set_attr);
    if(result != QAPI_OK)
    {
      LOG_INFO("Timer set failed.\n");
    }	
	else
	{
    LOG_INFO("start Timer(5seconds) again\n");	
	}

  }
  else
  {
    LOG_INFO("signarls %d to be handled", received_sigs);
  }

  return;
}


