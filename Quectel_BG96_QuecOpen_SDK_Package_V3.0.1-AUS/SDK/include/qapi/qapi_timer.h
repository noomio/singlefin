#ifndef __QAPI_TIMER_H__
#define __QAPI_TIMER_H__

/*=================================================================================
 *
 *                       TIMER SERVICES
 *
 *================================================================================*/

/** @file qapi_timer.h
 *
 * @brief Timer Services 
 *
 * @addtogroup qapi_timer
 * @{
 *
 * @details This interface implements Advanced Time Services (ATS) timer services. 
 *          This timer service is different than the RTOS timer service. 
 *          This timer service will be available in SOM mode.
 * 
 * 
 * @note1hang  These routines are fully re-entrant. In order to prevent memory leaks,
 *          whenever timer usage is done, the timer should be undefined using the 
 *          qapi_Timer_Undef() API. Timer callbacks should do minimal processing. 
 *          Time callbacks implementation should not contain any mutex or RPC.
 *          
 *
 * @code
 *
 *    * The code snippet below demonstrates usage of timer interface. In the
 *    * example below, a client defines a timer, sets a timer, stops the timer,
 *    * and undefines a timer.
 *    * For brevity, the sequence assumes that all calls succeed. 
 * 
 *    qapi_TIMER_handle_t timer_handle;	  
 *    
 *    qapi_TIMER_def_attr_t timer_def_attr; 
 *    timer_def_attr.cb_type = TIMER_FUNC1_CB_TYPE; //notification type
 *    timer_def_attr.sigs_func_ptr = &timer_test_cb; //callback to call when
 *                                                   //the timer expires
 *    timer_def_attr.sigs_mask_data = 0x1; //this data will be returned in
 *                                         //the callback
 *    timer_def_attr.deferrable = false; //set to true for nondeferrable timer
 *    
 *    //define the timer. Note: This call allocates memory and hence
 *    //qapi_Timer_Undef() should be called whenever the timer usage is done.
 *    qapi_Timer_def( &timer_handle, &timer_def_attr);
 *
 *    qapi_TIMER_set_attr_t timer_set_attr; 
 *    timer_set_attr.reload = reload_duration; //Timer would expire after every
                                               //reload_duration value.
 *    timer_set_attr.time = time_duration; 
 *    timer_set_attr.unit = T_MSEC; 
 *    
 *    //set or start the timer
 *    qapi_Timer_set( timer_handle, &timer_set_attr);
 *
 *    //stop a running timer
 *    qapi_Timer_stop( timer_handle);
 *
 *    //Undef the timer. Releases memory allocated in qapi_Timer_Def()
 *    qapi_Timer_undef( timer_handle);
 *
 *
 * @endcode
 *
 *  @}
*/

/*==================================================================================
       Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
                       All Rights Reserved.
       Confidential and Proprietary - Qualcomm Technologies, Inc.
==================================================================================*/

/* $Header: //components/rel/core.tx/1.0/api/services/qapi_timer.h#7 $ */

/*==========================================================================
when       who     what, where, why
--------   ---    ---------------------------------------------------------- 
01/15/18   leo    (Tech Comm) Edited/added Doxygen comments and markup.
09/04/17   leo    (Tech Comm) Edited/added Doxygen comments and markup.
04/24/17   leo    (Tech Comm) Edited/added Doxygen comments and markup.
05/01/16   gp     Created
===========================================================================*/


/*==================================================================================

                               TYPE DEFINITIONS

==================================================================================*/
#include "qapi_status.h"

/** @addtogroup qapi_timer
@{ */
/**
* Timer notification type.
*
* Enumeration of the notifications available on timer expiry. 
*/
typedef enum {
  QAPI_TIMER_NO_NOTIFY_TYPE,          /**< No notification. */  
  QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE,   /**< Signal an object. */    
  QAPI_TIMER_FUNC1_CB_TYPE,           /**< Call back a function. */    
  QAPI_TIMER_INVALID_NOTIFY_TYPE    
} qapi_TIMER_notify_t;

/**
* Timer unit type.
*
* Enumeration of the units in which timer duration can 
* be specified. 
*/
typedef enum {
  QAPI_TIMER_UNIT_TICK,     /**< Return time in ticks. */
  QAPI_TIMER_UNIT_USEC,     /**< Return time in microseconds. */
  QAPI_TIMER_UNIT_MSEC,     /**< Return time in milliseconds. */
  QAPI_TIMER_UNIT_SEC,      /**< Return time in seconds. */
  QAPI_TIMER_UNIT_MIN,      /**< Return time in minutes. */
  QAPI_TIMER_UNIT_HOUR,     /**< Return time in hours. */
  QAPI_TIMER_UNIT_MAX
}qapi_TIMER_unit_type;

/**
* Timer information type.
*
* Enumeration of the types of information that can be 
* obtained for a timer. 
*/
typedef enum{
  QAPI_TIMER_TIMER_INFO_ABS_EXPIRY = 0, /**< Return the timetick of timer expiry in native ticks. */
  QAPI_TIMER_TIMER_INFO_TIMER_DURATION, /**< Return the total duration of the timer in specified units. */
  QAPI_TIMER_TIMER_INFO_TIMER_REMAINING, /**< Return the remaining duration of the timer in specified units. */
  QAPI_TIMER_TIMER_INFO_MAX, 
}qapi_TIMER_info_type;


/**
* Timer define attribute type.
*
* This type is used to specify parameters when defining a timer. 
* @verbatim 
* sigs_func_ptr will depend on the value of qapi_TIMER_notify_t.
* qapi_TIMER_notify_t == QAPI_TIMER_NO_NOTIFY_TYPE, 
* sigs_func_ptr = Don't care 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_NATIVE_OS_SIGNAL_TYPE, 
* sigs_func_ptr = qurt signal object 
*  
* qapi_TIMER_notify_t == QAPI_TIMER_FUNC1_CB_TYPE, 
* sigs_func_ptr == specify a callback of type qapi_TIMER_cb_t
* @endverbatim
*/
typedef struct
{
  qbool_t                   deferrable; /**< FALSE = deferrable. */  
  qapi_TIMER_notify_t       cb_type;    /**< Type of notification to receive. */
  void*                     sigs_func_ptr; /**< Specify the signal object or callback function.  */  
  uint32_t                  sigs_mask_data; /**< Specify the signal mask or callback data. */
}qapi_TIMER_define_attr_t;

/**
* Type used to get a user space expired timer's callback information.
*
* This type is used to get a user space expired timer's callback information. 
* @verbatim 
* data = Specify the callback data for func_ptr,
* func_ptr = function pointer needs to be invoked.
*  
* @endverbatim
*/
typedef struct
{
  void*                     func_ptr; /**< Specify the callback function.  */  
  uint32_t                  data;     /**< Specify the callback data. */  
} qapi_TIMER_get_cbinfo_t;

/**
* Type used to specify parameters when starting a timer.
*/
typedef struct
{
  uint64_t              time;   /**< Timer duration. */  
  uint64_t              reload; /**< Reload duration. */
  qapi_TIMER_unit_type  unit;   /**< Specify units for timer duration. */  
}qapi_TIMER_set_attr_t;

/**
* Type used to get information for a given timer.
*/
typedef struct
{
  qapi_TIMER_info_type   timer_info;   /**< Timer information type. */
  qapi_TIMER_unit_type   unit;   /**< Specify units to use for return. */
}qapi_TIMER_get_info_attr_t;

/** @} */

/** @addtogroup qapi_timer
@{ */

/**
* Timer handle.
*
* Handle provided by the timer module to the client. Clients 
* must pass this handle as a token with subsequent timer calls. 
* Note that the clients should cache the handle. Once lost, it 
* cannot be queried back from the module. 
*/

typedef void* qapi_TIMER_handle_t;


/**
* Timer callback type.
*
* Timer callbacks should adhere to this signature.
*/
typedef void (*qapi_TIMER_cb_t)(uint32_t data);

/** @} */

/** @addtogroup qapi_timer
@{ */
/**
* Time type.
*
* Native timestamp type.
*/
typedef unsigned long qapi_qword[ 2 ];
typedef qapi_qword qapi_time_type;

/**
* Time in Julian format.
*/
typedef struct
{
  uint16_t year;        /**< Year (1980 through 2100). */
  uint16_t month;       /**< Month of the year (1 through 12). */
  uint16_t day;         /**< Day of the month (1 through 31). */
  uint16_t hour;        /**< Hour of the day (0 through 23). */
  uint16_t minute;      /**< Minute of the hour (0 through 59). */
  uint16_t second;      /**< Second of the minute (0 through 59). */
  uint16_t day_of_week; /**< Day of the week (0 through 6 or Monday through 
                           Sunday). */
}
qapi_time_julian_type;

/**
* Time unit type.
*
* Enumeration of the types of time that can be
* obtained from time get QAPI.
*/
typedef enum
{
  QAPI_TIME_STAMP = 0x10, /**< Return the time in timestamp format. */
  QAPI_TIME_MSECS, /**< Return the time in millisecond format. */
  QAPI_TIME_SECS, /**< Return the time in second format. */
  QAPI_TIME_JULIAN, /**< Return the time in Julian calendar format. */
}qapi_time_unit_type;

/**
* Used to specify parameters when getting the time. 
* @verbatim 
* Pointers depend on the value of qapi_time_unit_type.
* qapi_time_unit_type == QAPI_TIME_STAMP, 
* time_ts = Of type qapi_time_type
*  
* qapi_time_unit_type == QAPI_TIME_MSECS, 
* time_msecs = Of type unint64_t
*
* qapi_time_unit_type == QAPI_TIME_SECS, 
* time_secs = Of type unint64_t
*
* qapi_time_unit_type == QAPI_TIME_JULIAN, 
* time_julian = Of type qapi_time_julian_type
* @endverbatim
*/
typedef union
{
  qapi_time_type time_ts; /**< Specify the qapi_time_type variable pointer. */
  uint64_t time_msecs; /**< Variable for getting time in msec. */
  uint64_t time_secs; /**< Variable for getting time in sec. */
  qapi_time_julian_type time_julian; /**< Variable for getting time in Julian. */
}qapi_time_get_t;


/**
* @brief  
* Gets the time in the specified format. 
*
* @param[in] time_get_unit  Unit in which to get the time. 
* @param[in] time           Pointer to the #qapi_time_get_t variable. 

  @return
  QAPI_OK on success, an error code on failure.
*/
qapi_Status_t qapi_time_get(
   qapi_time_unit_type    time_get_unit,
   qapi_time_get_t* time
);

/**
  @brief  
* Allocates internal memory in the timer module. The internal 
* memory is then formatted with parameters provided in the 
* timer_def_attr variable. The timer_handle is returned to the 
* client, and this handle must be used for any subsequent timer 
* operations. 
  
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for defining the timer. 
  
  @return
  QAPI_OK on success, an error code on failure.
  
* @sideeffects
* Calling this API causes memory allocation. 
* Therefore, whenever the timer usage is done and
* not required, qapi_Timer_Undef() must be called
* to release the memory, otherwise it will cause a
* memory leak.
*/
qapi_Status_t qapi_Timer_Def(
   qapi_TIMER_handle_t*    timer_handle,
   qapi_TIMER_define_attr_t*  timer_attr  
);


/**
* @brief  
* Starts the timer with the duration specified in timer_attr. 
* If the timer is specified as a reload timer in timer_attr, 
* the timer will restart after its expiry.
*
* @param[in] timer_handle  Handle to the timer. 
* @param[in] timer_attr    Attributes for setting the timer. 
*
* @return
* QAPI_OK on success, an error code on failure.
*
* @dependencies
* The qapi_Timer_Def() API should be called for the timer before 
* calling qapi_Timer_Set function.
*/
qapi_Status_t qapi_Timer_Set(
   qapi_TIMER_handle_t    timer_handle,
   qapi_TIMER_set_attr_t* timer_attr  
);

/**
* @brief  
* Gets specified information about the timer.
*
* @param[in] timer_handle  Handle to the timer. 
* @param[out] timer_info   Type of information 
*       needed from the timer.
* @param[out] data         Returned timer information.
*
* @return
* QAPI_OK on success, an error code is returned on failure.
* 
*/
qapi_Status_t qapi_Timer_Get_Timer_Info(
   qapi_TIMER_handle_t         timer_handle,
   qapi_TIMER_get_info_attr_t* timer_info,
   uint64_t*                   data
);

/**
* @brief
* Timed wait. Blocks a thread for a specified time. 
*
* @param[in] timeout  Specify the duration to block the thread.
* @param[in] unit     Specify the units of the duration.
* @param[in] non_deferrable  TRUE = processor (if in deep 
*       sleep or power collapse) will be awakened on timeout. \n
*       FALSE = processor will not be awakened from deep sleep
*       or power collapse on timeout. \n Whenever the processor
*       wakes up due to some other reason after timeout, the
*       thread will be unblocked.
*
* @return
* QAPI_OK on success, an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Sleep(
  uint64_t             timeout,
  qapi_TIMER_unit_type unit,
  qbool_t              non_deferrable
);

/**
* @brief  
* Undefines the timer. This API must be called whenever 
* timer usage is done. Calling this API releases the 
* internal timer  memory that was allocated when the timer was 
* defined. 
*
* @param[in] timer_handle  Timer handle for which to undefine the timer.
*
* @return
* QAPI_OK on success, an error code on failure
* 
*/
qapi_Status_t qapi_Timer_Undef(
    qapi_TIMER_handle_t    timer_handle
);

/**
  @brief  
* Stops the timer.
*
* @note1hang This function does not deallocate the 
* memory that was allocated when the timer was defined.
  
* @param[in] timer_handle  Timer handle for which to stop the timer.
  
  @return
  QAPI_OK on success, an error code on failure.
* 
*/
qapi_Status_t qapi_Timer_Stop(
    qapi_TIMER_handle_t    timer_handle
);

/** @} */ /* end_addtogroup_group timer */

#endif /*__QAPI_TIMER_H__ */
