/*===========================================================================

  Copyright (c) 2017-2019 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------

===========================================================================*/

/*
!! IMPORTANT NOTE: "DATA SERVICES" VERSION CATEGORY. 
*/

/**
  @file
  qapi_cli.h

  @brief
  CLI interface file to interact with QCLI module. 
*/

#ifndef CLI_QAPI_H
#define CLI_QAPI_H

#include "stdbool.h"
#include "qapi_status.h"
#include "qapi_data_txm_base.h"
#include "qapi_driver_access.h"

/* CLI QAPI REQUEST ID DEFINES */
#define  TXM_QAPI_CLI_REG_APP                        TXM_QAPI_CLI_BASE + 1
#define  TXM_QAPI_CLI_DEREG_APP                      TXM_QAPI_CLI_BASE + 2

/** @addtogroup qapi_cli
@{ */

/**
*   Handler provided by CLI to the application.
*/
typedef void * qapi_CLI_App_Handler_t;

/**
   This structure contains the information for a single parameter entered
   into the command line.
*/
typedef struct qapi_CLI_Parameter_s
{
   uint8_t *String_Value;      /**< String value. */
   int32_t  Integer_Value;     /**< Integer value (if the string was successfully converted). */
   bool     Integer_Is_Valid;  /**< Flag indicating whether the integer is valid. */
} qapi_CLI_Parameter_t;

/**
   Type that represents the format of a function that processes
          commands from the CLI.
   @param handle  Handle received from qapi_CLI_Register_Command_Group().
   @param parameter_count  Number of parameters that were
          specified to the CLI for the function.
   @param parameter_list List of parameters specified to the CLI
          for the function.

   @return See Section @xref{hdr:QAPIStatust}. \n
           On success, QAPI_OK (0) is returned. Other value on error.
*/
typedef qapi_Status_t (*qapi_CLI_Command_Function_t)(uint32_t parameter_count,
                                                     qapi_CLI_Parameter_t *parameter_list);
typedef qapi_Status_t (*qapi_CLI_Command_Function_a)(void);


/**
   Structure that represents the information for a single command in a
   command list.
*/
typedef struct qapi_CLI_Command_s
{
   qapi_CLI_Command_Function_t   Command_Function; /**< Function to be called when the command is executed from the CLI. */
   bool                          Start_Thread;     /**< Flag that indicates whether the command is to start on its own thread. */
   const uint8_t                *Command_String;   /**< String representation of the function. */
   const uint8_t                *Usage_String;     /**< Usage string for the command. */
   const uint8_t                *Description;      /**< Description string for the commmand. */
   qapi_CLI_Command_Function_a   abort_function;   /**< Abort function. */
} qapi_CLI_Command_t;

/**
   Structure that represents a command group that can be
   registered with the CLI.
*/
typedef struct qapi_CLI_Command_Group_s
{
   const uint8_t            *group_string;   /**< String representation of the group. */
   uint32_t                  command_count;  /**< Number of commands in the group.    */
   const qapi_CLI_Command_t *command_list;   /**< List of commands for the group.     */

} qapi_CLI_Command_Group_t;

/**
   Structure that represents command parameters.
*/
typedef struct qapi_CLI_Parameter_Data_s
{
  uint8_t  index;                        /**< Index of the command group corresponding to the command. */
  uint32_t parameter_count;              /**< Number of parameters passed in the command.              */
  qapi_CLI_Parameter_t *parameter_list;  /**< List of parameters.                                      */
} qapi_CLI_Parameter_Data_t;

/** @} */ /* end_addtogroup qapi_cli */

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 *-----------------------------------------------------------------------*/

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

static __inline void qapi_cli_cb_uspace_dispatcher(UINT cb_id,
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
  void (*pfn_app_cb1) (qapi_CLI_App_Handler_t , qapi_CLI_Parameter_Data_t *, void *);

  if(cb_id == DATA_CUSTOM_CB_CLI_CMD)
  {
    pfn_app_cb1 = (void (*)(qapi_CLI_App_Handler_t , qapi_CLI_Parameter_Data_t *,
                   void *))app_cb;

   (pfn_app_cb1)((qapi_CLI_App_Handler_t)cb_param1, (qapi_CLI_Parameter_Data_t *)cb_param2,
                  (void *)cb_param3);
  }
}

static __inline int cli_pass_byte_pool(qapi_CLI_App_Handler_t hndl, void *cli_byte_pool_ptr)
{
  qapi_cb_params_uspace_ptr_t *uspace_memory_cli = NULL;
  int ret = QAPI_ERROR;

  tx_byte_allocate(cli_byte_pool_ptr, (VOID **) &uspace_memory_cli,(sizeof(qapi_cb_params_uspace_ptr_t)), TX_NO_WAIT);
  if (NULL == uspace_memory_cli)
  {
    return QAPI_ERROR;
  }

  memset (uspace_memory_cli,0, sizeof(qapi_cb_params_uspace_ptr_t));

  /* Initialize user space information */ 
  uspace_memory_cli->usr_pool_ptr = cli_byte_pool_ptr;

  ret = qapi_data_map_u_addr_to_handle((void *)(hndl), QAPI_APP_CLI ,uspace_memory_cli,0);
  return ret;
}

static __inline int cli_destroy_indirection(qapi_CLI_App_Handler_t hndl)
{
  int ret = QAPI_ERROR;
  qapi_cb_params_uspace_ptr_t *uspace_memory_cli = NULL;
  
  /* Retrieve the user space information stored internally */
  qapi_data_map_handle_to_u_addr(hndl, QAPI_APP_CLI, (void**)&uspace_memory_cli);
  
  /* Release the allocated resources */
  if (uspace_memory_cli)
    tx_byte_release(uspace_memory_cli);
  
  /* Dispatch the call to the module manager */
  ret = ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_CLI_DEREG_APP, (ULONG) hndl, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0));
  return ret;
}

#define qapi_CLI_Register_Command_Group(a,b,c,d)                    ((UINT) (_txm_module_system_call12)(TXM_QAPI_CLI_REG_APP, (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) d, (ULONG) qapi_cli_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_CLI_Unregister_Command_Group(a)                        cli_destroy_indirection(a)
#define qapi_CLI_Pass_Pool_Ptr(a,b)                                 cli_pass_byte_pool(a,b)

#else

qapi_Status_t qapi_CLI_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

typedef qapi_Status_t (*qapi_CLI_Command_CB_t)
(
  qapi_CLI_App_Handler_t handle, 
  qapi_CLI_Parameter_Data_t *param_data,
  void *user_data
);

/** @addtogroup qapi_cli
@{ */

/**
*  Registers a command group with the CLI.
*
* @param[in,out] handle  Handle that is provided to the application on successful registration.
* @param[in] user_data  Transparent user data payload (to be returned in the user callback).
* @param[in] command_group Command group to be registered. Note that
                           this function assumes the command group information will be
                           constant and simply stores a pointer to the data. If the command
                           group and its associated information is not constant, its memory
                           must be retained until the command is unregistered.
* @param[in] user_cb_fn  User client callback handle to forward data to the application.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/

qapi_Status_t qapi_CLI_Register_Command_Group
(
  qapi_CLI_App_Handler_t *handle,
  void *user_data,
  qapi_CLI_Command_Group_t *command_group,
  qapi_CLI_Command_CB_t user_cb_fn
);

/**
* Unregisters a command group from the CLI.
*
* @param[in] handle  Handle for the group to be unregistered. 
                     This will be the value returned form 
                     qapi_CLI_Register_Command_Group() when the function was registered.
*
* @return See Section @xref{hdr:QAPIStatust}. \n
* On success, QAPI_OK (0) is returned. Other value on error.
*/

qapi_Status_t qapi_CLI_Unregister_Command_Group
(
  qapi_CLI_App_Handler_t *handle
);


#endif

/** @} */ /* end_addtogroup qapi_cli */

#endif

