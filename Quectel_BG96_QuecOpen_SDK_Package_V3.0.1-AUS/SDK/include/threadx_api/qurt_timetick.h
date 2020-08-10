#ifndef QURT_TIMETICK_H
#define QURT_TIMETICK_H
/**
  @file qurt_timetick.h
  @brief  Prototypes of QuRT TimeTick APIs
  Qurt Time Tick APIs to get current tick count and conversion from Ticks to 
  Time or vice-versa

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2017  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.


=============================================================================*/

#include "qapi_txm_base.h"

#define  TXM_QURT_TIMETICK_GET_TICKS                   TXM_QURT_TIMETICK_BASE + 1
#define  TXM_QURT_TIMETICK_GET_CVT_TIME_TO_TICKS       TXM_QURT_TIMETICK_BASE + 2
#define  TXM_QURT_TIMETICK_SET_CVT_TICKS_TO_TIME       TXM_QURT_TIMETICK_BASE + 3


#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

typedef unsigned long long qurt_time_t;

/** qurt_time_unit_t types.*/
typedef enum {
  QURT_TIME_TICK,                  /**< -- Return time in Ticks */
  QURT_TIME_USEC,                  /**< -- Return time in Microseconds */
  QURT_TIME_MSEC,                  /**< -- Return time in Milliseconds */
  QURT_TIME_SEC,                   /**< -- Return time in Seconds */
  QURT_TIME_NONE=QURT_TIME_TICK    /**< -- Identifier to use if no particular return type is needed */ 
}qurt_time_unit_t;

qurt_time_t qurt_timetick_user_impl(UINT id, qurt_time_t a, UINT b);

qurt_time_t qurt_timetick_user_impl(UINT id, qurt_time_t a, UINT b)
{
  qurt_time_t qurt_time_tick_res = 0;
  UINT alower;
  UINT aupper;
  UINT c = 0, d = 0;
  switch(id)
  {
    case TXM_QURT_TIMETICK_GET_TICKS:
      if(TX_SUCCESS != (_txm_module_system_call12)(id, (ULONG) &c, (ULONG) &d, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
      {
        return (qurt_time_t)(~0x0);
      }
      break;
    case TXM_QURT_TIMETICK_GET_CVT_TIME_TO_TICKS:
    case TXM_QURT_TIMETICK_SET_CVT_TICKS_TO_TIME:
	  alower = a & 0xFFFFFFFF;					// lower word of 64-bits timetick 
	  aupper = (a>>32) & 0xFFFFFFFF ;			// upper word of 64-bits timetick
      if(TX_SUCCESS != (_txm_module_system_call12)(id, (ULONG) &c, (ULONG) &d, (ULONG) alower, (ULONG) aupper, (ULONG) b, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0, (ULONG) 0))
      {
        return (qurt_time_t)(~0x0);
      }
      break;
    default :
      return (qurt_time_t)(~0x0);
  }
  qurt_time_tick_res = ((qurt_time_t)(c) << 32) | (d);
  return qurt_time_tick_res;
}

#define qurt_timer_get_ticks()                  qurt_timetick_user_impl(TXM_QURT_TIMETICK_GET_TICKS, 0, 0)
#define qurt_timer_convert_time_to_ticks(a, b)  qurt_timetick_user_impl(TXM_QURT_TIMETICK_GET_CVT_TIME_TO_TICKS, a, b)
#define qurt_timer_convert_ticks_to_time(a, b)  qurt_timetick_user_impl(TXM_QURT_TIMETICK_SET_CVT_TICKS_TO_TIME, a, b)

#elif defined QAPI_TXM_SOURCE_CODE      // KERNEL_MODE_DEFS

UINT qurt_timetick_handler(UINT id, UINT a1, UINT a2, UINT a3, UINT a4, UINT a5, UINT a6, UINT a7, UINT a8, UINT a9, UINT a10, UINT a11, UINT a12);

/**@ingroup func_qurt_timer_get_ticks

   Gets current ticks. The ticks are accumulated since the RTOS has started.
  
   @param
   None.
  
   @return             
   Ticks since system started, in case of success. \n
   (qurt_time_t)(~0x0) - Failed to read current tick
   
   @dependencies
   None.
   
 */
qurt_time_t qurt_timer_get_ticks (void);

/**@ingroup func_qurt_timer_convert_time_to_ticks
   
  Converts to tick count the time value expressed in the specified time units.
  
  Converts time in given unit to ticks (currently based on 19.2 MHz clock)
  e.g. 	1 milli sec = 19200 ticks
		1 micro sec = 19 ticks
   
  @param[in] time         time value to convert. 
  @param[in] time_unit    time units that value is expressed in.
  
  @return
  Tick count in system ticks -- Success. \n
  (qurt_time_t)(~0x0) - Conversion failed.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_convert_time_to_ticks(qurt_time_t time, qurt_time_unit_t time_unit );

/**@ingroup func_qurt_timer_convert_ticks_to_time
   
  Converts tick count to the time value expressed in the specified time units.
  Currently, argument ticks in expected in 19.2 MHz clock.
	 1 tick = 1/19200000 s = 1/19200 ms = 1/19 micro sec. 	
	 
  @param[in] ticks        tick count to convert. 
  @param[in] time_unit    time units that return value is expressed in.  
  
  @return
  Time value expressed in the specified time units -- Success. \n
  (qurt_time_t)(~0x0) - Conversion failed.
   
  @dependencies
  None.
  
 */
qurt_time_t qurt_timer_convert_ticks_to_time(qurt_time_t ticks, qurt_time_unit_t time_unit);

#else   // DEF_END

#error "No QAPI flags defined"

#endif


#endif // QURT_TIMETICK_H
