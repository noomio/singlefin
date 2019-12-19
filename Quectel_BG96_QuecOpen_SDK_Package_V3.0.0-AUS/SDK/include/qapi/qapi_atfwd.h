/**
  @file qapi_atfwd.h

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none
*/

/*
 Copyright (c) 2017-2018  Qualcomm Technologies, Inc.  All Rights Reserved.
 Confidential and Proprietary - Qualcomm Technologies, Inc.
 ======================================================================*/
/*======================================================================
 *
 *        EDIT HISTORY FOR FILE
 *
 *	 This section contains comments describing changes made to the
 *	 module. Notice that changes are listed in reverse chronological
 *	 order.
 *
 *	
 *
 *
 * when         who   what, where, why
 * ----------   ---   ------------------------------------------------
 * 2018-01-11   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 * 2017-10-26   leo   (Tech Comm) Edited/added Doxygen comments and markup. 
 * 2017-04-25   leo   (Tech Comm) Edited/added Doxygen comments and markup.
 ======================================================================*/
 
#ifndef _QAPI_ATFWD
#define _QAPI_ATFWD

#include "at_fwd_ext_api.h"
#include "qapi_tsens_types.h"
#include "qapi_status.h"
#include "qapi_data_txm_base.h"
#include "qapi_driver_access.h"
#include "qapi_socket.h"



#define MAX_AT_CMD_NAME_LEN                               21
#define MAX_AT_TOKEN_LEN                                  1500
/* ATFWD QAPI REQUEST ID DEFINES */
#define  TXM_QAPI_ATFWD_REG_CB                            TXM_QAPI_ATFWD_BASE + 1
#define  TXM_QAPI_ATFWD_DEREG_CB                          TXM_QAPI_ATFWD_BASE + 2
#define  TXM_QAPI_ATFWD_SEND_RESP_CB                      TXM_QAPI_ATFWD_BASE + 3
#define  TXM_QAPI_ATFWD_SEND_URC_RESP_CB                  TXM_QAPI_ATFWD_BASE + 4

static __inline void qapi_atfwd_cb_uspace_dispatcher(UINT cb_id,
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
  void (*pfn_app_cb1) (boolean , char* , uint8_t*, uint8_t,uint32_t);

  if(cb_id == DATA_CUSTOM_CB_ATFWD_REG)
  {
    pfn_app_cb1 = (void (*)(boolean , char* , uint8_t*, uint8_t, uint32_t))app_cb;
   (pfn_app_cb1)((boolean)cb_param1,
                 (char*)cb_param2,
                 (uint8_t*)cb_param3,
                 (uint8_t)cb_param4,
                 (uint32_t)cb_param5);
  }	
}


#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_atfwd_reg(a,b)                                   ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_ATFWD_REG_CB             , (ULONG) a, (ULONG) b, (ULONG) qapi_atfwd_cb_uspace_dispatcher, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_atfwd_dereg(a)                                   ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_ATFWD_DEREG_CB             , (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_atfwd_send_resp(a,b,c)                           ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_ATFWD_SEND_RESP_CB               , (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_atfwd_send_urc_resp(a,b)                         ((qapi_Status_t) (_txm_module_system_call12)(TXM_QAPI_ATFWD_SEND_URC_RESP_CB                , (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

/** @addtogroup qapi_atfwd_macros
@{ */

/**
 * Macro that passes a Byte Pool pointer for the ATFWD application.
 * 
 * Parameter a -- Pointer to the callback corresponding with the AT commands
 * registered by the client with ATFWD. \n  
 * Parameter b -- Pointer to the Byte Pool.
 *
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 * @note1hang This macro is only used in the DAM space.
 */

#define qapi_atfwd_Pass_Pool_Ptr(a,b)                          atfwd_set_byte_pool(a,b)

/**
 * Macro that releases a Byte Pool pointer for the ATFWD application.
 *
 * Parameter a -- Pointer to the callback corresponding with the AT commands
 * registered by the client with ATFWD.
 *  
 * On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 * @note1hang This macro is only used in the DAM space.
 */

#define qapi_atfwd_release_byte_pool(a)                        atfwd_release_byte_pool(a)

/** @} */

static __inline int atfwd_set_byte_pool(void* callback_ptr, void *atfwd_byte_pool_ptr)
{
  qapi_cb_params_uspace_ptr_t *uspace_memory_atfwd = NULL;
  int ret = QAPI_ERROR;

  tx_byte_allocate(atfwd_byte_pool_ptr, (VOID **) &uspace_memory_atfwd,(sizeof(qapi_cb_params_uspace_ptr_t)), TX_NO_WAIT);
  if (NULL == uspace_memory_atfwd)
  {
    return QAPI_ERROR;
  }

  memset (uspace_memory_atfwd,0, sizeof(qapi_cb_params_uspace_ptr_t));

  tx_byte_allocate(atfwd_byte_pool_ptr, (VOID **) &(uspace_memory_atfwd->cb_param[0]),MAX_AT_CMD_NAME_LEN, TX_NO_WAIT);
  if (NULL == uspace_memory_atfwd->cb_param[0])
  {
    return QAPI_ERROR;
  }
  memset (uspace_memory_atfwd->cb_param[0], 0, MAX_AT_CMD_NAME_LEN);
  uspace_memory_atfwd->cb_count++;

  tx_byte_allocate(atfwd_byte_pool_ptr, (VOID **) &(uspace_memory_atfwd->cb_param[1]),MAX_AT_TOKEN_LEN, TX_NO_WAIT);
  if (NULL == uspace_memory_atfwd->cb_param[1])
  {
    return QAPI_ERROR;
  }
  memset (uspace_memory_atfwd->cb_param[1], 0, MAX_AT_TOKEN_LEN);
  uspace_memory_atfwd->cb_count++;

  ret = qapi_data_map_u_addr_to_handle((void *)(callback_ptr), QAPI_APP_ATFWD,uspace_memory_atfwd,MAX_AT_TOKEN_LEN+MAX_AT_CMD_NAME_LEN);
  return ret;

}

static __inline int atfwd_release_byte_pool(void* callback_ptr)
{
  qapi_cb_params_uspace_ptr_t* uspace_memory_atfwd = NULL;
  int i=0;

  if (qapi_data_map_handle_to_u_addr(callback_ptr, QAPI_APP_ATFWD, (void**)&uspace_memory_atfwd) == QAPI_OK)
  {
    if (uspace_memory_atfwd != NULL)
    {
      for (i=0;i<uspace_memory_atfwd->cb_count; i++)
      {
        if (uspace_memory_atfwd->cb_param[i] != NULL)
        {
          if(uspace_memory_atfwd->cb_param[i] != NULL)
          {
            tx_byte_release(uspace_memory_atfwd->cb_param[i]);
            uspace_memory_atfwd->cb_param[i]=NULL;
          }
          uspace_memory_atfwd->cb_count--;
        }
      }

      tx_byte_release(uspace_memory_atfwd);
      uspace_memory_atfwd = NULL;
    }

    return QAPI_OK;
  }

  if (uspace_memory_atfwd != NULL)
  {
    tx_byte_release(uspace_memory_atfwd);
    uspace_memory_atfwd = NULL;
  }
  return QAPI_ERROR;
}



#else

qapi_Status_t qapi_ATFWD_Handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);


/*=======================================================
 * QAPI ATFWD APIs                             *
 =======================================================*/


/** @ingroup qapi_atfwd_reg
 *
 * @brief Registers new custom AT commands along with their respective callbacks.
 *
 * @param[in] name  Pointer to an AT commands string.
 *
 * @param[in] atfwd_callback  Pointer to the callback corresponding to the AT commands.
 * 
 * @return On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 */

qapi_Status_t qapi_atfwd_reg(char *name, at_fwd_cb_type atfwd_callback);


/** @ingroup qapi_atfwd_dereg
 *
 * @brief Deregisters AT commands. 
 *
 * @param[in] name  Pointer to the AT commands list.
 *
 * @return On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 */

qapi_Status_t qapi_atfwd_dereg(char *name);


/** @ingroup qapi_atfwd_send_resp
 *
 * @brief Sends a response.
 *
 * @param[in] atcmd_name  Pointer to the particular AT command to which this response corresponds.
 *
 * @param[in] buf  Pointer to the buffer containing the response. 
 *
 * @param[in] result   0 -- Result ERROR. This is to be set in case of ERROR or CME ERROR. 
 *                                        The response buffer contains the entire details. \n
 *                     1 -- Result OK.    This is to be set if the final response is to send 
 *                                        an OK result code to the terminal. 
 *
 * @return On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 */

qapi_Status_t qapi_atfwd_send_resp(char *atcmd_name, char *buf, uint32_t result);


/** @ingroup qapi_atfwd_send_urc_resp
 *
 * @brief Sends a URC response.
 *
 * @param[in] atcmd_name Pointer to the particular AT command to which this response corresponds.
 *
 * @param[in] at_urc  Pointer to the buffer containing the response. 
 *
 * @return On success, QAPI_OK is returned. On error, QAPI_ERROR is returned.
 *
 */

qapi_Status_t qapi_atfwd_send_urc_resp(char *atcmd_name, char *at_urc);

#endif

#endif
