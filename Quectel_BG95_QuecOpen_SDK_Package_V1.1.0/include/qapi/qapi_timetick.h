#ifndef QAPI_TIMETICK_H
#define QAPI_TIMETICK_H

/**
  @file qapi_timetick.h
  @brief  Prototypes of QAPI TimeTick APIs

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2017 - 2019  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


=============================================================================*/

#include "qapi_txm_base.h"

//QAPI IDs for timetick handler
#define  TXM_QURT_TIMETICK_GET_TICKS                   TXM_QURT_TIMETICK_BASE + 1
#define  TXM_QURT_TIMETICK_GET_CVT_TIME_TO_TICKS       TXM_QURT_TIMETICK_BASE + 2
#define  TXM_QURT_TIMETICK_SET_CVT_TICKS_TO_TIME       TXM_QURT_TIMETICK_BASE + 3
#define  TXM_QURT_TIMETICK_ID_SIZE					   3

/**@ingroup func_qurt_timetick_handler

   Timetick QAPI handler that perform timetick related operations based on ID passed
  
   @param
   None.
  
   @return 
   0xFFFFFFFF - Operation not performed successfully   
   returns time or ticks based on ID passed
   
   @dependencies
   None.
   
 */

UINT qurt_timetick_handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

#endif // QAPI_TIMETICK_H
