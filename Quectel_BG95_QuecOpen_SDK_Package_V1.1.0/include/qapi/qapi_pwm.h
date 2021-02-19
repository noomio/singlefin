/*
 * Copyright (c) 2019 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*==================================================================================
                              EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //components/rel/core.tx/5.0/api/systemdrivers/qapi_pwm.h#3 $

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------------
 07/18/19   shm     Made QAPI handler public for FR57064.
 06/12/19   shm     Edited/added Doxygen comments and markup.
 03/15/19   shm     Initial version.
 ==================================================================================*/


#ifndef __QAPI_PWM__
#define __QAPI_PWM__
/**
 * @file qapi_pwm.h
 * 
 * Pulse Width Modulation (PWM)
 *
 * @addtogroup qapi_pwm
 * @{
 * 
 * @details Many peripherals are controlled through PWM signals. For example,
 *          an LED's brightness, a motor's speed, a buzzer and many more.
 *
 *          This module exposes an interface allowing its clients to configure
 *          the PWM block in the SoC. The configuration consists of setting the
 *          frequency and the duty cycle of the PWM output signal.
 *
 *          In the SoC, there can be multiple PWM frames, with each frame having
 *          multiple programmable instances. Please refer to the chipset's
 *          documentation for details pertaining to available PWM frames &
 *          instances.
 *
 * @code
 *    * The code snippet below demonstrates usage of the PWM interface. The
 *    * example below configures SoC PWM instance-0 to be generate a signal
 *    * with 25% duty cycle and operating at 960 KHz.
 *    * For this example, the PWM core clock frequency is assumed to be
 *    * configured to 19.2 MHz.
 *    * Clients can invoke qapi_PWM_Get_Clock_Frequency to obtain the frequency.
 *
 *    qapi_PWM_ID_t      pwm_id;
 *    qapi_Status_t      status = QAPI_OK;
 *
 *    status = qapi_PWM_Get_ID(QAPI_PWM_FRAME_0, QAPI_PWM_INSTANCE_0, &pwm_id);
 *
 *    if (status == QAPI_OK)
 *    {
 *      status = qapi_PWM_Set_Frequency(pwm_id, 960000);
 *
 *      if (status != QAPI_OK)
 *      {
 *        // Handle failed case here
 *      }
 *
 *      status = qapi_PWM_Set_Duty_Cycle(pwm_id, 25);
 *
 *      if (status != QAPI_OK)
 *      {
 *        // Handle failed case here
 *      }
 *
 *      status = qapi_PWM_Enable(pwm_id, true);
 *
 *      if (status != QAPI_OK)
 *      {
 *        // Handle failed case here
 *      }
 *    }
 *
 * @endcode
 * @}
 * @newpage */

/*==========================================================================

                       INCLUDE FILES FOR MODULE

==========================================================================*/
#include "qapi_types.h"
#include "qapi_status.h"
#include "stdint.h"
/*==========================================================================

                      PUBLIC PWM CONFIGURATION MACROS

==========================================================================*/

/** @addtogroup qapi_pwm
@{ */

/**
 *  PWM frame enumeration.
 *
 * @details The enumeration is used to get the ID to specific PWM frame's
 *          instance within the SoC.
 */
typedef enum
{
  QAPI_PWM_FRAME_0,
  QAPI_PWM_FRAME_1,
  QAPI_PWM_FRAME_2,

  QAPI_PWM_NUM_FRAMES
} qapi_PWM_Frame_t;

/**
 *  PWM instance enumeration.
 *
 * @details The enumeration is used to get the ID to specific PWM frame's
 *          instance within the SoC.
 */
typedef enum
{
  QAPI_PWM_INSTANCE_0,
  QAPI_PWM_INSTANCE_1,
  QAPI_PWM_INSTANCE_2,
  QAPI_PWM_INSTANCE_3,
  QAPI_PWM_INSTANCE_4,
  QAPI_PWM_INSTANCE_5,
  QAPI_PWM_INSTANCE_6,
  QAPI_PWM_INSTANCE_7,
  QAPI_PWM_INSTANCE_8,
  QAPI_PWM_INSTANCE_9,

  QAPI_PWM_NUM_INSTANCES
} qapi_PWM_Instance_t;

/* Mac_20191107 */
typedef struct{
	uint32_t pwm_gpio_id;
	uint32_t pwm_gpio_func;
}PWM_GPIO_MAP_TBL;


/*==========================================================================

                      PUBLIC DATA STRUCTURES

==========================================================================*/


/**
*  SoC PWM access ID.
*
* @details Unique ID provided by the module to the client. Clients must pass this
*          ID as a token with subsequent calls. Note that clients
*          should cache the ID.
*/
typedef uint32_t qapi_PWM_ID_t;


// Driver ID defines

#define  TXM_QAPI_PWM_GET_ID                 TXM_QAPI_PWM_BASE
#define  TXM_QAPI_PWM_RELEASE_ID             TXM_QAPI_PWM_BASE + 1
#define  TXM_QAPI_PWM_ENABLE                 TXM_QAPI_PWM_BASE + 2
#define  TXM_QAPI_PWM_SET_FREQUENCY          TXM_QAPI_PWM_BASE + 3
#define  TXM_QAPI_PWM_SET_DUTY_CYCLE         TXM_QAPI_PWM_BASE + 4
#define  TXM_QAPI_PWM_GET_CLOCK_FREQUENCY    TXM_QAPI_PWM_BASE + 5

#ifdef  QAPI_TXM_MODULE     // USER_MODE_DEFS

#define qapi_PWM_Get_ID(a,b,c)            ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_GET_ID,              (ULONG) a, (ULONG) b, (ULONG) c, (ULONG) 0))
#define qapi_PWM_Release_ID(a)            ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_RELEASE_ID,          (ULONG) a, (ULONG) 0, (ULONG) 0, (ULONG) 0))
#define qapi_PWM_Enable(a,b)              ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_ENABLE,              (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0))
#define qapi_PWM_Set_Frequency(a,b)       ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_SET_FREQUENCY,       (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0))
#define qapi_PWM_Set_Duty_Cycle(a,b)      ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_SET_DUTY_CYCLE,      (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0))
#define qapi_PWM_Get_Clock_Frequency(a,b) ((UINT) (_txm_module_system_call4)(TXM_QAPI_PWM_GET_CLOCK_FREQUENCY, (ULONG) a, (ULONG) b, (ULONG) 0, (ULONG) 0))

#elif defined QAPI_TXM_SOURCE_CODE      // KERNEL_MODE_DEFS

#include "qurt_txm_qapi_fwk.h"

/* ============================================================================
*  Function : qapi_PWM_Get_ID
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Gets a unique access ID.

  @details This function provides a unique access ID for a specified PWM instance.
           This is required in order to access PWM configuration APIs.

  @param[in] eInstance  PWM instance to get te ID for
  @param[in] pnID       Pointer to a location in which to store the access ID.

  @return
  QAPI_OK -- PWM ID was successfully created. \n
  QAPI_ERR -- PWM ID is currently in use or invalid instance was requested.

  Open interface:
  		QAPI_PWM_FRAME_0       QAPI_PWM_INSTANCE_0 -->Pin66(BG95)
  		QAPI_PWM_FRAME_0       QAPI_PWM_INSTANCE_1 -->Pin33(BG77)

*/

qapi_Status_t qapi_PWM_Get_ID
(
  qapi_PWM_Frame_t    eFrame,
  qapi_PWM_Instance_t eInstance,
  qapi_PWM_ID_t*      pnID
);


/* ============================================================================
*  Function : qapi_PWM_Release_ID
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Releases a SoC PWM instance.

  @details This function allows a client to relinquish the lock on a PWM
           instance.
           It facilitates sharing of a PWM instance between two drivers in
           different system modes where each driver may need to reconfigure the
           PWM instance. Using this function is not required unless such a
           condition dictates.

  @param[in]  nID       PWM instance access ID retrieved from the qapi_PWM_Get_ID() call.

  @return
  QAPI_OK  -- ID was released successfully. \n
  QAPI_ERR -- ID could not be released.

*/
qapi_Status_t qapi_PWM_Release_ID
(
  qapi_PWM_ID_t      nID
);


/* ============================================================================
*  Function : qapi_PWM_Enable
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Enables or disables a SoC PWM instance.

  @details This function enable the clock(s) required for enabling the
           requested PWM instance and also enables the same.

           It facilitates sharing of a PWM instance between two drivers in
           different system modes where each driver may need to reconfigure the
           PWM instance. Using this function is not required unless such a
           condition dictates.

  @param[in]  nID       PWM instance access ID retrieved from the qapi_PWM_Get_ID() call.
  @param[in]  bEnable   Boolean flag for enable (TRUE) or disable (FALSE) requset

  @return
  QAPI_OK  -- PWM instance was enabled successfully. \n
  QAPI_ERR -- PWM instance was not enabled

*/
qapi_Status_t qapi_PWM_Enable
(
  qapi_PWM_ID_t      nID,
  qbool_t            bEnable
);


/* ============================================================================
*  Function : qapi_PWM_Set_Frequency
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Sets the frequency of the output PWM signal.

  @details This function configures the frequency (period) of the requested
           PWM ID's signal.
           NOTE: The frequency can only be set once owing to H/W limitations.
                 This API needs to be called before setting the duty cycle.

           The PWM output frequency range (outlined below) is determined by the
           PWM core clock frequency.
           Clients can invoke qapi_PWM_Get_Clock_Frequency to obtain the
           frequency and check if it meets their requirements.

           Min PWM output frequency is (PWM clock frequency) / 2^16.
           Max PWM output frequency is (PWM clock frequency) / 2.

           For example, if the PWM clock frequency is 19.2 MHz, then:
           Min 292 Hz (19.2 MHz/2^16), max 9.6 MHz (4.8/2)

  @param[in] nID      ID retrieved from the qapi_PWM_Get_ID() call.
  @param[in] nFreqHz  Frequency of the PWM signal in Hz to set to.
    
  @return
  QAPI_OK  -- Frequency was configured successfully. \n
  QAPI_ERR -- Frequency could not be configured.\n
              Some of the reasons this could happen are:\n
              a) Client does not have ID for PWM instance\n
              b) Requested frequency is not within range\n
              c) Client tried to change the frequency
*/
qapi_Status_t qapi_PWM_Set_Frequency
(
  qapi_PWM_ID_t      nID,
  uint32_t           nFreqHz
);


/* ============================================================================
*  Function : qapi_PWM_Set_Duty_Cycle
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Sets the duty cycle of the output PWM signal.

  @details This function configures the duty cycle (active period) of the
           requested PWM ID's signal.

           NOTE: The duty cycle can only be set to quantized levels.
                 The specific levels depend upon the PWM output frequency and
                 the PWM core clock frequency.
                 Clients are responsible to check this and request the duty
                 cycle accordingly.
                 If the step size is not an integer, then use the rounded up
                 integer duty cycle.

                 For example, if step size is 6.25%, then 12.5%, 18.75% can be
                 supported.
                 Client needs to pass 19% as the parameter for 18.75%.
                 Client needs to pass 13% as the parameter for 12.5%.
                 Client needs to pass 7% as the parameter for 6.25%.
           
                 If client passes a duty cycle that cannot be achieved, then
                 the duty cycle will be set to the max achievable duty cycle
                 lesser than the requested one.
                 As per above example, if 21% is passed, then 18.75% would be
                 set.


           Duty cycle step size in %:
           100 * (PWM output frequency)/(PWM clock frequency)

           For example, if PWM clock frequency is 19.2 MHz and output is
           1.92 MHz, then the duty cycle step size will be:
           100 * 1.92/19.2 = 10%
           Hence, only duty cycles of 10%, 20%, 30% and so on can be obtained.
           In this example, for 1% step size, the user has to set the output
           frequency to 48 KHz or lower.

           The PWM clock frequency can be obtained from
           qapi_PWM_Get_Clock_Frequency.

  @param[in] nID      ID retrieved from the qapi_PWM_Get_ID() call.
  @param[in] nDuty    Duty cycle of the PWM signal in % (1-99)

  @return
  QAPI_OK  -- Duty cycle was configured successfully. \n
  QAPI_ERR -- Duty cycle could not be configured.\n
              Some of the reasons this could happen are:\n
              a) Client does not have ID for PWM instance\n
              b) Frequency has not been set\n
              c) Requested duty cycle is lesser than the\n
                 least achievable one
*/
qapi_Status_t qapi_PWM_Set_Duty_Cycle
(
  qapi_PWM_ID_t      nID,
  uint32_t           nDuty
);

/* ============================================================================
*  Function : qapi_PWM_Get_Clock_Frequency
* ============================================================================*/
/**
* @versiontable{2.0,2.45,
* Driver 1.2.0 & Introduced. @tblendline
* }

   Get the PWM clock frequency of a PWM instance.

  @details This function queries the PWM core clock frequency so that clients
           can ascertain the frequency range and duty cycle step size.

  @param[in]  nID       Pin access ID retrieved from the qapi_PWM_Get_ID() call.
  @param[out] pnFreqHz  Pointer to fill in with the frequency. A value of zero
                        indicates that the frequency could not be determined.

  @return
  QAPI_OK  -- Operation completed successfully. \n
  QAPI_ERR -- Operation failed.

*/
qapi_Status_t qapi_PWM_Get_Clock_Frequency
(
  qapi_PWM_ID_t    nID,
  uint32_t*        pnFreqHz
);

#else   // DEF_END

#error "No QAPI flags defined"

#endif

qapi_Status_t qapi_pwm_handler
(
  UINT id,
  UINT a,
  UINT b,
  UINT c,
  UINT d
);

/** @} */ /* end_addtogroup qapi_pwm */

 #endif /* __QAPI_PWM__ */
