/*===========================================================================
                         TEST_DATA_CALL_MODULE.H

DESCRIPTION
   This header file contains declarations of normal data call start and stop
   functions.
  
   Copyright (c) 2018 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

extern qapi_DSS_Hndl_t   dam_dss_hndl;

/*=============================================================================

FUNCTION DAM_DSS_START_DATACALL

DESCRIPTION
  This function will start data call with IP settings.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_start_datacall(void);

/*=============================================================================

FUNCTION DAM_DSS_NET_IS_CONN

DESCRIPTION
  This function call the related APIs after the data call is established.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
void dam_dss_net_is_conn(qapi_DSS_Hndl_t hndl);


/*=============================================================================

FUNCTION DAM_DSS_NET_NO_NET

DESCRIPTION
  This function call the related APIs after the data call is torn down.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None

==============================================================================*/
extern void dam_dss_net_no_net(qapi_DSS_Hndl_t hndl);

