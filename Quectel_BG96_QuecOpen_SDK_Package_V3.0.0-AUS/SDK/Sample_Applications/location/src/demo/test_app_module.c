/**************************************************************************************************************
 * Copyright © 2017 Qualcomm Technologies, Inc.
 * @file  test_app_module.c
 * @author  Qualcomm Technologies, Inc.
 * @brief File contains the implementation for the Presence location demo application
 *
 **************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "qapi_socket.h"
#include "qapi_cli.h"
#include "qflog_utils.h"

#define LOCATION_BYTE_POOL_SIZE 30720*8 
#define LOCATION_STACK_SIZE 16384 

UCHAR free_memory_location[LOCATION_BYTE_POOL_SIZE];
TX_BYTE_POOL *byte_pool_location;
TX_THREAD *app_thread_handle;

extern void location_app();

/*=============================================================================

FUNCTION TEST_OBJ_TASK_ENTRY

DESCRIPTION
This is the entry point for Location object task

DEPENDENCIES
None.

RETURN VALUE
none

SIDE EFFECTS
None

==============================================================================*/
void test_obj_task_entry(void *thread_ctxt)
{
    location_app();
}

 /**
 @brief This function is the main entry point for the application.
 */
int location_dam_app_start(void)
{
    int ret;
    CHAR *app_thread_stack_pointer = NULL;

    txm_module_object_allocate(&byte_pool_location, sizeof(TX_BYTE_POOL));
    tx_byte_pool_create(byte_pool_location, "Location application pool", free_memory_location, LOCATION_BYTE_POOL_SIZE);

    /*Sleep for 100 ms*/
    tx_thread_sleep(10);
    ret = txm_module_object_allocate(&app_thread_handle, sizeof(TX_THREAD));
    if (ret != TX_SUCCESS)
    {
        return ret;
    }
    /* Allocate the stack for presence sensor object application */

    tx_byte_allocate(byte_pool_location, (VOID **)&app_thread_stack_pointer, LOCATION_STACK_SIZE, TX_NO_WAIT);

    /* Create the sensor application thread.  */
    ret = tx_thread_create(app_thread_handle, "loc_obj", test_obj_task_entry,
        0, app_thread_stack_pointer, LOCATION_STACK_SIZE,
        150, 150, TX_NO_TIME_SLICE, TX_AUTO_START);

    if (ret != TX_SUCCESS)
    {
        if (app_thread_handle)
        {
            tx_byte_release(app_thread_handle);
        }
    }
    return ret;
}
