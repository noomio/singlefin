/*===========================================================================
                         TEST_SAMPLE_MODULE.H

DESCRIPTION
   This header file contains generic declarations for MQTT DAM
   functions.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

#ifndef TEST_SAMPLE_MODULE_H
#define TEST_SAMPLE_MODULE_H

/* Signal used in MQTT DAM, bit should start from 6 afterwards */
enum DAM_DNS_SIG
{
  DAM_SIG_MQTT_CONNECT = (0x1<<6),
  DAM_SIG_MQTT_DISCONNECT = (0x1<<7)
};

/* Mask include all MQTT related signals */
#define DAM_SIG_APP       DAM_SIG_MQTT_CONNECT | \
                          DAM_SIG_MQTT_DISCONNECT

/*=============================================================================

FUNCTION DAM_APP_ROUTINE

DESCRIPTION
  This function is common wrapper to run module routine after data setup.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_routine(qapi_DSS_Hndl_t hndl);

/*=============================================================================

FUNCTION DAM_APP_ROUTINE

DESCRIPTION
  This function is to check and process signals for the sample app modules.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_app_signal_wait(uint32_t received_sigs);

#endif

