/**************************************************************************************************************
 * Copyright © 2018 Qualcomm Technologies, Inc.
 * @file  test_app_module.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the dam demo application
 *
 **************************************************************************************************************/
#include "test_app_module.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "qflog_utils.h"
#include "test_utils.h"

/* DAM thread stack size: 16*1024, can be configured by user */
#define DAM_THREAD_STACK_SIZE 16384
/* DAM thread priority, not configurable */
#define DAM_THREAD_PRIORITY 150
/* Sleep duration in seconds */
#define DAM_SLEEP_DURATION 40

unsigned long __stack_chk_guard __attribute__((weak)) = 0xDEADDEAD;

void __attribute__((weak)) __stack_chk_fail (void)
{
    return;
}

TX_THREAD *app_thread_handle;


/**
   @brief This function is the main entry point for the application.
*/
int test_dam_app_start(void)
{
  int ret;
  CHAR *app_thread_stack_pointer = NULL;

  /* Sleep while device initialization to be done. Especially log
     module need some time to initilize before printing logs*/
  qapi_Timer_Sleep(DAM_SLEEP_DURATION, QAPI_TIMER_UNIT_SEC, true);

  LOG_INFO("DAM_APP:test_dam_app_start entry \n");

  do
  {
    ret = dam_byte_pool_init();
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("DAM_APP:Create DAM byte pool fail \n");
      break;
    }

    /* Allocate app_thread_handle (App thread) */
    ret = txm_module_object_allocate(&app_thread_handle, sizeof(TX_THREAD));
    if(ret != TX_SUCCESS)
    {
      LOG_ERROR("DAM_APP:Allocate app_thread_handle fail \n");
      break;
    }
    
    /* Allocate the stack for presence app_thread_handle */
    app_thread_stack_pointer = (CHAR *)data_malloc(DAM_THREAD_STACK_SIZE);
    if(NULL == app_thread_stack_pointer)
    {
      LOG_ERROR("DAM_APP:Allocate app_thread_stack_pointer fail \n");
      break;
    }

    /* Create app_thread_handle (App thread) */
    ret = tx_thread_create(app_thread_handle, "dam_obj", dam_obj_task_entry,
                           0, app_thread_stack_pointer, DAM_THREAD_STACK_SIZE,
                           DAM_THREAD_PRIORITY, DAM_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

    LOG_INFO("DAM_APP:tx_thread_create with ret[%d] \n", ret);

    if(ret != TX_SUCCESS)
    {
      if(app_thread_handle)
      {
        txm_module_object_deallocate(app_thread_handle);
      }
      LOG_ERROR("DAM_APP:Create app_thread_handle fail \n");
    }
  }while(0);

  LOG_INFO("DAM_APP:basic_dam_app_start exit with ret[%d] \n", ret);

  return ret;
}

