/*===========================================================================
                         TEST_DNS_MODULE.H

DESCRIPTION
   This header file contains declarations of DNS routine
   functions.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/* Signal used in this DNS module, bit should start from 6 afterwards */
enum DAM_SIG
{
  DAM_SIG_TIMER = (0x1<<0),
};

/* Mask include all NDS signals */
#define DAM_SIG_APP       DAM_SIG_TIMER

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
void dam_app_routine();

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


