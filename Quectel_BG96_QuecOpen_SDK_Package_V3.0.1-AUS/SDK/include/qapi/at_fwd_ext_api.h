#ifndef AT_FWD_EXT_API_H
#define AT_FWD_EXT_API_H

/*===========================================================================
                         AT_FWD_EXT_API_H.H

DESCRIPTION
  This header file contains API's which are exposed to external tasks/applications
  from AT_FWD task

  Copyright (c)2016 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*==============================================================================

                            EDIT HISTORY FOR FILE
when        who    what, where, why
--------    ---    -------------------------------------------------------------
09/16/2016   sr     created file
==============================================================================*/
#ifndef _UINT16_DEFINED
typedef unsigned short     uint16;
#define _UINT16_DEFINED
#endif
#ifndef _UINT32_DEFINED
typedef unsigned long int  uint32;
#define _UINT32_DEFINED
#endif
#ifndef _UINT8_DEFINED
typedef unsigned char      uint8;
#define _UINT8_DEFINED
#endif

#ifndef _BOOLEAN_DEFINED
typedef  unsigned char      boolean;     
#define _BOOLEAN_DEFINED
#endif

typedef enum
{
  AT_FWD_DS_MUX_MODE_MIN = 0,
  AT_FWD_DS_MUX_MODE_BASIC = AT_FWD_DS_MUX_MODE_MIN,
  AT_FWD_DS_MUX_MODE_ADVANCED = 1,
}ds_mux_at_fwd_mode_enum_type;


typedef enum
{
  AT_FWD_DS_MUX_SUBSET_MIN = 0,
  AT_FWD_DS_MUX_SUBSET_UIH = AT_FWD_DS_MUX_SUBSET_MIN,
  AT_FWD_DS_MUX_SUBSET_UI  = 1,
  AT_FWD_DS_MUX_SUBSET_I   = 2,
}ds_mux_at_fwd_subset_enum_type;

typedef enum
{
  AT_FWD_DS_MUX_PHY_PORT_SPEED_INVALID = 0,
  AT_FWD_DS_MUX_PHY_PORT_SPEED_1 = 1,  /* 9,600 bit/s */
  AT_FWD_DS_MUX_PHY_PORT_SPEED_2 = 2,  /* 19,200 bit/s */
  AT_FWD_DS_MUX_PHY_PORT_SPEED_3 = 3,  /* 38,400 bit/s */
  AT_FWD_DS_MUX_PHY_PORT_SPEED_4 = 4,  /* 57,600 bit/s */
  AT_FWD_DS_MUX_PHY_PORT_SPEED_5 = 5,  /* 115,200 bit/s */
  AT_FWD_DS_MUX_PHY_PORT_SPEED_6 = 6,  /* 230,400 bit/s */
}ds_mux_at_fwd_port_speed_enum_type;


/*==============================================================================
Global stracture for storing AT+CMUX command parameters received from AT-FWD
==============================================================================*/
typedef struct
{

  uint16   mask;
  /**<  represent fields        */
  ds_mux_at_fwd_mode_enum_type   operating_mode;               
  /**< Mode 0 - basic, 1- advanced.        */
  ds_mux_at_fwd_subset_enum_type  subset;                         
  /**< 0-UIH, 1-UI, 2-I frames.            */
  ds_mux_at_fwd_port_speed_enum_type  port_speed;                    
  /**< port speed valid values: 1-6.       */
  uint16  frame_size_N1;            
  /**< Max frame size (1-32768).           */
  uint16  response_timer_T1;            
  /**<  Time UE waits for an acknowledgement before
   resorting to other action (e.g. transmitting a frame)
   default 100ms min value 10 ms.
 */
  uint8   re_transmissions_N2;      
  /**< Max re-tries N2 (0-255).            */
   
  uint16  response_timer_T2;            
  /**< Time mux control channel waits before 
 re-transmitting a command default 300ms and
 min value 20 ms.
 */
  uint32  wake_up_timer_T3;            
  /**<  Time UE waits for response for a 
 power wake-up command default 10ms and min
 value 1 ms.
 */
  uint8   window_size_k;                
  /**< Window size default 2, range (1-7). */

}at_fwd_cmux_cmd_params_struct;


/*==============================================================================
Callback Type used by QTI bridge to register a callback with AT-FWD
==============================================================================*/
typedef void (*at_cmux_handler_cb_type)(uint8 *, uint8, char*, uint32);
typedef void (*at_post_process_cb_type)(void);
typedef void (*at_qmi_service_up_ind_cb_type)(void);


/*==============================================================================
Call back type used by AT-FWD to register a callback when qti_bridge
ds_mux validates all mux params and wants to send back response to AT-FWD
==============================================================================*/

typedef void (*at_fwd_qti_new_callback)(boolean, char*);

/*==============================================================================
Callback Type used to register a new custom callback with AT-FWD
==============================================================================*/
typedef void (*at_fwd_cb_type)(boolean, char*, uint8*, uint8, uint32);

/*==============================================================================
FUNCTION AT_FWD_REG_AT_CMUX_CB

DESCRIPTION
  This API will be used by QTI_BRIDGE to register callback when AT+CMUX is
  handled by AT-FWD task

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void at_fwd_reg_at_cmux_cb(at_cmux_handler_cb_type at_cmux_handler_cb,
                                   at_post_process_cb_type at_post_process_cb);

/*==============================================================================
Global stracture pointer to be used in qti_bridge_process command to be passed
to mux params handling function
==============================================================================*/

void at_fwd_to_qti_cb_reg(at_fwd_qti_new_callback qti_to_at_fwd_handler_cb);

/*===========================================================================
FUNCTION ATFWD_TIMER_CB

DESCRIPTION
  Callback invoked when timer expires

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
=========================================================================*/
void atfwd_timer_cb(void* unused);

/*==============================================================================
FUNCTION AT_REG_QMI_SERV_CB

DESCRIPTION
  This API will be used by QTI_BRIDGE to register callback when QMI service is UP

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

==============================================================================*/
void at_reg_qmi_serv_cb(at_qmi_service_up_ind_cb_type at_qmi_service_up_handler_cb);

#endif
