/*===========================================================================
  Copyright (c) 2017-2019 Qualcomm Technologies, Inc.  All Rights Reserved.  
  Confidential and Proprietary - Qualcomm Technologies, Inc. 
============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-06-11    PG    Updated Copyright format as per guidelines
2017-11-07   KCS    Create.
===========================================================================*/


#ifndef QAPI_PMIC_STATUS_H
#define QAPI_PMIC_STATUS_H

 /**
 * @file qapi_pmic_status.h
 *
 * @Error codes define for PMIC module.
 */
 
#include "qapi_txm_base.h"
#include "qapi_status.h"

/*==============================================================================
                              MACROS
=============================================================================*/

/** @addtogroup qapi_pmapp_vbatt
@{ */								                                      
 
/** Error macros defined for QAPI errors. */
#define ___QAPI_ERROR_PMIC(x)  ((qapi_Status_t)(__QAPI_ERROR(QAPI_MOD_BSP_PMIC,  x)))

/** Error macro for battery absent. */
#define QAPI_ERR_BATT_ABSENT              ___QAPI_ERROR_PMIC(1)

/** @} */ /* end_addtogroup qapi_pmapp_vbatt */

#endif /** QAPI_PMIC_STATUS_H*/
