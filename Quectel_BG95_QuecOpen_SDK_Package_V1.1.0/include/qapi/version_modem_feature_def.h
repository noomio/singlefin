/**
 * @file version_modem_feature_def.h
 *
 * @brief QAPI versions defintions for modem_feature category.
 *
 * @details This file defines the QAPI versions for modem_feature category.
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
Notice  that changes are listed in reverse chronological order.

$Header:

when       who           what, where, why
--------   ---     ----------------------------------------------------------
20/06/19   rkr           created this file.
===========================================================================*/

#ifndef __VERSION_MODEM_FEATURE_DEF_H__
#define __VERSION_MODEM_FEATURE_DEF_H__

/*** version for modem category***/

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

/** modem feature versions */
#define MODEM_FEATURE_MAJOR_NUM          1
#define MODEM_FEATURE_MINOR_NUM          2
#define MODEM_FEATURE_PATCH_NUM          0
#define MODEM_FEATURE_VER_STR            MODEM_FEATURE_MAJOR_NUM.MODEM_FEATURE_MINOR_NUM.MODEM_FEATURE_PATCH_NUM

#endif
