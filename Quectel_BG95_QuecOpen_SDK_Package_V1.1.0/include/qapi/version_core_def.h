/**
 * @file version_core_def.h
 *
 * @brief QAPI versions defintions for category driver and security.
 *
 * @details This file defines the QAPI versions for category driver and security.
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

when       who           what, where, why
--------   ---     ----------------------------------------------------------
15/06/19   cdeepthi     created this file.
===========================================================================*/

#ifndef __VERSION_CORE_DEF_H__
#define __VERSION_CORE_DEF_H__

/*** versions for category driver and security***/

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

/** driver versions */
#define DRIVER_MAJOR_NUM                 1
#define DRIVER_MINOR_NUM                 2
#define DRIVER_PATCH_NUM                 0
#define DRIVER_VER_STR                   DRIVER_MAJOR_NUM.DRIVER_MINOR_NUM.DRIVER_PATCH_NUM

/** security versions */
#define SECURITY_MAJOR_NUM               1
#define SECURITY_MINOR_NUM               2
#define SECURITY_PATCH_NUM               0
#define SECURITY_VER_STR                 SECURITY_MAJOR_NUM.SECURITY_MINOR_NUM.SECURITY_PATCH_NUM

#endif
