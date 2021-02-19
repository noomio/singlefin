/* Copyright (c) 2018 - 2019 Qualcomm Technologies, Inc.
   All rights reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.
*/

/**
 *
 * This file provides helper functions which are used for logging a debug message
 * @}
 */

#ifndef _DATA_QFLOG_H
#define _DATA_QFLOG_H

#include <stdint.h>

#include "txm_module.h" 
#include "qapi_types.h" // For qapi_Status_t and other qapi types
#include "qapi_status.h" // For QAPI_ERROR, QAPI_SUCCESS and other status codes
#include "qapi_txm_base.h" // For TXM_QAPI_DIAG_BASE
#include "qapi_data_txm_base.h"
#include "qapi_diag.h"

typedef void * qflog_App_Handler_t;

#define QFLOG_MSG(xx_ss_id, xx_ss_mask, xx_fmt, ...) \
  do { \
    if (xx_ss_mask & (MSG_BUILD_MASK_## xx_ss_id)) { \
       XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      qflog_msg_send (&xx_msg_const, MSG_NARG(xx_fmt, ## __VA_ARGS__), ## __VA_ARGS__); \
    } \
  } while (0) \

#define QFLOG2_MSG(handle, xx_ss_id, xx_ss_mask, xx_fmt, ...) \
  do { \
       if (xx_ss_mask & (MSG_BUILD_MASK_## xx_ss_id)) { \
         XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
         qflog_msg_send_v2 (handle, &xx_msg_const, MSG_NARG(xx_fmt, ## __VA_ARGS__), ## __VA_ARGS__); \
     } \
  } while (0) \


#define  TXM_DATA_QFLOG_PRINTF                         (TXM_DATA_QFLOG_BASE + 1)
#define  TXM_DATA_QFLOG_REGISTER_V2                    (TXM_DATA_QFLOG_BASE_V2 + 1)
#define  TXM_DATA_QFLOG_PRINTF_V2                      (TXM_DATA_QFLOG_BASE_V2 + 2)
#define  TXM_DATA_QFLOG_DEREGISTER_V2                  (TXM_DATA_QFLOG_BASE_V2 + 3)

#ifdef  QAPI_TXM_MODULE

#define qflog_msg_send(const_blk, num_args, ...) \
((qapi_Status_t) (_txm_module_system_call11) (TXM_DATA_QFLOG_PRINTF, (uint32_t)const_blk,(uint32_t)num_args, \
(uint32_t)GET_ARG(1, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(2, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(3, ## __VA_ARGS__ , PAD()),(uint32_t)GET_ARG(4, ## __VA_ARGS__ , PAD()), \
(uint32_t)GET_ARG(5, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(6, ## __VA_ARGS__ , PAD()),(uint32_t)GET_ARG(7, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(8, ## __VA_ARGS__ , PAD()), \
(uint32_t)GET_ARG(9, ## __VA_ARGS__ , PAD())))


#define qflog_msg_send_v2(handle, const_blk, num_args, ...) \
((qapi_Status_t) (_txm_module_system_call12) (TXM_DATA_QFLOG_PRINTF_V2, (uint32_t) handle, (uint32_t)const_blk,(uint32_t)num_args, \
(uint32_t)GET_ARG(1, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(2, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(3, ## __VA_ARGS__ , PAD()),(uint32_t)GET_ARG(4, ## __VA_ARGS__ , PAD()), \
(uint32_t)GET_ARG(5, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(6, ## __VA_ARGS__ , PAD()),(uint32_t)GET_ARG(7, ## __VA_ARGS__ , PAD()), (uint32_t)GET_ARG(8, ## __VA_ARGS__ , PAD()), \
(uint32_t)GET_ARG(9, ## __VA_ARGS__ , PAD())))

#define qflog_app_register(a,b) ((qapi_Status_t) (_txm_module_system_call12)(TXM_DATA_QFLOG_REGISTER_V2, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qflog_app_deregister(a) ((qapi_Status_t) (_txm_module_system_call12)(TXM_DATA_QFLOG_DEREGISTER_V2, (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))

#else
qapi_Status_t flog_handler(uint32_t request_ID, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5, uint32_t arg6, uint32_t arg7, uint32_t arg8, uint32_t arg9, uint32_t arg10, uint32_t arg11, uint32_t arg12);
qapi_Status_t qflog_msg_send(const msg_const_type * const_blk, uint32_t num_args, ...);
qapi_Status_t qflog_msg_send_v2(qflog_App_Handler_t handle,const msg_const_type * const_blk, uint32_t num_args, ...);
qapi_Status_t qflog_app_register(qflog_App_Handler_t *handle, char* app_name);
qapi_Status_t qflog_app_deregister(qflog_App_Handler_t handle);

#endif
  		  
#endif


