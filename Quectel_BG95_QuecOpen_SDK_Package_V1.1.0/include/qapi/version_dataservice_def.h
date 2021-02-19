/**
 * @file version_dataservice_def.h
 *
 * @brief QAPI versions defintions for category dataservice.
 *
 * @details This file defines the QAPI versions for category dataservice.
 */

/*===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/19   more       added qapi_Coap_Reconnect in dam module , updated behaviour of qapi_Coap_Get_Header
10/09/19   shanu      added qapi_DSS_Get_Apn_Rate_Control and qapi_DSS_Get_Splmn_Rate_Control 
02/09/19   xianma     Add qapi_DSS_Get_Link_Mtu_per_family(), change dataservices version to 1.3.0
20/06/19   baohan     created this file.
===========================================================================*/
#ifndef __VERSION_DATASERVICE_DEF_H__
#define __VERSION_DATASERVICE_DEF_H__

/*
*
* 3-number version format. For example 1.2.0
*
* 1) The major number 1, indicates backward compatibility.
*    It only gets incremented if the backward compatibility is broken intentionally.
* 2) The minor number 2, indicates a change in API.
*    It gets incremented when there is a change in API, struct, enums
* 3) The patch number 0 indicates non source code chagnes.
*    It gets incremented for non source code changes such as comments
*
* Following is the list of changes that are NOT backward compatible:
*
* 1) Updating/removing the existing enum values or identifiers.
* 2) Adding fields to existing structs.
* 3) Removing fields from existing structs.
* 4) Updating the signature of existing functions
*
*/

/** data service versions */
#define DATA_SERVICE_MAJOR_NUM           1
#define DATA_SERVICE_MINOR_NUM           5
#define DATA_SERVICE_PATCH_NUM           0
#define DATA_SERVICE_VER_STR             DATA_SERVICE_MAJOR_NUM.DATA_SERVICE_MINOR_NUM.DATA_SERVICE_PATCH_NUM

#endif
